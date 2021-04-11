/*
 * Lease.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_CPP_LIB_JBVPROTOCOL_LEASE_H_
#define MAIN_CPP_LIB_JBVPROTOCOL_LEASE_H_

#include "../misc/misc.h"
#include "softwareid.h"
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>


namespace JBVProtocol
{
	class Lease
	{
	public:
		uint32_t 	Id = 0;
		GUID 		Guid;
		DateTime 	ExpirationDate;
		SoftwareID 	SoftwareId = SoftwareID::Unknown;
		uint8_t 	Version[3] = {0, 0, 0};

		Lease()
		{
			Guid = GUID::HardwareGuid();
		}

		TimeSpan ExpiresIn()
		{
			return ExpirationDate - DateTime::Now();
		}

		bool IsValid()
		{
			if (ExpirationDate == DateTime::Empty()) return false;
			if (ExpirationDate < DateTime::Now()) return false;
			if (Guid.IsEmpty()) return false;
			return true;
		}

		std::string ToString()
		{
			std::string res;
			res += std::to_string(Id) + " ";
			res += Guid.ToString() + " ";
			res += std::to_string(ExpirationDate.utc) + " ";
			res += std::to_string((uint32_t)SoftwareId) + " ";
			res += std::to_string(Version[0]) + std::to_string(Version[1]) + std::to_string(Version[2]) + " ";
			return res;
		}


		uint8_t &operator [](uint32_t index)
		{
			switch(index)
			{
				case 0: return ((uint8_t*) &Id)[3];
				case 1: return ((uint8_t*) &Id)[2];
				case 2: return ((uint8_t*) &Id)[1];
				case 3: return ((uint8_t*) &Id)[0];
				case 4: return ((uint8_t*) &Guid.raw)[15];
				case 5: return ((uint8_t*) &Guid.raw)[14];
				case 6: return ((uint8_t*) &Guid.raw)[13];
				case 7: return ((uint8_t*) &Guid.raw)[12];
				case 8: return ((uint8_t*) &Guid.raw)[11];
				case 9: return ((uint8_t*) &Guid.raw)[10];
				case 10: return ((uint8_t*) &Guid.raw)[9];
				case 11: return ((uint8_t*) &Guid.raw)[8];
				case 12: return ((uint8_t*) &Guid.raw)[7];
				case 13: return ((uint8_t*) &Guid.raw)[6];
				case 14: return ((uint8_t*) &Guid.raw)[5];
				case 15: return ((uint8_t*) &Guid.raw)[4];
				case 16: return ((uint8_t*) &Guid.raw)[3];
				case 17: return ((uint8_t*) &Guid.raw)[2];
				case 18: return ((uint8_t*) &Guid.raw)[1];
				case 19: return ((uint8_t*) &Guid.raw)[0];
				case 20: return ((uint8_t*) &ExpirationDate.utc)[7];
				case 21: return ((uint8_t*) &ExpirationDate.utc)[6];
				case 22: return ((uint8_t*) &ExpirationDate.utc)[5];
				case 23: return ((uint8_t*) &ExpirationDate.utc)[4];
				case 24: return ((uint8_t*) &ExpirationDate.utc)[3];
				case 25: return ((uint8_t*) &ExpirationDate.utc)[2];
				case 26: return ((uint8_t*) &ExpirationDate.utc)[1];
				case 27: return ((uint8_t*) &ExpirationDate.utc)[0];
				case 28: return ((uint8_t*) &SoftwareId)[3];
				case 29: return ((uint8_t*) &SoftwareId)[2];
				case 30: return ((uint8_t*) &SoftwareId)[1];
				case 31: return ((uint8_t*) &SoftwareId)[0];
				case 32: return ((uint8_t*) &Version)[2];
				case 33: return ((uint8_t*) &Version)[1];
				case 34: return ((uint8_t*) &Version)[0];
				//default: return ((uint8_t*) NULL);
			}
		}

		uint32_t GetSize()
		{
			return 35;
		}
	};
}



#endif /* MAIN_CPP_LIB_JBVPROTOCOL_LEASE_H_ */
