/*
 * Device.h
 *
 *  Created on: Jan 16, 2021
 *      Author: Bas
 */

#ifndef MAIN_ONEWIRE_DEVICE_H_
#define MAIN_ONEWIRE_DEVICE_H_

#include <string>

#include "../onewire/bus.h"
#include "../onewire/devices.h"
namespace OneWire
{


	class Device
	{

		OneWireBus_ROMCode code;

	protected:
		Bus *bus;
	public:

		OneWireBus_ROMCode GetCode()
		{
			return code;
		}


		Device(Bus* owb, OneWireBus_ROMCode owCode)
		{
			bus = owb;
			code = owCode;
		}

		virtual ~Device()
		{

		}

		std::string GetCodeAsString()
		{
			char rom_code_s[17];
			owb_string_from_rom_code(code, rom_code_s, sizeof(rom_code_s));
			return std::string(rom_code_s);
		}

		virtual Devices GetType()
		{
			return Devices::Unknown;
		}
	};


}



#endif /* MAIN_ONEWIRE_DEVICE_H_ */
