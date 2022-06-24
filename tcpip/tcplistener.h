/*
 * tcplistener.h
 *
 *  Created on: Mar 18, 2021
 *      Author: Bas
 */

#ifndef MAIN_COMPONENTS_TCPIP_TCPLISTENER_H_
#define MAIN_COMPONENTS_TCPIP_TCPLISTENER_H_

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "tcpsocket.h"
#include "../misc/callback.h"
#include "../rtos/task.h"

class TCPListener
{
public:
	Callback<void, TCPSocket*> OnSocketAccepted;


private:
	int addr_family = AF_INET;
	int ip_protocol = IPPROTO_IP;
	int sock = 0;
	struct sockaddr_in dest_addr;
	int port = 31600;
	int keepAlive = 1;
	int keepIdle = 5;
	int keepInterval = 60;
	int keepCount = 3;

	void Work(FreeRTOS::Task* task, void *arg)
	{
		if (addr_family == AF_INET)
		{
			struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
			dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
			dest_addr_ip4->sin_family = AF_INET;
			dest_addr_ip4->sin_port = htons(port);
			ip_protocol = IPPROTO_IP;
		}


		sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if(sock >= 0)
		{
			int opt = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

			if(err == 0)
			{
				ESP_LOGI("TCPListener", "Socket bound, port %d", port);
				err = listen(sock, 1);

				if(err == 0)
				{
					while(true)
					{
						ESP_LOGI("TCPListener", "Socket listening");
						struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
						socklen_t addr_len = sizeof(source_addr);
						int acceptedsock = accept(sock, (struct sockaddr *)&source_addr, &addr_len);

						ESP_LOGI("TCPListener", "Socket accepted");

						if(acceptedsock > 0)
						{
							// Set tcp keepalive option
							setsockopt(acceptedsock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
							setsockopt(acceptedsock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
							setsockopt(acceptedsock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
							setsockopt(acceptedsock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

							TCPSocket* socket = new TCPSocket(acceptedsock);
							OnSocketAccepted.Invoke(socket);
						}
						else
						{
							ESP_LOGE("TCPListener", "Unable to accept connection: errno %d", errno);
							close(sock);
							vTaskDelay(1000 / portTICK_PERIOD_MS);
						}
					}
				}
				else
				{
					ESP_LOGE("TCPListener", "Error occurred during listen: errno %d", errno);
					close(sock);
					vTaskDelay(1000 / portTICK_PERIOD_MS);
				}

			}
			else
			{
				ESP_LOGE("TCPListener", "Socket unable to bind: errno %d", errno);
				ESP_LOGE("TCPListener", "IPPROTO: %d", addr_family);
				close(sock);
				vTaskDelay(1000 / portTICK_PERIOD_MS);
			}
		}
		else
		{
			ESP_LOGE("TCPListener", "Unable to create socket: errno %d, %s", errno, strerror(errno));
			shutdown(sock, 0);
			close(sock);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}


	}


	FreeRTOS::Task task;

public:
	TCPListener()
	{
		task.SetCallback(this, &TCPListener::Work);
	}
	
	void StartListener()
	{
		task.Run("TCPListener", 7, 1024*3);
	}
};




#endif /* MAIN_COMPONENTS_TCPIP_TCPLISTENER_H_ */
