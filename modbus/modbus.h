#pragma once
#include <stdint.h>

#include "../modbus/command.h"
#include "../modbus/commands/ReadHoldingRegisters.h"
#include "../modbus/commands/WriteSingleRegister.h"
#include "../freertos_cpp/freertos.h"

//https://www.fernhillsoftware.com/help/drivers/modbus/modbus-protocol.html
namespace Modbus
{
	class Modbus
	{
		FreeRTOS::RecursiveMutex mutex;
	protected:
		virtual Exception ExecuteCMD(uint8_t unitAddr, RequestCMD* request, ReplyCMD* reply, int timeout) = 0;

		Exception ExecuteCMDThreadSafe(uint8_t unitAddr, RequestCMD* request, ReplyCMD* reply, int timeout)
		{
			Exception result = Exception::UnknownException;
			if(mutex.Take(timeout))
			{
				result = ExecuteCMD(unitAddr, request, reply, timeout);
				mutex.Give();
			}
			else
				result = Exception::MutexFailed;
			return result;
		}

	public:

		//bool IsConnected() = 0;

		Modbus()
		{

		}

		virtual ~Modbus()
		{

		}


        Exception ReadHoldingRegisters(uint8_t unitAddr, uint16_t startAddress, uint16_t count, uint16_t* dst, uint16_t size, int timeout)
        {
            Exception result = Exception::UnknownException;
            if (size >= count * 2 && dst != NULL)
            {
                ReadHoldingRegistersReply reply;
                ReadHoldingRegistersRequest request;
				request.StartAddress = startAddress;
				request.Count = count;
				result = ExecuteCMDThreadSafe(unitAddr, &request, &reply, timeout);
                uint16_t rSize = reply.RegisterCount * 2;
                if (size >= rSize)
                    memcpy(dst, reply.RegisterData, rSize);
                else
                    result = Exception::InvalidArguments;
            }
            else
                result = Exception::InvalidArguments;

            return result;
        }

        Exception WriteSingleRegister(uint8_t unitAddr, uint16_t address, uint16_t value, int timeout)
        {
            Exception result = Exception::UnknownException;
            WriteSingleRegisterRequest request;
            WriteSingleRegisterReply reply;
            request.Address = address;
            request.Value = value;
            result = ExecuteCMDThreadSafe(unitAddr, &request, &reply, timeout);

            if (result == Exception::NoError)
            {
                //Check reply.

                if (reply.Address != address)
                    result = Exception::UnknownException;

                if (reply.Value != value)
                    result = Exception::UnknownException;
            }
            return result;
        }



		//virtual Exception WriteHoldingRegisters(uint8_t unitAddr, uint16_t startAddress, uint16_t count, ReadHoldingRegistersReply* reply) = 0;
	};
}

//I include these at the end of this file so the user only has to include "modbus.h" in order to be able to access all the modbus implementations.
//This is at the end of the file in order to prevent reference problems with classes.
//A better solution would be to create an component for modbus and split all the files into .h and .cpp files.

#include "rtu/ModbusRTU.h"
#include "../modbus/tcp/ModbusTCP.h"

#include "../modbus/property.h"

