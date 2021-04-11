/*
 * DS18B20.h
 *
 *  Created on: Jan 16, 2021
 *      Author: Bas
 */

#ifndef MAIN_ONEWIRE_DS18B20_H_
#define MAIN_ONEWIRE_DS18B20_H_

#include "../onewire/bus.h"
#include "../onewire/device.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


namespace OneWire
{
	class DS18B20 : public Device
	{

	public:
		DS18B20(Bus* owb, OneWireBus_ROMCode owCode) : Device(owb, owCode)
		{
		}

		void StartReading()
		{
			bus->Claim();
			bus->SelectDevice(this);
			bus->WriteByte(0x44);		//Convert T
			bus->Release();
		}

		float GetReading()
		{
			uint8_t data[2];
			float val = -999;
			bus->Claim();
			bus->SelectDevice(this);
			bus->WriteByte(0xBE);		// Read scratchpad
			bus->ReadBytes(data, 2);
			bus->Release();


			val = (float)(data[0]+(data[1]*256))/16;
			return val;
		}

		float GetTemp()
		{
			StartReading();
			vTaskDelay(750 / portTICK_PERIOD_MS);	//this delay can be replaced with, but that would be claiming the bus: while(bus->ReadByte() == 0);
			return GetReading();
		}





		virtual Devices GetType()
		{
			return Devices::DS18B20;
		}
	};


}



#endif /* MAIN_ONEWIRE_DS18B20_H_ */
