/*
 * propertybase.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_MISC_PROPERTYBASE_H_
#define MAIN_ESPLIB_MISC_PROPERTYBASE_H_

#include <cJSON.h>
#include "../rtos/semaphore.h"
#include "nvs.h"

enum class Types
{
	UNKNOWN,
	UINT8_t,
	UINT16_t,
	UINT32_t,
	UINT64_t,
	INT8_t,
	INT16_t,
	INT32_t,
	INT64_t,
	FLOAT,
	DOUBLE,
	STRING,
	PROPCONT,
};


class PropertyBase
{
protected:

	FreeRTOS::Mutex mutex;
public:
	virtual void* GetValPtr() = 0;
	std::string Name;
	virtual ~PropertyBase(){}
	virtual Types GetType() = 0;
	cJSON* ToJSON();
	void FromJSON(cJSON* json);

	void SaveToNVS(nvs_handle handle, std::string path);
	void LoadFromNVS(nvs_handle handle, std::string path);
};


#endif /* MAIN_ESPLIB_MISC_PROPERTYBASE_H_ */
