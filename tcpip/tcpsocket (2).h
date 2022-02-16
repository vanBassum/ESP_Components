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


class TCPSocket
{
	bool connected = false;
	FreeRTOS::Task task;
	Callback<void, TCPSocket*, size_t> OnDataReceived;
	void*  rxBuffer = NULL;
	size_t rxBufferSize = 0;
	int    sock = 0;

	void ReceiveHandler(FreeRTOS::Task* task, void* args)
	{
		if (rxBuffer != NULL 
			&& rxBufferSize > 0 
			&& OnDataReceived.IsBound() 
			&& sock != 0)
		{
			size_t len = recv(sock, rxBuffer, rxBufferSize, 0);
			if (len < 0) 
			{
				ESP_LOGE(TAG, "recv failed: errno %d", errno);
				connected = false;
			}
			else
			{
				OnDataReceived.Invoke(this, len);
			}
		}
	}

public:
	TCPSocket()
	{
		task.SetCallback(this, &TCPSocket::ReceiveHandler);
		
	}

	~TCPSocket()
	{
		shutdown(sock, 0);
		close(sock);
	}

	bool Connect(std::string host_ip, int port, bool autoReconnect, int timeout)
	{
		int addr_family = 0;
		int ip_protocol = 0;
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = inet_addr(host_ip.c_str());
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(port);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;


		//Create socket
		int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (sock < 0) {
			ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
			return false;
		}

		//Connect
		int err = connect(sock, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr_in6));
		if (err != 0) {
			ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
			return false;
		}

		connected = true;
	}

	bool IsConnected()
	{
		return connected;
	}

	void Disconnect()
	{
		connected = false;
		shutdown(sock, 0);
		close(sock);
	}

	void Send(uint8_t* data, uint32_t length)
	{
		int to_write = length;
		while (to_write > 0)
		{
			int written = send(sock, data + (length - to_write), to_write, 0);
			if (written < 0)
			{
				ESP_LOGE("TCPConnection.h", "Error occurred during sending: errno %d, %s", errno, strerror(errno));
				connected = false;
				return;
			}
			to_write -= written;
		}
	}

	template<typename T>
	bool StartReceive(void* receiveBuffer, size_t receiveBufferSize, T* instance, R(T::* onReceiveCallback)(TCPSocket* sender, size_t rxLen))
	{
		if (rxBuffer == NULL && receiveBuffer != NULL)
		{
			rxBuffer = receiveBuffer;
			rxBufferSize = receiveBufferSize;
			OnDataReceived.Bind(instance, onReceiveCallback);
			task.Run("TCPSocket", 6, 1024 * 2);
		}
	}
};




#endif

