/*
 * property.h
 *
 *  Created on: Jul 2, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_MISC_PROPERTY_H_
#define MAIN_ESPLIB_MISC_PROPERTY_H_

#include <nvs.h>
#include <cJSON.h>
#include <type_traits>
#include <vector>
#include <string>
#include "../rtos/semaphore.h"
#include "propertybase.h"
#include "propertycontainer.h"


template<typename T>
class Prop : public PropertyBase
{
	T val;
	FreeRTOS::Mutex mutex;
public:

	Prop(PropertyContainer *container, std::string name)
	{
		Name = name;
		container->AddProperty(this);
	}

	Prop(PropertyContainer *container, std::string name, T defVal)
	{
		Name = name;
		val = defVal;
		container->AddProperty(this);
	}

	T Get()
	{	//TODO protect this???
		return val;
	}

	void Set(T nVal)
	{
		mutex.Take();
		val = nVal;
		mutex.Give();
	}

	Types GetType() override
	{
		if(std::is_same<T, int8_t>::value) return Types::INT8_t;
		if(std::is_same<T, int16_t>::value) return Types::INT16_t;
		if(std::is_same<T, int32_t>::value) return Types::INT32_t;
		if(std::is_same<T, int64_t>::value) return Types::INT64_t;
		if(std::is_same<T, uint8_t>::value) return Types::UINT8_t;
		if(std::is_same<T, uint16_t>::value) return Types::UINT16_t;
		if(std::is_same<T, uint32_t>::value) return Types::UINT32_t;
		if(std::is_same<T, uint64_t>::value) return Types::UINT64_t;
		if(std::is_same<T, float>::value) return Types::FLOAT;
		if(std::is_same<T, double>::value) return Types::DOUBLE;
		if(std::is_same<T, std::string>::value) return Types::STRING;
		if(std::is_convertible<T, PropertyContainer>::value) return Types::PROPCONT;

		ESP_LOGE("PropertyBase", "Unknown type");
		return Types::UNKNOWN;
	}



	void SaveToNVS(nvs_handle handle, std::string path) override
	{
		std::string pn = path + Name;
		Types type = GetType();
		mutex.Take();
		switch(type)
		{
		case Types::UINT8_t:	nvs_set_u8(handle, pn.c_str(),  (uint8_t)  val);   break;
		case Types::UINT16_t:	nvs_set_u16(handle, pn.c_str(), (uint16_t) val);  break;
		case Types::UINT32_t:	nvs_set_u32(handle, pn.c_str(), (uint32_t) val);  break;
		case Types::UINT64_t:	nvs_set_u64(handle, pn.c_str(), (uint64_t) val);  break;
		case Types::INT8_t:		nvs_set_i8(handle, pn.c_str(),  (int8_t)   val);   break;
		case Types::INT16_t:	nvs_set_i16(handle, pn.c_str(), (int16_t)  val);  break;
		case Types::INT32_t:	nvs_set_i32(handle, pn.c_str(), (int32_t)  val);  break;
		case Types::INT64_t:	nvs_set_i64(handle, pn.c_str(), (int64_t)  val);  break;
		case Types::FLOAT:		nvs_set_u32(handle, pn.c_str(), (uint32_t) val);  break;
		case Types::DOUBLE:		nvs_set_u64(handle, pn.c_str(), (uint64_t) val);  break;
		case Types::STRING:		nvs_set_str(handle, pn.c_str(), ((std::string*)&val)->c_str());  break;
		case Types::PROPCONT:
		{
			PropertyContainer* pc = (PropertyContainer*)&val;
			pc->SaveToNVS(handle, pn + "/");
		}
			break;
		default:
			break;
		}
		mutex.Give();
	}

	void LoadFromNVS(nvs_handle handle, std::string path) override
	{
		std::string pn = path + Name;
		Types type = GetType();
		mutex.Take();
		switch(type)
		{
		case Types::UINT8_t:	nvs_get_u8(handle, pn.c_str(),  (uint8_t*)   &val);   break;
		case Types::UINT16_t:	nvs_get_u16(handle, pn.c_str(), (uint16_t*)  &val);  break;
		case Types::UINT32_t:	nvs_get_u32(handle, pn.c_str(), (uint32_t*)  &val);  break;
		case Types::UINT64_t:	nvs_get_u64(handle, pn.c_str(), (uint64_t*)  &val);  break;
		case Types::INT8_t:		nvs_get_i8(handle, pn.c_str(),  (int8_t*)    &val);   break;
		case Types::INT16_t:	nvs_get_i16(handle, pn.c_str(), (int16_t*)   &val);  break;
		case Types::INT32_t:	nvs_get_i32(handle, pn.c_str(), (int32_t*)   &val);  break;
		case Types::INT64_t:	nvs_get_i64(handle, pn.c_str(), (int64_t*)   &val);  break;
		case Types::FLOAT:		nvs_get_u32(handle, pn.c_str(), (uint32_t*)  &val);  break;
		case Types::DOUBLE:		nvs_get_u64(handle, pn.c_str(), (uint64_t*)  &val);  break;
		case Types::STRING:
		{
			size_t required_size;
			nvs_get_str(handle, pn.c_str(), NULL, &required_size);
			char* server_name = (char *)malloc(required_size);
			nvs_get_str(handle, pn.c_str(), server_name, &required_size);
			*((std::string*)&val) = std::string(server_name);

		}
			break;
		case Types::PROPCONT:
		{
			PropertyContainer* pc = (PropertyContainer*)&val;
			pc->LoadFromNVS(handle, pn + "/");
		}
			break;
		default:
			break;
		}
		mutex.Give();
	}




	cJSON* ToJSON() override
	{
		Types type = GetType();
		cJSON* property = NULL;
		mutex.Take();
		switch(type)
		{
		case Types::UINT8_t:	property = cJSON_CreateNumber((uint8_t) val);  break;
		case Types::UINT16_t:	property = cJSON_CreateNumber((uint16_t)val);  break;
		case Types::UINT32_t:	property = cJSON_CreateNumber((uint32_t)val);  break;
		case Types::UINT64_t:	property = cJSON_CreateNumber((uint64_t)val);  break;
		case Types::INT8_t:		property = cJSON_CreateNumber((int8_t)  val);  break;
		case Types::INT16_t:	property = cJSON_CreateNumber((int16_t) val);  break;
		case Types::INT32_t:	property = cJSON_CreateNumber((int32_t) val);  break;
		case Types::INT64_t:	property = cJSON_CreateNumber((int64_t) val);  break;
		case Types::FLOAT:		property = cJSON_CreateNumber((float)   val);  break;
		case Types::DOUBLE:		property = cJSON_CreateNumber((double)  val);  break;
		case Types::STRING:		property = cJSON_CreateString(((std::string*)&val)->c_str());  break;
		case Types::PROPCONT:
		{
			PropertyContainer* pc = (PropertyContainer*)&val;
			property = pc->ToJSON();
		}
			break;
		default:
			break;
		}
		mutex.Give();
		return property;
	}


	void FromJSON(cJSON* json) override
	{
		Types type = GetType();
		mutex.Take();
		switch(type)
		{
		case Types::UINT8_t:	*((uint8_t*)&val)	= json->valueint; break;
		case Types::UINT16_t:	*((uint16_t*)&val)	= json->valueint; break;
		case Types::UINT32_t:	*((uint32_t*)&val)	= json->valueint; break;
		case Types::UINT64_t:	*((uint64_t*)&val)	= json->valueint; break;
		case Types::INT8_t:		*((int8_t*)&val)	= json->valueint; break;
		case Types::INT16_t:	*((int16_t*)&val)	= json->valueint; break;
		case Types::INT32_t:	*((int32_t*)&val)	= json->valueint; break;
		case Types::INT64_t:	*((int64_t*)&val)	= json->valueint; break;
		case Types::FLOAT:		*((float*)&val)		= json->valuedouble; break;
		case Types::DOUBLE:		*((double*)&val)	= json->valuedouble; break;
		case Types::STRING:		*((std::string*)&val) 	= std::string(json->valuestring); break;
		case Types::PROPCONT:
		{
			PropertyContainer* pc = (PropertyContainer*)&val;
			pc->FromJSON(json);
		}
			break;
		default:
			break;
		}
		mutex.Give();
	}





};


#include "propertycontainer.h"



#endif /* MAIN_ESPLIB_MISC_PROPERTY_H_ */
