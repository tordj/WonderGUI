/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_gfxutil.h>
#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_patches.h>
#include <wg_gfxbase.h>

#include <algorithm>

namespace wg
{

	uint8_t Util::_limitUint8Table[768] = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

										0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
										0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
										0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
										0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
										0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
										0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
										0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
										0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
										0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
										0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
										0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
										0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
										0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
										0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
										0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
										0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,

										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
										0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };



/*
	LICENSE NOTE!

	Functions squareRoot() and powerOfTen() is from www.codeproject.com and licensed
	under The Code Project Open License (CPOL). See https://www.codeproject.com/info/cpol10.aspx
*/

// powerOfTen function is needed for squareRoot function (IMPORTANT)

double Util::squareRoot(double a)
{
	/*
		find more detail of this method on wiki methods_of_computing_square_roots

		*** Babylonian method cannot get exact zero but approximately value of the square_root
	*/
	double z = a;
	double rst = 0.0;
	int max = 8;	// to define maximum digit
	int i;
	double j = 1.0;
	for(i = max ; i > 0 ; i--){
		// value must be bigger then 0
		if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
		{
			while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
			{
				j++;
				if(j >= 10) break;

			}
			j--; //correct the extra value by minus one to j
			z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)); //find value of z

			rst += j * powerOfTen(i);	// find sum of a

			j = 1.0;


		}

	}

	for(i = 0 ; i >= 0 - max ; i--){
		if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
		{
			while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
			{
				j++;
				if(j >= 10) break;
			}
			j--;
			z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)); //find value of z

			rst += j * powerOfTen(i);	// find sum of a
			j = 1.0;
		}
	}
	// find the number on each digit
	return rst;
}

double Util::powerOfTen(int num)
{
	double rst = 1.0;
	if(num >= 0)
	{
		for(int i = 0; i < num ; i++)
			rst *= 10.0;
	}
	else
	{
		for(int i = 0; i < (0 - num ); i++)
			rst *= 0.1;
	}
	return rst;
}

int Util::gcd(int a, int b) 
{
	return b == 0 ? a : Util::gcd(b, a % b);
}


	//____ pixelFormatToDescription() _____________________________________________________

	bool Util::pixelFormatToDescription( PixelFormat format, PixelDescription& output )
	{
		switch( format )
		{
			case PixelFormat::BGR_8:
				format = GfxBase::defaultToSRGB() ? PixelFormat::BGR_8_sRGB : PixelFormat::BGR_8_linear;
			case PixelFormat::BGR_8_sRGB:
			case PixelFormat::BGR_8_linear:
				output.format = format;
				output.bits = 24;
				output.bIndexed = false;
				output.bLinear = format == PixelFormat::BGR_8_linear ? true : false;
				output.bBigEndian = false;

				output.R_bits = 8;
				output.G_bits = 8;
				output.B_bits = 8;
				output.A_bits = 0;

				output.R_loss = 0;
				output.G_loss = 0;
				output.B_loss = 0;
				output.A_loss = 8;

				output.R_mask = 0xFF0000;
				output.G_mask = 0xFF00;
				output.B_mask = 0xFF;
				output.A_mask = 0x0;

				output.R_shift = 16;
				output.G_shift = 8;
				output.B_shift = 0;
				output.A_shift = 0;

				return true;

			case PixelFormat::BGRX_8:
				format = GfxBase::defaultToSRGB() ? PixelFormat::BGRX_8_sRGB : PixelFormat::BGRX_8_linear;
			case PixelFormat::BGRX_8_sRGB:
			case PixelFormat::BGRX_8_linear:
				output.format = format;
				output.bits = 32;
				output.bIndexed = false;
				output.bLinear = format == PixelFormat::BGRX_8_linear ? true : false;
				output.bBigEndian = false;

				output.R_bits = 8;
				output.G_bits = 8;
				output.B_bits = 8;
				output.A_bits = 0;

				output.R_loss = 0;
				output.G_loss = 0;
				output.B_loss = 0;
				output.A_loss = 8;

				output.A_mask = 0x00000000;
				output.R_mask = 0xFF0000;
				output.G_mask = 0xFF00;
				output.B_mask = 0xFF;

				output.A_shift = 0;
				output.R_shift = 16;
				output.G_shift = 8;
				output.B_shift = 0;

				return true;


			case PixelFormat::BGRA_8:
				format = GfxBase::defaultToSRGB() ? PixelFormat::BGRA_8_sRGB : PixelFormat::BGRA_8_linear;
			case PixelFormat::BGRA_8_sRGB:
			case PixelFormat::BGRA_8_linear:
				output.format = format;
				output.bits = 32;
				output.bIndexed = false;
				output.bLinear = format == PixelFormat::BGRA_8_linear ? true : false;
				output.bBigEndian = false;

				output.R_bits = 8;
				output.G_bits = 8;
				output.B_bits = 8;
				output.A_bits = 8;

				output.R_loss = 0;
				output.G_loss = 0;
				output.B_loss = 0;
				output.A_loss = 0;

				output.A_mask = 0xFF000000;
				output.R_mask = 0xFF0000;
				output.G_mask = 0xFF00;
				output.B_mask = 0xFF;

				output.A_shift = 24;
				output.R_shift = 16;
				output.G_shift = 8;
				output.B_shift = 0;

				return true;

			case PixelFormat::BGRA_4_linear:
				output.format = format;
				output.bits = 16;
				output.bIndexed = false;
				output.bLinear = true;
				output.bBigEndian = false;

				output.R_bits = 4;
				output.G_bits = 4;
				output.B_bits = 4;
				output.A_bits = 4;

				output.R_loss = 4;
				output.G_loss = 4;
				output.B_loss = 4;
				output.A_loss = 4;

				output.A_mask = 0xF000;
				output.R_mask = 0x0F00;
				output.G_mask = 0x00F0;
				output.B_mask = 0x000F;

				output.A_shift = 12;
				output.R_shift = 8;
				output.G_shift = 4;
				output.B_shift = 0;

				return true;

			case PixelFormat::BGR_565_linear:
				output.format = format;
				output.bits = 16;
				output.bIndexed = false;
				output.bLinear = true;
				output.bBigEndian = false;

				output.R_bits = 5;
				output.G_bits = 6;
				output.B_bits = 5;
				output.A_bits = 0;

				output.R_loss = 3;
				output.G_loss = 2;
				output.B_loss = 3;
				output.A_loss = 8;

				output.A_mask = 0x0000;
				output.R_mask = 0xF800;
				output.G_mask = 0x07E0;
				output.B_mask = 0x001F;

				output.A_shift = 0;
				output.R_shift = 11;
				output.G_shift = 5;
				output.B_shift = 0;

				return true;

			case PixelFormat::RGB_565_bigendian:
				output.format = format;
				output.bits = 16;
				output.bIndexed = false;
				output.bLinear = true;
				output.bBigEndian = true;

				output.R_bits = 5;
				output.G_bits = 6;
				output.B_bits = 5;
				output.A_bits = 0;

				output.R_loss = 3;
				output.G_loss = 2;
				output.B_loss = 3;
				output.A_loss = 8;

				output.A_mask = 0x0000;
				output.R_mask = 0x001F;
				output.G_mask = 0x07E0;
				output.B_mask = 0xF100;

				output.A_shift = 0;
				output.R_shift = 0;
				output.G_shift = 5;
				output.B_shift = 11;

				return true;



			case PixelFormat::A_8:
				output.format = format;
				output.bits = 8;
				output.bIndexed = false;
				output.bLinear = false;
				output.bBigEndian = false;

				output.R_bits = 0;
				output.G_bits = 0;
				output.B_bits = 0;
				output.A_bits = 8;

				output.R_loss = 8;
				output.G_loss = 8;
				output.B_loss = 8;
				output.A_loss = 0;

				output.R_mask = 0x00;
				output.G_mask = 0x00;
				output.B_mask = 0x00;
				output.A_mask = 0xFF;

				output.R_shift = 0;
				output.G_shift = 0;
				output.B_shift = 0;
				output.A_shift = 0;
				return true;

			case PixelFormat::CLUT_8:
				format = GfxBase::defaultToSRGB() ? PixelFormat::CLUT_8_sRGB : PixelFormat::CLUT_8_linear;
			case PixelFormat::CLUT_8_sRGB:
			case PixelFormat::CLUT_8_linear:
				output.format = format;
				output.bits = 8;
				output.bIndexed = true;
				output.bLinear = format == PixelFormat::CLUT_8_linear ? true : false;
				output.bBigEndian = false;

				output.R_bits = 8;
				output.G_bits = 8;
				output.B_bits = 8;
				output.A_bits = 8;

				output.R_loss = 0;
				output.G_loss = 0;
				output.B_loss = 0;
				output.A_loss = 0;

				output.R_mask = 0x00;
				output.G_mask = 0x00;
				output.B_mask = 0x00;
				output.A_mask = 0x00;

				output.R_shift = 0;
				output.G_shift = 0;
				output.B_shift = 0;
				output.A_shift = 0;
				return true;

			default:
				output.format = PixelFormat::Undefined;
				output.bits = 0;
				output.bIndexed = false;
				output.bLinear = false;
				output.bBigEndian = false;

				output.R_bits = 0;
				output.G_bits = 0;
				output.B_bits = 0;
				output.A_bits = 0;

				output.R_loss = 0;
				output.G_loss = 0;
				output.B_loss = 0;
				output.A_loss = 0;

				output.R_mask = 0;
				output.G_mask = 0;
				output.B_mask = 0;
				output.A_mask = 0;

				output.R_shift = 0;
				output.G_shift = 0;
				output.B_shift = 0;
				output.A_shift = 0;
				return false;
		}
	}

	//____ pixelDescriptionToFormat() _________________________________________

	PixelFormat	Util::pixelDescriptionToFormat(const PixelDescription& description)
	{
		switch (description.bits)
		{

		case 8:

			if (description.A_mask == 0xFF)
				return PixelFormat::A_8;
			if (description.bIndexed && description.bLinear)
				return PixelFormat::CLUT_8_linear;
			if (description.bIndexed && !description.bLinear)
				return PixelFormat::CLUT_8_sRGB;

			break;

		case 16:

#if WG_IS_LITTLE_ENDIAN
			if (description.A_mask == 0x00 && description.R_mask == 0xF800 && description.G_mask == 0x07E0 && description.B_mask == 0x001F)
				return description.bLinear ? PixelFormat::BGR_565_linear : PixelFormat::Undefined;

			if (description.A_mask == 0xF000 && description.R_mask == 0x0F00 && description.G_mask == 0x00F0 && description.B_mask == 0x000F)
				return description.bLinear ? PixelFormat::BGRA_4_linear : PixelFormat::Undefined;
#else
			if (description.A_mask == 0x00 && description.R_mask == 0xF8 && description.G_mask == 0x0E007 && description.B_mask == 0x1F00)
				return description.bLinear ? PixelFormat::BGR_565_linear : PixelFormat::Undefined;

			if (description.A_mask == 0x00F0 && description.R_mask == 0x000F && description.G_mask == 0xF000 && description.B_mask == 0x0F00)
				return description.bLinear ? PixelFormat::BGRA_4_linear : PixelFormat::Undefined;
#endif
			break;

		case 24:

#if WG_IS_LITTLE_ENDIAN
			if (description.A_mask == 0x00000000 && description.R_mask == 0xFF0000 && description.G_mask == 0xFF00 && description.B_mask == 0xFF)
				return description.bLinear ? PixelFormat::BGR_8_linear : PixelFormat::BGR_8_sRGB;
#else
			if (description.A_mask == 0x00000000 && description.R_mask == 0xFF00 && description.G_mask == 0xFF0000 && description.B_mask == 0xFF000000)
				return description.bLinear ? PixelFormat::BGR_8_linear : PixelFormat::BGR_8_sRGB;
#endif
			break;


		case 32:

#if WG_IS_LITTLE_ENDIAN
			if (description.A_mask == 0xFF000000 && description.R_mask == 0xFF0000 && description.G_mask == 0xFF00 && description.B_mask == 0xFF)
				return description.bLinear ? PixelFormat::BGRA_8_linear : PixelFormat::BGRA_8_sRGB;

			if (description.A_mask == 0x00000000 && description.R_mask == 0xFF0000 && description.G_mask == 0xFF00 && description.B_mask == 0xFF)
				return description.bLinear ? PixelFormat::BGRX_8_linear : PixelFormat::BGRX_8_sRGB;
#else
			if (description.A_mask == 0xFF && description.R_mask == 0xFF00 && description.G_mask == 0xFF0000 && description.B_mask == 0xFF000000)
				return description.bLinear ? PixelFormat::BGRA_8_linear : PixelFormat::BGRA_8_sRGB;

			if (description.A_mask == 0x00000000 && description.R_mask == 0xFF00 && description.G_mask == 0xFF0000 && description.B_mask == 0xFF000000)
				return description.bLinear ? PixelFormat::BGRX_8_linear : PixelFormat::BGRX_8_sRGB;
#endif
			break;
		}

		return PixelFormat::Undefined;
		
	}


	//____ mostSignificantBit() _______________________________________________

	// Uses the de Bruijn algorithm for finding the most significant bit in an integer.

	uint32_t Util::mostSignificantBit(uint32_t value)
	{
		static const int MultiplyDeBruijnBitPosition[32] =
		{
			0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
			8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
		};

		value |= value >> 1; // first round down to one less than a power of 2
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;

		return MultiplyDeBruijnBitPosition[(uint32_t)(value * 0x07C4ACDDU) >> 27];
	}


} // namespace wg
