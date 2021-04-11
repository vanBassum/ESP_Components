/*
 * Framing.h
 *
 *  Created on: 1 Aug 2020
 *      Author: Bas
 */


#ifndef COMPONENTS_JBVPROTOCOL_IO_FRAMING_H_
#define COMPONENTS_JBVPROTOCOL_IO_FRAMING_H_

#include "../Misc/Callback.h"
#include <stdint.h>
#include <stdlib.h>
#include "frame.h"



#define JSOF	'&'
#define JEOF	'%'
#define JESC	'\\'
#define JNOP	'*'

#define WRT(x) if(data != NULL && wrPtr < size) data[wrPtr] = x; wrPtr++

namespace JBVProtocol
{

	class Framing
	{
		bool startFound = false;
		bool esc = false;
		Frame 		*rxFrame = 0;
		uint32_t 	wrPtr = 0;
	public:

		Callback<void, Frame*> OnFrameCollected;
		Callback<void> OnMallocError;

		void Unstuff(uint8_t data)
		{
			bool record = false;

			if (esc)
			{
				record = true;
				esc = false;
			}
			else
			{
				switch (data)
				{
					case JESC:
						esc = true;
						break;

					case JSOF:
						startFound = true;
						rxFrame = new Frame();
						wrPtr = 0;
						break;

					case JEOF:
						{
							startFound = false;
							OnFrameCollected.Invoke(rxFrame);
						}
						break;

					case JNOP:
						break;

					default:
						record = true;
						break;
				}
			}
			if (record && startFound)
			{
				(*rxFrame)[wrPtr++] = data;
			}
		}


		void Unstuff(uint8_t *bt, uint32_t length)
		{
			for(int i=0; i<length; i++)
				Unstuff(bt[i]);
		}


		Callback<void, Frame*> OnEscapedFrame;



		static uint32_t EscapeFrame(Frame *txFrame, uint8_t *data, uint32_t size)
		{
			int totalSize = txFrame->GetTotalLength();
			int wrPtr = 0;
			WRT(JSOF);
			for(int rdPtr = 0; rdPtr < totalSize; rdPtr++)
			{
				uint8_t d = (*txFrame)[rdPtr];

				if(d == JSOF || d == JEOF || d == JESC || d == JNOP)
				{
					WRT(JESC);
				}
				WRT(d);
			}

			WRT(JEOF);
			return wrPtr;
		}
	};
}



#endif /* COMPONENTS_JBVPROTOCOL_IO_FRAMING_H_ */
