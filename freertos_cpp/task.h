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
#include "../misc/callback.h"

//#include "callback.h"

namespace FreeRTOS {


	class Task
	{
		void *_arg;
		char const *_name;
		portBASE_TYPE _priority;
		portSHORT _stackDepth;
		Callback<void, void*> _work;

		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth);
		static void TaskFunction(void* parm);

	protected:
		xTaskHandle taskHandle;

	public:
		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, void (*fp)(void *arg));
		template<typename T>
		Task(const char *name, portBASE_TYPE priority, portSHORT stackDepth, T *instance, void (T::*mp)(void *arg));
		virtual ~Task();
		void Run(void *arg);

	};

} /* namespace FreeRTOS */

#endif /* COMPONENTS_FREERTOS_CPP_TASK_H_ */
