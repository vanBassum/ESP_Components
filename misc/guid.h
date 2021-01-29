/*
 * Guid.h
 *
 *  Created on: 9 Sep 2020
 *      Author: Bas
 */

#ifndef COMPONENTS_CPP_LIB_MISC_GUID_H_
#define COMPONENTS_CPP_LIB_MISC_GUID_H_

#include <stdint.h>
#include <string>
#include <esp_system.h>

class Guid
{
public:
	uint8_t raw[16];

	Guid()
	{
		memset(raw, 0, 16);
	}


	Guid(uint8_t *data)
	{
		memcpy(raw, data, 16);
	}


	bool IsEmpty()
	{
		bool zero = true;
		for(int i=0; i<sizeof(raw); i++)
			zero &= raw[i] == 0;
		return zero;
	}

	uint32_t ToArray(uint8_t *data)
	{
		if(data != NULL)
		{
			memcpy(data, raw, 16);
		}
		return 16;
	}

	static Guid NewGuid()
	{
		Guid guid;
		esp_fill_random(guid.raw, 16);
		return guid;
	}

	std::string ToString()
	{
		char buff[64];
		sprintf(buff, "%x%x%x%x-%x%x-%x%x-%x%x-%x%x%x%x%x%x", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7], raw[8], raw[9], raw[10], raw[11], raw[12], raw[13], raw[14], raw[15]);
		return buff;
	}

};


inline bool operator==(Guid const &lhs, Guid const &rhs) { return memcmp(lhs.raw, rhs.raw, sizeof(lhs.raw)) == 0; }


#endif /* COMPONENTS_CPP_LIB_MISC_GUID_H_ */