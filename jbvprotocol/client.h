/*
 * JBVClient.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_CPP_LIB_JBVCLIENT_H_
#define MAIN_CPP_LIB_JBVCLIENT_H_


#include "../rtos/freertos.h"
#include "command.h"
#include "../tcpip/connection.h"
#include "../misc/callback.h"
#include "frame.h"
#include "framing.h"
#include "lease.h"
#include "softwareid.h"
#include <string>
#include <map>
#include <vector>

namespace JBVProtocol
{
/*
	class Client
	{
		//Commands:
		void CMD_HELP(Client *client, Frame *rxFrame);
		void CMD_DLEASE(Client *client, Frame *rxFrame);
		void CMD_CLEASE(Client *client, Frame *rxFrame);

		std::map<std::string, Callback<void, Client*, Frame*>*> commandList;
		Framing framing;
		Lease lease;
		Connection *connection = 0;
		FreeRTOS::Task task;
		FreeRTOS::Queue<Frame*> pendingFrames = FreeRTOS::Queue<Frame*>(5);

		void SendFrame(Frame *frame);
		void Work(void *arg);
		void FrameRecieved(Frame *frame);
		void DataRecieved(Connection* con, uint8_t* data, uint32_t len);

	public:
		Client(SoftwareID softID);
		~Client();

		Callback<void, Client*, Frame*> BroadcastReceived;
		Callback<void, Client*, Frame*> HandleFrame;

		GUID GetGuid();
		//Frame *SendRequest(uint32_t rxID, std::string data, int timeout = 1000);
		void SendReply(Frame *origionalFrame, std::string data);
		//void SendBroadcastRequest(uint16_t seq, std::string data);
		//void SendBroadcastReply(Frame *origionalFrame, std::string data);
		void SetConnection(Connection *con);
	};
	*/
}







#endif /* MAIN_CPP_LIB_JBVCLIENT_H_ */
