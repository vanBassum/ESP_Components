/*
 * DS18B20.h
 *
 *  Created on: Jan 16, 2021
 *      Author: Bas
 */

#ifndef MAIN_ONEWIRE_DS18B20_H_
#define MAIN_ONEWIRE_DS18B20_H_

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bus.h"
#include "device.h"


namespace OneWire
{
	class DS18B20 : public Device
	{

	public:
		DS18B20(Bus* owb, OneWireBus_ROMCode owCode) : Device(owb, owCode)
		{
		}

		float GetTemp()
		{
			uint8_t data[2];
			float val = -999;
			bus->SelectDevice(this);
			bus->WriteByte(0x44);		//Convert T
			while(bus->ReadByte() == 0);
			bus->SelectDevice(this);
			bus->WriteByte(0xBE);		// Read scratchpad
			bus->ReadBytes(data, 2);
			val = (float)(data[0]+(data[1]*256))/16;

			return val;
		}


		virtual Devices GetType()
		{
			return Devices::DS18B20;
		}
	};


}



#endif /* MAIN_ONEWIRE_DS18B20_H_ */
