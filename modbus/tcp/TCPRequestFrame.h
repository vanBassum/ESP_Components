#pragma once
#include <stdint.h>

#include "../../modbus/frame.h"
#include "../../modbus/misc.h"

namespace Modbus
{

    class TCPRequestFrame : public RequestFrame
    {
    public:
        uint16_t TXID = 0;			//identifies the transaction.
        uint16_t ProtID = 0;			//is zero to indicate Modbus protocol.
        uint8_t UnitAddr = 1;			//is the device address encoded as single byte.


        uint16_t GetLength()
        {
            if (Message != NULL)
            {
                uint16_t msgLength = Message->GetLength();
                return sizeof(TXID) + sizeof(ProtID) + sizeof(msgLength) + sizeof(UnitAddr) + msgLength;
            }
            return 0;
        }

        uint16_t CopyTo(uint8_t* data, uint16_t maxLength)
        {

            if (data != NULL && Message != NULL)
            {
                uint16_t lengthField = Message->GetLength() + sizeof(UnitAddr);
                uint16_t totalLength = GetLength();

                if (maxLength >= totalLength)
                {

                    int wrPtr = 0;
                    wrPtr += nmemcpyswapendian(&data[wrPtr], &TXID, sizeof(TXID));
                    wrPtr += nmemcpyswapendian(&data[wrPtr], &ProtID, sizeof(ProtID));
                    wrPtr += nmemcpyswapendian(&data[wrPtr], &lengthField, sizeof(lengthField));
                    wrPtr += nmemcpyswapendian(&data[wrPtr], &UnitAddr, sizeof(UnitAddr));
                    wrPtr += Message->CopyTo(&data[wrPtr], maxLength - wrPtr);
                    return wrPtr;
                }
            }
            return 0;
        }

        uint16_t GetReplyLength()
        {
            if (Message == NULL)
                return 0;
            return Message->GetReplyLength() + 7;
        }
    };
}
