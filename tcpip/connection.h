/*
 * Connection.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_CPP_LIB_JBVPROTOCOL_CONNECTION_H_
#define MAIN_CPP_LIB_JBVPROTOCOL_CONNECTION_H_

#include "stdint.h"
#include "../Misc/Callback.h"


class Connection
{

public:
	virtual ~Connection(){}
	Callback<void, Connection*, uint8_t*, uint32_t> OnDataReceived;
	virtual void Send(uint8_t* data, uint32_t length) = 0;
};


#endif /* MAIN_CPP_LIB_JBVPROTOCOL_CONNECTION_H_ */
