/*
 * queue.h
 *
 *  Created on: Jan 29, 2021
 *      Author: Bas
 */

#ifndef COMPONENTS_FREERTOS_CPP_QUEUE_H_
#define COMPONENTS_FREERTOS_CPP_QUEUE_H_



#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "queueset.h"


namespace FreeRTOS
{


	template <typename T>
	class Queue : public QueueSetAddable
	{
		QueueHandle_t  handle;
		int _size;
	public:

		Queue(int size)
		{
			_size = size;
			handle = xQueueCreate( size, sizeof( T ) );
		}

		~Queue()
		{
			vQueueDelete( handle );
		}

		int GetSize() {return _size;}
		QueueSetMemberHandle_t GetHandle() {return (QueueSetMemberHandle_t)handle;}

		bool Enqueue(T *item, int timeout)
		{
			return xQueueSend( handle, ( void * ) item, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

		bool EnqueueBack(T *item, int timeout)
		{
			return xQueueSendToBack( handle, ( void * ) item, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

		bool EnqueueFromISR(T *item, BaseType_t *pxHigherPriorityTaskWoken)
		{
			return xQueueSendFromISR( handle, ( void * ) item, pxHigherPriorityTaskWoken = NULL) == pdPASS;
		}

		bool EnqueueBackFromISR(T *item, BaseType_t *pxHigherPriorityTaskWoken)
		{
			return xQueueSendToBackFromISR( handle, ( void * ) item, pxHigherPriorityTaskWoken = NULL) == pdPASS;
		}

		bool Dequeue(T *item, int timeout)
		{
			return xQueueReceive( handle, ( void * ) item, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

		bool DequeueFromISR(T *item, BaseType_t *pxHigherPriorityTaskWoken)
		{
			return xQueueReceiveFromISR( handle, ( void * ) item, pxHigherPriorityTaskWoken) == pdPASS;
		}

	};
}


#endif /* COMPONENTS_FREERTOS_CPP_QUEUE_H_ */
