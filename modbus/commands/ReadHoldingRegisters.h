#pragma once
#include <stdint.h>

#include "../../modbus/command.h"
#include "../../modbus/exceptions.h"
#include "../../modbus/misc.h"


namespace Modbus
{

    class ReadHoldingRegistersRequest : public RequestCMD
    {
        const uint8_t FunctionCode = 0x03;

    public:
        uint16_t StartAddress = 0x0000;
        uint16_t Count = 0x0000;            //Number of registers to read, 1 register = 2 bytes.


        uint16_t GetLength()
        {
            return sizeof(FunctionCode) + sizeof(StartAddress) + sizeof(Count);
        }

        uint16_t CopyTo(uint8_t* data, uint16_t maxLength)
        {
            int length = GetLength();

            if (data != NULL || maxLength >= length)
            {
                int wrPtr = 0;
                wrPtr += nmemcpyswapendian(&data[wrPtr], &FunctionCode, sizeof(FunctionCode));
                wrPtr += nmemcpyswapendian(&data[wrPtr], &StartAddress, sizeof(StartAddress));
                wrPtr += nmemcpyswapendian(&data[wrPtr], &Count, sizeof(Count));
                return wrPtr;
            }
            return 0;
        }

        uint16_t GetReplyLength()
        {
            return 2 + Count * 2;
        }
    };

    class ReadHoldingRegistersReply : public ReplyCMD
    {
        const uint8_t FunctionCode = 0x03;
    public:

        uint16_t* RegisterData = NULL;
        uint8_t RegisterCount = 0;

        ~ReadHoldingRegistersReply()
        {
            if (RegisterData != NULL)
                delete RegisterData;
        }


        Exception Populate(uint8_t* data, uint16_t count)
        {
            Exception result = Exception::UnknownException;

            if (data != NULL)
            {
                if (count >= 2 && data != NULL)
                {
                    int rxPtr = 0;
                    uint8_t replySize = 0;
                    uint8_t fCode = 0;
                    rxPtr += nmemcpyswapendian(&fCode, &data[rxPtr], sizeof(fCode));
                    rxPtr += nmemcpyswapendian(&replySize, &data[rxPtr], sizeof(replySize));

                    if (fCode == FunctionCode)
                    {
                        if (replySize % 2 == 0)
                        {
                            RegisterData = (uint16_t*)malloc(replySize);
                            RegisterCount = replySize / 2;
                            for (int i = 0; i < RegisterCount; i++)
                            {
                                rxPtr += nmemcpyswapendian(&RegisterData[i], &data[rxPtr], sizeof(uint16_t));
                            }
                            result = Exception::NoError;
                        }
                        else
                            result = Exception::InvalidReplyLength;
                    }
                    else
                        result = Exception::InvalidReplyFunctionCode;
                }
                else
                    result = Exception::InvalidArguments;
            }
            else
                result = Exception::InvalidReplyLength;
            

            return result;
        }

        uint8_t GetFunctionCode()
        {
            return FunctionCode;
        }
    };

}
