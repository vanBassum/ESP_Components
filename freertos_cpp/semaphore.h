/*
 * semaphore.h
 *
 *  Created on: Jan 17, 2021
 *      Author: Bas
 */

#ifndef COMPONENTS_FREERTOS_CPP_SEMAPHORE_H_
#define COMPONENTS_FREERTOS_CPP_SEMAPHORE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace FreeRTOS
{

	class RecursiveMutex
	{
		SemaphoreHandle_t handle = NULL;
	public:
		RecursiveMutex()
		{
			handle = xSemaphoreCreateRecursiveMutex();
		}

		bool Take()
		{
			bool oke = xSemaphoreTakeRecursive(handle, portMAX_DELAY ) == pdTRUE;
			return oke;
		}

		bool Take(int timeout)
		{
			return xSemaphoreTakeRecursive(handle, timeout / portTICK_PERIOD_MS) == pdTRUE;
		}

		bool Give()
		{
			return xSemaphoreGiveRecursive(handle) == pdTRUE;
		}

	};
}

#endif /* COMPONENTS_FREERTOS_CPP_SEMAPHORE_H_ */
