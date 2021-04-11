#pragma once
#include "../../modbus/exceptions.h"
#include "../../modbus/misc.h"
#include "../../modbus/modbus.h"
#include "../../modbus/tcp/TCPReplyFrame.h"
#include "../../modbus/tcp/TCPRequestFrame.h"
#include "../../freertos_cpp/freertos.h"
#include "../../tcpip/tcpsocket.h"

namespace Modbus
{


    class ModbusTCP : public Modbus
    {
    	TCPSocket tcp;
    	FreeRTOS::SemaphoreBinary semphr;
    	Exception rxResult = Exception::UnknownException;
    	TCPReplyFrame* rxFrame = NULL;

    	void DataRecieved(Connection* sender, uint8_t* data, uint32_t size)
    	{
    		if(rxFrame != NULL)
    		{
    			rxResult = rxFrame->Populate(data, size);
				semphr.Give();
    		}
    	}

        Exception ExecFrame(TCPRequestFrame txFrame, TCPReplyFrame* rxFrame, int timeout)
        {
            Exception result = Exception::UnknownException;
            uint16_t txSize = txFrame.GetLength();

            if (txSize != 0 && rxFrame != NULL)
            {
            	this->rxFrame = rxFrame;
                uint8_t* txBuffer = (uint8_t*)malloc(txSize);

                txFrame.CopyTo(txBuffer, txSize);
                semphr.Take(0);
                tcp.Send(txBuffer, txSize);
                if(semphr.Take(timeout))
                {
                	result = rxResult;
                }
                else
                	result = Exception::Timeout;

                free(txBuffer);
            }
            else
                result = Exception::InvalidArguments;

            return result;
        }

    protected:

        virtual Exception ExecuteCMD(uint8_t unitAddr, RequestCMD* request, ReplyCMD* reply, int timeout)
        {
            Exception result = Exception::UnknownException;

            if (reply != NULL && request != NULL)
            {
                TCPRequestFrame frame;
                frame.UnitAddr = unitAddr;
                frame.Message = request;

                TCPReplyFrame rx;
                rx.Message = reply;
                result = ExecFrame(frame, &rx, timeout);
            }
            else
                result = Exception::InvalidArguments;
            return result;
        }


    public:

        void Connect(const char *host, uint16_t port, bool autoReconnect)
        {
            tcp.Connect(host, port, autoReconnect);
        }


        ModbusTCP()
        {
        	tcp.OnDataReceived.Bind(this, &ModbusTCP::DataRecieved);
        }
    };
}



