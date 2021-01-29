#pragma once
#include "../../freertos_cpp/freertos.h"
#include "../../modbus/exceptions.h"
#include "../../modbus/modbus.h"
#include "../../modbus/rtu/RTUReplyFrame.h"
#include "../../modbus/rtu/RTURequestFrame.h"
#include "driver/uart.h"
#include "driver/gpio.h"

namespace Modbus
{


    class ModbusRTU : public Modbus
    {
        uart_port_t port = UART_NUM_2;

        Exception ExecFrame(RTURequestFrame txFrame, RTUReplyFrame* rxFrame, int timeout)
        {
            Exception result = Exception::UnknownException;
            uint16_t txSize = txFrame.GetLength();
            uint16_t rxSize = txFrame.GetReplyLength();

            if (txSize != 0 && rxFrame != NULL)
            {
                uint8_t* txBuffer = (uint8_t*)malloc(txSize);
                uint8_t* rxBuffer = (uint8_t*)malloc(rxSize);

                txFrame.CopyTo(txBuffer, txSize);
                uart_write_bytes(port, (char*)txBuffer, txSize);
                int length = uart_read_bytes(port, rxBuffer, rxSize, timeout / portTICK_RATE_MS);
                if (length != 0)
                {
                    result = rxFrame->Populate(rxBuffer, length);
                }
                else
                	result = Exception::Timeout;

                free(txBuffer);
                free(rxBuffer);
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
                RTURequestFrame frame;
                frame.UnitAddr = unitAddr;
                frame.Message = request;

                RTUReplyFrame rx;
                rx.Message = reply;
                result = ExecFrame(frame, &rx, timeout);
            }
            else
                result = Exception::InvalidArguments;
            return result;
        }


    public:
        ModbusRTU(uart_port_t port, uint16_t baud)
        {
            this->port = port;
            uart_config_t uart_config;
            uart_config.baud_rate = baud;
            uart_config.data_bits = UART_DATA_8_BITS;
            uart_config.parity = UART_PARITY_DISABLE;
            uart_config.stop_bits = UART_STOP_BITS_1;
            uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            uart_config.rx_flow_ctrl_thresh = 122;
            //uart_config.source_clk = UART_SCLK_APB;
            ESP_ERROR_CHECK(uart_param_config(port, &uart_config));
            ESP_ERROR_CHECK(uart_set_pin(port, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            ESP_ERROR_CHECK(uart_driver_install(port, 1024 * 2, 0, 0, NULL, 0)); 
        }
    };
}
