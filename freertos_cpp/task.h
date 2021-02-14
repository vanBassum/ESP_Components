/*
 * task.h
 *
 *  Created on: Jan 17, 2021
 *      Author: Bas
 */

#ifndef COMPONENTS_FREERTOS_CPP_TASK_H_
#define COMPONENTS_FREERTOS_CPP_TASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../misc/misc.h"


namespace FreeRTOS
{


	class Task
	{
		void *_arg = 0;
		char const *_name = 0;
		portBASE_TYPE _priority = 99;
		portSHORT _stackDepth = 0;
		Callback<void, void*> _work;

		static void TaskFunction(void* parm)
		{
			//ESP_LOGI("TASK", "TaskFunction");
			Task* t = static_cast<Task*>(parm);
			if(t->_work.IsBound())
				t->_work.Invoke(t->_arg);
			t->taskHandle = NULL;
			vTaskDelete(NULL);
		}

		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth)
		{
			if(stackDepth < configMINIMAL_STACK_SIZE)
				stackDepth = configMINIMAL_STACK_SIZE;

			_arg = 0;
			_name = name;
			_priority = priority;
			_stackDepth = stackDepth;
			taskHandle = NULL;
		}

	protected:
		xTaskHandle taskHandle = 0;


	public:
		template<typename T>
		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, T *instance, void (T::*mp)(void *arg)) : Task(name, priority, stackDepth)
		{
			_work.Bind(instance, mp);
		}

		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, void (*fp)(void *arg)) : Task(name, priority, stackDepth)
		{
			_work.Bind(fp);
		}


		virtual ~Task()
		{
			if(taskHandle != NULL)
				vTaskDelete(taskHandle);
		}
		void Run(void *arg)
		{
			_arg = arg;
			xTaskCreate(&TaskFunction, _name, _stackDepth, this, _priority, &taskHandle);
		}


		void Run(void *arg, const BaseType_t _xCoreID)
		{
			_arg = arg;
			xTaskCreatePinnedToCore(&TaskFunction, _name, _stackDepth, this, _priority, &taskHandle, _xCoreID);
		}


	};


	class NotifyableTask : public Task
	{

	public:

		template<typename R>
		NotifyableTask(const char *name, portBASE_TYPE priority, portSHORT stackDepth, R *instance, void (R::*mp)(void *arg)) : Task(name, priority, stackDepth, instance, mp)
		{
		}

		NotifyableTask(const char *name, portBASE_TYPE priority, portSHORT stackDepth, void (*fp)(void *arg)) : Task(name, priority, stackDepth, fp)
		{
		}

		bool GetNotifications(uint32_t *pulNotificationValue, int timeout)
		{
			return xTaskNotifyWait( 0x0000, 0xFFFF, pulNotificationValue, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

		void Notify(uint32_t bits)
		{
			xTaskNotify(taskHandle, bits, eSetBits);
		}

		void NotifyFromISR(uint32_t bits)
		{
			xTaskNotifyFromISR(taskHandle, bits, eSetBits, NULL);
		}

		void NotifyFromISR(uint32_t bits, BaseType_t *pxHigherPriorityTaskWoken)
		{
			xTaskNotifyFromISR(taskHandle, bits, eSetBits, pxHigherPriorityTaskWoken);
		}
	};








} /* namespace FreeRTOS */

#endif /* COMPONENTS_FREERTOS_CPP_TASK_H_ */
