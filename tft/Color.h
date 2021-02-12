/*
 * Color.h
 *
 *  Created on: 2 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_DISPLAY_COLOR_H_
#define MAIN_DISPLAY_COLOR_H_

#include <stdint.h>



class Color
{


public:
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;

	Color()
	{

	}

	Color(uint8_t R, uint8_t G, uint8_t B)
	{
		this->R = R;
		this->G = G;
		this->B = B;
	}



	uint16_t Get_RGB565()
	{
		return (((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3));
	}





	/*
	static Color Red = Color(255, 0, 0);
	static const Color Green = Color(0, 255, 0);
	static const Color Blue = Color(0, 0, 255);
	static const Color Black = Color(0, 0, 0);
	static const Color White = Color(255, 255, 255);
*/




	/*
#define RED			0xf800
#define GREEN			0x07e0
#define BLUE			0x001f
#define BLACK			0x0000
#define WHITE			0xffff
#define GRAY			0x8c51
#define YELLOW			0xFFE0
#define CYAN			0x07FF
#define PURPLE			0xF81F
	*/
};



#endif /* MAIN_DISPLAY_COLOR_H_ */
