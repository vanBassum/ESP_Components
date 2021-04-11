/*
 * TFT.h
 *
 *  Created on: 2 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_DISPLAY_TFT_H_
#define MAIN_DISPLAY_TFT_H_

extern "C"
{
	#include "ili9340.h"
}

#include <string>
#include "Color.h"
#include "Font.h"

//https://github.com/nopnop2002/esp-idf-ili9340/tree/master/main

class TFT
{
	TFT_t dev;

protected:

	enum class Models
	{
		ILI9225 	= 0x9225,
		ILI9225G 	= 0x9226,
		ILI9340 	= 0x9340,
		ILI9341		= 0x9341,
		ST7735		= 0x7735,
		ST7796		= 0x7796,
	};

	uint16_t Width = 0;
	uint16_t Height = 0;
	uint16_t Offsetx = 0;
	uint16_t Offsety = 0;
	Models Model = Models::ST7735;


public:

	TFT()
	{

	}

	void Init(	int16_t GPIO_CS 	= 15,
				int16_t GPIO_DC 	= 4,
				int16_t GPIO_RESET 	= 2,
				int16_t GPIO_BL 	= 5,
				int16_t GPIO_MOSI 	= 16,
				int16_t GPIO_SCK 	= 17,
				int16_t GPIO_MISO 	= -1)
	{
		spi_master_init(&dev, GPIO_CS, GPIO_DC, GPIO_RESET, GPIO_BL, GPIO_SCK, GPIO_MOSI, GPIO_MISO);
		lcdInit(&dev, (uint16_t)Model, Width, Height, Offsetx, Offsety);
	}


	static TFT Get_ILI9341()
	{
		TFT tft;

		tft.Width = 240;
		tft.Height = 320;
		tft.Offsetx = 0;
		tft.Offsety = 0;
		tft.Model = Models::ILI9341;

		return tft;
	};






	void DrawPixel(uint16_t x, uint16_t y, Color color)
	{
		lcdDrawPixel( & dev, x, y, color.Get_RGB565());
	}

	void DrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
	{
		lcdDrawFillRect( & dev, x1, y1, x2, y2, color.Get_RGB565());
	}

	void FillScreen(Color color)
	{
		lcdFillScreen( & dev, color.Get_RGB565());
	}

	void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
	{
		lcdDrawLine( & dev, x1, y1, x2, y2, color.Get_RGB565());
	}

	void DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
	{
		lcdDrawRect( & dev, x1, y1, x2, y2, color.Get_RGB565());
	}


	int DrawChar(Font *fxs, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color)
	{
		uint16_t xx,yy,bit,ofs;
		unsigned char fonts[128]; // font pattern
		unsigned char pw, ph;
		int h,w;
		uint16_t mask;
		bool rc;

		rc = fxs->GetFontx(ascii, fonts, &pw, &ph);
		if (!rc) return 0;

		uint16_t xd1 = 0;
		uint16_t yd1 = 0;
		uint16_t xd2 = 0;
		uint16_t yd2 = 0;
		uint16_t xss = 0;
		uint16_t yss = 0;
		uint16_t xsd = 0;
		uint16_t ysd = 0;
		int next = 0;
		uint16_t x0  = 0;
		uint16_t x1  = 0;
		uint16_t y0  = 0;
		uint16_t y1  = 0;
		if (dev._font_direction == 0)
		{
			xd1 = +1;
			yd1 = +1; //-1;
			xd2 =  0;
			yd2 =  0;
			xss =  x;
			yss =  y - (ph - 1);
			xsd =  1;
			ysd =  0;
			next = x + pw;
			x0  = x;
			y0  = y - (ph-1);
			x1  = x + (pw-1);
			y1  = y;
		}
		else if (dev._font_direction == 2)
		{
			xd1 = -1;
			yd1 = -1; //+1;
			xd2 =  0;
			yd2 =  0;
			xss =  x;
			yss =  y + ph + 1;
			xsd =  1;
			ysd =  0;
			next = x - pw;
			x0  = x - (pw-1);
			y0  = y;
			x1  = x;
			y1  = y + (ph-1);
		}
		else if (dev._font_direction == 1)
		{
			xd1 =  0;
			yd1 =  0;
			xd2 = -1;
			yd2 = +1; //-1;
			xss =  x + ph;
			yss =  y;
			xsd =  0;
			ysd =  1;
			next = y + pw; //y - pw;
			x0  = x;
			y0  = y;
			x1  = x + (ph-1);
			y1  = y + (pw-1);
		}
		else if (dev._font_direction == 3)
		{
			xd1 =  0;
			yd1 =  0;
			xd2 = +1;
			yd2 = -1; //+1;
			xss =  x - (ph - 1);
			yss =  y;
			xsd =  0;
			ysd =  1;
			next = y - pw; //y + pw;
			x0  = x - (ph-1);
			y0  = y - (pw-1);
			x1  = x;
			y1  = y;
		}

	    if (dev._font_fill)
	    {
	    	lcdDrawFillRect(&dev, x0, y0, x1, y1, dev._font_fill_color);
	    }

		int bits;
		ofs = 0;
		yy = yss;
		xx = xss;
		for(h=0;h<ph;h++)
		{
			if(xsd)
				xx = xss;
			if(ysd)
				yy = yss;

			bits = pw;
			for(w=0;w<((pw+4)/8);w++)
			{
				mask = 0x80;
				for(bit=0;bit<8;bit++)
				{
					bits--;
					if (bits < 0)
						continue;

					if (fonts[ofs] & mask)
						lcdDrawPixel(&dev, xx, yy, color);
					if (h == (ph-2) && dev._font_underline)
						lcdDrawPixel(&dev, xx, yy, dev._font_underline_color);
					if (h == (ph-1) && dev._font_underline)
						lcdDrawPixel(&dev, xx, yy, dev._font_underline_color);
					xx = xx + xd1;
					yy = yy + yd2;
					mask = mask >> 1;
				}
				ofs++;
			}
			yy = yy + yd1;
			xx = xx + xd2;
		}

		if (next < 0) next = 0;
		return next;
	}




	int DrawString(Font *fx, uint16_t x, uint16_t y, const char * ascii, Color color)
	{
		int length = strlen((char *)ascii);
		for(int i=0;i<length;i++)
		{
			if (dev._font_direction == 0)
				x = DrawChar(fx, x, y, ascii[i], color.Get_RGB565());
			if (dev._font_direction == 1)
				y = DrawChar( fx, x, y, ascii[i], color.Get_RGB565());
			if (dev._font_direction == 2)
				x = DrawChar( fx, x, y, ascii[i], color.Get_RGB565());
			if (dev._font_direction == 3)
				y = DrawChar( fx, x, y, ascii[i], color.Get_RGB565());
		}
		if (dev._font_direction == 0) return x;
		if (dev._font_direction == 2) return x;
		if (dev._font_direction == 1) return y;
		if (dev._font_direction == 3) return y;
		return 0;
	}

	int DrawString(Font *fx, uint16_t x, uint16_t y, std::string ascii, Color color)
	{
		return DrawString(fx, x, y, ascii.c_str(), color);
	}

	void test()
	{
		FontxFile fx16G[2];
		InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	}


	void SetFontFill(Color color)
	{
		lcdSetFontFill( & dev, color.Get_RGB565());
	}

	void UnsetFontFill()
	{
		lcdUnsetFontFill( & dev);
	}


	/*



	void WriteRegisterWord(uint16_t addr, uint16_t data)
	{
		lcdWriteRegisterWord( & dev, addr, data);
	}

	void WriteRegisterByte(uint8_t addr, uint16_t data)
	{
		lcdWriteRegisterByte( & dev,  addr, data);
	}



	void DrawMultiPixels(uint16_t x, uint16_t y, uint16_t size, uint16_t * colors)
	{
		lcdDrawMultiPixels( & dev, x, y, size, colors);
	}



	void DisplayOff()
	{
		lcdDisplayOff( & dev);
	}

	void DisplayOn()
	{
		lcdDisplayOn( & dev);
	}

	void InversionOff()
	{
		lcdInversionOff( & dev);
	}

	void InversionOn()
	{
		lcdInversionOn( & dev);
	}

	void BGRFilter()
	{
		lcdBGRFilter( & dev);
	}



	void DrawRectAngle(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color)
	{
		lcdDrawRectAngle( & dev, xc, yc, w, h, angle, color);
	}

	void DrawTriangle(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color)
	{
		lcdDrawTriangle( & dev, xc, yc, w, h, angle, color);
	}

	void DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
	{
		lcdDrawCircle( & dev, x0, y0, r, color);
	}

	void DrawFillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
	{
		lcdDrawFillCircle( & dev, x0, y0, r, color);
	}

	void DrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color)
	{
		lcdDrawRoundRect( & dev, x1, y1, x2, y2, r, color);
	}

	void DrawArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color)
	{
		lcdDrawArrow( & dev, x0, y0, x1, y1, w, color);
	}

	void DrawFillArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color)
	{
		lcdDrawFillArrow( & dev, x0, y0, x1, y1, w, color);
	}

	int DrawChar(FontxFile *fx, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color)
	{
		return lcdDrawChar( & dev, fx, x, y,  ascii, color);
	}

	int DrawString(FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color)
	{
		return lcdDrawString( & dev, fx, x, y, ascii, color);
	}

	void SetFontDirection(uint16_t dir)
	{
		lcdSetFontDirection( & dev, dir);
	}



	void SetFontUnderLine(uint16_t color)
	{
		lcdSetFontUnderLine( & dev, color);
	}

	void UnsetFontUnderLine()
	{
		lcdUnsetFontUnderLine( & dev);
	}

	void BacklightOff()
	{
		lcdBacklightOff( & dev);
	}

	void BacklightOn()
	{
		lcdBacklightOn( & dev);
	}

	void SetScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa)
	{
		lcdSetScrollArea( & dev, tfa, vsa, bfa);
	}

	void ResetScrollArea(uint16_t vsa)
	{
		lcdResetScrollArea( & dev, vsa);
	}

	void Scroll(uint16_t vsp)
	{
		lcdScroll( & dev, vsp);
	}

*/

};




#endif /* MAIN_DISPLAY_TFT_H_ */
