/*
 * property.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: Bas
 */

#include <esp_system.h>
#include <esp_log.h>
#include "property.h"
#include <propertycontainer.h>

	cJSON* PropertyBase::ToJSON()
	{
		Types type = GetType();
		cJSON* property = NULL;
		mutex.Take();
		switch(type)
		{
		case Types::UINT8_t:	property = cJSON_CreateNumber((double)*((uint8_t*)GetValPtr()));  break;
		case Types::UINT16_t:	property = cJSON_CreateNumber((double)*((uint16_t*)GetValPtr()));  break;
		case Types::UINT32_t:	property = cJSON_CreateNumber((double)*((uint32_t*)GetValPtr()));  break;
		case Types::UINT64_t:	property = cJSON_CreateNumber((double)*((uint64_t*)GetValPtr()));  break;
		case Types::INT8_t:		property = cJSON_CreateNumber((double)*((int8_t*)  GetValPtr()));  break;
		case Types::INT16_t:	property = cJSON_CreateNumber((double)*((int16_t*) GetValPtr()));  break;
		case Types::INT32_t:	property = cJSON_CreateNumber((double)*((int32_t*) GetValPtr()));  break;
		case Types::INT64_t:	property = cJSON_CreateNumber((double)*((int64_t*) GetValPtr()));  break;
		case Types::FLOAT:		property = cJSON_CreateNumber((double)*((float*)   GetValPtr()));  break;
		case Types::DOUBLE:		property = cJSON_CreateNumber((double)*((double*)  GetValPtr()));  break;
		case Types::STRING:		property = cJSON_CreateString(((std::string*)GetValPtr())->c_str());  break;
		case Types::PROPCONT:
		{
			PropertyContainer* pc = (PropertyContainer*)GetValPtr();
			property = pc->ToJSON();
		}
			break;
		default:
			break;
		}
		mutex.Give();
		return property;
	}

	void PropertyBase::FromJSON(cJSON* json)
	{
		Types type = GetType();
		void* valPtr = GetValPtr();
		mutex.Take();
		switch(type)
		{
		case Types::UINT8_t:	(*(uint8_t*)valPtr)  	= json->valueint; break;
		case Types::UINT16_t:	(*(uint16_t*)valPtr) 	= json->valueint; break;
		case Types::UINT32_t:	(*(uint32_t*)valPtr) 	= json->valueint; break;
		case Types::UINT64_t:	(*(uint64_t*)valPtr)	= json->valueint; break;
		case Types::INT8_t:		(*(int8_t*)valPtr)  	= json->valueint; break;
		case Types::INT16_t:	(*(int16_t*)valPtr) 	= json->valueint; break;
		case Types::INT32_t:	(*(int32_t*)valPtr) 	= json->valueint; break;
		case Types::INT64_t:	(*(int64_t*)valPtr) 	= json->valueint; break;
		case Types::FLOAT:		(*(float*)valPtr) 		= json->valuedouble; break;
		case Types::DOUBLE:		(*(double*)valPtr) 		= json->valuedouble; break;
		case Types::STRING:		(*(std::string*)valPtr) = std::string(json->valuestring); break;
		case Types::PROPCONT:
		{
			PropertyContainer* pc = (PropertyContainer*)valPtr;
			pc->FromJSON(json);
		}
			break;
		default:
			break;
		}
		mutex.Give();
	}






