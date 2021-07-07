/*
 * propertybase.h
 *
 *  Created on: Jul 5, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_MISC_PROPERTYBASE_H_
#define MAIN_ESPLIB_MISC_PROPERTYBASE_H_

#include <cJSON.h>
#include <nvs.h>
#include "../rtos/semaphore.h"


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
public:
	std::string Name;
	virtual ~PropertyBase(){}
	virtual Types GetType() = 0;
	virtual cJSON* ToJSON() = 0;
	virtual void FromJSON(cJSON* json) = 0;
	virtual void SaveToNVS(nvs_handle handle, std::string path) = 0;
	virtual void LoadFromNVS(nvs_handle handle, std::string path) = 0;
};


#endif /* MAIN_ESPLIB_MISC_PROPERTYBASE_H_ */
