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

namespace JBVProtocol
{

	class Client
	{
		Framing framing;
		Lease lease;
		Connection *connection = 0;
		FreeRTOS::Task *task;
		FreeRTOS::Queue<Frame*> pendingFrames = FreeRTOS::Queue<Frame*>(5);

	public:
		Callback<void, Client*, Frame*> HandleFrame;

		void SendFrame(Frame *frame)
		{
			//Don't check for a lease. We can always send a frame. The router needs to handle invalid leases. In case of direct comminucation leases arent required
			frame->TxID = lease.Id;
			frame->SetType(FrameTypes::DataFrame);
			if (connection != NULL)
			{
				uint32_t len = frame->GetTotalLength() + 10; 	//Assume 10 extra bytes is enough for SOF EOF and some escaping
				uint8_t *buffer = (uint8_t*) malloc(len);
				uint32_t actLen = framing.EscapeFrame(frame, buffer, len);

				if(actLen > len)	//So our assumption was not large enough
				{
					free(buffer);
					buffer = (uint8_t*) malloc(actLen);
					framing.EscapeFrame(frame, buffer, actLen);
				}

				connection->Send(buffer, actLen);
				free(buffer);
			}
			else
				ESP_LOGE("JBVClient", "No connection");

		}

		GUID GetGuid()
		{
			return lease.Guid;
		}





	private:
		/*
		void RequestLeaseFromLeaseServer()
		{
			//ESP_LOGI("JBVClient", "Request lease");
			void *leasePtr = lease.GetDataPointer();
			ProtocolFrame frame = ProtocolFrame::PrepareCommand(0, ProtocolCommands::RequestLease, leasePtr, lease.GetDataSize());
			SendFrame(&frame);
		}

		void ReplyMyLease(uint32_t rxId)
		{
			void *leasePtr = lease.GetDataPointer();
			ProtocolFrame frame = ProtocolFrame::PrepareCommand(rxId, ProtocolCommands::ReplyLease, leasePtr, lease.GetDataSize());
			SendFrame(&frame);
		}
*/

		void SendASCIIReply(std::string reply, JBVProtocol::Frame *rxFrame)
		{
			//Prepare reply.
			JBVProtocol::Frame txFrame;
			txFrame.Sequence = rxFrame->Sequence;
			txFrame.SetOptions(rxFrame->GetOptions());
			txFrame.SetType(rxFrame->GetType());
			txFrame.RxID = rxFrame->TxID;
			txFrame.SetData(reply.c_str(), reply.length());
			SendFrame(&txFrame);
		}


		void Work(void *arg)
		{
			while(true)
			{
				if(lease.ExpiresIn().GetTotalMinutes() < 5)
				{
					//RequestLeaseFromLeaseServer(); //@TODO, When direct connect we never get a lease. Meight want to optimize this.
				}

				Frame *frame = 0;
				while(pendingFrames.Dequeue(&frame, 1000) != 0)
				{
					if(frame->GetType() == FrameTypes::ProtocolFrame)
					{

						if((frame->GetOptions() & FrameOptions::ASCII) == FrameOptions::ASCII)
						{
							std::vector<std::string> args = frame->GetArgs();

							if(args[0] == "CLEASE")
							{
								//In order to accept a new lease, the GUID must match with ours. SID and VER aren't nessesary.

								if(args.size() == 4)
								{
									GUID rxGuid = GUID::FromString(args[2]);
									if(rxGuid == lease.Guid)
									{
										lease.Id = strtoull( args[1].c_str(), NULL, 10);
										lease.ExpirationDate.utc = strtoull( args[3].c_str(), NULL, 10);
										SendASCIIReply("ACK", frame);
									}
									else
										SendASCIIReply("NGUID", frame);
								}
								else
									SendASCIIReply("NARG", frame);
							}
							else if(args[0] == "DLEASE")
							{
								//Reply if, no arguments or argument matches sid

								if(args.size() == 1)
								{
									SendASCIIReply(lease.ToString(), frame);
								}
								else if(args.size() == 2)
								{
									SoftwareID reqId = (SoftwareID)strtoull( args[1].c_str(), NULL, 10);

									if(reqId == lease.SoftwareId)
									{
										SendASCIIReply(lease.ToString(), frame);
									}
								}
							}
							else
							{
								SendASCIIReply("NCMD", frame);
							}
						}
					}
					else if(frame->GetType() == FrameTypes::DataFrame)
					{
						HandleFrame.Invoke(this, frame);
					}

					delete frame;
					frame = 0;
				}
			}
		}


		void FrameRecieved(Frame *frame)
		{
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
			connection->OnDataReceived.Bind(this, &Client::DataRecieved);
		}

		Client(SoftwareID softID)
		{
			lease.SoftwareId = softID;
			framing.OnFrameCollected.Bind(this, &Client::FrameRecieved);

			task = new FreeRTOS::Task("JBVClient", 7, 1024 * 3, this, &Client::Work);
			task->Run(NULL);
		}

		~Client()
		{

		}

	};
}







#endif /* MAIN_CPP_LIB_JBVCLIENT_H_ */
