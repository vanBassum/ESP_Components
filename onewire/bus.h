/*
 * bus.h
 *
 *  Created on: Jan 17, 2021
 *      Author: Bas
 */

#ifndef COMPONENTS_ONEWIRE_BUS_H_
#define COMPONENTS_ONEWIRE_BUS_H_

#include "owb.h"
#include "owb_rmt.h"
//#include "owb/include/owb.h"
//#include "owb/include/owb_rmt.h"
#include "driver/gpio.h"

namespace OneWire
{

	class Device;
	class DS18B20;

	class Bus
	{
		owb_rmt_driver_info rmt_driver_info;


	public:
		OneWireBus *owb = 0;

		Bus(gpio_num_t gpio);
		~Bus();
		int Search(Device **devices, int max);
		bool ResetPulse();
		void WriteByte(uint8_t data);
		void WriteRomCode(OneWireBus_ROMCode code);
		uint8_t ReadByte();
		bool ReadBytes(uint8_t *data, int length);
		void SelectDevice(Device *device);

	};


}

#endif /* COMPONENTS_ONEWIRE_BUS_H_ */
