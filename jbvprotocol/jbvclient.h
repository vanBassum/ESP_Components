/*
 * JBVClient.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_CPP_LIB_JBVCLIENT_H_
#define MAIN_CPP_LIB_JBVCLIENT_H_


#include "../freertos_cpp/freertos.h"
#include "command.h"
#include "../tcpip/connection.h"
#include "frame.h"
#include "framing.h"
#include "lease.h"
#include "softwareid.h"

class JBVClient
{
	Framing framing;
	Lease lease;
	Connection *connection = 0;
	SoftwareID softwareID = SoftwareID::Unknown;
	FreeRTOS::Task *task;
	FreeRTOS::Queue<Frame*> pendingFrames = FreeRTOS::Queue<Frame*>(5);

public:
	Callback<void, JBVClient*, Frame*> HandleFrame;

	void SendFrame(Frame *frame)
	{
		//frame->Log();
		if (lease.IsValid() || ((uint8_t)(frame->Options & FrameOPT::Broadcast) > 0))
		{
			frame->TxID = lease.ID;
			if (connection != 0)
			{
				uint32_t len = framing.GetEscapedSize(frame);
				uint8_t *buffer = (uint8_t*) malloc(len);
				if(framing.EscapeFrame(frame, buffer, len) == len)
					connection->Send(buffer, len);
				free(buffer);
			}
			else
				ESP_LOGE("JBVClient", "No connection");
		}
	}

	Guid GetGuid()
	{
		return lease.Key;
	}

private:
	void RequestLease()
	{
		//ESP_LOGI("JBVClient", "Request lease");
		Frame frame = Frame::PrepareRequest(0, 0, 16, Commands::RequestLease, FrameOPT::Broadcast);
		lease.Key.ToArray(frame.Data);
		SendFrame(&frame);
	}

	void ReplyLease(Frame *rx)
	{
		Lease temp = Lease(rx->Data);
		if (temp.Key == lease.Key)
		{
			ESP_LOGI("JBVClient", "Lease acquired");
			lease = temp;
		}
	}


	void RequestID(Frame *rx)
	{
		uint16_t id = *(uint16_t*)rx->Data;
        if (id == lease.ID)
        {
        	ESP_LOGI("JBVClient", "Request ID");
        	Frame frame = Frame::PrepareReply(rx, 2, Commands::ReplyID, FrameOPT::Broadcast);
			memcpy(frame.Data, &lease.ID, frame.DataLength);
			SendFrame(&frame);
        }
	}



	void RequestSID(Frame *rx)
	{
		SoftwareID sid = (SoftwareID)(*(uint32_t*)rx->Data);
        if(sid == SoftwareID::Unknown || sid == softwareID)
        {
    		ESP_LOGI("JBVClient", "Request SID");
    		Frame frame = Frame::PrepareReply(rx, 4, Commands::ReplySID);
    		memcpy(frame.Data, &softwareID, frame.DataLength);
    		SendFrame(&frame);
        }
	}

	void RequestLease(Frame *rx)
	{
		return;
		//Specifically requested from me. So reply our lease.
		if(rx->RxID == lease.ID)
		{
			Frame frame = Frame::PrepareReply(rx, 26, Commands::ReplyLease);
			lease.ToArray(frame.Data);
			SendFrame(&frame);
		}
	}

	void Work(void *arg)
	{
		while(true)
		{
			if(lease.ExpiresIn().GetTotalMinutes() < 5)
			{
				RequestLease();
			}



			Frame *frame = 0;
			pendingFrames.Dequeue(&frame, 1000);


			while(frame != 0)
			{
				Commands cmd = (Commands)frame->CommandID;

				//ESP_LOGI("client", "cmdno %d", frame->CommandID);
				switch(cmd)
				{
				case Commands::INVALID: 								break;
				case Commands::RequestLease:	RequestLease(frame); 	break;
				case Commands::ReplyID: 								break;
				case Commands::ReplySID: 								break;
				case Commands::RequestID:		RequestID(frame);		break;
				case Commands::ReplyLease:		ReplyLease(frame);		break;
				case Commands::RequestSID:		RequestSID(frame);		break;
				default:
					HandleFrame.Invoke(this, frame);
					break;
				}


				delete frame;
				frame = 0;
				pendingFrames.Dequeue(&frame, 1000);
			}

		}
	}


	void FrameRecieved(Frame *frame)
	{
		//frame->Log();
		pendingFrames.Enqueue(&frame, 1000);
	}

	void DataRecieved(Connection* con, uint8_t* data, uint32_t len)
	{
		framing.Unstuff(data, len);
	}

public:



	void SetConnection(Connection *con)
	{
		connection = con;
		connection->OnDataReceived.Bind(this, &JBVClient::DataRecieved);
	}

	JBVClient(SoftwareID softID)
	{
		softwareID = softID;

		framing.OnFrameCollected.Bind(this, &JBVClient::FrameRecieved);

		task = new FreeRTOS::Task("JBVClient", 7, 1024 * 4, this, &JBVClient::Work);
		task->Run(NULL);
	}

	~JBVClient()
	{

	}

};








#endif /* MAIN_CPP_LIB_JBVCLIENT_H_ */
