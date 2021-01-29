#pragma once
#include <stdint.h>

#include "../../modbus/exceptions.h"
#include "../../modbus/misc.h"
#include "../Frame.h"


namespace Modbus
{

    class RTUReplyFrame : public ReplyFrame
    {
        uint8_t UnitAddr = 1;
        uint16_t CRC = 0;
        

    public:

        Exception Populate(uint8_t* data, uint16_t count)
        {
            Exception result = Exception::UnknownException;
            if (Message != NULL && data != NULL)
            {
                if (count >= 3) //This is a valid reply
                {
                    int rxPtr = 0;
                    rxPtr += nmemcpyswapendian(&UnitAddr, &data[rxPtr], sizeof(UnitAddr));
                    result = Message->Populate(&data[rxPtr], count - 3);
                    rxPtr += nmemcpyswapendian(&CRC, &data[count-2], sizeof(CRC));

                    //TODO: CRC check!!!
                }
                else if (count >= 2) //This is a valid reply, but the reply is an exception send by device.
                {
                    if (data[0] == 0x90)
                    {
                    	switch(data[1])
                    	{
                    	case 0x01: result = Exception::IlligalFunction; break;
                    	case 0x02: result = Exception::IlligalDataAddress; break;
                    	case 0x03: result = Exception::IllegalDataValue; break;
                    	case 0x04: result = Exception::SlaveDeviceFailure; break;
                    	default:   result = Exception::UnknownException; break;
                    	}
                    }
                    else
                        result = Exception::UnknownException;
                }
                else
                    result = Exception::InvalidReplyLength;
            }
            else
                result = Exception::InvalidArguments;
            
            return result;
        }
    };

}
