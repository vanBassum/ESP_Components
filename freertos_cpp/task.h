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
		void *_arg;
		char const *_name;
		portBASE_TYPE _priority;
		portSHORT _stackDepth;
		Callback<void, void*> _work;

		static void TaskFunction(void* parm)
		{
			Task* t = static_cast<Task*>(parm);
			if(t->_work.IsBound())
				t->_work.Invoke(t->_arg);
			t->taskHandle = NULL;
			vTaskDelete(NULL);
		}

		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth)
		{
			ESP_LOGI("TASK", "STACK??? %d",  stackDepth);

			if(stackDepth < configMINIMAL_STACK_SIZE)
				stackDepth = configMINIMAL_STACK_SIZE;

			ESP_LOGI("TASK", "Task %s created with %d stack", name, stackDepth);

			_arg = 0;
			_name = name;
			_priority = priority;
			_stackDepth = stackDepth;
			taskHandle = NULL;
		}

	protected:
		xTaskHandle taskHandle;


	public:
		template<typename T>
		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, T *instance, void (T::*mp)(void *arg)) : Task(name, priority, stackDepth)
		{
			ESP_LOGI("TASK", "Task %s run %d stack", name, stackDepth);
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
			ESP_LOGI("TASK", "Task %s run %d stack", _name, _stackDepth);
			xTaskCreate(&TaskFunction, _name, _stackDepth, this, _priority, &taskHandle);
		}

		void Run(void *arg, int core)
		{
			_arg = arg;
			xTaskCreatePinnedToCore(&TaskFunction, _name, _stackDepth, this, _priority, &taskHandle, core);
		}

	};



	class NotifyableTask : public Task
	{

	public:

		template<typename T>
		NotifyableTask(const char *name, portBASE_TYPE priority, portSHORT stackDepth, T *instance, void (T::*mp)(void *arg)) : Task(name, priority, stackDepth, instance, mp)
		{
			ESP_LOGI("TASK", "Task %s run %d stack", name, stackDepth);
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
