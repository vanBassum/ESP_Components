/*
 * Frame.cpp
 *
 *  Created on: Mar 18, 2021
 *      Author: Bas
 */

#include "frame.h"
#include <stdlib.h>

namespace JBVProtocol
{

	Frame::Frame()
	{
		// TODO Auto-generated constructor stub

	}

	Frame::~Frame()
	{
		if(Data != NULL)
			free(Data);
		Data = 0;
	}




	std::string Frame::ToString()
	{

		std::string str;
		str+= "Opt = " + std::to_string(Options) + ", ";
		str+= "Hops = " + std::to_string(Hops) + ", ";
		str+= "TxID = " + std::to_string(TxID) + ", ";
		str+= "RxID = " + std::to_string(RxID) + ", ";
		str+= "Sequence = " + std::to_string(Sequence) + ", ";
		str+= "DataLength = " + std::to_string(DataLength) + ", ";
		return str;
	}


} /* namespace JBVProtocol */
