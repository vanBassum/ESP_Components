#pragma once


#include <stdint.h>
#include <stddef.h>
#include <FreeRTOS.h>
#include <stream_buffer.h>
#include <message_buffer.h>

namespace FreeRTOS
{

	class StreamBuffer
	{
		StreamBufferHandle_t handle = 0;
		
	public:
		
		void Init(size_t size, size_t triggerLevel)
		{
			if (handle != 0)
				vStreamBufferDelete(handle);
			handle = xStreamBufferCreate(size, triggerLevel);
		}
		
		size_t Send(const void* data, size_t size, TickType_t wait)
		{
			if (handle != 0)
				return xStreamBufferSend(handle, data, size, wait);
			return 0;
		}
		
		size_t Receive(void* data, size_t size, TickType_t wait)
		{
			if (handle != 0)
				return xStreamBufferReceive(handle, data, size, wait);
			return 0;
		}
		
		size_t BytesAvailable()
		{
			if (handle != 0)
				xStreamBufferBytesAvailable(handle);
			return 0;
		}
		
		void SetTriggerLevel(size_t triggerLevel)
		{
			if (handle != 0)
				xStreamBufferSetTriggerLevel(handle, triggerLevel);
		}
	};
	
	
	class MessageBuffer
	{
		MessageBufferHandle_t handle = 0;
		
	public:
		
		void Init(size_t size)
		{
			if (handle != 0)
				vMessageBufferDelete(handle);
			
			handle = xMessageBufferCreate(size);
		}
		
		size_t Send(const void* data, size_t size, TickType_t wait)
		{
			if (handle != 0)
			{
				return xMessageBufferSend(handle, data, size, wait);
			}
			return 0;
		}
		
		size_t Receive(void* data, size_t size, TickType_t wait)
		{
			if (handle != 0)
			{
				return xMessageBufferReceive(handle, data, size, wait);
			}
			return 0;
		}
	};
}

