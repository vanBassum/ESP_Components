/*
 * property.h
 *
 *  Created on: Jul 2, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_MISC_PROPERTY_H_
#define MAIN_ESPLIB_MISC_PROPERTY_H_

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

protected:

	void* GetValPtr() override
	{
		return &val;
	}

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

};


#include "propertycontainer.h"



#endif /* MAIN_ESPLIB_MISC_PROPERTY_H_ */
