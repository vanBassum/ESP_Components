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

	template<typename TaskType>
	class TaskBase
	{
		void* _arg;
		char const* _name;
		portBASE_TYPE _priority;
		portSHORT _stackDepth;
		Callback<void, TaskType*, void*> _work;

		static void TaskFunction(void* parm)
		{
			TaskType* t = static_cast<TaskType*>(parm);
			if (t->_work.IsBound())
				t->_work.Invoke(t, t->_arg);
			t->taskHandle = NULL;
			vTaskDelete(NULL);
		}
	protected:
		xTaskHandle taskHandle;


	public:
		virtual ~TaskBase()
		{
			if (taskHandle != NULL)
				vTaskDelete(taskHandle);
		}

		template<typename T>
		void SetCallback(T* instance, void (T::* mp)(TaskType*, void* arg))
		{
			_work.Bind(instance, mp);
		}

		void SetCallback(void (*fp)(TaskType*, void* arg))
		{
			_work.Bind(fp);
		}

		virtual void Run(const char* name, portBASE_TYPE priority, portSHORT stackDepth, void* arg = NULL)
		{
			if (stackDepth < configMINIMAL_STACK_SIZE)
				stackDepth = configMINIMAL_STACK_SIZE;

			_arg = arg;
			_name = name;
			_priority = priority;
			_stackDepth = stackDepth;
			xTaskCreate(&TaskFunction, _name, _stackDepth, this, _priority, &taskHandle);
		}
	};



	class Task : public TaskBase<Task>
	{
		
	};


	class NotifyableTask : public TaskBase<NotifyableTask>
	{

	public:
		bool WaitForNotification(uint32_t* pulNotificationValue, int timeout)
		{
			return xTaskNotifyWait(0x0000, 0xFFFF, pulNotificationValue, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

		void Notify(uint32_t bits)
		{
			xTaskNotify(taskHandle, bits, eSetBits);
		}

		void NotifyFromISR(uint32_t bits)
		{
			xTaskNotifyFromISR(taskHandle, bits, eSetBits, NULL);
		}

		void NotifyFromISR(uint32_t bits, BaseType_t* pxHigherPriorityTaskWoken)
		{
			xTaskNotifyFromISR(taskHandle, bits, eSetBits, pxHigherPriorityTaskWoken);
		}
	};




} /* namespace FreeRTOS */

#endif /* COMPONENTS_FREERTOS_CPP_TASK_H_ */
