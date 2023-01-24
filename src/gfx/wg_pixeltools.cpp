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

namespace wg { namespace PixelTool
{

uint8_t linearToSRGBTable[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
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

uint8_t sRGBToLinearTable[256] = {0, 21, 28, 34, 39, 43, 46, 50, 53, 56, 59, 61, 64, 66, 68, 70, 72,
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

uint8_t conv_4_to_8_sRGB[16] = {0, 1, 3, 7, 14, 23, 34, 48, 64, 83, 105, 129, 156, 186, 219, 255 };

uint8_t conv_5_to_8_sRGB[32] = {0, 0, 1, 1, 3, 5, 7, 10, 13, 17, 21, 26, 32, 38, 44, 52, 60, 68,
	77, 87, 97, 108, 120, 132, 145, 159, 173, 188, 204, 220, 237, 255 };

uint8_t conv_6_to_8_sRGB[64] = {0, 0, 0, 0, 1, 1, 1, 2, 3, 4, 4, 5, 7, 8, 9, 11, 13, 14, 16, 18,
	20, 23, 25, 28, 31, 33, 36, 40, 43, 46, 50, 54, 57, 61, 66, 70, 74, 79, 84, 89, 94, 99, 105, 110,
	116, 122, 128, 134, 140, 147, 153, 160, 167, 174, 182, 189, 197, 205, 213, 221, 229, 238, 246, 255 };

uint8_t conv_4_to_8_linear[16] 	= {	0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
uint8_t conv_5_to_8_linear[32]	= {	0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
									0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff };

uint8_t conv_6_to_8_linear[64] 	= {	0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
									0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
									0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
									0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff };



static void readBGRA8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* (uint32_t*) pDst = *(uint32_t*) pSrc;
		pSrc += 4;
		pDst += 4;
	}
}

static void readConvBGRA8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
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

static void readBGRX8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* (uint32_t*) pDst = (*(uint32_t*) pSrc) | 0xFF000000;
		pSrc += 4;
		pDst += 4;
	}
}

static void readConvBGRX8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
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


static void readBGR8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = *pSrc++;
		* pDst++ = *pSrc++;
		* pDst++ = *pSrc++;
		* pDst++ = 255;
	}
}

static void readConvBGR8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
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

static void readAlpha8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
{
	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* pDst++ = ((*pSrc++) << 24) | 0x00FFFFFF;
	}
}

static void readIndex8( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
{
	uint32_t * pPalette = (uint32_t*) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		* (uint32_t*) pDst = pPalette[*pSrc++];
		pDst += 4;
	}
}

static void readBGRA_4( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
{
	uint8_t * pConvTab = (uint8_t*) p1;

	for( int i = 0 ; i < nbPixels ; i++ )
	{
		uint8_t bg = * pSrc++;
		* pDst++ = pConvTab[bg&0xF];
		* pDst++ = pConvTab[bg>>4];

		uint8_t ra = * pSrc++;
		* pDst++ = pConvTab[ra&0xF];
		* pDst++ = conv_4_to_8_linear[ra>>4];
	}
}


static void readBGR_565( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2  )
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

static void readRGB_565_bigendian( const uint8_t * pSrc, uint8_t * pDst, int nbPixels, void * p1, void * p2 )
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

//____ copyPixels() ___________________________________________________________

bool copyPixels( int width, int height, uint8_t * pSrc, PixelFormat srcFmt, int srcPitchAdd,
				 uint8_t * pDst, PixelFormat dstFmt, int dstPitchAdd, Color8 * pSrcPalette,
				 Color8 * pDstPalette, int srcPaletteEntries, int maxDstPaletteEntries, int& dstPaletteEntries )
{
	// TODO: Straight copy with palette conversion when converting between Index_8_linear and Index_8_sRGB.
	// TODO: Optimize copy to A8 in several ways.

	int nAllocatedBytes = 0;
	
	PixelDescription srcDesc;
	PixelDescription dstDesc;

	Util::pixelFormatToDescription( srcFmt, srcDesc );
 	Util::pixelFormatToDescription( dstFmt, dstDesc );

	if( (srcFmt == dstFmt) || (srcFmt == PixelFormat::BGRA_8_sRGB && dstFmt == PixelFormat::BGRX_8_sRGB) ||
		(srcFmt == PixelFormat::BGRA_8_linear && dstFmt == PixelFormat::BGRX_8_linear) ||
		(srcDesc.bIndexed && dstDesc.bIndexed && maxDstPaletteEntries >= srcPaletteEntries) )
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

		if(srcDesc.bIndexed && dstDesc.bIndexed && srcDesc.bLinear != dstDesc.bLinear )
		{
			// Palette needs to be converted between sRGB and Linear.

			uint8_t * pTable = srcDesc.bLinear ? linearToSRGBTable : sRGBToLinearTable;
			readConvBGRA8( (uint8_t*) pSrcPalette, (uint8_t*) pDstPalette, maxDstPaletteEntries, pTable, nullptr );
		}
	}
	else
	{
		void(*pReadFunc)( const uint8_t *, uint8_t *, int, void *, void * );

		void * pTab1 = nullptr;
		void * pTab2 = nullptr;

		switch( srcFmt )
		{
			case PixelFormat::BGR_8_sRGB:
			case PixelFormat::BGR_8_linear:
				if(srcDesc.bLinear == dstDesc.bLinear)
					pReadFunc = readBGR8;
				else
				{
					pReadFunc = readConvBGR8;
					pTab1 = srcDesc.bLinear ? linearToSRGBTable : sRGBToLinearTable;
				}
				break;


			case PixelFormat::BGRX_8_sRGB:
			case PixelFormat::BGRX_8_linear:
				if(srcDesc.bLinear == dstDesc.bLinear)
					pReadFunc = readBGRX8;
				else
				{
					pReadFunc = readConvBGRX8;
					pTab1 = srcDesc.bLinear ? linearToSRGBTable : sRGBToLinearTable;
				}
				break;

			case PixelFormat::BGRA_8_sRGB:
			case PixelFormat::BGRA_8_linear:
				if(srcDesc.bLinear == dstDesc.bLinear)
					pReadFunc = readBGRA8;
				else
				{
					pReadFunc = readConvBGRA8;
					pTab1 = srcDesc.bLinear ? linearToSRGBTable : sRGBToLinearTable;
				}
				break;

			case PixelFormat::BGRA_4_linear:
				pReadFunc = readBGRA_4;
				if( dstDesc.bLinear )
					pTab1 = conv_4_to_8_linear;
				else
					pTab1 = conv_4_to_8_sRGB;
				
				break;
				
			case PixelFormat::BGR_565_linear:
				pReadFunc = readBGR_565;
				if( dstDesc.bLinear )
				{
					pTab1 = conv_5_to_8_linear;
					pTab2 = conv_6_to_8_linear;
				}
				else
				{
					pTab1 = conv_5_to_8_sRGB;
					pTab2 = conv_6_to_8_sRGB;
				}
				break;
				
			case PixelFormat::CLUT_8_sRGB:
				pReadFunc = readIndex8;
				if( !dstDesc.bLinear )
					pTab1 = pSrcPalette;
				else
				{
					nAllocatedBytes = srcPaletteEntries*sizeof(Color8);
					Color8 * pConvPalette = (Color8*) GfxBase::memStackAlloc(nAllocatedBytes);
					readConvBGRA8( (uint8_t*) pSrcPalette, (uint8_t*) pConvPalette, srcPaletteEntries, sRGBToLinearTable, nullptr);
					pTab1 = pConvPalette;
				}
				break;
				
			case PixelFormat::CLUT_8_linear:
				pReadFunc = readIndex8;
				if( dstDesc.bLinear )
					pTab1 = pSrcPalette;
				else
				{
					nAllocatedBytes = srcPaletteEntries*sizeof(Color8);
					Color8 * pConvPalette = (Color8*) GfxBase::memStackAlloc(nAllocatedBytes);
					readConvBGRA8( (uint8_t*) pSrcPalette, (uint8_t*) pConvPalette, srcPaletteEntries, linearToSRGBTable, nullptr);
					pTab1 = pConvPalette;
				}
				break;

				
			case PixelFormat::RGB_565_bigendian:
				pReadFunc = readRGB_565_bigendian;
				if( dstDesc.bLinear )
				{
					pTab1 = conv_5_to_8_linear;
					pTab2 = conv_6_to_8_linear;
				}
				else
				{
					pTab1 = conv_5_to_8_sRGB;
					pTab2 = conv_6_to_8_sRGB;
				}
				break;

			case PixelFormat::A_8:
				pReadFunc = readAlpha8;
				break;
			default:
				goto error;
		}

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
						pSrc += srcDesc.bits * 8;

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
						pSrc += srcDesc.bits / 8 * widthLeft;

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
						pSrc += srcDesc.bits * 8;

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
						pSrc += srcDesc.bits / 8 * widthLeft;

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
						pSrc += srcDesc.bits * 8;

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
						pSrc += srcDesc.bits / 8 * widthLeft;

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

			case PixelFormat::CLUT_8_sRGB:
			case PixelFormat::CLUT_8_linear:
			{
				uint8_t		buffer[64*4];
				int 		nColors = 0;

				for( int y = 0 ; y < height ; y++ )
				{
					int x = 0;
					int widthLeft = width;
					while( widthLeft > 64 )
					{
						pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
						pSrc += srcDesc.bits * 8;

						for( int i = 0 ; i < 64 ; i++ )
						{
							Color8 col = * (Color8*) &buffer[i];

							int ofs = 0;
							while( ofs < nColors && pDstPalette[ofs] != col )
								ofs++;
							if( ofs == nColors )
							{
								if( nColors == maxDstPaletteEntries )
									goto error;
								
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
						pSrc += srcDesc.bits / 8 * widthLeft;

						for( int i = 0 ; i < widthLeft ; i++ )
						{
							Color8 col = * (Color8*) &buffer[i];

							int ofs = 0;
							while( ofs < nColors && pDstPalette[ofs] != col )
								ofs++;
							if( ofs == nColors )
							{
								if( nColors == maxDstPaletteEntries )
									goto error;
							
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
						pSrc += srcDesc.bits * 8;

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
						pSrc += srcDesc.bits / 8 * widthLeft;

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

			case PixelFormat::A_8:
			{
				uint8_t	buffer[64*4];

				for( int y = 0 ; y < height ; y++ )
				{
					int x = 0;
					int widthLeft = width;
					while( widthLeft > 64 )
					{
						pReadFunc( pSrc, buffer, 64, pTab1, pTab2 );
						pSrc += srcDesc.bits / 8 * 64;

						for( int i = 0 ; i < 64 ; i++ )
							* pDst++ = buffer[i*4+3];

						widthLeft -= 64;
					}

					if( widthLeft > 0 )
					{
						pReadFunc( pSrc, buffer, widthLeft, pTab1, pTab2 );
						pSrc += srcDesc.bits / 8 * widthLeft;

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
				int srcPitch = width * srcDesc.bits / 8 + srcPitchAdd;
				int dstPitch = width * dstDesc.bits / 8 + dstPitchAdd;

				for( int y = 0 ; y < height ; y++ )
				{
					pReadFunc( pSrc, pDst, width, pTab1, pTab2 );
					pSrc += srcPitch;
					pDst += dstPitch;
				}
				break;
			}
		}
	}
	
	if( nAllocatedBytes > 0 )
		GfxBase::memStackRelease(nAllocatedBytes);
	return true;
	
error:
	if( nAllocatedBytes > 0 )
		GfxBase::memStackRelease(nAllocatedBytes);
	return false;
}


} } // namespace wg::PixelTool

