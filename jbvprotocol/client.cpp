/*
 * client.cpp
 *
 *  Created on: Apr 12, 2021
 *      Author: Bas
 */

#include "client.h"
#include "../rtos/freertos.h"

namespace JBVProtocol
{
	/*
	//Private commands:

	void Client::CMD_HELP(Client *client, Frame *rxFrame)
	{
		std::string reply;
		for (auto const& [key, val] : commandList)
		{
			reply += key + " ";
		}
		SendReply(rxFrame, reply);
	}

	void Client::CMD_DLEASE(Client *client, Frame *rxFrame)
	{
		std::vector<std::string> args = rxFrame->GetArgs();
		if(args.size() == 1)
		{
			SendReply(rxFrame, lease.ToString());
		}
		else if(args.size() == 2)
		{
			SoftwareID reqId = (SoftwareID)strtoull( args[1].c_str(), NULL, 10);

			if(reqId == lease.SoftwareId)
			{
				SendReply(rxFrame, lease.ToString());
			}
		}
		else
			SendReply(rxFrame, "NACK Invalid\ arguments");
	}

	void Client::CMD_CLEASE(Client *client, Frame *rxFrame)
	{

	}


	//Public:

	Client::Client(SoftwareID softID)
	{
		lease.SoftwareId = softID;
		framing.OnFrameCollected.Bind(this, &Client::FrameRecieved);

		task.SetCallback(this, &Client::Work);
		task.Run("JBVClient", 7, 1024 * 3, NULL);

		commandList["HELP"] = new Callback<void, Client*, Frame*>();
		commandList["DLEASE"] = new Callback<void, Client*, Frame*>(this, &Client::CMD_DLEASE);
		//commandList["CLEASE"] = new Callback<void, Client*, Frame*>(this, &Client::CMD_CLEASE);
	}

	Client::~Client()
	{

	}

	void Client::SetConnection(Connection *con)
	{
		connection = con;
		connection->OnDataReceived.Bind(this, &Client::DataRecieved);
	}



	GUID Client::GetGuid()
	{
		return lease.Guid;
	}


	void Client::SendReply(Frame *origionalFrame, std::string data)
	{
		//Prepare reply.
		JBVProtocol::Frame txFrame;
		txFrame.Opts = origionalFrame->Opts;
		txFrame.Opts = txFrame.Opts &~ Options::Request;	//Copy options, but change request to reply.
		txFrame.Hops = 0;
		txFrame.TxID = lease.Id;
		txFrame.RxID = origionalFrame->TxID;
		txFrame.Sequence = origionalFrame->Sequence;
		txFrame.SetData(data.c_str(), data.length());
		SendFrame(&txFrame);
	}


	//Private:

	void Client::SendFrame(Frame *frame)
	{
		//Don't check for a lease. We can always send a frame. The router needs to handle invalid leases. In case of direct comminucation leases arent required
		frame->TxID = lease.Id;
		frame->Hops = 0;
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

	void Client::Work(void *arg)
	{
		while(true)
		{
			if(lease.ExpiresIn().GetTotalMinutes() < 5)
			{
				//RequestLeaseFromLeaseServer(); //@TODO, When direct connect we never get a lease. Meight want to optimize this.
			}

			Frame *rxFrame = 0;
			while(pendingFrames.Dequeue(&rxFrame, 1000) != 0)
			{

				if(HASFLAG(rxFrame->Opts, Options::ASCII))
				{

					if(HASFLAG(rxFrame->Opts, Options::Request))
					{
						std::string cmd = rxFrame->GetCommand();

						ESP_LOGI("client", "Request recieved '%s'", cmd.c_str());

						auto it = commandList.find(cmd);
						if(it != commandList.end())
						{
							ESP_LOGI("client", "Command in list");
							Callback<void, Client*, Frame*> *callback = it->second;
							callback->Invoke(this, rxFrame);
						}
						else
						{
							//Command not found
							SendReply(rxFrame, "NACK Invalid\ command");
						}
					}
					else
					{
						//@TODO: Handle replies
					}
				}
				else
				{
					SendReply(rxFrame, "NACK Only\ ASCII\ mode\ supported");
					ESP_LOGI("JBVClient", "Non ASCII mode not supported!");
				}

				delete rxFrame;
				rxFrame = 0;
			}
		}
	}


	void Client::FrameRecieved(Frame *frame)
	{
		pendingFrames.Enqueue(&frame, 1000);
	}

	void Client::DataRecieved(Connection* con, uint8_t* data, uint32_t len)
	{
		framing.Unstuff(data, len);
	}
*/
}




/*
		void SendASCIIReply(std::string reply, JBVProtocol::Frame *rxFrame)
		{

		}




	public:

		 These will be implemented later, when ascii mode is not wanted.
		Frame *SendRequest(uint32_t rxID, uint32_t dataLength, uint8_t *data, bool ascii = true)
		{

		}

		void SendBroadcastRequest(uint16_t seq, uint32_t dataLength, uint8_t *data)
		{

		}

		void SendBroadcastReply(Frame *origionalFrame, uint32_t dataLength, uint8_t *data)
		{

		}

		Frame *SendRequest(uint32_t rxID, std::string data, int timeout = 1000)
		{

		}

		void SendReply(Frame *origionalFrame, std::string data, int timeout = 1000)
		{

		}

		void SendBroadcastRequest(uint16_t seq, std::string data)
		{

		}

		void SendBroadcastReply(Frame *origionalFrame, std::string data)
		{

		}




*/


