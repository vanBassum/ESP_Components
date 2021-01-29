#pragma once
#include <stdint.h>

#include "../../modbus/command.h"
#include "../../modbus/exceptions.h"
#include "../../modbus/misc.h"


namespace Modbus
{

    class WriteSingleRegisterRequest : public RequestCMD
    {
        const uint8_t FunctionCode = 0x06;

    public:
        uint16_t Address = 0x0000;
        uint16_t Value = 0x0000;        


        uint16_t GetLength()
        {
            return sizeof(FunctionCode) + sizeof(Address) + sizeof(Value);
        }

        uint16_t CopyTo(uint8_t* data, uint16_t maxLength)
        {
            int length = GetLength();

            if (data != NULL || maxLength >= length)
            {
                int wrPtr = 0;
                wrPtr += nmemcpyswapendian(&data[wrPtr], &FunctionCode, sizeof(FunctionCode));
                wrPtr += nmemcpyswapendian(&data[wrPtr], &Address, sizeof(Address));
                wrPtr += nmemcpyswapendian(&data[wrPtr], &Value, sizeof(Value));
                return wrPtr;
            }
            return 0;
        }

        uint16_t GetReplyLength()
        {
            return 5;
        }
    };


    class WriteSingleRegisterReply : public ReplyCMD
    {
        const uint8_t FunctionCode = 0x06;
    public:

        uint16_t Address = 0x0000;
        uint16_t Value = 0x0000;

        Exception Populate(uint8_t* data, uint16_t count)
        {
            Exception result = Exception::UnknownException;

            if (data != NULL)
            {
                if (count >= 5)
                {
                    int rxPtr = 0;
                    uint8_t fCode = 0;
                    rxPtr += nmemcpyswapendian(&fCode, &data[rxPtr], sizeof(fCode));
                    if (fCode == FunctionCode)
                    {
                        rxPtr += nmemcpyswapendian(&Address, &data[rxPtr], sizeof(Address));
                        rxPtr += nmemcpyswapendian(&Value, &data[rxPtr], sizeof(Value));
                        result = Exception::NoError;
                    }
                    else
                        result = Exception::InvalidReplyFunctionCode;
                }
                else
                    result = Exception::InvalidReplyLength;
            }
            else
                result = Exception::InvalidArguments;

            return result;
        }

        uint8_t GetFunctionCode()
        {
            return FunctionCode;
        }
    };

}
