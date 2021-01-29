/*
 * Property.h
 *
 *  Created on: Jan 26, 2021
 *      Author: Bas
 */

#ifndef MAIN_SMARTHOME_LIB_MODBUS_PROPERTY_H_
#define MAIN_SMARTHOME_LIB_MODBUS_PROPERTY_H_

#include <vector>
#include <stdint.h>

#include "../modbus/misc.h"
#include "../modbus/modbus.h"
#include "../freertos_cpp/freertos.h"

namespace Modbus
{
	class Property
	{
		//@TODO: Hardcoded unit address!!!
		uint8_t unitAddress = 1;
		uint16_t address = 0;
		uint16_t value = 0;
		FreeRTOS::RecursiveMutex mutex;
		bool changed = false;

	public:
		//Is called whenever the value was changed by the host device.
		Event<Property*> OnChange;

		//Call this every once in a while to sync registers over modbus.
		Exception Sync(Modbus *modbus, int timeout)
		{
			Exception result = Exception::UnknownException;

			if(mutex.Take(timeout))
			{
				if(changed)
				{
					//Value was changed by Set method, so write this to the device.
					result = modbus->WriteSingleRegister(unitAddress, address, value, timeout);
					if(result == Exception::NoError)
						changed = false;
				}
				else
				{
					//Value wasnt changed by set method, so read from the device to see if the value was changed.
					uint16_t newVal = 0;
					result = modbus->ReadHoldingRegisters(unitAddress, address, 1, &newVal, 2, timeout);
					if(result == Exception::NoError)
					{
						if (value != newVal)
						{
							value = newVal;
							OnChange(this);
						}
					}
				}
				mutex.Give();
			}
			else
				result = Exception::MutexFailed;


			if(result != Exception::NoError)
				ESP_LOGE("Property", "Error while syncing %d", (int)result);

			return result;
		}

		void Set(uint16_t val)
		{
			if(mutex.Take(100))
			{
				if(value != val)
				{
					value = val;
					changed = true;
				}
				mutex.Give();
			}
		}

		uint16_t Get()
		{
			uint16_t val = 0;
			if(mutex.Take(100))
			{
				val = value;
				mutex.Give();
			}
			return val;
		}

		uint16_t GetAddress()
		{
			return address;
		}

		void SetAddress(uint16_t addr)
		{
			address = addr;
		}

		void AddAddress(uint16_t offset)
		{
			address += offset;
		}


		Property(uint16_t address, std::vector<Property*> *propList = NULL)
		{
			this->address = address;
			if(propList != NULL)
				propList->push_back(this);
		}
	};

}

#endif /* MAIN_SMARTHOME_LIB_MODBUS_PROPERTY_H_ */
