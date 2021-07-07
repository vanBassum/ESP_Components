/*
 * log.h
 *
 *  Created on: Jul 7, 2021
 *      Author: Bas
 */

#ifndef MAIN_ESPLIB_MISC_LOH_H_
#define MAIN_ESPLIB_MISC_LOH_H_

#include <esp_partition.h>
#include <esp_log.h>

enum Flags : uint8_t
{
	Empty 		= 0x01,		//Both 0 is full!
	Active		= 0x02,		//Only active if not empty!
};




struct SectorHeader
{
	Flags flags = (Flags)0xFF;
};

struct EntryHeader
{
	Flags flags = (Flags)0xFF;
	uint16_t size;
};


class Log
{
	const esp_partition_t* partition = NULL;
	uint32_t partitionSize = 0;
	uint32_t sectorSize = 0x1000;
	uint16_t maxSize = 	  0x100;
public:

	Log()
	{

	}

	void Init()
	{
		partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "log");
		if(partition == NULL)
			ESP_LOGE("Log", "Error initializing log");
		partitionSize = partition->size;
	}

	void Format()
	{
		esp_partition_erase_range(partition, 0x0, partitionSize);
	}

	//Returns the start address of the active sector
	uint32_t FindActiveSector()
	{
		uint32_t addr = 0x0;
		bool sens = true;
		uint32_t next = 0x0;
		SectorHeader header;

		while(addr < partitionSize)
		{
			esp_partition_read(partition, addr, &header, sizeof(header));

			if(header.flags & Flags::Empty)			//Sector is empty
			{
				if(sens)
					next = addr;
				sens = false;
			}
			else if(header.flags & Flags::Active)		//Sector is active
				return addr;
			else											//Sector is full
				sens = true;
			addr += sectorSize;
		}

		//Make active
		uint8_t d = ~(Flags::Empty);
		esp_partition_write(partition, next, &d, 1);	//Clear the empty bit to make this the active sector.

		return next;
	}

	uint32_t FindOldestSector()
	{
		uint32_t addr = 0x0;
		uint32_t next = 0x0;
		bool sens = true;
		bool addFull = false;
		SectorHeader header;

		while(addr < partitionSize)
		{
			esp_partition_read(partition, addr, &header, sizeof(header));


			if(header.flags & Flags::Empty)			//Sector is empty
			{
				sens = true;
			}
			else if(header.flags & Flags::Active)	//Sector is active
			{
				if(sens && !addFull)
				{
					next = addr;
				}
				sens = true;
			}
			else									//Sector is full
			{
				if(sens)
				{
					next = addr;
					addFull = true;
				}
			}
			addr += sectorSize;
		}

		return next;
	}

	//Returns the start address of the first empty spot in the sector. Or 0 if error!
	uint32_t FindEmptySpace(uint32_t sectorAddr)
	{
		uint32_t addr = sectorAddr + 1;

		EntryHeader header;

		while(addr - sectorAddr < sectorSize)	//Still within sector?
		{
			esp_partition_read(partition, addr, &header, sizeof(header));
			if(header.flags & Flags::Empty)
				break;
			else
				addr += header.size + sizeof(header);
		}

		if(addr - sectorAddr >= sectorSize)	//Outside sector?
			addr = 0;
		return addr;
	}

	void Write(void* data, uint16_t size)
	{
		ESP_LOGI("LOG", "Writing %d bytes", size);

		if(size < maxSize)
		{
			uint32_t activeSectorAddr = FindActiveSector();
			uint32_t emptyEntryAddr = FindEmptySpace(activeSectorAddr);

			if(emptyEntryAddr > 0)
			{
				uint32_t sizeLeft = sectorSize - (emptyEntryAddr - activeSectorAddr);

				if(size > sizeLeft)
				{
					uint32_t nextSector = activeSectorAddr + sectorSize;
					if(nextSector >= partitionSize)
						nextSector = 0;

					uint8_t d;
					//Erase next sector
					esp_partition_erase_range(partition, nextSector, sectorSize);

					//Close current sector
					d = ~(Flags::Active | Flags::Empty);					//Clear the empty bit and the active bit.
					esp_partition_write(partition, activeSectorAddr, &d, 1);

					//Activate next sector
					d = ~(Flags::Empty);
					esp_partition_write(partition, nextSector, &d, 1);	//Clear the empty bit to make this the active sector.

					activeSectorAddr = nextSector;
					emptyEntryAddr = activeSectorAddr + 1;
				}

				//Write entry
				EntryHeader header;
				header.flags = (Flags)~Flags::Empty;
				header.size = size;
				esp_partition_write(partition, emptyEntryAddr, &header, sizeof(header));
				esp_partition_write(partition, emptyEntryAddr + 3, data, size);
				ESP_LOGI("LOG", "Writing %d bytes @ %X", size, emptyEntryAddr);
			}
			else
				ESP_LOGE("Log", "Error while finding empty entry");
		}
		else
			ESP_LOGE("Log", "Error while writing log, data too large");
	}
};




#endif /* MAIN_ESPLIB_MISC_LOH_H_ */
