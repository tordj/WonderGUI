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
#include <wg_pixeltools.h>
#include <wg_gfxutil.h>
#include <wg_gfxbase.h>

#include <cstring>

namespace wg { namespace PixelTools
{

const uint8_t conv_2_linear_to_8_sRGB[4] 	= {0, 155, 212, 255 };

const uint8_t conv_3_linear_to_8_sRGB[8] 	= {0, 105, 144, 173, 198, 219, 238, 255};

const uint8_t conv_4_linear_to_8_sRGB[16] 	= {0, 74, 102, 123, 140, 155, 168, 180, 192, 202,
	212, 221, 230, 239, 247, 255};

const uint8_t conv_5_linear_to_8_sRGB[32] 	= {0, 54, 73, 88, 101, 111, 121, 130, 138, 145, 152,
	159, 166, 172, 178, 183, 189, 194, 199, 204, 209, 214, 218, 223, 227, 231, 235, 239, 243, 247,
	251, 255};

const uint8_t conv_6_linear_to_8_sRGB[64] 	= {0, 39, 53, 64, 73, 81, 88, 94, 100, 105, 110, 115,
	120, 124, 129, 133, 137, 141, 144, 148, 151, 155, 158, 161, 164, 168, 171, 173, 176, 179, 182,
	185, 187, 190, 193, 195, 198, 200, 203, 205, 207, 210, 212, 214, 217, 219, 221, 223, 225, 227,
	230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 249, 251, 253, 255 };

const uint8_t conv_7_linear_to_8_sRGB[128] 	= {0, 28, 39, 46, 53, 59, 64, 68, 73, 77, 80, 84, 87,
	90, 94, 97, 99, 102, 105, 108, 110, 113, 115, 117, 120, 122, 124, 126, 128, 130, 132, 134, 136,
	138, 140, 142, 144, 146, 147, 149, 151, 153, 154, 156, 158, 159, 161, 162, 164, 165, 167, 168,
	170, 171, 173, 174, 176, 177, 179, 180, 181, 183, 184, 185, 187, 188, 189, 191, 192, 193, 195,
	196, 197, 198, 199, 201, 202, 203, 204, 206, 207, 208, 209, 210, 211, 212, 214, 215, 216, 217,
	218, 219, 220, 221, 222, 223, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237,
	238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 249, 250, 251, 252, 253, 254, 255 };

const uint8_t conv_8_linear_to_8_sRGB[256] = {0, 21, 28, 34, 39, 43, 46, 50, 53, 56, 59, 61, 64, 66, 68, 70, 72,
	74, 76, 78, 80, 82, 84, 85, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 102, 103, 105, 106, 107,
	109, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 122, 123, 124, 125, 126, 127, 128, 129,
	130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 146, 147,
	148, 149, 150, 151, 151, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 160, 161, 162, 163,
	164, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 177,
	178, 178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185, 186, 186, 187, 188, 188, 189, 190,
	190, 191, 192, 192, 193, 194, 194, 195, 195, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202,
	202, 203, 203, 204, 205, 205, 206, 206, 207, 207, 208, 209, 209, 210, 210, 211, 212, 212, 213,
	213, 214, 214, 215, 215, 216, 217, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 223, 223,
	224, 224, 225, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 232, 233,
	233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242,
	243, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 251, 251,
	252, 252, 253, 253, 254, 254, 255, 255 };


const uint8_t conv_2_sRGB_to_8_linear[4] 	= {0, 23, 105, 255};

const uint8_t conv_3_sRGB_to_8_linear[8] 	= {0, 4, 16, 40, 74, 122, 182, 255};

const uint8_t conv_4_sRGB_to_8_linear[16] = {0, 1, 3, 7, 14, 23, 34, 48, 64, 83, 105, 129, 156, 186, 219, 255};

const uint8_t conv_5_sRGB_to_8_linear[32] = { 0, 0, 1, 1, 3, 5, 7, 10, 13, 17, 21, 26, 32, 38, 44, 52,
	60, 68,	77, 87, 97, 108, 120, 132, 145, 159, 173, 188, 204, 220, 237, 255};

const uint8_t conv_6_sRGB_to_8_linear[64] = { 0, 0, 0, 0, 1, 1, 1, 2, 3, 4, 4, 5, 7, 8, 9, 11, 13, 14,
	16, 18, 20, 23, 25, 28, 31, 33, 36, 40, 43, 46, 50, 54, 57, 61, 66, 70, 74, 79, 84, 89, 94,
	99, 105, 110, 116, 122, 128, 134, 140, 147, 153, 160, 167, 174, 182, 189, 197, 205, 213, 221,
	229, 238, 246, 255};

const uint8_t conv_7_sRGB_to_8_linear[128] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3,
	3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24,
	25, 26, 27, 29, 30, 31, 33, 34, 36, 37, 39, 40, 42, 44, 45, 47, 49, 51, 53, 55, 56, 58, 60, 62,
	65, 67, 69, 71, 73, 75, 78, 80, 82, 85, 87, 90, 92, 95, 97, 100, 103, 105, 108, 111, 114, 117,
	120, 122, 125, 128, 132, 135, 138, 141, 144, 147, 151, 154, 157, 161, 164, 168, 171, 175, 179,
	182, 186, 190, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 238, 242, 246, 251, 255};

const uint8_t conv_8_sRGB_to_8_linear[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8,
	8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
	19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33,
	34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53,
	54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 77, 78,
	79, 81, 82, 83, 84, 85, 87, 88, 89, 90, 91, 93, 94, 95, 97, 98, 99, 100, 102, 103, 105, 106, 107,
	109, 110, 111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 127, 129, 130, 132, 133, 135,
	137, 138, 140, 141, 143, 145, 146, 148, 149, 151, 153, 154, 156, 158, 159, 161, 163, 165, 166,
	168, 170, 172, 173, 175, 177, 179, 181, 182, 184, 186, 188, 190, 192, 194, 196, 197, 199, 201,
	203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227, 229, 231, 234, 236, 238, 240,
	242, 244, 246, 248, 251, 253, 255 };


const uint8_t conv_0_to_8_straight[1] 	= { 0xff };
const uint8_t conv_1_to_8_straight[2] 	= { 0, 0xff };
const uint8_t conv_2_to_8_straight[4] 	= {	0, 0x55, 0xaa, 0xff };
const uint8_t conv_3_to_8_straight[8] 	= { 0, 0x24, 0x48, 0x6d, 0x91, 0xb6, 0xda, 0xff };

const uint8_t conv_4_to_8_straight[16] = { 0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
	0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

const uint8_t conv_5_to_8_straight[32] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41,
	0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b, 0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5,
	0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff };

const uint8_t conv_6_to_8_straight[64] 	= {	0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
	0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61,
	0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d, 0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1,
	0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe, 0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2,
	0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff };

const uint8_t conv_7_to_8_straight[128] = { 0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14,
	0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34,
	0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52,
	0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70,
	0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e,
	0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac,
	0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe, 0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca,
	0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8,
	0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xff };

const uint8_t conv_8_to_8_straight[256] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
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
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };


const uint8_t * conv_srgb_to_linear_tabs[9] = { conv_0_to_8_straight, conv_1_to_8_straight,
	conv_2_sRGB_to_8_linear, conv_3_sRGB_to_8_linear, conv_4_sRGB_to_8_linear,
	conv_5_sRGB_to_8_linear, conv_6_sRGB_to_8_linear, conv_7_sRGB_to_8_linear, conv_8_sRGB_to_8_linear };

const uint8_t * conv_linear_to_sRGB_tabs[9] = { conv_0_to_8_straight, conv_1_to_8_straight, conv_2_linear_to_8_sRGB,
	conv_3_linear_to_8_sRGB, conv_4_linear_to_8_sRGB, conv_5_linear_to_8_sRGB,
	conv_6_linear_to_8_sRGB, conv_7_linear_to_8_sRGB, conv_8_linear_to_8_sRGB };

const uint8_t * conv_straight_tabs[9] = { conv_0_to_8_straight, conv_1_to_8_straight, conv_2_to_8_straight,
	conv_3_to_8_straight, conv_4_to_8_straight, conv_5_to_8_straight,
	conv_6_to_8_straight, conv_7_to_8_straight, conv_8_to_8_straight };


static void readBGRA8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* (uint32_t*) pDst = *(uint32_t*) pSrc;
		pSrc += 4;
		pDst += 4;
	}
}

static void readConvBGRA8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	uint8_t * pRGBConvTab = (uint8_t*) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = * pSrc++;
	}
}

static void readBGRX8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* (uint32_t*) pDst = (*(uint32_t*) pSrc) | 0xFF000000;
		pSrc += 4;
		pDst += 4;
	}
}

static void readConvBGRX8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	uint8_t * pRGBConvTab = (uint8_t*) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = 255;
	}
}


static void readBGR8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = *pSrc++;
		* pDst++ = *pSrc++;
		* pDst++ = *pSrc++;
		* pDst++ = 255;
	}
}

static void readConvBGR8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	uint8_t * pRGBConvTab = (uint8_t *) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = pRGBConvTab[*pSrc++];
		* pDst++ = 255;
	}
}

static void readAlpha8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = ((*pSrc++) << 24) | 0x00FFFFFF;
	}
}

static void readIndex8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	uint32_t * pPalette = (uint32_t*) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* (uint32_t*) pDst = pPalette[*pSrc++];
		pDst += 4;
	}
}

static void readIndex16(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* p1, const void* p2)
{
	uint32_t* pPalette = (uint32_t*)p1;

	for (int i = 0; i < nbPixels; i++)
	{

		*(uint32_t*)pDst = pPalette[*((uint16_t*)pSrc)];
		pDst += 4;
		pSrc += 2;
	}
}


static void readBGRA_4( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	uint8_t * pConvTab = (uint8_t*) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		uint8_t bg = * pSrc++;
		* pDst++ = pConvTab[bg&0xF];
		* pDst++ = pConvTab[bg>>4];

		uint8_t ra = * pSrc++;
		* pDst++ = pConvTab[ra&0xF];
		* pDst++ = conv_4_to_8_straight[ra>>4];
	}
}


static void readBGR_565( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2  )
{
	uint8_t * pConvTab5 = (uint8_t*) p1;
	uint8_t * pConvTab6 = (uint8_t*) p2;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		uint16_t bgr = *(uint16_t*) pSrc;

		* pDst++ = pConvTab5[bgr & 0x1F];
		* pDst++ = pConvTab6[(bgr >> 5)  & 0x3F];
		* pDst++ = pConvTab5[bgr >> 11];
		* pDst++ = 255;
		pSrc += 2;
	}
}

static void readRGB_565_bigendian( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void * p1, const void * p2 )
{
	uint8_t * pConvTab5 = (uint8_t*) p1;
	uint8_t * pConvTab6 = (uint8_t*) p2;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		uint16_t rgb = *(uint16_t*) pSrc;
		rgb = (rgb >> 8 | rgb << 8);

		* pDst++ = pConvTab5[rgb >> 11];
		* pDst++ = pConvTab6[(rgb >> 5)  & 0x3F];
		* pDst++ = pConvTab5[rgb & 0x1F];
		* pDst++ = 255;
		pSrc += 2;
	}
}

struct ShiftReadConvTab
{
	int maskR;
	int maskG;
	int maskB;
	int maskA;
	int shiftR;
	int shiftG;
	int shiftB;
	int shiftA;
	const uint8_t* pConvR;
	const uint8_t* pConvG;
	const uint8_t* pConvB;
	const uint8_t* pConvA;
};


static void shiftReadConv_32bit( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, const void* pConvTab, const void* pDummy)
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint32_t rgba = *(uint32_t*)pSrc;

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = t.pConvA[(rgba & t.maskA) >> t.shiftA];
		pSrc += 4;
	}
}

static void shiftReadConv_32bit_swap(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void * pConvTab, const void * pDummy )
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint32_t rgba = Util::endianSwap(*(uint32_t*)pSrc);

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = t.pConvA[(rgba & t.maskA) >> t.shiftA];
		pSrc += 4;
	}
}


static void shiftReadConv_24bit_le(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* pConvTab, const void* pDummy)
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint32_t rgba = pSrc[0] + (uint32_t(pSrc[1]) << 8) + (uint32_t(pSrc[2]) << 16);

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = 255;
		pSrc += 3;
	}
}

static void shiftReadConv_24bit_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* pConvTab, const void* pDummy)
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint32_t rgba = pSrc[2] + (uint32_t(pSrc[1]) << 8) + (uint32_t(pSrc[0]) << 16);

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = 255;
		pSrc += 3;
	}
}

static void shiftReadConv_16bit(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* pConvTab, const void* pDummy)
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint16_t rgba = *(uint16_t*)pSrc;

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = t.pConvA[(rgba & t.maskA) >> t.shiftA];
		pSrc += 2;
	}
}

static void shiftReadConv_16bit_swap(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* pConvTab, const void* pDummy)
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint16_t rgba = *(uint16_t*)pSrc;
		rgba = (rgba >> 8 | rgba << 8);

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = t.pConvA[(rgba & t.maskA) >> t.shiftA];
		pSrc += 2;
	}
}

static void shiftReadConv_8bit(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* pConvTab, const void* pDummy)
{
	ShiftReadConvTab& t = *(ShiftReadConvTab*)pConvTab;

	for (int i = 0; i < nbPixels; i++)
	{
		uint8_t rgba = * pSrc++;

		*pDst++ = t.pConvB[(rgba & t.maskB) >> t.shiftB];
		*pDst++ = t.pConvG[(rgba & t.maskG) >> t.shiftG];
		*pDst++ = t.pConvR[(rgba & t.maskR) >> t.shiftR];
		*pDst++ = t.pConvA[(rgba & t.maskA) >> t.shiftA];
	}
}

static void readConv_planes_8i_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;
	
	while(nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	plane3 = (pSrc[4] << 8) + pSrc[5];
		uint16_t	plane4 = (pSrc[6] << 8) + pSrc[7];
		uint16_t	plane5 = (pSrc[8] << 8) + pSrc[9];
		uint16_t	plane6 = (pSrc[10] << 8) + pSrc[11];
		uint16_t	plane7 = (pSrc[12] << 8) + pSrc[13];
		uint16_t	plane8 = (pSrc[14] << 8) + pSrc[15];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14) | ((plane3 & 0x80) >> 13) | ((plane4 & 0x80) >> 12)
				| ((plane5 & 0x80) >> 11) | ((plane6 & 0x80) >> 10) | ((plane7 & 0x80) >> 9) | ((plane8 & 0x80) >> 8);
			*(uint32_t*)pDst = pPalette[index];
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;
			plane4 <<= 1;
			plane5 <<= 1;
			plane6 <<= 1;
			plane7 <<= 1;
			plane8 <<= 1;
		}

		pSrc += 16;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_5i_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	plane3 = (pSrc[4] << 8) + pSrc[5];
		uint16_t	plane4 = (pSrc[6] << 8) + pSrc[7];
		uint16_t	plane5 = (pSrc[8] << 8) + pSrc[9];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14) | ((plane3 & 0x80) >> 13) | ((plane4 & 0x80) >> 12) | ((plane5 & 0x80) >> 11);
			*(uint32_t*)pDst = pPalette[index];
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;
			plane4 <<= 1;
			plane5 <<= 1;
		}

		pSrc += 10;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_4i_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	plane3 = (pSrc[4] << 8) + pSrc[5];
		uint16_t	plane4 = (pSrc[6] << 8) + pSrc[7];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14) | ((plane3 & 0x80) >> 13) | ((plane4 & 0x80) >> 12);
			*(uint32_t*)pDst = pPalette[index];
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;
			plane4 <<= 1;
		}

		pSrc += 8;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_2i_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14);
			*(uint32_t*)pDst = pPalette[index];
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
		}

		pSrc += 4;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_1i_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15);
			*(uint32_t*)pDst = pPalette[index];
			pDst += 4;

			plane1 <<= 1;
		}

		pSrc += 2;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_8i_a1_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	plane3 = (pSrc[4] << 8) + pSrc[5];
		uint16_t	plane4 = (pSrc[6] << 8) + pSrc[7];
		uint16_t	plane5 = (pSrc[8] << 8) + pSrc[9];
		uint16_t	plane6 = (pSrc[10] << 8) + pSrc[11];
		uint16_t	plane7 = (pSrc[12] << 8) + pSrc[13];
		uint16_t	plane8 = (pSrc[14] << 8) + pSrc[15];
		uint16_t	planeA = (pSrc[16] << 8) + pSrc[17];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14) | ((plane3 & 0x80) >> 13) | ((plane4 & 0x80) >> 12)
				| ((plane5 & 0x80) >> 11) | ((plane6 & 0x80) >> 10) | ((plane7 & 0x80) >> 9) | ((plane8 & 0x80) >> 8);
			
			uint32_t argb = pPalette[index];

			argb |= conv_1_to_8_straight[((planeA & 0x80) >> 15)];

			*(uint32_t*)pDst = argb;
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;
			plane4 <<= 1;
			plane5 <<= 1;
			plane6 <<= 1;
			plane7 <<= 1;
			plane8 <<= 1;
			planeA <<= 1;
		}

		pSrc += 18;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_5i_a1_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	plane3 = (pSrc[4] << 8) + pSrc[5];
		uint16_t	plane4 = (pSrc[6] << 8) + pSrc[7];
		uint16_t	plane5 = (pSrc[8] << 8) + pSrc[9];
		uint16_t	planeA = (pSrc[10] << 8) + pSrc[11];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14) | ((plane3 & 0x80) >> 13) | ((plane4 & 0x80) >> 12) | ((plane5 & 0x80) >> 11);

			uint32_t argb = pPalette[index];

			argb |= conv_1_to_8_straight[((planeA & 0x80) >> 15)];

			*(uint32_t*)pDst = argb;
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;
			plane4 <<= 1;
			plane5 <<= 1;
			planeA <<= 1;
		}

		pSrc += 12;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_4i_a1_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	plane3 = (pSrc[4] << 8) + pSrc[5];
		uint16_t	plane4 = (pSrc[6] << 8) + pSrc[7];
		uint16_t	planeA = (pSrc[8] << 8) + pSrc[9];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14) | ((plane3 & 0x80) >> 13) | ((plane4 & 0x80) >> 12);

			uint32_t argb = pPalette[index];

			argb |= conv_1_to_8_straight[((planeA & 0x80) >> 15)];

			*(uint32_t*)pDst = argb;
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;
			plane4 <<= 1;
			planeA <<= 1;
		}

		pSrc += 10;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_2i_a1_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	plane2 = (pSrc[2] << 8) + pSrc[3];
		uint16_t	planeA = (pSrc[4] << 8) + pSrc[5];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15) | ((plane2 & 0x80) >> 14);

			uint32_t argb = pPalette[index];

			argb |= conv_1_to_8_straight[((planeA & 0x80) >> 15)];

			*(uint32_t*)pDst = argb;
			pDst += 4;

			plane1 <<= 1;
			plane2 <<= 1;
			planeA <<= 1;
		}

		pSrc += 6;
		nbPixels -= pixNow;
	}
}

static void readConv_planes_1i_a1_be(const uint8_t* pSrc, uint8_t* pDst, int nbPixels, const void* _pPalette, const void* pDummy)
{
	auto pPalette = (const uint32_t *) _pPalette;

	while (nbPixels > 0)
	{
		uint16_t	plane1 = (pSrc[0] << 8) + pSrc[1];
		uint16_t	planeA = (pSrc[2] << 8) + pSrc[3];

		int pixNow = std::min(16, nbPixels);
		for (int i = 0; i < pixNow; i++)
		{
			int		index = ((plane1 & 0x80) >> 15);

			uint32_t argb = pPalette[index];

			argb |= conv_1_to_8_straight[((planeA & 0x80) >> 15)];

			*(uint32_t*)pDst = argb;
			pDst += 4;

			plane1 <<= 1;
			planeA <<= 1;
		}

		pSrc += 4;
		nbPixels -= pixNow;
	}
}

//____ convertPixelsToKnownType() _________________________________________________________

static bool convertPixelsToKnownType( int width, int height, const uint8_t * pSrc, int srcPixelBits, int srcPitchAdd,
										uint8_t * pDst, PixelFormat dstFmt, int dstPitchAdd, Color8 * pDstPalette, int& dstPaletteEntries, int maxDstPaletteEntries,
										void(*pReadFunc)( const uint8_t *, uint8_t *, int, const void *, const void * ), const void * pTab1, const void * pTab2 )
{
	switch( dstFmt)
	{
		case PixelFormat::BGR_8_sRGB:
		case PixelFormat::BGR_8_linear:
		{
			uint8_t	buffer[64*4];

			for( int y = 0 ; y < height ; y++ )
			{
				int x = 0;
				int widthLeft = width;
				while( widthLeft > 64 )
				{
					pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
					pSrc += srcPixelBits * 8;

					uint8_t * p = (uint8_t*) buffer;
					for( int i = 0 ; i < 64 ; i++ )
					{
						* pDst++ = * p++;
						* pDst++ = * p++;
						* pDst++ = * p++;
						p += 1;
					}
					widthLeft -= 64;
				}

				if( widthLeft > 0 )
				{
					pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * widthLeft;

					uint8_t * p = (uint8_t*) buffer;
					for( int i = 0 ; i < widthLeft ; i++ )
					{
						* pDst++ = * p++;
						* pDst++ = * p++;
						* pDst++ = * p++;
						p += 1;
					}
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}

		case PixelFormat::BGRA_4_linear:
		{
			uint8_t	buffer[64*4];

			for( int y = 0 ; y < height ; y++ )
			{
				int x = 0;
				int widthLeft = width;
				while( widthLeft > 64 )
				{
					pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
					pSrc += srcPixelBits * 8;

					uint8_t * p = (uint8_t*) buffer;
					for( int i = 0 ; i < 64 ; i++ )
					{
						uint8_t dst = ((* p++) >> 4);
						dst |= ((* p++) & 0xF0);
						* pDst++ = dst;

						dst = ((* p++) >> 4);
						dst |= ((* p++) & 0xF0);
						* pDst++ = dst;
					}
					widthLeft -= 64;
				}

				if( widthLeft > 0 )
				{
					pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * widthLeft;

					uint8_t * p = (uint8_t*) buffer;
					for( int i = 0 ; i < widthLeft ; i++ )
					{
						uint8_t dst = ((* p++) >> 4);
						dst |= ((* p++) & 0xF0);
						* pDst++ = dst;

						dst = ((* p++) >> 4);
						dst |= ((* p++) & 0xF0);
						* pDst++ = dst;
					}
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}

		case PixelFormat::BGR_565_linear:
		{
			uint8_t	buffer[64*4];

			for( int y = 0 ; y < height ; y++ )
			{
				int x = 0;
				int widthLeft = width;
				while( widthLeft > 64 )
				{
					pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
					pSrc += srcPixelBits * 8;

					for( int i = 0 ; i < 64 ; i++ )
					{
						uint32_t col = buffer[i];
						* (uint16_t*)pDst = ((col >> 19) & 0x1F) | ((col >> 5) & 0x7E0) | ((col & 0xF8) << 8);
						pDst+= 2;
					}

					widthLeft -= 64;
				}

				if( widthLeft > 0 )
				{
					pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * widthLeft;

					for( int i = 0 ; i < widthLeft ; i++ )
					{
						uint32_t col = buffer[i];
						* (uint16_t*)pDst = ((col >> 19) & 0x1F) | ((col >> 5) & 0x7E0) | ((col & 0xF8) << 8);
						pDst+= 2;
					}
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}

		case PixelFormat::Index_8_sRGB:
		case PixelFormat::Index_8_linear:
		{
			uint8_t		buffer[64*4];
			int 		nColors = dstPaletteEntries;

			for( int y = 0 ; y < height ; y++ )
			{
				int x = 0;
				int widthLeft = width;
				while( widthLeft > 64 )
				{
					pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
					pSrc += srcPixelBits * 8;

					for( int i = 0 ; i < 64 ; i++ )
					{
						Color8 col = * (Color8*) &buffer[i];

						int ofs = 0;
						while( ofs < nColors && pDstPalette[ofs] != col )
							ofs++;
						if( ofs == nColors )
						{
							if( nColors == maxDstPaletteEntries )
							{
								GfxBase::throwError(ErrorLevel::Error, ErrorCode::FailedPrerequisite, "Pixel copying aborted. Out of palette entries in destination.", nullptr, nullptr, __func__, __FILE__, __LINE__ );
								return false;
							}
							
							pDstPalette[ofs] = col;
							nColors++;
						}
						* pDst++ = (uint8_t) ofs;
					}

					widthLeft -= 64;
				}

				if( widthLeft > 0 )
				{
					pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * widthLeft;

					for( int i = 0 ; i < widthLeft ; i++ )
					{
						Color8 col = * (Color8*) &buffer[i];

						int ofs = 0;
						while( ofs < nColors && pDstPalette[ofs] != col )
							ofs++;
						if( ofs == nColors )
						{
							if( nColors == maxDstPaletteEntries )
							{
								GfxBase::throwError(ErrorLevel::Error, ErrorCode::FailedPrerequisite, "Pixel copying aborted. Out of palette entries in destination.", nullptr, nullptr, __func__, __FILE__, __LINE__ );
								return false;
							}
							
							pDstPalette[ofs] = col;
							nColors++;
						}
						* pDst++ = (uint8_t) ofs;
					}
				}

				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			
			dstPaletteEntries = nColors;
			break;
		}


		case PixelFormat::RGB_565_bigendian:
		{
			uint8_t	buffer[64*4];

			for( int y = 0 ; y < height ; y++ )
			{
				int x = 0;
				int widthLeft = width;
				while( widthLeft > 64 )
				{
					pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
					pSrc += srcPixelBits * 8;

					for( int i = 0 ; i < 64 ; i++ )
					{
						uint32_t col = buffer[i];
						uint16_t out = ((col >> 19) & 0x1F) | ((col >> 5) & 0x7E0) | ((col & 0xF8) << 8);
						out = (out >> 8 | out << 8);
						* (uint16_t*)pDst = out;
						pDst+= 2;
					}

					widthLeft -= 64;
				}

				if( widthLeft > 0 )
				{
					pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * widthLeft;

					for( int i = 0 ; i < widthLeft ; i++ )
					{
						uint32_t col = buffer[i];
						uint16_t out = ((col >> 19) & 0x1F) | ((col >> 5) & 0x7E0) | ((col & 0xF8) << 8);
						out = (out >> 8 | out << 8);
						* (uint16_t*)pDst = out;
						pDst+= 2;
					}

				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}

		case PixelFormat::Alpha_8:
		{
			uint8_t	buffer[64*4];

			for( int y = 0 ; y < height ; y++ )
			{
				int x = 0;
				int widthLeft = width;
				while( widthLeft > 64 )
				{
					pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * 64;

					for( int i = 0 ; i < 64 ; i++ )
						* pDst++ = buffer[i*4+3];

					widthLeft -= 64;
				}

				if( widthLeft > 0 )
				{
					pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
					pSrc += srcPixelBits / 8 * widthLeft;

					for( int i = 0 ; i < widthLeft ; i++ )
						* pDst++ = buffer[i*4+3];

				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}

		default:
		{
			int srcPitch = width * srcPixelBits / 8 + srcPitchAdd;
			int dstPitch = width * srcPixelBits / 8 + dstPitchAdd;

			for( int y = 0 ; y < height ; y++ )
			{
				pReadFunc( pSrc, pDst, width, pTab1, pTab2 );
				pSrc += srcPitch;
				pDst += dstPitch;
			}
			break;
		}
	}
	
	return true;
}

//____ shiftAndBitsFromMask() _________________________________________________

inline void shiftAndBitsFromMask( uint64_t mask64, int& shift, int& bits )
{
	shift = 0;
	bits = 0;
	
	if( mask64 == 0 )
		return;
	
	while( (mask64 & 0xFF) == 0)
	{
		shift += 8;
		mask64 >>= 8;
	}

	uint32_t mask32 = (uint32_t)mask64;	// It all fits into 32 bits now, so lets be nice to 32-bit CPUs. ;)

	while( (mask32 & 0x1) == 0 )
	{
		shift++;
		mask32 >>= 1;
	}

	while ((mask32 & 0xF) == 0xF)
	{
		bits+=4;
		mask32 >>= 4;
	}

	while( (mask32 & 0x1) == 1 )
	{
		bits++;
		mask32 >>= 1;
	}
}


//____ copyPixels() [PixelDescription] ________________________________________

bool copyPixels(int width, int height, const uint8_t* pSrc, const PixelDescription& srcDesc, int srcPitchAdd,
	uint8_t* pDst, PixelFormat dstFmt, int dstPitchAdd, const Color8* pSrcPalette,
	Color8* pDstPalette, int srcPaletteEntries, int& dstPaletteEntries, int maxDstPaletteEntries)
{
	auto dstDesc = Util::pixelFormatToDescription( dstFmt );

	//TODO: Optimize by calling other copyPixels() if source pixel format is known.
	
	
	if (srcDesc.type == PixelType::Bitplanes)
	{
		GfxBase::throwError(ErrorLevel::Error, ErrorCode::IllegalCall, "Conversion from bitplanes not supported yet", nullptr, nullptr, __func__, __FILE__, __LINE__);
		return false;
	}
	else if(srcDesc.type == PixelType::Index)
	{
		GfxBase::throwError(ErrorLevel::Error, ErrorCode::IllegalCall, "Conversion from palette indexes not supported yet", nullptr, nullptr, __func__, __FILE__, __LINE__);
		return false;
	}
	else
	{
		ShiftReadConvTab convTab;

		convTab.maskR = srcDesc.R_mask;
		convTab.maskG = srcDesc.G_mask;
		convTab.maskB = srcDesc.B_mask;
		convTab.maskA = srcDesc.A_mask;

		int bitsR, bitsG, bitsB, bitsA;
		
		shiftAndBitsFromMask( srcDesc.R_mask, convTab.shiftR, bitsR );
		shiftAndBitsFromMask( srcDesc.G_mask, convTab.shiftG, bitsG );
		shiftAndBitsFromMask( srcDesc.B_mask, convTab.shiftB, bitsB );
		shiftAndBitsFromMask( srcDesc.A_mask, convTab.shiftA, bitsA );

		const uint8_t ** pTabList;
		
		if( dstDesc.colorSpace == srcDesc.colorSpace )
			pTabList = conv_straight_tabs;
		else if( srcDesc.colorSpace == ColorSpace::Linear )
			pTabList = conv_linear_to_sRGB_tabs;
		else
			pTabList = conv_srgb_to_linear_tabs;

		
		convTab.pConvR = pTabList[bitsR];
		convTab.pConvG = pTabList[bitsG];
		convTab.pConvB = pTabList[bitsB];
		convTab.pConvA = pTabList[bitsA];
	

		void(*pReadFunc)( const uint8_t *, uint8_t *, int, const void *, const void * );

		switch( srcDesc.bits )
		{
			case 8:
				pReadFunc = shiftReadConv_8bit;
				break;
			case 16:
				pReadFunc = (srcDesc.type == PixelType::Chunky_BE) ? shiftReadConv_16bit_swap : shiftReadConv_16bit;
				break;
			case 24:
				pReadFunc = (srcDesc.type == PixelType::Chunky_BE) ? shiftReadConv_24bit_be : shiftReadConv_24bit_le;
				break;
			case 32:
				pReadFunc = (srcDesc.type == PixelType::Chunky_BE) ? shiftReadConv_32bit_swap : shiftReadConv_32bit;
				break;
			default:
				GfxBase::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Conversion from chunky pixels requires 8, 16, 24 or 32 bit pixels.", nullptr, nullptr, __func__, __FILE__, __LINE__);
				return false;
		}
		
		
		return convertPixelsToKnownType(width, height, pSrc, srcDesc.bits, srcPitchAdd, pDst, dstFmt, dstPitchAdd,
										  pDstPalette, dstPaletteEntries, maxDstPaletteEntries, pReadFunc, &convTab, nullptr );
	}
}

//____ copyPixels() [PixelFormat] _____________________________________________

bool copyPixels( int width, int height, const uint8_t * pSrc, PixelFormat srcFmt, int srcPitchAdd,
				 uint8_t * pDst, PixelFormat dstFmt, int dstPitchAdd, const Color8 * pSrcPalette,
				 Color8 * pDstPalette, int srcPaletteEntries, int& dstPaletteEntries, int maxDstPaletteEntries )
{
	
	
	// TODO: Straight copy with palette conversion when converting between Index_8_linear and Index_8_sRGB.
	// TODO: Optimize copy to A8 in several ways.

	int nAllocatedBytes = 0;
	
	srcFmt = Util::translatePixelFormat(srcFmt);
	dstFmt = Util::translatePixelFormat(dstFmt);
	
	auto& srcDesc = Util::pixelFormatToDescription( srcFmt );
 	auto& dstDesc = Util::pixelFormatToDescription( dstFmt );
	
	if( (srcFmt == dstFmt) || (srcFmt == PixelFormat::BGRA_8_sRGB && dstFmt == PixelFormat::BGRX_8_sRGB) ||
		(srcFmt == PixelFormat::BGRA_8_linear && dstFmt == PixelFormat::BGRX_8_linear) ||
		(srcDesc.type == PixelType::Index && dstDesc.type == PixelType::Index && maxDstPaletteEntries >= srcPaletteEntries) )
	{
		if( srcPitchAdd + dstPitchAdd == 0 )
			std::memcpy( pDst, pSrc, srcDesc.bits * width * height / 8 );
		else
		{
			int pitch = width * srcDesc.bits / 8;

			for( int y = 0 ; y < height ; y++ )
			{
				for( int x = 0 ; x < width ; x++ )
				{
					std::memcpy( pDst, pSrc, srcDesc.bits * width * height / 8 );
					pDst += pitch + dstPitchAdd;
					pSrc += pitch + srcPitchAdd;
				}
			}
		}

		if(srcDesc.type == PixelType::Index && dstDesc.type == PixelType::Index && srcDesc.colorSpace != dstDesc.colorSpace )
		{
			// Palette needs to be converted between sRGB and Linear.

			const uint8_t * pTable = srcDesc.colorSpace == ColorSpace::Linear ? conv_8_linear_to_8_sRGB : conv_8_sRGB_to_8_linear;
			readConvBGRA8( (uint8_t*) pSrcPalette, (uint8_t*) pDstPalette, maxDstPaletteEntries, pTable, nullptr );
		}
	}
	else
	{
		void(*pReadFunc)( const uint8_t *, uint8_t *, int, const void *, const void * );

		bool bLinearSource = (srcDesc.colorSpace == ColorSpace::Linear);
		bool bLinearDest = (dstDesc.colorSpace == ColorSpace::Linear);

		const void * pTab1 = nullptr;
		const void * pTab2 = nullptr;

		switch( srcFmt )
		{
			case PixelFormat::BGR_8_sRGB:
			case PixelFormat::BGR_8_linear:
				if(srcDesc.colorSpace == dstDesc.colorSpace)
					pReadFunc = readBGR8;
				else
				{
					pReadFunc = readConvBGR8;
					pTab1 = bLinearSource ? conv_8_linear_to_8_sRGB : conv_8_sRGB_to_8_linear;
				}
				break;


			case PixelFormat::BGRX_8_sRGB:
			case PixelFormat::BGRX_8_linear:
				if(srcDesc.colorSpace == dstDesc.colorSpace)
					pReadFunc = readBGRX8;
				else
				{
					pReadFunc = readConvBGRX8;
					pTab1 = bLinearSource ? conv_8_linear_to_8_sRGB : conv_8_sRGB_to_8_linear;
				}
				break;

			case PixelFormat::BGRA_8_sRGB:
			case PixelFormat::BGRA_8_linear:
				if(srcDesc.colorSpace == dstDesc.colorSpace)
					pReadFunc = readBGRA8;
				else
				{
					pReadFunc = readConvBGRA8;
					pTab1 = bLinearSource ? conv_8_linear_to_8_sRGB : conv_8_sRGB_to_8_linear;
				}
				break;

			case PixelFormat::BGRA_4_linear:
				pReadFunc = readBGRA_4;
				if( bLinearDest )
					pTab1 = conv_4_to_8_straight;
				else
					pTab1 = conv_4_linear_to_8_sRGB;
				
				break;
				
			case PixelFormat::BGR_565_linear:
				pReadFunc = readBGR_565;
				if( bLinearDest )
				{
					pTab1 = conv_5_to_8_straight;
					pTab2 = conv_6_to_8_straight;
				}
				else
				{
					pTab1 = conv_5_linear_to_8_sRGB;
					pTab2 = conv_6_linear_to_8_sRGB;
				}
				break;
				
			case PixelFormat::Index_8_sRGB:
				pReadFunc = readIndex8;
				if( !bLinearDest )
					pTab1 = pSrcPalette;
				else
				{
					nAllocatedBytes = srcPaletteEntries*sizeof(Color8);
					Color8 * pConvPalette = (Color8*) GfxBase::memStackAlloc(nAllocatedBytes);
					readConvBGRA8( (uint8_t*) pSrcPalette, (uint8_t*) pConvPalette, srcPaletteEntries, conv_8_sRGB_to_8_linear, nullptr);
					pTab1 = pConvPalette;
				}
				break;
				
			case PixelFormat::Index_8_linear:
				pReadFunc = readIndex8;
				if( bLinearDest )
					pTab1 = pSrcPalette;
				else
				{
					nAllocatedBytes = srcPaletteEntries*sizeof(Color8);
					Color8 * pConvPalette = (Color8*) GfxBase::memStackAlloc(nAllocatedBytes);
					readConvBGRA8( (uint8_t*) pSrcPalette, (uint8_t*) pConvPalette, srcPaletteEntries, conv_8_linear_to_8_sRGB, nullptr);
					pTab1 = pConvPalette;
				}
				break;

				
			case PixelFormat::RGB_565_bigendian:
				pReadFunc = readRGB_565_bigendian;
				if( bLinearDest )
				{
					pTab1 = conv_5_to_8_straight;
					pTab2 = conv_6_to_8_straight;
				}
				else
				{
					pTab1 = conv_5_linear_to_8_sRGB;
					pTab2 = conv_6_linear_to_8_sRGB;
				}
				break;

			case PixelFormat::Alpha_8:
				pReadFunc = readAlpha8;
				break;
			default:
				goto error;
		}

		if( !convertPixelsToKnownType(	width, height, pSrc, srcDesc.bits, srcPitchAdd, pDst, dstFmt, dstPitchAdd,
										pDstPalette, dstPaletteEntries, maxDstPaletteEntries,
										pReadFunc, pTab1, pTab2 ) )
		   goto error;
		
	}
	
	if( nAllocatedBytes > 0 )
		GfxBase::memStackFree(nAllocatedBytes);
	return true;
	
error:
	if( nAllocatedBytes > 0 )
		GfxBase::memStackFree(nAllocatedBytes);
	return false;
}

//____ colorToPixelBytes() ____________________________________________________

int colorToPixelBytes( HiColor color, PixelFormat type, uint8_t pixelArea[18], Color8* pPalette, int paletteEntries )
{
	switch( type )
	{
		case PixelFormat::BGR_8:
		{
			uint8_t * pConvTab = GfxBase::defaultToSRGB() ? HiColor::packSRGBTab : HiColor::packLinearTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			return 3;
		}
			
		case PixelFormat::BGR_8_sRGB:
		{
			uint8_t * pConvTab = HiColor::packSRGBTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			return 3;
		}
			
		case PixelFormat::BGR_8_linear:
		{
			uint8_t * pConvTab = HiColor::packLinearTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			return 3;
		}
			
		case PixelFormat::BGRX_8:
		{
			uint8_t * pConvTab = GfxBase::defaultToSRGB() ? HiColor::packSRGBTab : HiColor::packLinearTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			pixelArea[3] = 0;
			return 4;
		}
			
		case PixelFormat::BGRX_8_sRGB:
		{
			uint8_t * pConvTab = HiColor::packSRGBTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			pixelArea[3] = 0;
			return 4;
		}
			
		case PixelFormat::BGRX_8_linear:
		{
			uint8_t * pConvTab = HiColor::packLinearTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			pixelArea[3] = 0;
			return 4;
		}
			
		case PixelFormat::BGRA_8:
		{
			uint8_t * pConvTab = GfxBase::defaultToSRGB() ? HiColor::packSRGBTab : HiColor::packLinearTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			pixelArea[3] = HiColor::packLinearTab[color.a];
			return 4;
		}
			
		case PixelFormat::BGRA_8_sRGB:
		{
			uint8_t * pConvTab = HiColor::packSRGBTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			pixelArea[3] = HiColor::packLinearTab[color.a];
			return 4;
		}
			
		case PixelFormat::BGRA_8_linear:
		{
			uint8_t * pConvTab = HiColor::packLinearTab;
			
			pixelArea[0] = pConvTab[color.b];
			pixelArea[1] = pConvTab[color.g];
			pixelArea[2] = pConvTab[color.r];
			pixelArea[3] = HiColor::packLinearTab[color.a];
			return 4;
		}
			
		case PixelFormat::BGRA_4_linear:
		{
			uint8_t * pConvTab = HiColor::packLinearTab;

			int b = pConvTab[color.b];
			int g = pConvTab[color.b];
			int r = pConvTab[color.b];
			int a = pConvTab[color.b];

			uint16_t pixel = (b & 0xF0 << 8) | (g & 0xF0 << 4) | (r & 0xF0) | (a >> 4);
			* (uint16_t*) pixelArea = pixel;
			return 2;
		}
			
		case PixelFormat::BGR_565_linear:
		{
			uint8_t * pConvTab = HiColor::packLinearTab;

			int b = pConvTab[color.b];
			int g = pConvTab[color.b];
			int r = pConvTab[color.b];
			int a = pConvTab[color.b];

			uint16_t pixel = (b & 0xF8 << 8) | (g & 0xFC << 3) | (r >> 3);
			* (uint16_t*) pixelArea = pixel;
			return 2;
		}

		case PixelFormat::Index_8:
		{
			pixelArea[0] = (uint8_t) findBestMatchInPalette(color, GfxBase::defaultToSRGB(), pPalette,256);
			return 1;
		}

		case PixelFormat::Index_8_sRGB:
		{
			pixelArea[0] = (uint8_t) findBestMatchInPalette(color, true, pPalette,256);
			return 1;
		}

		case PixelFormat::Index_8_linear:
		{
			pixelArea[0] = (uint8_t) findBestMatchInPalette(color, false, pPalette,256);
			return 1;
		}
			
		case PixelFormat::RGB_565_bigendian:
		{
			uint8_t * pConvTab = HiColor::packLinearTab;

			int b = pConvTab[color.b];
			int g = pConvTab[color.b];
			int r = pConvTab[color.b];
			int a = pConvTab[color.b];

			uint16_t pixel = (r & 0xF8 << 8) | (g & 0xFC << 3) | (b >> 3);
			* (uint16_t*) pixelArea = Util::endianSwap(pixel);
			return 2;
		}

		case PixelFormat::Alpha_8:
			pixelArea[0] = HiColor::packLinearTab[color.a];
			return 1;
			
			
		default:
			return 0;
	}
}



//____ fillBitmap() ___________________________________________________________

void fillBitmap(uint8_t* pBitmap, PixelFormat pixelFormat, int pitch, RectI fillRect, HiColor color )
{
	pixelFormat = Util::translatePixelFormat(pixelFormat);
	
	auto&	pixelDesc = Util::pixelFormatToDescription(pixelFormat);
	
	uint8_t pixelArea[18];
	int pixelBytes = colorToPixelBytes(color, pixelFormat, pixelArea);

	if( pixelDesc.type == PixelType::Bitplanes )
	{
	}
	else
	{
		uint8_t * pDest = pBitmap + fillRect.y*pitch + fillRect.x*pixelBytes;
		int eolAdd = pitch - fillRect.w * pixelBytes;
		
		switch(pixelBytes)
		{
			case 1:
			{
				uint8_t pixel = pixelArea[0];
				
				for( int y = 0 ; y < fillRect.h ; y++ )
				{
					for( int x = 0 ; x < fillRect.w ; x++ )
						* pDest++ = pixel;
					pDest += eolAdd;
				}
				break;
			}

			case 2:
			{
				uint16_t pixel = * (uint16_t*) pixelArea;
				
				for( int y = 0 ; y < fillRect.h ; y++ )
				{
					for( int x = 0 ; x < fillRect.w ; x++ )
					{
						* ((uint16_t*)pDest) = pixel;
						pDest += 2;
					}
					pDest += eolAdd;
				}
				break;
			}

			case 3:
			{
				for( int y = 0 ; y < fillRect.h ; y++ )
				{
					for( int x = 0 ; x < fillRect.w ; x++ )
					{
						* pDest++ = pixelArea[0];
						* pDest++ = pixelArea[1];
						* pDest++ = pixelArea[2];
					}
					pDest += eolAdd;
				}
				break;
			}

			case 4:
			{
				uint32_t pixel = * (uint32_t*) pixelArea;
				
				for( int y = 0 ; y < fillRect.h ; y++ )
				{
					for( int x = 0 ; x < fillRect.w ; x++ )
					{
						* ((uint32_t*)pDest) = pixel;
						pDest += 4;
					}
					pDest += eolAdd;
				}
				break;
			}
		}
	}
}

//____ findBestMatchInPalette() ____________________________________________

int findBestMatchInPalette( HiColor color, bool bSRGB, Color8* pPalette, int paletteEntries)
{
	uint8_t * pConvTab = bSRGB ? HiColor::packSRGBTab : HiColor::packLinearTab;

	Color8 col;
	
	col.b = pConvTab[color.b];
	col.g = pConvTab[color.b];
	col.r = pConvTab[color.b];
	col.a = pConvTab[color.b];

	// First we make a quick check for exact match
	
	for( int i = 0 ; i < paletteEntries ; i++ )
		if( col == pPalette[i] )
			return i;
	
	// Find best match
	
	int bestMatchIndex = 0;
	int bestMatchDiff = 256*256*4;
	
	for( int i = 0 ; i < paletteEntries ; i++ )
	{
		Color8& palCol = pPalette[i];
		
		int diffB = abs(col.b - palCol.b);
		int diffG = abs(col.g - palCol.g);
		int diffR = abs(col.r - palCol.r);
		int diffA = abs(col.a - palCol.a);

		int combDiff = diffB*diffB + diffG*diffG + diffR*diffR + diffA*diffA;
		if(combDiff < bestMatchDiff)
		{
			bestMatchDiff = combDiff;
			bestMatchIndex = i;
		}
	}
	
	return bestMatchIndex;
}

//____ extractAlphaChannel() __________________________________________________

bool extractAlphaChannel(PixelFormat format, const uint8_t* pSrc, int srcPitch, RectI srcRect, uint8_t* pDst, int dstPitch, const Color8* pPalette)
{

	auto& pixDesc = Util::pixelFormatToDescription(format);

	if (pixDesc.A_mask == 0 && pixDesc.type != PixelType::Index)
		return false;										// These pixels have no alpha.

	pSrc += srcRect.y * srcPitch + srcRect.x * pixDesc.bits/8;

	int srcPitchAdd = srcPitch - srcRect.w * pixDesc.bits/8;	//TODO: Won't work for bitplane graphics.
	int dstPitchAdd = dstPitch - srcRect.w;

	switch (pixDesc.type)
	{
		case PixelType::Index:
		{
			if (pixDesc.bits == 8)
			{
				for (int y = 0; y < srcRect.h; y++)
				{
					for (int x = 0; x < srcRect.w; x++)
						*pDst++ = pPalette[*pSrc++].a;

					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			else if (pixDesc.bits == 16)
			{
				for (int y = 0; y < srcRect.h; y++)
				{
					for (int x = 0; x < srcRect.w; x++)
					{
						*pDst++ = pPalette[*(uint16_t*)pSrc].a;
						pSrc += 2;
					}

					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			return true;
		}

		case PixelType::Chunky:
		{
			switch (pixDesc.bits)
			{
				case 1:									// Only PixelFormat::Alpha_8 has this size.
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
							*pDst++ = *pSrc++;

						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}

				case 2:									// Only PixelFormat::BGRA_4 has this size and alpha.
				{
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							uint16_t val = (* ((uint16_t*)pSrc)) >> 12;
							*pDst++ = val | (val << 4);
							pSrc += 2;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
					break;
				}
				case 4:									// Only PixelFormat::BGRA_8 has this size and alpha
				{
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							*pDst++ = pSrc[3];
							pSrc += 4;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
					break;
				}
			}

		}
		
		default:
			return false;		// PixelFormat not supported yet.
	}
}



} } // namespace wg::PixelTool

