#pragma once
#include <stdint.h>

#include "../../modbus/exceptions.h"
#include "../../modbus/frame.h"
#include "../../modbus/misc.h"


namespace Modbus
{

    class TCPReplyFrame : public ReplyFrame
    {
        uint16_t TXID = 0;			    //identifies the transaction.
        uint16_t ProtID = 0;			//is zero to indicate Modbus protocol.
        uint16_t Length = 0;			//
        uint8_t UnitAddr = 1;			//

    public:

        Exception Populate(uint8_t* data, uint16_t count)
        {
            Exception result = Exception::UnknownException;


            if (Message != NULL && data != NULL)
            {
                if (count >= 7) //This is a valid reply
                {
                    int rxPtr = 0;
                    rxPtr += nmemcpyswapendian(&TXID, &data[rxPtr], sizeof(TXID));
                    rxPtr += nmemcpyswapendian(&ProtID, &data[rxPtr], sizeof(ProtID));
                    rxPtr += nmemcpyswapendian(&Length, &data[rxPtr], sizeof(Length));
                    rxPtr += nmemcpyswapendian(&UnitAddr, &data[rxPtr], sizeof(UnitAddr));

                    if (Length - 1 == count - 7)
                    {
                        if (data[rxPtr] == Message->GetFunctionCode())
                        {
                            result = Message->Populate(&data[rxPtr], Length - 1);
                        }
                        else
                            result = Exception::InvalidReplyFunctionCode;
                    }
                    else
                        result = Exception::InvalidReplyLength;
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
