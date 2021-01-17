/*
 * bus.cpp
 *
 *  Created on: Jan 17, 2021
 *      Author: Bas
 */

#include "onewire.h"

namespace OneWire
{
	Bus::Bus(gpio_num_t gpio)
	{

		owb = owb_rmt_initialize(&rmt_driver_info, gpio, RMT_CHANNEL_1, RMT_CHANNEL_0);
		owb_use_crc(owb, false);  // enable CRC check for ROM code
	}

	int Bus::Search(Device **devices, int max)
	{
		mutex.Take();
		int num_devices = 0;
		OneWireBus_SearchState search_state = {0};
		bool found = false;
		owb_search_first(owb, &search_state, &found);
		while (num_devices < max && found)
		{
			switch(search_state.rom_code.fields.family[0]) //https://www.owfs.org/index_php_page_family-code-list.html
			{
			case 0x28: //DS18B20
				devices[num_devices] = new DS18B20(this, search_state.rom_code);
				break;
			default:
				devices[num_devices] = new Device(this, search_state.rom_code);
				break;
			}
			num_devices++;
			owb_search_next(owb, &search_state, &found);
		}
		mutex.Give();
		return num_devices;
	}


	bool Bus::ResetPulse()
	{
		bool isPresent;
		mutex.Take();
		owb_reset(owb, &isPresent);
		mutex.Give();
		return isPresent;
	}

	void Bus::WriteByte(uint8_t data)
	{
		mutex.Take();
		owb_write_byte(owb, data);
		mutex.Give();
	}

	void Bus::WriteRomCode(OneWireBus_ROMCode code)
	{
		mutex.Take();
		owb_write_rom_code(owb, code);
		mutex.Give();
	}

	void Bus::SelectDevice(Device *device)
	{
		mutex.Take();
		ResetPulse();
		WriteByte(OWB_ROM_MATCH);
		WriteRomCode(device->GetCode());
		mutex.Give();
	}

	uint8_t Bus::ReadByte()
	{
		mutex.Take();
		uint8_t result;
		owb_read_byte(owb, &result);
		mutex.Give();
		return result;
	}

	bool Bus::ReadBytes(uint8_t *data, int length)
	{
		bool result = false;
		mutex.Take();
		result = owb_read_bytes(owb, data, length) == OWB_STATUS_OK;
		mutex.Give();
		return result;
	}

	bool Bus::Claim()
	{
		return mutex.Take();
	}

	bool Bus::Release()
	{
		return mutex.Give();
	}

}
