/*
 * Frame.h
 *
 *  Created on: Mar 18, 2021
 *      Author: Bas
 */

#ifndef MAIN_COMPONENTS_JBVPROTOCOL_FRAME_H_
#define MAIN_COMPONENTS_JBVPROTOCOL_FRAME_H_

#define ENUM_FLAG_OPERATORS(T)                                                                                                                                            \
    inline T operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }                                                                       \
    inline T operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }

#define HASFLAG(ENUM, FLAGS)  (ENUM & (FLAGS)) == (FLAGS)


#include "stdint.h"
#include <string>
#include <vector>
#include <string.h>

namespace JBVProtocol
{
	class Framing;


	enum class Options : uint8_t
	{
		None		= 0,
		Broadcast	= (1<<0),	// when true, message is send to all clients in network. When frame is reply and broadcast, the frame will not be handled as broadcast!
		ASCII		= (1<<1),	// when true, commands are send as ASCII otherwise commands are send as raw data, currently only ascii mode supported.
		Request 	= (1<<2),	// frame is either a request or a reply.
		//RFU		= (1<<3),	// Suggestion: CRC, true when CRC of frame is also send. Might be helpful when using things like RS232.
		//RFU 		= (1<<4),	// Suggestion: Encryption, Together with next field determines what encryption is used. (Since this is the first byte, the rest of the message can be encrypted.)
		//RFU 		= (1<<5),	// Suggestion: Encryption,
		//RFU		= (1<<6),	//
		//RFU		= (1<<7),	//

	};

	ENUM_FLAG_OPERATORS(Options);



	class Frame
	{


	public:
		Options 	Opts		= Options::None;
		uint8_t 	Hops 		= 0;
		uint32_t 	TxID  		= 0;
		uint32_t 	RxID  		= 0;
		uint16_t	Sequence	= 0;
		uint16_t 	DataLength	= 0;
		uint8_t 	*Data 		= 0;

		Frame();
		virtual ~Frame();
		std::string ToString();


		bool SetData(const void *data, int len)
		{
			DataLength = len;
			if(Data != NULL)
				free(Data);
			Data = (uint8_t*)malloc(len);
			if(Data)
			{
				memcpy(Data, data, len);
				return true;
			}
			return false;
		}


		//Only in ASCII mode
		std::string GetCommand()
		{
			std::string s;
			if(HASFLAG(Opts, Options::ASCII))
			{
				bool esc = false;
				for(int i = 0; i<DataLength; i++)
				{

					if(esc)
					{
						s+=Data[i];
						esc = false;
					}
					else
					{
						if(Data[i] == '\\')
							esc = true;
						else if(Data[i] == ' ')
							return s;
						else
							s+=Data[i];
					}
				}
			}
			return s;
		}

		//Only in ASCII mode
		std::vector<std::string> GetArgs()
		{
			std::vector<std::string> list;
			if((Opts & Options::ASCII) == Options::ASCII)
			{
				std::string s;
				bool esc = false;
				for(int i = 0; i<DataLength; i++)
				{

					if(esc)
					{
						s+=Data[i];
						esc = false;
					}
					else
					{
						if(Data[i] == '\\')
						{
							esc = true;
						}
						else if(Data[i] == ' ')
						{
							list.push_back(s);
							s = std::string();
						}
						else
						{
							s+=Data[i];
						}
					}
				}

				list.push_back(s);
			}
			return list;
		}


		uint8_t &operator [](uint32_t index)
		{
			switch(index)
			{
			case 0: return *(uint8_t*) &Opts;
			case 1: return *(uint8_t*) &Hops;
			case 2: return ((uint8_t*) &TxID)[3];
			case 3: return ((uint8_t*) &TxID)[2];
			case 4: return ((uint8_t*) &TxID)[1];
			case 5: return ((uint8_t*) &TxID)[0];
			case 6: return ((uint8_t*) &RxID)[3];
			case 7: return ((uint8_t*) &RxID)[2];
			case 8: return ((uint8_t*) &RxID)[1];
			case 9: return ((uint8_t*) &RxID)[0];
			case 10: return ((uint8_t*) &Sequence)[1];
			case 11: return ((uint8_t*) &Sequence)[0];
			case 12: return ((uint8_t*) &DataLength)[1];
			case 13: return ((uint8_t*) &DataLength)[0];
			default:
				if(Data == 0)
					Data = (uint8_t*) malloc(DataLength);
			return Data[index - 14];	//TODO: Check malloc!!!
			}
		}

		uint32_t GetTotalLength()
		{
			return 14 + DataLength;
		}
	};

} /* namespace JBVProtocol */

#endif /* MAIN_COMPONENTS_JBVPROTOCOL_FRAME_H_ */

