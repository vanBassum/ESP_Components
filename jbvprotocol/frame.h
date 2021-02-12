/*
 * Frame.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_CPP_LIB_JBVPROTOCOL_FRAME_H_
#define MAIN_CPP_LIB_JBVPROTOCOL_FRAME_H_

#include "stdint.h"
#include "command.h"
#include <string>

enum class FrameOPT : uint8_t
{
	None		= 0,
	Broadcast 	= (1<<0),
};

inline FrameOPT operator|(FrameOPT a, FrameOPT b)
{
	return static_cast<FrameOPT>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline FrameOPT operator&(FrameOPT a, FrameOPT b)
{
	return static_cast<FrameOPT>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}



class Frame
{



public:

	FrameOPT 	Options 	= FrameOPT::None;
	uint8_t 	Hops 		= 0;
	uint16_t 	TxID  		= 0;
	uint16_t 	RxID  		= 0;
	uint16_t	Sequence	= 0;
	uint32_t 	CommandID 	= (uint32_t) Commands::INVALID;
	uint16_t 	DataLength	= 0;
	uint8_t	   *Data  		= 0;


	~Frame()
	{
		if(Data != 0)
			free(Data);
	}

	void SetData(uint8_t* data, uint16_t length)
	{
		DataLength = length;
		Data = (uint8_t*)malloc(length);
		memcpy(Data, data, length);
	}

	uint32_t GetTotalLength()
	{
		return DataLength + 2 + 4 + 2 + 2 + 2 + 1 + 1;
	}

	void Log()
	{
		ESP_LOGI("Frame", "(%d -> %d) %x", TxID, RxID, CommandID);
	}

	static Frame PrepareRequest(uint16_t RxID, uint16_t seq, uint16_t dataLength, Commands cmd, FrameOPT opt = FrameOPT::None)
	{
		Frame frame;
		frame.Options = opt;
		frame.CommandID = (uint32_t) cmd;
		frame.RxID = RxID;
		frame.Sequence = seq;
		frame.DataLength = dataLength;
		frame.Data = (uint8_t*) malloc(frame.DataLength);
		return frame;
	}

	static Frame PrepareReply(Frame* rx, uint16_t dataLength, Commands cmd, FrameOPT opt = FrameOPT::None)
	{
		Frame frame;
		frame.Options = opt;
		frame.CommandID = (uint32_t) cmd;
		frame.RxID = rx->TxID;
		frame.Sequence = rx->Sequence;
		frame.DataLength = dataLength;
		frame.Data = (uint8_t*) malloc(frame.DataLength);
		return frame;
	}




	uint8_t &operator [](uint32_t index)
	{
		switch(index)
		{
		case 0: return *(uint8_t*) &Options;
		case 1: return *(uint8_t*) &Hops;
		case 2: return ((uint8_t*) &TxID)[1];
		case 3: return ((uint8_t*) &TxID)[0];
		case 4: return ((uint8_t*) &RxID)[1];
		case 5: return ((uint8_t*) &RxID)[0];
		case 6: return ((uint8_t*) &Sequence)[1];
		case 7: return ((uint8_t*) &Sequence)[0];
		case 8: return ((uint8_t*) &CommandID)[3];
		case 9: return ((uint8_t*) &CommandID)[2];
		case 10: return ((uint8_t*) &CommandID)[1];
		case 11: return ((uint8_t*) &CommandID)[0];
		case 12: return ((uint8_t*) &DataLength)[1];
		case 13: return ((uint8_t*) &DataLength)[0];
		default:
			if(Data == 0)
				Data = (uint8_t*) malloc(DataLength);
			return Data[index - 14];
		}
	}
};



#endif /* MAIN_CPP_LIB_JBVPROTOCOL_FRAME_H_ */
