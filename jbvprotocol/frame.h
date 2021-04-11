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

#include "stdint.h"
#include <string>
#include <vector>
#include <string.h>

namespace JBVProtocol
{
	class Framing;


	enum class FrameOptions : uint8_t
	{
		None		= 0,
		Broadcast 	= (1<<0),	//Frame will be send to all within the network.
		ASCII		= (1<<1),	//The data field has to be interpreted as ASCII, also the reply will be send in ASCII.
	};

	ENUM_FLAG_OPERATORS(FrameOptions);

	enum class FrameTypes : uint8_t
	{
		ProtocolFrame 	= 0x00,
		DataFrame 		= 0x01,
	};





	class Frame
	{
		uint8_t 	Options		= 0;

	public:
		uint8_t 	Hops 		= 0;
		uint32_t 	TxID  		= 0;
		uint32_t 	RxID  		= 0;
		uint16_t	Sequence	= 0;
		uint16_t 	DataLength	= 0;
		uint8_t 	*Data 		= 0;

		Frame();
		virtual ~Frame();
		std::string ToString();

		FrameTypes GetType()
		{
			return (FrameTypes)((Options & 0xF0) >> 4);
		}

		void SetType(FrameTypes ty)
		{
			Options = (uint8_t)((Options & 0x0F) | (((uint8_t)ty << 4) & 0xF0));
		}


		FrameOptions GetOptions()
		{
			return (FrameOptions)((Options & 0x0F));
		}

		void SetOptions(FrameOptions opt)
		{
			Options = (uint8_t)((Options & 0xF0) | ((uint8_t)opt & 0x0F));
		}

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

		std::vector<std::string> GetArgs()
		{
			std::vector<std::string> list;
			FrameOptions opts = GetOptions();
			if((opts & FrameOptions::ASCII) == FrameOptions::ASCII)
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
			case 0: return *(uint8_t*) &Options;
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
				return Data[index - 14];
			}
		}

		uint32_t GetTotalLength()
		{
			return 14 + DataLength;
		}
	};

} /* namespace JBVProtocol */

#endif /* MAIN_COMPONENTS_JBVPROTOCOL_FRAME_H_ */

