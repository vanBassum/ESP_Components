/*
 * task.cpp
 *
 *  Created on: Jan 17, 2021
 *      Author: Bas
 */

#include "../freertos_cpp/task.h"

namespace FreeRTOS
{

	Task::Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth)
	{
		if(stackDepth < configMINIMAL_STACK_SIZE)
			stackDepth = configMINIMAL_STACK_SIZE;

		_arg = 0;
		_name = name;
		_priority = priority;
		_stackDepth = stackDepth;
		taskHandle = NULL;
	}

	Task::Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, void (*fp)(void *arg)) : Task(name, priority, stackDepth)
	{
		_work.Bind(fp);
	}

	template<typename T>
	Task::Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, T *instance, void (T::*mp)(void *arg)) : Task(name, priority, stackDepth)
	{
		_work.Bind(instance, mp);
	}


	Task::~Task()
	{
		if(taskHandle != NULL)
			vTaskDelete(taskHandle);
	}

	void Task::TaskFunction(void* parm)
	{
		Task* t = static_cast<Task*>(parm);
		if(t->_work.IsBound())
			t->_work.Invoke(t->_arg);
		t->taskHandle = NULL;
		vTaskDelete(NULL);
	}

	void Task::Run(void *arg)
	{
		_arg = arg;
		xTaskCreate(&TaskFunction, _name, _stackDepth, this, _priority, &taskHandle);
	}

} /* namespace FreeRTOS */
