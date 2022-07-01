#pragma once

#include <stdint.h>
#include <vector>
#include "esp_log.h"
#include "datetime.h"
#include "callback.h"
#include "../rtos/task.h"


class StatemachineInfo
{
public:
	uint32_t actualState = 0;
	uint32_t nextState = 0;
	uint32_t previousState = 0;
	uint32_t notifications = 0;
	
	///How long to wait before another iteration of this state occurs.
	///Is interrupted by notifications.
	uint32_t delay = 0;			
	
	///True for the first cycle after a state change.
	bool onEntry;
	
	///Timestamp of last state change
	DateTime lastStateChange;
};


class StateItem
{
public:
	uint32_t id = 0;
	Callback<void, StatemachineInfo*> workCallback;
	char name[32];
};

class StateMachine
{
	TraceDebugger* tracer = NULL;
	int traceNo = 0;
	std::vector<StateItem*> states;
	FreeRTOS::NotifyableTask task;
	
	StateItem* GetStateItem(std::vector<StateItem*> states, uint32_t id)
	{
		for (int i = 0; i < states.size(); i++)
		{
			if (id == states[i]->id)
				return states[i];
		}
		return NULL;
	}
	
	void Work(FreeRTOS::NotifyableTask* task, void* args)
	{
		StatemachineInfo statemachineInfo;
		statemachineInfo.lastStateChange = DateTime::Now();
		statemachineInfo.onEntry = true;
		statemachineInfo.delay = 1000;
		
		
		StateItem* stateItem = NULL;
		stateItem = GetStateItem(states, 0);
		
		while (true)
		{
			
			uint32_t notifications = 0;
			task->WaitForNotification(&notifications, statemachineInfo.delay);
			
			
			if (stateItem != NULL)
			{
				if (stateItem->workCallback.IsBound())
				{
					stateItem->workCallback.Invoke(&statemachineInfo);
				}
			}

			
			if (statemachineInfo.actualState != statemachineInfo.nextState)
			{
				statemachineInfo.previousState = statemachineInfo.actualState;
				statemachineInfo.actualState = statemachineInfo.nextState;
				statemachineInfo.lastStateChange = DateTime::Now();
				statemachineInfo.onEntry = true;
				
				if ((stateItem = GetStateItem(states, statemachineInfo.actualState)) != NULL)
				{
					ESP_LOGI("StateMachine", "State changed to %d, %s", stateItem->id, stateItem->name);
					if (tracer != NULL)
					{
						tracer->SendMeasurementTest(statemachineInfo.lastStateChange.utc, statemachineInfo.actualState, traceNo);
					}
				}
				else
				{
					ESP_LOGE("StateMachine", "State with id %d not found!!!", statemachineInfo.actualState);
				}
			}
			else
			{
				statemachineInfo.onEntry = false;
			}
		}
	}
public:
	StateMachine()
	{
		task.SetCallback(this, &StateMachine::Work);
	}
	
	~StateMachine()
	{
		for (int i = 0; i < states.size(); i++)
			delete states[i];
	}
	
	template<typename T>
	void AddState(uint32_t id, const char* name, T* instance, void(T::*method)(StatemachineInfo*))
	{
		StateItem* item = new StateItem();
		item->id = id;
		item->workCallback.Bind(instance, method);
		
		int tLen = sizeof(item->name);
		int len = strlen(name);
		if (tLen < len)
			len = tLen;
		memcpy(item->name, name, len);
		states.push_back(item);
	}

	void Start(const char* name, portBASE_TYPE priority, portSHORT stackDepth, void* arg = NULL)
	{
		task.Run(name, priority, stackDepth, arg);
	}
	
	void AttachTracer(TraceDebugger* tracer, int traceNo)
	{
		this->tracer = tracer;
		this->traceNo = traceNo;
	}

};


