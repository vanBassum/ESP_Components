#pragma once
#include <stdint.h>

#include "../modbus/command.h"

namespace Modbus
{

    class RequestFrame
    {
    public:
        RequestCMD* Message = NULL;

        ~RequestFrame()
        {
            //if (Message != NULL)
            //    delete Message;
        }

        virtual uint16_t GetLength() = 0;
        virtual uint16_t CopyTo(uint8_t* data, uint16_t maxLength) = 0;
        virtual uint16_t GetReplyLength() = 0;
    };


    class ReplyFrame
    {
    public:
        ReplyCMD* Message = NULL;

        ~ReplyFrame()
        {
            //if (Message != NULL)
            //    delete Message;
        }
    };



}

