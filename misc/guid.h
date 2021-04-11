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
#include <string.h>

class GUID
{
public:
	uint8_t raw[16];

	GUID()
	{
		memset(raw, 0, 16);
	}


	GUID(uint8_t *data)
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

	//Creates an GUID based on the mac address.
	static GUID HardwareGuid()
	{
		GUID guid;
		memset(guid.raw, 0, 16);
		esp_efuse_mac_get_default(guid.raw);
		return guid;
	}

	static GUID NewGuid()
	{
		GUID guid;
		esp_fill_random(guid.raw, 16);
		return guid;
	}

	static int char2int(char input)
	{
	  if(input >= '0' && input <= '9')
	    return input - '0';
	  if(input >= 'A' && input <= 'F')
	    return input - 'A' + 10;
	  if(input >= 'a' && input <= 'f')
	    return input - 'a' + 10;
	  return 0;
	}

	// This function assumes src to be a zero terminated sanitized string with
	// an even number of [0-9a-f] characters, and target to be sufficiently large
	static void hex2bin(const char* src, uint8_t* target)
	{
	  while(*src && src[1])
	  {
	    *(target++) = char2int(*src)*16 + char2int(src[1]);
	    src += 2;
	  }
	}

	static GUID FromString(std::string str)
	{
		GUID guid;
		hex2bin(str.c_str(), guid.raw);
		return guid;
	}

	std::string ToString()
	{
		char buff[64];
		sprintf(buff, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7], raw[8], raw[9], raw[10], raw[11], raw[12], raw[13], raw[14], raw[15]);
		return buff;
	}

};


inline bool operator==(GUID const &lhs, GUID const &rhs) { return memcmp(lhs.raw, rhs.raw, sizeof(lhs.raw)) == 0; }


#endif /* COMPONENTS_CPP_LIB_MISC_GUID_H_ */
