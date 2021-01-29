#pragma once
#include <stdint.h>

#include "../modbus/exceptions.h"

namespace Modbus
{

    class RequestCMD
    {
    public:
    	virtual ~RequestCMD(){}
        virtual uint16_t GetLength() = 0;
        virtual uint16_t CopyTo(uint8_t* data, uint16_t maxLength) = 0;
        virtual uint16_t GetReplyLength() = 0;

    };


    
    class ReplyCMD
    {
    public:
    	virtual ~ReplyCMD(){}
        virtual Exception Populate(uint8_t* data, uint16_t count) = 0;
        virtual uint8_t GetFunctionCode() = 0;
    };
    

}





