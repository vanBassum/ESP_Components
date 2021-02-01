/*
 * Lease.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_CPP_LIB_JBVPROTOCOL_LEASE_H_
#define MAIN_CPP_LIB_JBVPROTOCOL_LEASE_H_

#include "../misc/misc.h"
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

class Lease
{
public:
	uint16_t ID = 0;
	Guid Key;
	DateTime Expire = DateTime::Empty();


	TimeSpan ExpiresIn()
	{
		return Expire - DateTime::Now();
	}

	bool IsValid()
	{
		if (Expire == DateTime::Empty()) return false;
		if (Expire < DateTime::Now()) return false;
		if (Key.IsEmpty()) return false;
		return true;
	}

	std::string ToString()
	{
		std::string id, key, exp;
		std::stringstream val;
		StringConverter::ToString(&id, &ID);
		val << id;
		val << ", ";
		val << Key.ToString();
		val << ", ";
		val << Expire.ToString();
		return val.str();
	}

	Lease()
	{
		Key = Guid::HardwareGuid();
	}

	Lease(uint8_t *data)
	{
		ID = *(uint16_t*) &data[0];
		Key = Guid((uint8_t*) &data[2]);
		Expire = DateTime::FromTimeT(*(time_t*) &data[18]);
	}

	uint32_t ToArray(uint8_t *data)
	{
		if(data != NULL)
		{
			memcpy(&data[0], (uint8_t*) &ID, 2);
			Key.ToArray(&data[2]);
			memcpy(&data[18],(uint8_t*) Expire.MKTime(), 8);
		}
		return 26;
	}

};




#endif /* MAIN_CPP_LIB_JBVPROTOCOL_LEASE_H_ */
