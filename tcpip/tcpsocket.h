/*
 * TCPConnection.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_TCPIP_TCPCLIENT_H_
#define MAIN_TCPIP_TCPCLIENT_H_


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <string>
#include "connection.h"
#include "../rtos/freertos.h"

#define ISSET(events, ev)	(events & (uint32_t)ev)

class TCPSocket
{
public:
	Callback<void, TCPSocket*, uint8_t*, uint32_t> OnDataReceived;

private:
	FreeRTOS::NotifyableTask task;
	FreeRTOS::SemaphoreBinary connectedSemph;
	int sock = 0;
	bool autoReconnect = false;
	int addr_family = 0;
	int ip_protocol = 0;
	struct sockaddr_in dest_addr;



	enum class State : uint8_t		//Don't change the existing numbers, its used for logging. You can add extra states.
	{
		Initializing,
		Disconnected,
		Connected,
		Cleanup,
	};

	enum class Events : uint32_t	//These are flags!
	{
		StateChanged = (1 << 0),
		Connect = (1 << 1),
		Disconnect = (1 << 2),
		DoReceive = (1 << 3),
	};

	State prevState = State::Initializing;
	State actState = State::Initializing;
	State nextState = State::Initializing;

	void Work(FreeRTOS::NotifyableTask* task, void* args)
	{
		int len;
		uint8_t rx_buffer[128];

		while (true)
		{
			uint32_t events = 0;

			task->WaitForNotification(&events, 1000);	//When there are no events to be handled, the default cycletime is set by this timeout.

			switch (actState)
			{
			case State::Initializing:
				if (ISSET(events, Events::StateChanged))
				{
					//On entry.
				}

				//Continuously


				//Transitions
				nextState = State::Disconnected;	//TODO This is where we should be waiting for wifi or ETH.
				break;


			case State::Disconnected:
				if (ISSET(events, Events::Connect) || autoReconnect)
				{
					sock = socket(addr_family, SOCK_STREAM, ip_protocol);
					if (sock >= 0)
					{
						int err = connect(sock, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr_in6));
						if (err == 0)
						{
							//Success
							ESP_LOGD("TCPConnection", "Socket connected sucessfully %d", sock);
							nextState = State::Connected;
						}
						else
						{
							ESP_LOGE("TCPConnection", "Socket unable to connect: errno %d, %s", errno, strerror(errno));
							nextState = State::Cleanup;
						}
					}
					else
					{
						ESP_LOGE("TCPConnection", "Unable to create socket: errno %d, %s", errno, strerror(errno));
						nextState = State::Cleanup;
					}
					if (nextState != State::Connected)
						connectedSemph.Give();
				}
				break;

			case State::Connected:
				connectedSemph.Give();
				len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
				if (len > 0)
				{
					//Data recieved.
					if (OnDataReceived.IsBound())
						OnDataReceived.Invoke(this, rx_buffer, len);
					task->Notify((uint32_t)Events::DoReceive);	//This makes sure we immediately start receiveing again.
				}
				else if (len == 0)
				{
					//Disconnected
					nextState = State::Cleanup;
					ESP_LOGE("TCPConnection.h", "Disconnected by other");
				}
				else
				{
					//Error while receiving
					switch (errno)
					{
					case 128:	//Disconnected, Happens when e disconnect
						break;
					case 9:		//Bad file number, Happens when e disconnect
						break;
					default:
						ESP_LOGE("TCPConnection.h", "Error occurred during receiving: errno %d, %s", errno, strerror(errno));
						break;
					}
					nextState = State::Cleanup;
				}
				break;

			case State::Cleanup:
				shutdown(sock, 0);
				close(sock);
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				nextState = State::Disconnected;
				break;

			default:
				ESP_LOGE("TCPConnection", "State not handled'");
				break;
			}


			// -------------------------------
			// ---- State switching logic ----
			// -------------------------------

			if (actState != nextState)
			{
				prevState = actState;
				actState = nextState;

				task->Notify((uint32_t)Events::StateChanged);	//Setting this will skip the 1sec delay and let the new state know its its first cycle.
			}
		}
	}


public:

	void Send(uint8_t* data, uint32_t length)
	{
		int to_write = length;
		while (to_write > 0)
		{
			int written = send(sock, data + (length - to_write), to_write, 0);
			if (written < 0)
			{
				switch (errno)
				{
				case 119:
					break;
				case 9:
					break;
				default:
					ESP_LOGE("TCPConnection.h", "Error occurred during sending: errno %d, %s", errno, strerror(errno));
					break;
				}
				return;
			}
			to_write -= written;
		}
	}

	bool IsConnected()
	{
		return actState == State::Connected;
	}


	TCPSocket()
	{
		task.SetCallback(this, &TCPSocket::Work);
		task.Run("TCPConnection", 6, 1024 * 2);
	}

	~TCPSocket()
	{
		shutdown(sock, 0);
		close(sock);
	}

	bool Connect(std::string host_ip, int port, bool autoReconnect, int timeout)
	{
		if (IsConnected())
			return true;
		dest_addr.sin_addr.s_addr = inet_addr(host_ip.c_str());
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(port);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;
		this->autoReconnect = autoReconnect;
		connectedSemph.Take(0);
		task.Notify((uint32_t)Events::Connect);
		connectedSemph.Take(timeout);
		return IsConnected();
	}

	void Disconnect()
	{
		this->autoReconnect = false;
		shutdown(sock, 0);
		close(sock);
	}
};




#endif

