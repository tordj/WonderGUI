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

#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>
#include <wg_util.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <wg_base.h>

#include <cassert>

using namespace std;

namespace wg 
{
	using namespace Util;
	
	const char SoftGfxDevice::CLASSNAME[] = {"SoftGfxDevice"};
	
	int SoftGfxDevice::s_mulTab[256];

	SoftGfxDevice::PlotOp_p		SoftGfxDevice::s_plotOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::FillOp_p		SoftGfxDevice::s_fillOpTab[BlendMode_size][TintMode_size][PixelFormat_size];
	SoftGfxDevice::LineOp_p		SoftGfxDevice::s_LineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::ClipLineOp_p SoftGfxDevice::s_clipLineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::PlotListOp_p SoftGfxDevice::s_plotListOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::WaveOp_p		SoftGfxDevice::s_waveOpTab[BlendMode_size][PixelFormat_size];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_pass2OpTab[BlendMode_size][PixelFormat_size];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGRA_8_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGR_8_OpTab[PixelFormat_size][2];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGRA_8_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGR_8_OpTab[PixelFormat_size][2]; 

	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_BGRA_8_OpTab[PixelFormat_size][2][2];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_BGR_8_OpTab[PixelFormat_size][2][2];

	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformBlendTo_BGRA_8_OpTab[PixelFormat_size][2][2];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformBlendTo_BGR_8_OpTab[PixelFormat_size][2][2];



	const uint8_t s_channel_4_1[256] = {	0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

	const uint8_t s_channel_4_2[256] = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
											0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
											0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
											0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
											0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
											0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
											0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
											0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
											0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
											0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
											0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
											0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
											0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
											0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
											0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	const uint8_t s_channel_5[256]	= {		0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff, 
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff, 
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff, 
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff };

	const uint8_t s_channel_6[256] = {		0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff,
											0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff,
											0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff,
											0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff	};


	//____ read_pixel() _______________________________________________________

	inline void SoftGfxDevice::_read_pixel(const uint8_t * pPixel, PixelFormat format, const Color * pClut, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (format == PixelFormat::BGRA_8)
		{
			outB = pPixel[0];
			outG = pPixel[1];
			outR = pPixel[2];
			outA = pPixel[3];
		}

		if (format == PixelFormat::BGR_8)
		{
			outB = pPixel[0];
			outG = pPixel[1];
			outR = pPixel[2];
			outA = 255;
		}

		if (format == PixelFormat::BGR_565)
		{
			uint16_t pixel = *(uint16_t*)pPixel;
#if IS_BIG_ENDIAN
			pixel = (pixel << 8) | (pixel >> 8);
#endif
			outB = s_channel_5[(uint8_t) pixel];
			outG = s_channel_6[(uint8_t) (pixel >> 5)];
			outR = s_channel_5[(uint8_t) (pixel >> 11)];
			outA = 255;
		}

		if (format == PixelFormat::BGRA_4)
		{
			outB = s_channel_4_1[pPixel[0]];
			outG = s_channel_4_2[pPixel[0]];
			outR = s_channel_4_1[pPixel[1]];
			outA = s_channel_4_2[pPixel[1]];
		}

		if (format == PixelFormat::A8)
		{
			outB = 255;
			outG = 255;
			outR = 255;
			outA = *pPixel;
		}

		if (format == PixelFormat::I8)
		{
			const Color& c = pClut[*pPixel];
			outB = c.b;
			outG = c.g;
			outR = c.r;
			outA = c.a;
		}


	}

	//____ write_pixel() _______________________________________________________

	inline void SoftGfxDevice::_write_pixel(uint8_t * pPixel, PixelFormat format, uint8_t b, uint8_t g, uint8_t r, uint8_t a)
	{
		if (format == PixelFormat::BGRA_8)
		{
			pPixel[0] = b;
			pPixel[1] = g;
			pPixel[2] = r;
			pPixel[3] = a;
		}

		if (format == PixelFormat::BGR_8)
		{
			pPixel[0] = b;
			pPixel[1] = g;
			pPixel[2] = r;
		}

		if (format == PixelFormat::BGR_565)
		{
			pPixel[0] = (b >> 3) | ((g & 0xFC) << 3);
			pPixel[1] = (g >> 5) | (r & 0xF8);

		}

		if (format == PixelFormat::BGRA_4)
		{
			pPixel[0] = (b >> 4) | (g & 0xF0);
			pPixel[1] = (r >> 4) | (a & 0xF0);
		}

	}


	//____ _blend_pixels() ____________________________________________________

	inline void	SoftGfxDevice::_blend_pixels(	BlendMode mode, uint8_t srcB, uint8_t srcG, uint8_t srcR, uint8_t srcA,
												uint8_t backB, uint8_t backG, uint8_t backR, uint8_t backA,
												uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		// Note: We use if-statements instead of switch/case here since some compilers
		// won't fully eliminate the switch/case statements when this code is inlined
		// into templates where only one BlendMode is possible. With if-statements,
		// the statements are properly detected as never occuring and completely removed.

		if (mode == BlendMode::Replace)
		{
			outB = srcB;
			outG = srcG;
			outR = srcR;
			outA = srcA;
		}

		if (mode == BlendMode::Blend)
		{
				int alpha = s_mulTab[srcA];
				int invAlpha = 65536 - alpha;

				outB = (backB * invAlpha + srcB * alpha) >> 16;
				outG = (backG * invAlpha + srcG * alpha) >> 16;
				outR = (backR * invAlpha + srcR * alpha) >> 16;
				outA = (backA * invAlpha + 255 * alpha) >> 16;
		}

		if (mode == BlendMode::Add)
		{
			int alpha = s_mulTab[srcA];

			outB = limitUint8(backB + (srcB * alpha >> 16));
			outG = limitUint8(backG + (srcG * alpha >> 16));
			outR = limitUint8(backR + (srcR * alpha >> 16));
			outA = backA;
		}

		if (mode == BlendMode::Subtract)
		{
			int alpha = s_mulTab[srcA];

			outB = limitUint8(backB + (srcB * alpha >> 16));
			outG = limitUint8(backG + (srcG * alpha >> 16));
			outR = limitUint8(backR + (srcR * alpha >> 16));
			outA = backA;
		}

		if (mode == BlendMode::Multiply)
		{
			outB = (s_mulTab[backB] * srcB) >> 16;
			outG = (s_mulTab[backG] * srcG) >> 16;
			outR = (s_mulTab[backR] * srcR) >> 16;
			outA = backA;
		}

		if (mode == BlendMode::Invert)
		{
			int srcB2 = s_mulTab[srcB];
			int srcG2 = s_mulTab[srcG];
			int srcR2 = s_mulTab[srcR];

			outB = (srcB2 * (255 - backB) + backB * (65536 - srcB2)) >> 16;
			outG = (srcG2 * (255 - backG) + backG * (65536 - srcG2)) >> 16;
			outR = (srcR2 * (255 - backR) + backR * (65536 - srcR2)) >> 16;
			outA = backA;
		}
	}


	//____ _init_tint_color() _________________________________________________

	inline void	SoftGfxDevice::_init_tint_color(TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (tintMode == TintMode::Color)
		{
			outB = (inB * s_mulTab[tint.baseTint.b]) >> 16;
			outG = (inG * s_mulTab[tint.baseTint.g]) >> 16;
			outR = (inR * s_mulTab[tint.baseTint.r]) >> 16;
			outA = (inA * s_mulTab[tint.baseTint.a]) >> 16;
		}
		else
		{
			outB = inB;
			outG = inG;
			outR = inR;
			outA = inA;
		}
	}

	//____ _init_tint_color_line() _________________________________________________

	inline void	SoftGfxDevice::_init_tint_color_line(int lineNb, TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (tintMode == TintMode::MapY || tintMode == TintMode::MapXY)
		{
			outB = (inB * s_mulTab[tint.pTintY[lineNb].b]) >> 16;
			outG = (inG * s_mulTab[tint.pTintY[lineNb].g]) >> 16;
			outR = (inR * s_mulTab[tint.pTintY[lineNb].r]) >> 16;
			outA = (inA * s_mulTab[tint.pTintY[lineNb].a]) >> 16;
		}
		else
		{
			outB = inB;
			outG = inG;
			outR = inR;
			outA = inA;
		}
	}

	//____ _tint_color() ______________________________________________________

	inline void SoftGfxDevice::_tint_color(int columnNb, TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (tintMode == TintMode::MapX || tintMode == TintMode::MapXY)
		{
			outB = (inB * s_mulTab[tint.pTintX[columnNb].b]) >> 16;
			outG = (inG * s_mulTab[tint.pTintX[columnNb].g]) >> 16;
			outR = (inR * s_mulTab[tint.pTintX[columnNb].r]) >> 16;
			outA = (inA * s_mulTab[tint.pTintX[columnNb].a]) >> 16;
		}
		else
		{
			outB = inB;
			outG = inG;
			outR = inR;
			outA = inA;
		}
	}

	//____ _init_tint_texel() _________________________________________________

	inline void SoftGfxDevice::_init_tint_texel(TintMode tintMode, const ColTrans& tint, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (tintMode == TintMode::Color)
		{
			outB = s_mulTab[tint.baseTint.b];
			outG = s_mulTab[tint.baseTint.g];
			outR = s_mulTab[tint.baseTint.r];
			outA = s_mulTab[tint.baseTint.a];
		}
		else
		{
			outB = 255;
			outG = 255;
			outR = 255;
			outA = 255;
		}
	}

	//____ _init_tint_texel_line() _________________________________________________

	inline void SoftGfxDevice::_init_tint_texel_line(int lineNb, TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (tintMode == TintMode::MapY || tintMode ==  TintMode::MapXY )						// We are tinting lines individually
		{
			outB = tint.pTintY[lineNb].b;
			outG = tint.pTintY[lineNb].g;
			outR = tint.pTintY[lineNb].r;
			outA = tint.pTintY[lineNb].a;
		}
		else
		{
			outB = inB;
			outG = inG;
			outR = inR;
			outA = inA;
		}
	}

	//____ _tint_texel() ______________________________________________________

	inline void SoftGfxDevice::_tint_texel(int columnNb, TintMode tintMode, const ColTrans& tint, uint8_t texB, uint8_t texG, uint8_t texR, uint8_t texA, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
	{
		if (tintMode == TintMode::None)
		{
			outB = texB;
			outG = texG;
			outR = texR;
			outA = texA;
		}
		
		if( tintMode == TintMode::Color || tintMode == TintMode::MapY )
		{
			outB = (texB * s_mulTab[inB]) >> 16;
			outG = (texG * s_mulTab[inG]) >> 16;
			outR = (texR * s_mulTab[inR]) >> 16;
			outA = (texA * s_mulTab[inA]) >> 16;
		}
		
		if( tintMode == TintMode::MapX )
		{
			outB = (texB * s_mulTab[tint.pTintX[columnNb].b]) >> 16;
			outG = (texG * s_mulTab[tint.pTintX[columnNb].g]) >> 16;
			outR = (texR * s_mulTab[tint.pTintX[columnNb].r]) >> 16;
			outA = (texA * s_mulTab[tint.pTintX[columnNb].a]) >> 16;
		}


		if( tintMode == TintMode::MapXY )
		{
			uint8_t	tintB = (inB * s_mulTab[tint.pTintX[columnNb].b]) >> 16;
			uint8_t	tintG = (inG * s_mulTab[tint.pTintX[columnNb].g]) >> 16;
			uint8_t	tintR = (inR * s_mulTab[tint.pTintX[columnNb].r]) >> 16;
			uint8_t	tintA = (inA * s_mulTab[tint.pTintX[columnNb].a]) >> 16;

			outB = (texB * s_mulTab[tintB]) >> 16;
			outG = (texG * s_mulTab[tintG]) >> 16;
			outR = (texR * s_mulTab[tintR]) >> 16;
			outA = (texA * s_mulTab[tintA]) >> 16;
		}
	}



	//____ _plot() ____________________________________________________________

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_plot(uint8_t * pDst, Color col, const ColTrans& tint)
	{
		int tintB, tintG, tintR, tintA;

		if (TINTFLAGS & 0x1)
		{
			tintB = s_mulTab[tint.baseTint.b];
			tintG = s_mulTab[tint.baseTint.g];
			tintR = s_mulTab[tint.baseTint.r];
			tintA = s_mulTab[tint.baseTint.a];
		}

		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;

		srcB = col.b;
		srcG = col.g;
		srcR = col.r;
		srcA = col.a;

		// Step 1.5: Apply any tint to source

		if (TINTFLAGS & 0x1)
		{
			srcB = (col.b*tintB) >> 16;
			srcG = (col.g*tintG) >> 16;
			srcR = (col.r*tintR) >> 16;
			srcA = (col.a*tintA) >> 16;
		}

		// Step 2: Get color components of background pixel blending into backX

		uint8_t backB, backG, backR, backA;

		_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

		// Step 3: Blend srcX and backX into outX

		uint8_t outB, outG, outR, outA;
		_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

		// Step 4: Write resulting pixel to destination

		_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
	}

	//____ plot_list() ________________________________________________________

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_plot_list(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint)
	{
		int tintB, tintG, tintR, tintA;

		if (TINTFLAGS & 0x1)
		{
			tintB = s_mulTab[tint.baseTint.b];
			tintG = s_mulTab[tint.baseTint.g];
			tintR = s_mulTab[tint.baseTint.r];
			tintA = s_mulTab[tint.baseTint.a];
		}

		for (int i = 0; i < nCoords; i++)
		{
			const int x = pCoords[i].x;
			const int y = pCoords[i].y;

			if (y >= clip.y && y <= clip.y + clip.h - 1 && x >= clip.x && x <= clip.x + clip.w - 1)
			{
				uint8_t * pDst = pCanvas + y * pitchY + x * pitchX;

				// Step 1: Read source pixels

				uint8_t srcB, srcG, srcR, srcA;

				srcB = pColors[i].b;
				srcG = pColors[i].g;
				srcR = pColors[i].r;
				srcA = pColors[i].a;

				// Step 1.5: Apply any tint to source

				if (TINTFLAGS & 0x1)
				{
					srcB = (srcB*tintB) >> 16;
					srcG = (srcG*tintG) >> 16;
					srcR = (srcR*tintR) >> 16;
					srcA = (srcA*tintA) >> 16;
				}

				// Step 2: Get color components of background pixel blending into backX

				uint8_t backB, backG, backR, backA;

				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				// Step 3: Blend srcX and backX into outX

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

				// Step 4: Write resulting pixel to destination

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
		}
	}

	//____ _draw_line() _______________________________________________________

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_draw_line(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint)
	{
		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;

		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;

		// Step 1.5: Apply any baseTint

		if (TINTFLAGS & 0x1)
		{
			srcB = (srcB * s_mulTab[tint.baseTint.b]) >> 16;
			srcG = (srcG * s_mulTab[tint.baseTint.g]) >> 16;
			srcR = (srcR * s_mulTab[tint.baseTint.r]) >> 16;
			srcA = (srcA * s_mulTab[tint.baseTint.a]) >> 16;
		}

		for (int i = 0; i < length; i++)
		{
			int beg = pos >> 16;
			int end = (pos + width) >> 16;
			int ofs = beg;

			uint8_t * pDst = pRow + ofs * pixelInc;

			if (beg == end)
			{
				// Special case, one pixel wide row

				int alpha = (color.a * width) >> 16;

				uint8_t backB, backG, backR, backA;
				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				// First anti-aliased pixel of column

				int alpha = (srcA * (65536 - (pos & 0xFFFF))) >> 16;		// Special AA

				uint8_t backB, backG, backR, backA;
				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				ofs++;
				pDst += pixelInc;

				// All non-antialiased middle pixels of column

				if (ofs < end)
				{
					do
					{
						// Step 2: Get color components of background pixel blending into backX

						uint8_t backB, backG, backR, backA;
						_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

						// Step 3: Blend srcX and backX into outX

						uint8_t outB, outG, outR, outA;
						_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

						// Step 4: Write resulting pixel to destination

						_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
						ofs++;
						pDst += pixelInc;

					} while (ofs < end);
				}

				// Last anti-aliased pixel of column

				int overflow = (pos + width) & 0xFFFF;
				if (overflow > 0)
				{
					int alpha = srcA * overflow >> 16;		// Special AA

					uint8_t backB, backG, backR, backA;
					_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

					uint8_t outB, outG, outR, outA;
					_blend_pixels(BLEND, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
			}

			pRow += rowInc;
			pos += slope;
		}
	}

	//____ _clip_draw_line() __________________________________________________

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_clip_draw_line(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint)
	{
		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;

		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;

		// Step 1.5: Apply any baseTint

		if (TINTFLAGS & 0x1)
		{
			srcB = (srcB * s_mulTab[tint.baseTint.b]) >> 16;
			srcG = (srcG * s_mulTab[tint.baseTint.g]) >> 16;
			srcR = (srcR * s_mulTab[tint.baseTint.r]) >> 16;
			srcA = (srcA * s_mulTab[tint.baseTint.a]) >> 16;
		}

		for (int i = 0; i < length; i++)
		{

			if (pos >= clipEnd || pos + width <= clipStart)
			{
				pRow += rowInc;
				pos += slope;
				continue;
			}

			int clippedPos = pos;
			int clippedWidth = width;

			if (clippedPos < clipStart)
			{
				clippedWidth -= clipStart - clippedPos;
				clippedPos = clipStart;
			}

			if (clippedPos + clippedWidth > clipEnd)
				clippedWidth = clipEnd - clippedPos;


			int beg = clippedPos >> 16;
			int end = (clippedPos + clippedWidth) >> 16;
			int ofs = beg;

			uint8_t * pDst = pRow + ofs * pixelInc;

			if (beg == end)
			{
				// Special case, one pixel wide row

				int alpha = (color.a * width) >> 16;

				uint8_t backB, backG, backR, backA;
				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				// First anti-aliased pixel of column

				int alpha = (srcA * (65536 - (clippedPos & 0xFFFF))) >> 16;		// Special AA

				uint8_t backB, backG, backR, backA;
				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				ofs++;
				pDst += pixelInc;

				// All non-antialiased middle pixels of column

				if (ofs < end)
				{
					do
					{
						// Step 2: Get color components of background pixel blending into backX

						uint8_t backB, backG, backR, backA;
						_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

						// Step 3: Blend srcX and backX into outX

						uint8_t outB, outG, outR, outA;
						_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

						// Step 4: Write resulting pixel to destination

						_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
						ofs++;
						pDst += pixelInc;

					} while (ofs < end);
				}

				// Last anti-aliased pixel of column

				int overflow = (clippedPos + clippedWidth) & 0xFFFF;
				if (overflow > 0)
				{
					int alpha = srcA * overflow >> 16;		// Special AA

					uint8_t backB, backG, backR, backA;
					_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

					uint8_t outB, outG, outR, outA;
					_blend_pixels(BLEND, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
			}

			pRow += rowInc;
			pos += slope;
		}
	}


	//____ _fill() ____________________________________________________________

	template<BlendMode BLEND, SoftGfxDevice::TintMode TINTMODE, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_fill(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint)
	{
		// Step 1: Read source pixels and prepare tint

		uint8_t baseB, baseG, baseR, baseA;

		_init_tint_color(TINTMODE, tint, col.b, col.g, col.r, col.a, baseB, baseG, baseR, baseA);


		for (int y = 0; y < nLines; y++)
		{

			// Step 2: Apply any vertical tint-map

			uint8_t lineB, lineG, lineR, lineA;
			_init_tint_color_line(y, TINTMODE, tint, baseB, baseG, baseR, baseA, lineB, lineG, lineR, lineA);

			for (int x = 0; x < lineLength; x++)
			{
				// Step 3: Apply any horizontal tint-map

				uint8_t srcB, srcG, srcR, srcA;
				_tint_color(x, TINTMODE, tint, lineB, lineG, lineR, lineA, srcB, srcG, srcR, srcA);

				// Step 4: Get color components of background pixel blending into backX

				uint8_t backB, backG, backR, backA;

				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				// Step 5: Blend srcX and backX into outX

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

				// Step 6: Write resulting pixel to destination

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);

				// Step 7: Increment destination pointers

				pDst += pitchX;
			}
			pDst += pitchY;
		}

	}

	//____ draw_wave_column() _________________________________________________
/*
	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_draw_wave_column(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch)
	{
		// 16 binals on leftPos, rightPos and most calculations.

		int i = 0;

		int amount[4];
		int inc[4];

		int columnBeg = (min(leftPos[0], rightPos[0]) & 0xFFFF0000) + 32768;		// Column starts in middle of first pixel

																					// Calculate start amount and increment for our 4 fields

		for (int i = 0; i < 4; i++)
		{
			int yBeg;
			int64_t xInc;

			if (leftPos[i] < rightPos[i])
			{
				yBeg = leftPos[i];
				xInc = (int64_t)65536 * 65536 / (rightPos[i] - leftPos[i] + 1);
			}
			else
			{
				yBeg = rightPos[i];
				xInc = (int64_t)65536 * 65536 / (leftPos[i] - rightPos[i] + 1);
			}

			limit(xInc, (int64_t)0, (int64_t)65536);

			inc[i] = (int)xInc;

			int64_t startAmount = -((xInc * (yBeg - columnBeg)) >> 16);
			amount[i] = (int)startAmount;
		}

		// Do clipping

		if (columnBeg < (clipBeg << 16))
		{
			int64_t forwardAmount = (clipBeg << 16) - columnBeg;

			for (int i = 0; i < 4; i++)
				amount[i] += (int)((inc[i] * forwardAmount) >> 16);

			columnBeg = (clipBeg << 16);
		}

		uint8_t * pDstClip = pColumn + (clipBeg + clipLen) * linePitch;

		// Render the column

		uint8_t * pDst = pColumn + linePitch * (columnBeg >> 16);

		// First render loop, run until we are fully into fill (or later section).
		// This needs to cover all possibilities since topLine, fill and bottomLine might be less than 1 pixel combined
		// in which case they should all be rendered.

		while (amount[1] < 65536 && pDst < pDstClip)
		{
			int aFrac = amount[0];
			int bFrac = amount[1];
			int cFrac = amount[2];
			int dFrac = amount[3];
			limit(aFrac, 0, 65536);
			limit(bFrac, 0, 65536);
			limit(cFrac, 0, 65536);
			limit(dFrac, 0, 65536);

			aFrac = ((aFrac - bFrac)*col[0].a) / 255;
			bFrac = ((bFrac - cFrac)*col[1].a) / 255;
			cFrac = ((cFrac - dFrac)*col[2].a) / 255;

			int backFraction = 65536 - aFrac - bFrac - cFrac;

			pDst[0] = (pDst[0] * backFraction + col[0].b * aFrac + col[1].b * bFrac + col[2].b * cFrac) >> 16;
			pDst[1] = (pDst[1] * backFraction + col[0].g * aFrac + col[1].g * bFrac + col[2].g * cFrac) >> 16;
			pDst[2] = (pDst[2] * backFraction + col[0].r * aFrac + col[1].r * bFrac + col[2].r * cFrac) >> 16;
			pDst[3] = (pDst[3] * backFraction + 255 * aFrac + 255 * bFrac + 255 * cFrac) >> 16;

			// Step 4: Get color components of background pixel blending into backX

			uint8_t backB, backG, backR, backA;

			_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

			// Step 5: Blend srcX and backX into outX

			uint8_t outB, outG, outR, outA;
			_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

			// Step 6: Write resulting pixel to destination

			_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);

			pDst += linePitch;

			for (int i = 0; i < 4; i++)
				amount[i] += inc[i];
		}

		// Second render loop, optimzed fill-section loop until bottomLine starts to fade in.

		if (amount[2] <= 0 && pDst < pDstClip)
		{
			if (col[1].a == 255)
			{
				while (amount[2] <= 0 && pDst < pDstClip)
				{
					pDst[0] = col[1].b;
					pDst[1] = col[1].g;
					pDst[2] = col[1].r;
					pDst[3] = col[1].a;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
			}
			else
			{
				int fillFrac = (65536 * col[1].a) / 255;

				int fillB = col[1].b * fillFrac;
				int fillG = col[1].g * fillFrac;
				int fillR = col[1].r * fillFrac;
				int fillA = 255 * fillFrac;
				int backFraction = 65536 - fillFrac;

				while (amount[2] <= 0 && pDst < pDstClip)
				{
					pDst[0] = (pDst[0] * backFraction + fillB) >> 16;
					pDst[1] = (pDst[1] * backFraction + fillG) >> 16;
					pDst[2] = (pDst[2] * backFraction + fillR) >> 16;
					pDst[3] = (pDst[3] * backFraction + fillA) >> 16;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
			}
		}


		// Third render loop, from when bottom line has started to fade in.
		// We can safely ignore topLine (not visible anymore) and amount[2] is guaranteed to have reached 65536.

		while (amount[3] < 65536 && pDst < pDstClip)
		{
			int cFrac = amount[2];
			int dFrac = amount[3];
			limit(cFrac, 0, 65536);
			limit(dFrac, 0, 65536);

			int bFrac = ((65536 - cFrac)*col[1].a) / 255;
			cFrac = ((cFrac - dFrac)*col[2].a) / 255;

			int backFraction = 65536 - bFrac - cFrac;

			pDst[0] = (pDst[0] * backFraction + col[1].b * bFrac + col[2].b * cFrac) >> 16;
			pDst[1] = (pDst[1] * backFraction + col[1].g * bFrac + col[2].g * cFrac) >> 16;
			pDst[2] = (pDst[2] * backFraction + col[1].r * bFrac + col[2].r * cFrac) >> 16;
			pDst[3] = (pDst[3] * backFraction + 255 * bFrac + 255 * cFrac) >> 16;
			pDst += linePitch;

			amount[2] += inc[2];
			amount[3] += inc[3];
		}
	}
*/



	//_____ _simple_blit() ____________________________________________________________

	template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_simple_blit(const uint8_t * pSrc, uint8_t * pDst, const Color * pClut, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint)
	{
		int tintB, tintG, tintR, tintA;

		if (TINTFLAGS & 0x1)
		{
			tintB = s_mulTab[tint.baseTint.b];
			tintG = s_mulTab[tint.baseTint.g];
			tintR = s_mulTab[tint.baseTint.r];
			tintA = s_mulTab[tint.baseTint.a];
		}

		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				// Step 1: Read source pixels

				uint8_t srcB, srcG, srcR, srcA;
				_read_pixel(pSrc, SRCFORMAT, pClut, srcB, srcG, srcR, srcA);

				// Step 1.5: Apply any tint

				if (TINTFLAGS & 0x1)
				{
					srcB = (srcB*tintB) >> 16;
					srcG = (srcG*tintG) >> 16;
					srcR = (srcR*tintR) >> 16;
					srcA = (srcA*tintA) >> 16;
				}

				// Step 2: Get color components of background pixel blending into backX

				uint8_t backB, backG, backR, backA;

				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				// Step 3: Blend srcX and backX into outX

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

				// Step 4: Write resulting pixel to destination

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);

				// Step 6: Increment source and destination pointers

				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}


	//____ _complex_blit __________________________________________

	template<PixelFormat SRCFORMAT, ScaleMode SCALEMODE, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_complex_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint)
	{
		int srcPixelBytes = pSrcSurf->m_pixelDescription.bits / 8;
		int	srcPitch = pSrcSurf->m_pitch;

		int pixelIncX = (int)(matrix[0][0] * 32768);
		int pixelIncY = (int)(matrix[0][1] * 32768);

		int lineIncX = (int)(matrix[1][0] * 32768);
		int lineIncY = (int)(matrix[1][1] * 32768);


		int tintB, tintG, tintR, tintA;

		if (TINTFLAGS & 0x1)
		{
			tintB = s_mulTab[tint.baseTint.b];
			tintG = s_mulTab[tint.baseTint.g];
			tintR = s_mulTab[tint.baseTint.r];
			tintA = s_mulTab[tint.baseTint.a];
		}

		for (int y = 0; y < nLines; y++)
		{
			int ofsX = (int)(pos.x * 32768 + lineIncX * y);
			int ofsY = (int)(pos.y * 32768 + lineIncY * y);		// We use 15 binals for all calculations 

			for (int x = 0; x < lineLength; x++)
			{
				// Step 1: Read source color.

				uint8_t srcB, srcG, srcR, srcA;
				uint8_t * p = pSrcSurf->m_pData + (ofsY >> 15) * srcPitch + (ofsX >> 15)*srcPixelBytes;

				if (SCALEMODE == ScaleMode::Interpolate)
				{
					// Read separate source color components for our 2x2 pixel square.


					uint8_t src11_b, src11_g, src11_r, src11_a;
					uint8_t src12_b, src12_g, src12_r, src12_a;
					uint8_t src21_b, src21_g, src21_r, src21_a;
					uint8_t src22_b, src22_g, src22_r, src22_a;

					_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, src11_b, src11_g, src11_r, src11_a);
					_read_pixel(p + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src12_b, src12_g, src12_r, src12_a);
					_read_pixel(p + srcPitch, SRCFORMAT, pSrcSurf->m_pClut, src21_b, src21_g, src21_r, src21_a);
					_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src22_b, src22_g, src22_r, src22_a);

					// Interpolate our 2x2 source colors into one source color, srcX

					int fracX2 = ofsX & 0x7FFF;
					int fracX1 = 32768 - fracX2;

					int fracY2 = ofsY & 0x7FFF;
					int fracY1 = 32768 - fracY2;

					int mul11 = fracX1 * fracY1 >> 15;
					int mul12 = fracX2 * fracY1 >> 15;
					int mul21 = fracX1 * fracY2 >> 15;
					int mul22 = fracX2 * fracY2 >> 15;

					srcB = (src11_b * mul11 + src12_b * mul12 + src21_b * mul21 + src22_b * mul22) >> 15;
					srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> 15;
					srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> 15;
					srcA = (src11_a * mul11 + src12_a * mul12 + src21_a * mul21 + src22_a * mul22) >> 15;

				}
				else
				{
					_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, srcB, srcG, srcR, srcA);
				}


				// Step 3.5: Apply any tinting

				if (TINTFLAGS & 0x1)
				{
					srcB = (srcB*tintB) >> 16;
					srcG = (srcG*tintG) >> 16;
					srcR = (srcR*tintR) >> 16;
					srcA = (srcA*tintA) >> 16;
				}

				// Step 3: Get color components of background pixel blending into backX

				uint8_t backB, backG, backR, backA;

				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				// Step 4: Blend srcX and backX into outX

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

				// Step 5: Write resulting pixel to destination

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);

				// Step 6: Increment source and destination pointers

				ofsX += pixelIncX;
				ofsY += pixelIncY;
				pDst += dstPitchX;
			}
			pDst += dstPitchY;
		}
	}

	//____ _stretch_blit __________________________________________

	template<PixelFormat SRCFORMAT, ScaleMode SCALEMODE, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_stretch_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint)
	{
		int srcPixelBytes = pSrcSurf->m_pixelDescription.bits / 8;
		int	srcPitch = pSrcSurf->m_pitch;

		int pixelIncX = (int)(matrix[0][0] * 32768);
		int lineIncY = (int)(matrix[1][1] * 32768);

		int tintB, tintG, tintR, tintA;

		if (TINTFLAGS & 0x1)
		{
			tintB = s_mulTab[tint.baseTint.b];
			tintG = s_mulTab[tint.baseTint.g];
			tintR = s_mulTab[tint.baseTint.r];
			tintA = s_mulTab[tint.baseTint.a];
		}

		int ofsY = (int)(pos.y * 32768);		// We use 15 binals for all calculations 

		for (int y = 0; y < nLines; y++)
		{
			int ofsX = (int)(pos.x * 32768);

			uint8_t * pSrc = pSrcSurf->m_pData + (ofsY >> 15) * srcPitch;

			for (int x = 0; x < lineLength; x++)
			{
				// Step 1: Read source color.

				uint8_t srcB, srcG, srcR, srcA;
				uint8_t * p = pSrc + (ofsX >> 15)*srcPixelBytes;

				if (SCALEMODE == ScaleMode::Interpolate)
				{
					// Separate source color components for our 2x2 pixel square.

					uint8_t src11_b, src11_g, src11_r, src11_a;
					uint8_t src12_b, src12_g, src12_r, src12_a;
					uint8_t src21_b, src21_g, src21_r, src21_a;
					uint8_t src22_b, src22_g, src22_r, src22_a;

					_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, src11_b, src11_g, src11_r, src11_a);
					_read_pixel(p + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src12_b, src12_g, src12_r, src12_a);
					_read_pixel(p + srcPitch, SRCFORMAT, pSrcSurf->m_pClut, src21_b, src21_g, src21_r, src21_a);
					_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src22_b, src22_g, src22_r, src22_a);

					// Interpolate our 2x2 source colors into one source color, srcX

					int fracX2 = ofsX & 0x7FFF;
					int fracX1 = 32768 - fracX2;

					int fracY2 = ofsY & 0x7FFF;
					int fracY1 = 32768 - fracY2;

					int mul11 = fracX1 * fracY1 >> 15;
					int mul12 = fracX2 * fracY1 >> 15;
					int mul21 = fracX1 * fracY2 >> 15;
					int mul22 = fracX2 * fracY2 >> 15;

					srcB = (src11_b * mul11 + src12_b * mul12 + src21_b * mul21 + src22_b * mul22) >> 15;
					srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> 15;
					srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> 15;
					srcA = (src11_a * mul11 + src12_a * mul12 + src21_a * mul21 + src22_a * mul22) >> 15;
				}
				else
				{
					_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, srcB, srcG, srcR, srcA);
				}

				// Step 3.5: Apply any tinting

				if (TINTFLAGS & 0x1)
				{
					srcB = (srcB*tintB) >> 16;
					srcG = (srcG*tintG) >> 16;
					srcR = (srcR*tintR) >> 16;
					srcA = (srcA*tintA) >> 16;
				}

				// Step 3: Get color components of background pixel blending into backX

				uint8_t backB, backG, backR, backA;

				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA );

				// Step 4: Blend srcX and backX into outX

				uint8_t outB, outG, outR, outA;
				_blend_pixels(BLEND, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);

				// Step 5: Write resulting pixel to destination

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);

				// Step 6: Increment source and destination pointers

				ofsX += pixelIncX;
				pDst += dstPitchX;
			}

			ofsY += lineIncY;
			pDst += dstPitchY;
		}
	}


	//____ create() _______________________________________________________________
	
	SoftGfxDevice_p SoftGfxDevice::create()
	{
		return SoftGfxDevice_p(new SoftGfxDevice());
	}
	
	SoftGfxDevice_p SoftGfxDevice::create( Surface * pCanvas )
	{
		return SoftGfxDevice_p(new SoftGfxDevice(pCanvas));
	}
	
	
	//____ Constructor _____________________________________________________________
	
	SoftGfxDevice::SoftGfxDevice() : GfxDevice(Size(0,0))
	{
		m_bEnableCustomFunctions = false;
		m_bUseCustomFunctions = false;
		m_pCanvas = nullptr;
		m_pCanvasPixels = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		_initTables();
		_clearCustomFunctionTable();
		
	}
	
	SoftGfxDevice::SoftGfxDevice( Surface * pCanvas ) : GfxDevice( pCanvas?pCanvas->size():Size() )
	{
		m_bEnableCustomFunctions = false;
		m_bUseCustomFunctions = false;
		m_pCanvas = pCanvas;
		m_pCanvasPixels = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		_initTables();
		_clearCustomFunctionTable();
	}
	
	//____ Destructor ______________________________________________________________
	
	SoftGfxDevice::~SoftGfxDevice()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool SoftGfxDevice::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return GfxDevice::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * SoftGfxDevice::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	SoftGfxDevice_p SoftGfxDevice::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SoftGfxDevice_p( static_cast<SoftGfxDevice*>(pObject) );
	
		return 0;
	}
	
	//____ surfaceClassName() _______________________________________________________
	
	const char * SoftGfxDevice::surfaceClassName( void ) const
	{
		return SoftSurface::CLASSNAME;
	}
	
	//____ surfaceClassName() _______________________________________________________

	SurfaceFactory_p SoftGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = SoftSurfaceFactory::create();
	
		return m_pSurfaceFactory;
	}
	
	//____ setCanvas() _______________________________________________________________
	
	bool SoftGfxDevice::setCanvas( Surface * pCanvas )
	{	
		if (m_pCanvas == pCanvas)
			return true;			// Not an error.

		if( !pCanvas )
		{
			m_pCanvas		= nullptr;
			m_canvasSize	= Size();
			m_clip			= Rect();

			// Make sure this also is cleared, in case we are rendering.

			m_pCanvasPixels = nullptr;
			m_canvasPixelBits = 0;
			m_canvasPitch = 0;
			return true;
		}


		PixelFormat format = pCanvas->pixelFormat();
		if( format != PixelFormat::BGRA_8 && format != PixelFormat::BGR_8 &&
			format != PixelFormat::BGRX_8 && format != PixelFormat::BGRA_4 &&
			format != PixelFormat::BGR_565 )
			return false;
	
		if( m_pCanvasPixels )
			m_pCanvas->unlock();

		m_pCanvas		= pCanvas;
		m_canvasSize	= pCanvas->size();
		m_clip			= { 0,0,m_canvasSize };

		// Update stuff if we are rendering

		if( m_pCanvasPixels )
		{
			m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
			m_canvasPixelBits = m_pCanvas->pixelDescription()->bits;
			m_canvasPitch = m_pCanvas->pitch();

			_updateBlitFunctions();

			// Call custom function, let it decide if it can render or not.

			if( m_bEnableCustomFunctions && m_customFunctions.setCanvas )
			{
				int retVal = m_customFunctions.setCanvas( m_pCanvasPixels, (int) pCanvas->pixelFormat(), m_canvasPitch );
				m_bUseCustomFunctions = retVal != 0;
			}
		}
	
	return true;
	}

	//____ setTintColor() _____________________________________________________

	void SoftGfxDevice::setTintColor(Color color)
	{
		if (color == m_tintColor)
			return;

		bool bUpdateNeeded = (color.a != m_tintColor.a) && (color.a == 255 || m_tintColor.a == 255);
		GfxDevice::setTintColor(color);
		m_colTrans.baseTint = color;

		if(bUpdateNeeded )
			_updateBlitFunctions();
	}

	//____ setBlendMode() _____________________________________________________

	bool SoftGfxDevice::setBlendMode(BlendMode blendMode)
	{
		if (blendMode == m_blendMode)
			return true;

		bool retVal = GfxDevice::setBlendMode(blendMode);
		if (retVal)
			_updateBlitFunctions();
		return retVal;
	}


	//____ beginRender() _______________________________________________________

	bool SoftGfxDevice::beginRender()
	{
		if( !m_pCanvas)
			return false;

		m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
		m_canvasPixelBits = m_pCanvas->pixelDescription()->bits;
		m_canvasPitch = m_pCanvas->pitch();
		
		_updateBlitFunctions();

		if( !m_pCanvasPixels )
			return false;

		// Call custom functions
		
		if( m_bEnableCustomFunctions )
		{
			if( m_customFunctions.beginRender )
				m_customFunctions.beginRender();

			if( m_customFunctions.setCanvas )
			{
				int retVal = m_customFunctions.setCanvas( m_pCanvasPixels, (int) m_pCanvas->pixelFormat(), m_canvasPitch );
				m_bUseCustomFunctions = retVal != 0;
			}			
			else
				m_bUseCustomFunctions = true;

		}
		
		return true;	
	}

	//____ endRender() _________________________________________________________
	
	bool SoftGfxDevice::endRender()
	{
		if( !m_pCanvasPixels )
			return false;

		// Call custom function.
		
		if( m_bEnableCustomFunctions && m_customFunctions.endRender )
			m_customFunctions.endRender();
			
		// Clean up.
			
		m_pCanvas->unlock();
		m_pCanvasPixels = 0;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;

		_updateBlitFunctions();		// Good to have dummies in place when we are not allowed to blit.
		return true;
	}


	//____ fill() ____________________________________________________________________

	void SoftGfxDevice::fill(const Rect& _rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		// Clipping

		Rect rect(_rect, m_clip);
		if (rect.w == 0 || rect.h == 0)
			return;

		// Prepare colors

		Color fillColor = col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };

		// Skip calls that won't affect destination

		if (fillColor.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		// Optimize calls

		BlendMode blendMode = m_blendMode;
		if (blendMode == BlendMode::Blend && fillColor.a == 255)
			blendMode = BlendMode::Replace;

		//

		int pixelBytes = m_canvasPixelBits / 8;
		uint8_t * pDst = m_pCanvasPixels + rect.y *m_canvasPitch + rect.x * pixelBytes;

		FillOp_p pFunc = s_fillOpTab[(int)blendMode][0][(int)m_pCanvas->pixelFormat()];

		if (pFunc)
			pFunc(pDst, pixelBytes, m_canvasPitch - rect.w*pixelBytes, rect.h, rect.w, fillColor, colTrans);
	}

	//____ fill() ____________________________________________________________________

	void SoftGfxDevice::fill(const RectF& _rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		// Clipping

		RectF rect(_rect, m_clip);
		if (rect.w == 0 || rect.h == 0)
			return;

		// Prepare colors

		Color fillColor = col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };


		// Skip calls that won't affect destination

		if (fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p pOp = s_fillOpTab[(int)m_blendMode][0][(int)m_pCanvas->pixelFormat()];

		// Fill all but anti-aliased edges

		int x1 = (int)(rect.x + 0.999f);
		int y1 = (int)(rect.y + 0.999f);
		int x2 = (int)(rect.x + rect.w);
		int y2 = (int)(rect.y + rect.h);


		uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x1 * pixelBytes;
		pOp(pDst, pixelBytes, m_canvasPitch - (x2-x1)*pixelBytes, y2 - y1, x2 - x1, col, colTrans);
		//		fill(Rect(x1, y1, x2 - x1, y2 - y1), col);

		// Draw the sides

		int aaLeft = (256 - (int)(rect.x * 256)) & 0xFF;
		int aaTop = (256 - (int)(rect.y * 256)) & 0xFF;
		int aaRight = ((int)((rect.x + rect.w) * 256)) & 0xFF;
		int aaBottom = ((int)((rect.y + rect.h) * 256)) & 0xFF;

		int aaTopLeft = aaTop * aaLeft / 256;
		int aaTopRight = aaTop * aaRight / 256;
		int aaBottomLeft = aaBottom * aaLeft / 256;
		int aaBottomRight = aaBottom * aaRight / 256;


		if (m_blendMode == BlendMode::Replace)
		{
			pOp = s_fillOpTab[(int)BlendMode::Blend][0][(int)m_pCanvas->pixelFormat()];		// Need to blend edges and corners anyway
		}
		else
		{
			int alpha = s_mulTab[fillColor.a];

			aaLeft = aaLeft * alpha >> 16;
			aaTop = aaTop * alpha >> 16;
			aaRight = aaRight * alpha >> 16;
			aaBottom = aaBottom * alpha >> 16;

			aaTopLeft = aaTopLeft * alpha >> 16;
			aaTopRight = aaTopRight* alpha >> 16;
			aaBottomLeft = aaBottomLeft * alpha >> 16;
			aaBottomRight = aaBottomRight * alpha >> 16;
		}


		if (aaTop != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + x1 * pixelBytes;
			int length = x2 - x1;
			fillColor.a = aaTop;
			pOp(pDst, pixelBytes, 0, 1, length, fillColor,colTrans);
		}

		if (aaBottom != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
			int length = x2 - x1;
			fillColor.a = aaBottom;
			pOp(pDst, pixelBytes, 0, 1, length, fillColor, colTrans);
		}

		if (aaLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + ((int)rect.x) * pixelBytes;
			int length = y2 - y1;
			fillColor.a = aaLeft;
			pOp(pDst, m_canvasPitch, 0, 1, length, fillColor, colTrans);
		}

		if (aaRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
			int length = y2 - y1;
			fillColor.a = aaRight;
			pOp(pDst, m_canvasPitch, 0, 1, length, fillColor, colTrans);
		}

		// Draw corner pieces

		PlotOp_p pPlotOp = s_plotOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (aaTopLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + ((int)rect.x) * pixelBytes;
			fillColor.a = aaTopLeft;
			pPlotOp(pDst, fillColor, colTrans);
		}

		if (aaTopRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + x2 * pixelBytes;
			fillColor.a = aaTopRight;
			pPlotOp(pDst, fillColor, colTrans);
		}

		if (aaBottomLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + ((int)rect.x) * pixelBytes;
			fillColor.a = aaBottomLeft;
			pPlotOp(pDst, fillColor, colTrans);
		}

		if (aaBottomRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
			fillColor.a = aaBottomRight;
			pPlotOp(pDst, fillColor, colTrans);
		}
	}
	

	//____ drawLine() ____ [start/direction] __________________________________
/*
	void SoftGfxDevice::drawLine(Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		//TODO: Check how much slower thick vertical lines gets than horizontal due to cache trashing caused by us drawing vertically.

		if (thickness <= 0.f)
			return;

		_col = _col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };


		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pOp = s_fillOpTab[(int)m_blendMode][0][(int)m_pCanvas->pixelFormat()];

		switch (dir)
		{
		case Direction::Left:
			begin.x -= length;
		case Direction::Right:
		{
			if (thickness <= 1.f)
			{
				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);
				uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, 0, 1, length, col, colTrans);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				int beginY = begin.y - expanse;
				int endY = begin.y + expanse + 1;

				uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, 0, 1, length, edgeColor, colTrans);

				pBegin = m_pCanvasPixels + (endY-1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, 0, 1, length, edgeColor, colTrans);

				int bodyThickness = endY - beginY - 2;
				pBegin = m_pCanvasPixels + (beginY+1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, m_canvasPitch-bodyThickness*pixelBytes, bodyThickness, length, _col, colTrans);

//				_drawStraightLine({ begin.x, beginY }, Orientation::Horizontal, length, edgeColor);
//				_drawStraightLine({ begin.x, endY - 1 }, Orientation::Horizontal, length, edgeColor);
//				fill({ begin.x, beginY + 1, length, endY - beginY - 2 }, _col);
			}
			break;
		}
		case Direction::Up:
			begin.y -= length;
		case Direction::Down:
		{
			if (thickness <= 1.f)
			{
				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);

				uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, col, colTrans);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				int beginX = begin.x - expanse;
				int endX = begin.x + expanse + 1;

				uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, colTrans);

				pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX-1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, colTrans);

				int bodyThickness = endX - beginX - 2;
				pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX+1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, pixelBytes - m_canvasPitch-bodyThickness, bodyThickness, length, _col, colTrans);

//				_drawStraightLine({ beginX, begin.y }, Orientation::Vertical, length, edgeColor);
//				_drawStraightLine({ endX - 1, begin.y }, Orientation::Vertical, length, edgeColor);
//				fill({ beginX + 1, begin.y, endX - beginX - 2, length }, _col);
			}
			break;
		}
		}
	}
*/

	//____ drawLine() ____ [from/to] ______________________________________

	void SoftGfxDevice::drawLine( Coord beg, Coord end, Color color, float thickness )
	{
		if( !m_pCanvas || !m_pCanvasPixels )
			return;
	
		Color fillColor = color * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };

		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;
		
		
		uint8_t *	pRow;
		int		rowInc, pixelInc;
		int 	length, width;
		int		pos, slope;
		int		clipStart, clipEnd;

		if( std::abs(beg.x-end.x) > std::abs(beg.y-end.y) )
		{
			// Prepare mainly horizontal line segment
			
			if( beg.x > end.x )
				swap( beg, end );
			
			length = end.x - beg.x;
			slope = ((end.y - beg.y) << 16) / length;

			width = _scaleLineThickness( thickness, slope );
			pos = (beg.y << 16) - width/2;		
					
			rowInc = m_canvasPixelBits/8;
			pixelInc =m_canvasPitch;

			pRow = m_pCanvasPixels + beg.x * rowInc;

			// Do clipping for line segment
			
			if( beg.x > m_clip.x + m_clip.w || end.x < m_clip.x )
				return;										// Segement not visible.
				
			if( beg.x < m_clip.x )
			{
				int cut = m_clip.x - beg.x;
				length -= cut;
				pRow += rowInc*cut;
				pos += slope*cut;
			}

			if( end.x > m_clip.x + m_clip.w )
				length -= end.x - (m_clip.x+m_clip.w);

			clipStart = m_clip.y << 16;
			clipEnd = (m_clip.y + m_clip.h) <<16;
		}
		else
		{
			// Prepare mainly vertical line segment
			
			if( beg.y > end.y )
				swap( beg, end );
			
			length = end.y - beg.y;
			if( length == 0 )
				return;											// TODO: Should stil draw the caps!

			slope = ((end.x - beg.x) << 16) / length;
			width = _scaleLineThickness( thickness, slope );
			pos = (beg.x << 16) - width/2;		
					
			rowInc =m_canvasPitch;
			pixelInc = m_canvasPixelBits/8;

			pRow = m_pCanvasPixels + beg.y * rowInc;		

			// Do clipping for line segment
			
			if( beg.y > m_clip.y + m_clip.h || end.y < m_clip.y )
				return;										// Segement not visible.
				
			if( beg.y < m_clip.y )
			{
				int cut = m_clip.y - beg.y;
				length -= cut;
				pRow += rowInc*cut;
				pos += slope*cut;
			}

			if( end.y > m_clip.y + m_clip.h )
				length -= end.y - (m_clip.y+m_clip.h);
				
			clipStart = m_clip.x << 16;
			clipEnd = (m_clip.x + m_clip.w) <<16;
		}

		ClipLineOp_p pOp = s_clipLineOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if( pOp )
			pOp( clipStart, clipEnd, pRow, rowInc, pixelInc, length, width, pos, slope, fillColor, colTrans );
	}

	//____ drawLine() ____ [start/direction] ______________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void SoftGfxDevice::drawLine(Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		if (thickness <= 0.f)
			return;

		_col = _col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pOp = s_fillOpTab[(int)m_blendMode][0][(int)m_pCanvas->pixelFormat()];

		switch (dir)
		{
		case Direction::Left:
			begin.x -= length;
		case Direction::Right:
		{
			if (begin.x > m_clip.x + m_clip.w)
				return;

			if (begin.x < m_clip.x)
			{
				length -= m_clip.x - begin.x;
				if (length <= 0)
					return;
				begin.x = m_clip.x;
			}

			if (begin.x + length > m_clip.x + m_clip.w)
			{
				length = m_clip.x + m_clip.w - begin.x;
				if (length <= 0)
					return;
			}

			if (thickness <= 1.f)
			{
				if (begin.y < m_clip.y || begin.y >= m_clip.y + m_clip.h)
					return;

				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);

				uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, 0, 1, length, col, colTrans);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				if (begin.y + expanse <= m_clip.y || begin.y - expanse >= m_clip.y + m_clip.h)
					return;

				int beginY = begin.y - expanse;
				int endY = begin.y + expanse + 1;

				if (beginY < m_clip.y)
					beginY = m_clip.y - 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
					pOp(pBegin, pixelBytes, 0, 1, length, edgeColor, colTrans);
//					_drawStraightLine({ begin.x, beginY }, Orientation::Horizontal, length, edgeColor);
				}

				if (endY > m_clip.y + m_clip.h)
					endY = m_clip.y + m_clip.h + 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + (endY - 1) * m_canvasPitch + begin.x * pixelBytes;
					pOp(pBegin, pixelBytes, 0, 1, length, edgeColor, colTrans);
				}

				int bodyThickness = endY - beginY - 2;
				uint8_t * pBegin = m_pCanvasPixels + (beginY + 1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, m_canvasPitch - bodyThickness * pixelBytes, bodyThickness, length, _col, colTrans);
			}

			break;
		}
		case Direction::Up:
			begin.y -= length;
		case Direction::Down:
			if (begin.y > m_clip.y + m_clip.h)
				return;

			if (begin.y < m_clip.y)
			{
				length -= m_clip.y - begin.y;
				if (length <= 0)
					return;
				begin.y = m_clip.y;
			}

			if (begin.y + length > m_clip.y + m_clip.h)
			{
				length = m_clip.y + m_clip.h - begin.y;
				if (length <= 0)
					return;
			}

			if (thickness <= 1.f)
			{
				if (begin.x < m_clip.x || begin.x >= m_clip.x + m_clip.w)
					return;

				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);

				uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, col, colTrans);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				if (begin.x + expanse <= m_clip.x || begin.x - expanse >= m_clip.x + m_clip.w)
					return;

				int beginX = begin.x - expanse;
				int endX = begin.x + expanse + 1;

				if (beginX < m_clip.x)
					beginX = m_clip.x - 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
					pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, colTrans);
				}

				if (endX > m_clip.x + m_clip.w)
					endX = m_clip.x + m_clip.w + 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX - 1) * pixelBytes;
					pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, colTrans);
				}


				int bodyThickness = endX - beginX - 2;
				uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX + 1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, pixelBytes - m_canvasPitch * bodyThickness, bodyThickness, length, _col, colTrans);
			}

			break;
		}
	}


	//____ drawHorrWave() _____________________________________________________

	void SoftGfxDevice::drawHorrWave(Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		// Do early rough X-clipping with margin (need to trace lines with margin of thickest line).

		int ofs = 0;
		if (m_clip.x > begin.x || m_clip.x + m_clip.w < begin.x + length)
		{
			int margin = (int)(max(pTopBorder->thickness, pBottomBorder->thickness) / 2 + 0.99);

			if (m_clip.x > begin.x + margin )
			{
				ofs = m_clip.x - begin.x - margin;
				begin.x += ofs;
				length -= ofs;
			}

			if (begin.x + length - margin > m_clip.x + m_clip.w)
				length = m_clip.x + m_clip.w - begin.x + margin;

			if (length <= 0)
				return;
		}

		// Generate line traces

		int	bufferSize = (length+1) * 2 * sizeof(int) *2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
		char * pBuffer = Base::memStackAlloc(bufferSize);
		int * pTopBorderTrace = (int*)pBuffer;
		int * pBottomBorderTrace = (int*) (pBuffer + bufferSize/2);

		_traceLine(pTopBorderTrace, length + 1, pTopBorder, ofs);
		_traceLine(pBottomBorderTrace, length + 1, pBottomBorder, ofs);

		// Do proper X-clipping

		int startColumn = 0;
		if (begin.x < m_clip.x)
		{
			startColumn = m_clip.x - begin.x;
			length -= startColumn;
			begin.x += startColumn;
		}

		if (begin.x + length > m_clip.x + m_clip.w)
			length = m_clip.x + m_clip.w - begin.x;

		// Render columns

		uint8_t * pColumn = m_pCanvasPixels + begin.y * m_canvasPitch + begin.x * (m_canvasPixelBits / 8);
		int pos[2][4];						// Startpositions for the 4 fields of the column (topline, fill, bottomline, line end) for left and right edge of pixel column. 16 binals.

		int clipBeg = m_clip.y - begin.y;
		int clipLen = m_clip.h;

		Color	col[4];
		col[0] = pTopBorder->color;
		col[1] = frontFill;
		col[2] = pBottomBorder->color;
		col[3] = backFill;


		for (int i = startColumn; i <= length+startColumn; i++)
		{
			// Old right pos becomes new left pos and old left pos will be reused for new right pos

			int * pLeftPos = pos[i % 2];
			int * pRightPos = pos[(i + 1) % 2];

			// Check if lines have intersected and in that case swap top and bottom lines and colors

			if (pTopBorderTrace[i * 2] > pBottomBorderTrace[i * 2])
			{
				swap(col[0], col[2]);
				swap(col[1], col[3]);
				swap(pTopBorderTrace, pBottomBorderTrace);

				// We need to regenerate leftpos since we now have swapped top and bottom line.

				if (i > startColumn)
				{
					int j = i - 1;
					pLeftPos[0] = pTopBorderTrace[j * 2] << 8;
					pLeftPos[1] = pTopBorderTrace[j * 2 + 1] << 8;

					pLeftPos[2] = pBottomBorderTrace[j * 2] << 8;
					pLeftPos[3] = pBottomBorderTrace[j * 2 + 1] << 8;

					if (pLeftPos[2] < pLeftPos[1])
					{
						pLeftPos[2] = pLeftPos[1];
						if (pLeftPos[3] < pLeftPos[2])
							pLeftPos[3] = pLeftPos[2];
					}
				}
			}

			// Generate new rightpos table

			pRightPos[0] = pTopBorderTrace[i * 2] << 8;
			pRightPos[1] = pTopBorderTrace[i * 2 + 1] << 8;

			pRightPos[2] = pBottomBorderTrace[i * 2] << 8;
			pRightPos[3] = pBottomBorderTrace[i * 2 + 1] << 8;


			if (pRightPos[2] < pRightPos[1])
			{
				pRightPos[2] = pRightPos[1];
				if (pRightPos[3] < pRightPos[2])
					pRightPos[3] = pRightPos[2];
			}

			// Render the column

			if (i > startColumn)
			{
				WaveOp_p pOp = s_waveOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
				pOp(clipBeg, clipLen, pColumn, pLeftPos, pRightPos, col, m_canvasPitch);
				pColumn += m_canvasPixelBits / 8;
			}
		}

		// Free temporary work memory

		Base::memStackRelease(bufferSize);
	}

	//_____ _clip_wave_blend_24() ________________________________________________

	void SoftGfxDevice::_clip_wave_blend_24(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch)
	{
		// 16 binals on leftPos, rightPos and most calculations.

		int i = 0;

		int amount[4];
		int inc[4];

		int columnBeg = (min(leftPos[0], rightPos[0]) & 0xFFFF0000); // +32768;	Column does NOT starts in middle of first pixel anymore

		// Calculate start amount and increment for our 4 fields

		for (int i = 0; i < 4; i++)
		{
			int yBeg;
			int64_t xInc;

			if (leftPos[i] < rightPos[i])
			{
				yBeg = leftPos[i];
				xInc = (int64_t) 65536*65536 / (rightPos[i] - leftPos[i]+1);
			}
			else
			{
				yBeg = rightPos[i];
				xInc = (int64_t) 65536*65536 / (leftPos[i] - rightPos[i]+1);
			}

			limit(xInc, (int64_t) 0, (int64_t) 65536);

			inc[i] = (int) xInc;

			int64_t startAmount = xInc - ((xInc * (yBeg-columnBeg)) >> 16);
			amount[i] = (int)startAmount;
		}

		// Do clipping

		if (columnBeg < (clipBeg<<16))
		{
			int64_t forwardAmount = (clipBeg<<16) - columnBeg;

			for (int i = 0; i < 4; i++)
				amount[i] += (int) ((inc[i]*forwardAmount) >> 16);

			columnBeg = (clipBeg<<16);
		}

		uint8_t * pDstClip = pColumn + (clipBeg + clipLen) * linePitch;

		// Render the column

		uint8_t * pDst = pColumn + linePitch * (columnBeg>>16);

		// First render loop, run until we are fully into fill (or later section).
		// This needs to cover all possibilities since topLine, fill and bottomLine might be less than 1 pixel combined
		// in which case they should all be rendered.

		while (amount[1] < 65536 && pDst < pDstClip)
		{
			int aFrac = amount[0];
			int bFrac = amount[1];
			int cFrac = amount[2];
			int dFrac = amount[3];
			limit(aFrac, 0, 65536);
			limit(bFrac, 0, 65536);
			limit(cFrac, 0, 65536);
			limit(dFrac, 0, 65536);

			aFrac = ((aFrac - bFrac)*col[0].a) / 255;
			bFrac = ((bFrac - cFrac)*col[1].a) / 255;
			cFrac = ((cFrac - dFrac)*col[2].a) / 255;

			int backFraction = 65536 - aFrac - bFrac - cFrac;

			pDst[0] = (pDst[0] * backFraction + col[0].b * aFrac + col[1].b * bFrac + col[2].b * cFrac) >> 16;
			pDst[1] = (pDst[1] * backFraction + col[0].g * aFrac + col[1].g * bFrac + col[2].g * cFrac) >> 16;
			pDst[2] = (pDst[2] * backFraction + col[0].r * aFrac + col[1].r * bFrac + col[2].r * cFrac) >> 16;
			pDst += linePitch;

			for (int i = 0; i < 4; i++)
				amount[i] += inc[i];
		}

		// Second render loop, optimzed fill-section loop until bottomLine starts to fade in.

		if(amount[2] <= 0 && pDst < pDstClip)
		{
			if (col[1].a == 255)
			{
				while (amount[2] <= 0 && pDst < pDstClip)
				{
					pDst[0] = col[1].b;
					pDst[1] = col[1].g;
					pDst[2] = col[1].r;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
			}
			else
			{
				int fillFrac = (65536 * col[1].a) / 255;

				int fillB = col[1].b * fillFrac;
				int fillG = col[1].g * fillFrac;
				int fillR = col[1].r * fillFrac;
				int backFraction = 65536 - fillFrac;

				while (amount[2] <= 0 && pDst < pDstClip)
				{
					pDst[0] = (pDst[0] * backFraction + fillB) >> 16;
					pDst[1] = (pDst[1] * backFraction + fillG) >> 16;
					pDst[2] = (pDst[2] * backFraction + fillR) >> 16;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
			}
		}


		// Third render loop, from when bottom line has started to fade in.
		// We can safely ignore topLine (not visible anymore) and amount[2] is guaranteed to have reached 65536.

		while (amount[3] < 65536 && pDst < pDstClip)
		{
			int cFrac = amount[2];
			int dFrac = amount[3];
			limit(cFrac, 0, 65536);
			limit(dFrac, 0, 65536);

			int bFrac = ((65536 - cFrac)*col[1].a) / 255;
			cFrac = ((cFrac - dFrac)*col[2].a) / 255;

			int backFraction = 65536 - bFrac - cFrac;

			pDst[0] = (pDst[0] * backFraction + col[1].b * bFrac + col[2].b * cFrac) >> 16;
			pDst[1] = (pDst[1] * backFraction + col[1].g * bFrac + col[2].g * cFrac) >> 16;
			pDst[2] = (pDst[2] * backFraction + col[1].r * bFrac + col[2].r * cFrac) >> 16;
			pDst += linePitch;

			amount[2] += inc[2];
			amount[3] += inc[3];
		}
	}

	//_____ _clip_wave_blend_32() ________________________________________________

	void SoftGfxDevice::_clip_wave_blend_32(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch)
	{
		// 16 binals on leftPos, rightPos and most calculations.

		int i = 0;

		int amount[4];
		int inc[4];

		int columnBeg = (min(leftPos[0], rightPos[0]) & 0xFFFF0000); // +32768;	 Column does NOT start in middle of first pixel anymore.

																					// Calculate start amount and increment for our 4 fields

		for (int i = 0; i < 4; i++)
		{
			int yBeg;
			int64_t xInc;

			if (leftPos[i] < rightPos[i])
			{
				yBeg = leftPos[i];
				xInc = (int64_t)65536 * 65536 / (rightPos[i] - leftPos[i] + 1);
			}
			else
			{
				yBeg = rightPos[i];
				xInc = (int64_t)65536 * 65536 / (leftPos[i] - rightPos[i] + 1);
			}

			limit(xInc, (int64_t)0, (int64_t)65536);

			inc[i] = (int)xInc;

			int64_t startAmount = xInc - ((xInc * (yBeg - columnBeg)) >> 16);
			amount[i] = (int)startAmount;
		}

		// Do clipping

		if (columnBeg < (clipBeg << 16))
		{
			int64_t forwardAmount = (clipBeg << 16) - columnBeg;

			for (int i = 0; i < 4; i++)
				amount[i] += (int)((inc[i] * forwardAmount) >> 16);

			columnBeg = (clipBeg << 16);
		}

		uint8_t * pDstClip = pColumn + (clipBeg + clipLen) * linePitch;

		// Render the column

		uint8_t * pDst = pColumn + linePitch * (columnBeg >> 16);

		// First render loop, run until we are fully into fill (or later section).
		// This needs to cover all possibilities since topLine, fill and bottomLine might be less than 1 pixel combined
		// in which case they should all be rendered.

		while (amount[1] < 65536 && pDst < pDstClip)
		{
			int aFrac = amount[0];
			int bFrac = amount[1];
			int cFrac = amount[2];
			int dFrac = amount[3];
			limit(aFrac, 0, 65536);
			limit(bFrac, 0, 65536);
			limit(cFrac, 0, 65536);
			limit(dFrac, 0, 65536);

			aFrac = ((aFrac - bFrac)*col[0].a) / 255;
			bFrac = ((bFrac - cFrac)*col[1].a) / 255;
			cFrac = ((cFrac - dFrac)*col[2].a) / 255;

			int backFraction = 65536 - aFrac - bFrac - cFrac;

			pDst[0] = (pDst[0] * backFraction + col[0].b * aFrac + col[1].b * bFrac + col[2].b * cFrac) >> 16;
			pDst[1] = (pDst[1] * backFraction + col[0].g * aFrac + col[1].g * bFrac + col[2].g * cFrac) >> 16;
			pDst[2] = (pDst[2] * backFraction + col[0].r * aFrac + col[1].r * bFrac + col[2].r * cFrac) >> 16;
			pDst[3] = (pDst[3] * backFraction + 255 * aFrac + 255 * bFrac + 255 * cFrac) >> 16;
			pDst += linePitch;

			for (int i = 0; i < 4; i++)
				amount[i] += inc[i];
		}

		// Second render loop, optimzed fill-section loop until bottomLine starts to fade in.

		if (amount[2] <= 0 && pDst < pDstClip)
		{
			if (col[1].a == 255)
			{
				while (amount[2] <= 0 && pDst < pDstClip)
				{
					pDst[0] = col[1].b;
					pDst[1] = col[1].g;
					pDst[2] = col[1].r;
					pDst[3] = col[1].a;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
			}
			else
			{
				int fillFrac = (65536 * col[1].a) / 255;

				int fillB = col[1].b * fillFrac;
				int fillG = col[1].g * fillFrac;
				int fillR = col[1].r * fillFrac;
				int fillA = 255 * fillFrac;
				int backFraction = 65536 - fillFrac;

				while (amount[2] <= 0 && pDst < pDstClip)
				{
					pDst[0] = (pDst[0] * backFraction + fillB) >> 16;
					pDst[1] = (pDst[1] * backFraction + fillG) >> 16;
					pDst[2] = (pDst[2] * backFraction + fillR) >> 16;
					pDst[3] = (pDst[3] * backFraction + fillA) >> 16;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
			}
		}


		// Third render loop, from when bottom line has started to fade in.
		// We can safely ignore topLine (not visible anymore) and amount[2] is guaranteed to have reached 65536.

		while (amount[3] < 65536 && pDst < pDstClip)
		{
			int cFrac = amount[2];
			int dFrac = amount[3];
			limit(cFrac, 0, 65536);
			limit(dFrac, 0, 65536);

			int bFrac = ((65536 - cFrac)*col[1].a) / 255;
			cFrac = ((cFrac - dFrac)*col[2].a) / 255;

			int backFraction = 65536 - bFrac - cFrac;

			pDst[0] = (pDst[0] * backFraction + col[1].b * bFrac + col[2].b * cFrac) >> 16;
			pDst[1] = (pDst[1] * backFraction + col[1].g * bFrac + col[2].g * cFrac) >> 16;
			pDst[2] = (pDst[2] * backFraction + col[1].r * bFrac + col[2].r * cFrac) >> 16;
			pDst[3] = (pDst[3] * backFraction + 255 * bFrac + 255 * cFrac) >> 16;
			pDst += linePitch;

			amount[2] += inc[2];
			amount[3] += inc[3];
		}
	}

	//____ drawSegments() ______________________________________________________

	const static int c_maxSegments = 16;

	void SoftGfxDevice::drawSegments(const Rect& dest, int nSegments, Color * pSegmentColors, int * pEdges, int edgeStripPitch)
	{
		SegmentEdge edges[c_maxSegments];

		Rect clipped(m_clip, dest);

		int clipBeg = (clipped.y - dest.y) * 256;
		int clipEnd = clipBeg + (clipped.h * 256);

		uint8_t * pStripStart = m_pCanvasPixels + dest.y * m_canvasPitch + clipped.x * (m_canvasPixelBits / 8);

		int clippedX = clipped.x - dest.x;
		pEdges += clippedX * edgeStripPitch;

		for (int x = 0; x < clipped.w; x++)
		{
			for ( int y = 0; y < nSegments - 1; y++)
			{
				int beg = pEdges[y];
				int end = pEdges[y + edgeStripPitch];

				if (beg > end)
					swap(beg, end);

				int coverageInc = (end == beg) ? 0 : (65536 * 256) / (end - beg);

				edges[y].begin = beg;
				edges[y].end = end;
				edges[y].coverage = 0;
				edges[y].coverageInc = coverageInc;
			}

			_clipDrawSegmentStrip(clipBeg, clipEnd, pStripStart, m_canvasPitch, nSegments-1, edges, pSegmentColors);
			pEdges += edgeStripPitch;
			pStripStart += (m_canvasPixelBits / 8);
		}
	}

	//____ _clipDrawSegmentStrip() _______________________________________________

	void SoftGfxDevice::_clipDrawSegmentStrip(int clipBeg, int clipEnd, uint8_t * pStripStart, int pixelPitch, int nEdges, SegmentEdge * pEdges, Color * pSegmentColors)
	{
		// Do clipping of edges, part 1 - completely remove segments that are fully clipped

		while (nEdges > 0 && pEdges[nEdges - 1].begin >= clipEnd)
		{
			nEdges--;																	// Edge fully below clip rectangle, segment following edge will never be shown
		}

		while (nEdges > 0 && pEdges[0].end <= clipBeg)
		{
			pSegmentColors++;
			pEdges++;																	// Edge fully above clip rectangel, segment preceeding edge will never be shown
			nEdges--;
		}

		// Do clipping of edges, part 2 - adjust edges of partially clipped segments

		for (int i = 0; i < nEdges; i++)
		{
			SegmentEdge * p = pEdges + i;

			if (p->begin < clipBeg)
			{
				int cut = clipBeg - p->begin;
				p->begin = clipBeg;
				p->coverage += (p->coverageInc*cut) >> 8;
			}

			if (p->end > clipEnd)
				p->end = clipEnd;
		}

		// Render the column

		int offset = clipBeg;				// 24.8 format, but binals cleared (always pointing at beginning of full pixel).
		uint8_t * pDst = pStripStart + (offset>>8)*pixelPitch;

		while (offset < clipEnd)
		{

			//			for (int i = 0; i < nEdges; i++)
			//			{
			//				if (pEdges[i].coverage > 65536)
			//					pEdges[i].coverage = 65536;
			//			}


			if (nEdges == 0 || offset + 255 < pEdges[0].begin)
			{
				// We are fully inside a segment, no need to take any edge into account.

				int end = nEdges == 0 ? clipEnd : std::min(clipEnd, pEdges[0].begin);
				Color segmentColor = *pSegmentColors;

				if (segmentColor.a == 0)
				{
					pDst = pStripStart + (end >> 8) * pixelPitch;
					offset = end & 0xFFFFFF00;												// Just skip segment since it is transparent
				}
				else
				{
					while (offset + 255 < end)
					{
						uint8_t backB, backG, backR, backA;
						_read_pixel(pDst, PixelFormat::BGR_8, nullptr, backB, backG, backR, backA);

						uint8_t outB, outG, outR, outA;
						_blend_pixels(BlendMode::Blend, segmentColor.b, segmentColor.g, segmentColor.r, segmentColor.a, backB, backG, backR, backA, outB, outG, outR, outA);

						_write_pixel(pDst, PixelFormat::BGR_8, outB, outG, outR, outA);

						pDst += pixelPitch;
						offset += 256;
					}
				}
			}
			else
			{
				Color	* pCol = pSegmentColors;

				{
					int edge = 0;

					int backFraction = 65536;
					int	segmentFractions[c_maxSegments];
					int remainingFractions = 65536;

					while (edge < nEdges && offset + 255 >= pEdges[edge].begin)
					{
						int frac;				// Fractions of pixel below edge.

						if (offset + 255 < pEdges[edge].end)
						{
							int beginHeight = 256 - (pEdges[edge].begin & 0xFF);
							int coverageInc = (pEdges[edge].coverageInc * beginHeight) >> 8;

							frac = ((pEdges[edge].coverage + coverageInc / 2) * beginHeight) >> 8;

							pEdges[edge].coverage += coverageInc;
							pEdges[edge].begin = offset + 256;
						}
						else
						{
							frac = ((((pEdges[edge].coverage + 65536) / 2) * (pEdges[edge].end - pEdges[edge].begin)) >> 8)
								+ (256 - pEdges[edge].end & 0xFF) * 65536 / 256;
						}

						segmentFractions[edge] = ((remainingFractions - frac) * pCol[edge].a) / 255;

						if (segmentFractions[edge] < 0 || segmentFractions[edge] > 65536)
							int err = 1;

						backFraction -= segmentFractions[edge];
						remainingFractions = frac;
						edge++;
					}

					segmentFractions[edge] = (remainingFractions * pCol[edge].a) / 255;
					backFraction -= segmentFractions[edge];

					uint8_t backB, backG, backR, backA;

					_read_pixel(pDst, PixelFormat::BGR_8, nullptr, backB, backG, backR, backA);

					int accB = backB * backFraction;
					int accG = backG * backFraction;
					int accR = backR * backFraction;

					for (int i = 0; i <= edge; i++)
					{
						accB += pCol[i].b * segmentFractions[i];
						accG += pCol[i].g * segmentFractions[i];
						accR += pCol[i].r * segmentFractions[i];
					}

					//					uint8_t outB, outG, outR, outA;
					//					_blend_pixels(BlendMode::Blend, accB >> 16, accG >> 16, accR >> 16, segmentColor.a, backB, backG, backR, backA, outB, outG, outR, outA);

					//					_write_pixel(pDst, PixelFormat::BGR_8, outB, outG, outR, outA);

					_write_pixel(pDst, PixelFormat::BGR_8, accB >> 16, accG >> 16, accR >> 16, 255);		//TODO: Handle alpha correctly when writing to destination with alpha channel.
				}
				pDst += pixelPitch;
				offset += 256;
			}

			while (nEdges > 0 && offset >= pEdges[0].end)
			{
				pEdges++;
				nEdges--;
				pSegmentColors++;
			}

		}

	}



	//____ clipDrawHorrWave() _____________________________________________________
/*
	void SoftGfxDevice::clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		// Do early rough X-clipping with margin (need to trace lines with margin of thickest line).

		int ofs = 0;
		if (clip.x > begin.x || clip.x + clip.w < begin.x + length)
		{
			int margin = (int)(max(pTopBorder->thickness, pBottomBorder->thickness) / 2 + 0.99);

			if (clip.x > begin.x + margin)
			{
				ofs = clip.x - begin.x - margin;
				begin.x += ofs;
				length -= ofs;
			}

			if (begin.x + length - margin > clip.x + clip.w)
				length = clip.x + clip.w - begin.x + margin;

			if (length <= 0)
				return;
		}

		// Trace lines and generate edges

		int bufferSize = (length + 1) * sizeof(SegmentEdge)*4;
		char * pBuffer = Base::memStackAlloc(bufferSize);

		SegmentEdge * pEdges = (SegmentEdge *)pBuffer;

		_lineToEdges(pTopBorder, ofs, length+1, pEdges, 4);
		_lineToEdges(pBottomBorder, ofs, length + 1, &pEdges[2], 4);


		// Generate line traces
//
//		int	bufferSize = (length + 1) * 2 * sizeof(int) * 2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
//		char * pBuffer = Base::memStackAlloc(bufferSize);
//		int * pTopBorderTrace = (int*)pBuffer;
//		int * pBottomBorderTrace = (int*)(pBuffer + bufferSize / 2);

//		_traceLine2(pTopBorderTrace, length + 1, pTopBorder, ofs);
//		_traceLine2(pBottomBorderTrace, length + 1, pBottomBorder, ofs);

		// Do proper X-clipping

		int startColumn = 0;
		if (begin.x < clip.x)
		{
			startColumn = clip.x - begin.x;
			length -= startColumn;
			begin.x += startColumn;
		}

		if (begin.x + length > clip.x + clip.w)
			length = clip.x + clip.w - begin.x;

		// Render columns

		uint8_t * pColumn = m_pCanvasPixels + begin.y * m_canvasPitch + begin.x * (m_canvasPixelBits / 8);

		int clipBeg = clip.y - begin.y;
		int clipLen = clip.h;


		Color	topBorderColor = pTopBorder->color;
		Color	bottomBorderColor = pBottomBorder->color;

		Color	fillColor = frontFill, otherFillColor = backFill;


		SegmentEdge	edges[4];
		Color	col[5];
		col[0] = Color::Transparent;


		for (int i = startColumn; i <= length + startColumn; i++)
		{

			// Check if lines have intersected and in that case swap top and bottom lines and colors

			if (pEdges[0].begin > pEdges[2].begin)
			{
				swap(pEdges[0], pEdges[2]);
				swap(pEdges[1], pEdges[3]);

				swap(topBorderColor, bottomBorderColor);
				swap(fillColor, otherFillColor);
			}

			// Generate new segment edges

			int nEdges = 4;

			col[1] = topBorderColor;
			col[2] = fillColor;

			if (pEdges[2].begin < pEdges[1].begin)			// Check if lower line is partially covering upper line. If it is, we need to remove edge between upper line and fill area.
			{
				nEdges--;

				pEdges[1] = pEdges[2];
				pEdges[2] = pEdges[3];
			}

			col[nEdges-1] = bottomBorderColor;
			col[nEdges] = Color::Transparent;

			// Render the column

			if (i >= startColumn)
			{
				_clipDrawSegmentColumn(clipBeg, clipBeg+clipLen, pColumn, m_canvasPitch, nEdges, pEdges, col);				
				pColumn += m_canvasPixelBits / 8;
				pEdges += 4;
			}
		}

		// Free temporary work memory

		Base::memStackRelease(bufferSize);
	}
*/


	//____ _lineToEdges() __________________________________________________________

	void SoftGfxDevice::_lineToEdges(const WaveLine * pWave, int offset, int nPoints, SegmentEdge * pDest, int pitch )
	{
		static int brush[128];
		static float prevThickness = -1.f;

		float thickness = pWave->thickness;
		int brushSteps = (int)(thickness / 2 + 0.99f);

		// Generate brush

		if (thickness != prevThickness)
		{
			int scaledThickness = (int)(thickness / 2 * 256);

			brush[0] = scaledThickness;
			for (int i = 1; i < brushSteps; i++)
			{
				brush[i] = (scaledThickness * s_pCurveTab[c_nCurveTabEntries - (i*c_nCurveTabEntries) / brushSteps - 1]) >> 16;
				//				printf( "%d - %d - %d\n", i, brush[i], m_pCurveTab[(c_nCurveTabEntries - 1) - (i * c_nCurveTabEntries) / brushSteps]);
			}
			prevThickness = thickness;
		}

		int nTracePoints = max(0, min(nPoints, pWave->length - offset));
		int nFillPoints = nPoints - nTracePoints;

		// Trace...

		int * pSrc = pWave->pWave + offset;
		SegmentEdge * pWrite = pDest;
		for (int i = 0; i < nTracePoints; i++)
		{
			// Start with top and bottom for current point

			int top = pSrc[i] - brush[0];
			int bottom = pSrc[i] + brush[0];

			// Check brush's coverage from previous points

			int end = min(i + 1, brushSteps);

			for (int j = 1; j < end; j++)
			{
				int topCover = pSrc[i - j] - brush[j];
				int bottomCover = pSrc[i - j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Check brush's coverage from following points

			end = min(nPoints - i, brushSteps);

			for (int j = 1; j < end; j++)
			{
				int topCover = pSrc[i + j] - brush[j];
				int bottomCover = pSrc[i + j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Save traced values

			pWrite[0].begin = top;
			pWrite[1].begin = bottom;


			pWrite += pitch;

		}

		// Fill out the edge specifications if we didn't have enough wave data

		if (nFillPoints)
		{
			int top = pWave->hold - brush[0];
			int bottom = pWave->hold + brush[0];

			for (int i = 0; i < nFillPoints; i++)
			{
				pWrite[0].begin = top;
				pWrite[0].end = top;
				pWrite[0].coverage = 0;
				pWrite[0].coverageInc = 0;

				pWrite[1].begin = bottom;
				pWrite[1].end = bottom;
				pWrite[1].coverage = 0;
				pWrite[1].coverageInc = 0;

				pWrite += pitch;
			}
		}

		// Calculate rest of the edge data for our traced points (not needed for the filled points)

		int nEdges = nFillPoints > 0 ? nTracePoints : nTracePoints-1;

		pWrite = pDest;
		for (int i = 0; i < nEdges; i++)
		{
			{
				int edgeBegin = pWrite[0].begin;
				int edgeEnd = pWrite[pitch].begin;
				int coverageStart = 0;

				if (edgeBegin > edgeEnd)
					swap(edgeBegin, edgeEnd);

				int coverageInc = (edgeEnd == edgeBegin) ? 0 : (65536 * 256) / (edgeEnd - edgeBegin);


				pWrite[0].begin = edgeBegin;
				pWrite[0].end = edgeEnd;
				pWrite[0].coverage = coverageStart;
				pWrite[0].coverageInc = coverageInc;
			}

			{
				int edgeBegin = pWrite[1].begin;
				int edgeEnd = pWrite[1+pitch].begin;
				if (edgeBegin > edgeEnd)
					swap(edgeBegin, edgeEnd);

				int coverageInc = (edgeEnd == edgeBegin) ? 0 : (65536 * 256) / (edgeEnd - edgeBegin);

				int coverageStart = 0;

				pWrite[1].begin = edgeBegin;
				pWrite[1].end = edgeEnd;
				pWrite[1].coverage = coverageStart;
				pWrite[1].coverageInc = coverageInc;
			}

			pWrite += pitch;
		}
	}



	

	//____ plotPixels() ____________________________________________________

	void SoftGfxDevice::plotPixels( int nCoords, const Coord * pCoords, const Color * pColors)
	{
		const int pitch =m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits/8;

		ColTrans	colTrans{ Color::White, nullptr, nullptr };

		PlotListOp_p pOp = s_plotListOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (pOp)
			pOp(m_clip, nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch, colTrans);
	}

	//____ setBlitSource() ____________________________________________________

	bool SoftGfxDevice::setBlitSource(Surface * pSource)
	{
		if (pSource == m_pBlitSource)
			return true;

		if (!pSource || !pSource->isInstanceOf(SoftSurface::CLASSNAME))
		{
			m_pBlitSource = nullptr;
			GfxDevice::m_pBlitSource = nullptr;
			return false;
		}

		SoftSurface * pSrcSurf = (SoftSurface*)pSource;

		if (!pSrcSurf->m_pData)
		{
			m_pBlitSource = nullptr;
			GfxDevice::m_pBlitSource = nullptr;
			return false;
		}

		m_pBlitSource = pSrcSurf;
		GfxDevice::m_pBlitSource = pSource;

		_updateBlitFunctions();
		return true;
	}


	//____ transformBlit() [simple] ____________________________________________________

	void SoftGfxDevice::transformBlit(const Rect& _dest, Coord _src, const int simpleTransform[2][2])
	{
		Rect	dest = _dest;
		Coord	src = _src;

		// Do Clipping

		if ((m_clip.x > dest.x) || (m_clip.x + m_clip.w < dest.x + dest.w) ||
			(m_clip.y > dest.y) || (m_clip.y + m_clip.h < dest.y + dest.h))
		{

			if ((m_clip.x > dest.x + dest.w) || (m_clip.x + m_clip.w < dest.x) ||
				(m_clip.y > dest.y + dest.h) || (m_clip.y + m_clip.h < dest.y))
				return;																					// Totally outside clip-rect.

			if (dest.x < m_clip.x)
			{
				int xDiff = m_clip.x - dest.x;
				dest.w -= xDiff;
				dest.x = m_clip.x;

				src.x += xDiff * simpleTransform[0][0];
				src.y += xDiff * simpleTransform[1][0];
			}

			if (dest.y < m_clip.y)
			{
				int yDiff = m_clip.y - dest.y;
				dest.h -= yDiff;
				dest.y = m_clip.y;

				src.x += yDiff * simpleTransform[0][1];
				src.y += yDiff * simpleTransform[1][1];
			}

			if (dest.x + dest.w > m_clip.x + m_clip.w)
				dest.w = (m_clip.x + m_clip.w) - dest.x;

			if (dest.y + dest.h > m_clip.y + m_clip.h)
				dest.h = (m_clip.y + m_clip.h) - dest.y;
		}

		// Render

		(this->*m_pSimpleBlitOp)(dest, src, simpleTransform);
	}

	//____ transformBlit() [complex] ____________________________________________________

	void SoftGfxDevice::transformBlit(const Rect& _dest, CoordF _src, const float complexTransform[2][2])
	{
		Rect	dest = _dest;
		CoordF	src = _src;

		// Do Clipping

		if ((m_clip.x > dest.x) || (m_clip.x + m_clip.w < dest.x + dest.w) ||
			(m_clip.y > dest.y) || (m_clip.y + m_clip.h < dest.y + dest.h))
		{

			if ((m_clip.x > dest.x + dest.w) || (m_clip.x + m_clip.w < dest.x) ||
				(m_clip.y > dest.y + dest.h) || (m_clip.y + m_clip.h < dest.y))
				return;																					// Totally outside clip-rect.

			if (dest.x < m_clip.x)
			{
				int xDiff = m_clip.x - dest.x;
				dest.w -= xDiff;
				dest.x = m_clip.x;

				src.x += xDiff * complexTransform[0][0];
				src.y += xDiff * complexTransform[1][0];
			}

			if (dest.y < m_clip.y)
			{
				int yDiff = m_clip.y - dest.y;
				dest.h -= yDiff;
				dest.y = m_clip.y;

				src.x += yDiff * complexTransform[0][1];
				src.y += yDiff * complexTransform[1][1];
			}

			if (dest.x + dest.w > m_clip.x + m_clip.w)
				dest.w = (m_clip.x + m_clip.w) - dest.x;

			if (dest.y + dest.h > m_clip.y + m_clip.h)
				dest.h = (m_clip.y + m_clip.h) - dest.y;
		}

		// Render

		(this->*m_pComplexBlitOp)(dest, src, complexTransform);
	}

	//____ transformBlitPatches() [simple] ____________________________________

	void SoftGfxDevice::transformBlitPatches(const Rect& _dest, Coord _src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{
		// Clip and render the patches

		Rect clip(_dest, m_clip);

		for (int i = 0; i < nPatches; i++)
		{
			Rect  dest = pPatches[i];

			Coord src = _src;

			Coord	patchOfs = dest.pos() - _dest.pos();


			if ((clip.x > dest.x) || (clip.x + clip.w < dest.x + dest.w) ||
				(clip.y > dest.y) || (clip.y + clip.h < dest.y + dest.h))
			{

				if ((clip.x > dest.x + dest.w) || (clip.x + clip.w < dest.x) ||
					(clip.y > dest.y + dest.h) || (clip.y + clip.h < dest.y))
					continue;																					// Totally outside clip-rect.

				if (dest.x < clip.x)
				{
					int xDiff = clip.x - dest.x;
					dest.w -= xDiff;
					dest.x = clip.x;
					patchOfs.x += xDiff;
				}

				if (dest.y < clip.y)
				{
					int yDiff = clip.y - dest.y;
					dest.h -= yDiff;
					dest.y = clip.y;
					patchOfs.y += yDiff;
				}

				if (dest.x + dest.w > clip.x + clip.w)
					dest.w = (clip.x + clip.w) - dest.x;

				if (dest.y + dest.h > clip.y + clip.h)
					dest.h = (clip.y + clip.h) - dest.y;
			}

			//

			src.x += patchOfs.x * simpleTransform[0][0] + patchOfs.y * simpleTransform[1][0];
			src.y += patchOfs.x * simpleTransform[0][1] + patchOfs.y * simpleTransform[1][1];

			(this->*m_pSimpleBlitOp)(dest, src, simpleTransform);
		}
	}

	//____ transformBlitPatches() [complex] ____________________________________

	void SoftGfxDevice::transformBlitPatches(const Rect& _dest, CoordF _src, const float complexTransform[2][2], int nPatches, const Rect * pPatches)
	{
		// Clip and render the patches

		Rect clip(_dest, m_clip);

		for (int i = 0; i < nPatches; i++)
		{
			Rect	dest = pPatches[i];
			CoordF	src = _src;

			Coord	patchOfs = dest.pos() - _dest.pos();

			// Do Clipping

			if ((clip.x > dest.x) || (clip.x + clip.w < dest.x + dest.w) ||
				(clip.y > dest.y) || (clip.y + clip.h < dest.y + dest.h))
			{

				if ((clip.x > dest.x + dest.w) || (clip.x + clip.w < dest.x) ||
					(clip.y > dest.y + dest.h) || (clip.y + clip.h < dest.y))
					return;																					// Totally outside clip-rect.

				if (dest.x < clip.x)
				{
					int xDiff = clip.x - dest.x;
					dest.w -= xDiff;
					dest.x = clip.x;
					patchOfs.x += xDiff;
				}

				if (dest.y < clip.y)
				{
					int yDiff = clip.y - dest.y;
					dest.h -= yDiff;
					dest.y = clip.y;
					patchOfs.y += yDiff;
				}

				if (dest.x + dest.w > clip.x + clip.w)
					dest.w = (clip.x + clip.w) - dest.x;

				if (dest.y + dest.h > clip.y + clip.h)
					dest.h = (clip.y + clip.h) - dest.y;
			}

			//

			src.x += patchOfs.x * complexTransform[0][0] + patchOfs.y * complexTransform[1][0];
			src.y += patchOfs.x * complexTransform[0][1] + patchOfs.y * complexTransform[1][1];

			// Do render

			(this->*m_pComplexBlitOp)(dest, src, complexTransform);
		}
	}

/*
	//____ transformBlitMulti() [simple] ____________________________________

	void SoftGfxDevice::transformBlitMulti(Surface * _pSrc, const int simpleTransform[2][2], int nBlits, const SimpleBlit * pBlits)
	{
		if (!_pSrc || !m_pCanvas || !_pSrc->isInstanceOf(SoftSurface::CLASSNAME))
			return;

		SoftSurface * pSrcSurf = (SoftSurface*)_pSrc;

		if (!m_pCanvasPixels || !pSrcSurf->m_pData)
			return;

		//

		ColTrans		colTrans{ m_tintColor, nullptr, nullptr };

		int				tintMode = m_tintColor == Color::White ? 0 : 1;
		PixelFormat		srcFormat = pSrcSurf->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pCanvas->pixelFormat();

		// Try to find a suitable one-pass operation

		SimpleBlitOp_p	pOnePassOp = nullptr;
		SimpleBlitOp_p	pReader = nullptr;
		SimpleBlitOp_p	pWriter = nullptr;

		if (m_blendMode == BlendMode::Blend)
		{
			if (dstFormat == PixelFormat::BGRA_8)
				pOnePassOp = s_blendTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				pOnePassOp = s_blendTo_BGR_8_OpTab[(int)srcFormat][tintMode];
		}
		else if (m_blendMode == BlendMode::Replace)
		{
			if (dstFormat == PixelFormat::BGRA_8)
				pOnePassOp = s_moveTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				pOnePassOp = s_moveTo_BGR_8_OpTab[(int)srcFormat][tintMode];
		}

		// Fall back to two-pass rendering.

		if (pOnePassOp == nullptr)
		{
			pReader = s_moveTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
			pWriter = s_pass2OpTab[(int)m_blendMode][(int)dstFormat];

			if (pReader == nullptr || pWriter == nullptr)
				return;
		}

		// Clip and render the patches

		Rect& clip = m_clip;

		for (int i = 0; i < nBlits; i++)
		{
			Rect  dest = pBlits[i].dest;
			Coord src = pBlits[i].src;

			if ((clip.x > dest.x) || (clip.x + clip.w < dest.x + dest.w) ||
				(clip.y > dest.y) || (clip.y + clip.h < dest.y + dest.h))
			{

				if ((clip.x > dest.x + dest.w) || (clip.x + clip.w < dest.x) ||
					(clip.y > dest.y + dest.h) || (clip.y + clip.h < dest.y))
					continue;																					// Totally outside clip-rect.

				if (dest.x < clip.x)
				{
					int xDiff = clip.x - dest.x;
					dest.w -= xDiff;
					dest.x = clip.x;
					src.x += xDiff * simpleTransform[0][0];
					src.y += xDiff * simpleTransform[0][1];
				}

				if (dest.y < clip.y)
				{
					int yDiff = clip.y - dest.y;
					dest.h -= yDiff;
					dest.y = clip.y;
					src.x += yDiff * simpleTransform[1][0];
					src.y += yDiff * simpleTransform[1][1];
				}

				if (dest.x + dest.w > clip.x + clip.w)
					dest.w = (clip.x + clip.w) - dest.x;

				if (dest.y + dest.h > clip.y + clip.h)
					dest.h = (clip.y + clip.h) - dest.y;
			}

			//

			if (pOnePassOp)
				_onePassSimpleBlit(pOnePassOp, dest, pSrcSurf, src, simpleTransform, colTrans);
			else
				_twoPassSimpleBlit(pReader, pWriter, dest, pSrcSurf, src, simpleTransform, colTrans);
		}

	}

	//____ transformBlitMulti() [complex] ____________________________________

	void SoftGfxDevice::transformBlitMulti(Surface * _pSrc, const float complexTransform[][2][2], int nBlits, const ComplexBlit * pBlits)
	{
		if (!_pSrc || !m_pCanvas || !_pSrc->isInstanceOf(SoftSurface::CLASSNAME))
			return;

		SoftSurface * pSrcSurf = (SoftSurface*)_pSrc;

		if (!m_pCanvasPixels || !pSrcSurf->m_pData)
			return;

		// 


		int				tintMode = m_tintColor == Color::White ? 0 : 1;
		ScaleMode		scaleMode = pSrcSurf->scaleMode();
		PixelFormat		srcFormat = pSrcSurf->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pCanvas->pixelFormat();

		// Try to find a suitable one-pass operation

		ComplexBlitOp_p	pOnePassOp = nullptr;
		ComplexBlitOp_p pReader = nullptr;
		SimpleBlitOp_p	pWriter = nullptr;

		if (m_blendMode == BlendMode::Blend)
		{
			if (dstFormat == PixelFormat::BGRA_8)
				pOnePassOp = s_transformBlendTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				pOnePassOp = s_transformBlendTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
		}
		else if (m_blendMode == BlendMode::Replace)
		{
			if (dstFormat == PixelFormat::BGRA_8)
				pOnePassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				pOnePassOp = s_transformTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
		}

		// Fall back to two-pass rendering.

		if (pOnePassOp == nullptr)
		{
			pReader = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			pWriter = s_pass2OpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

			if (pReader == nullptr || pWriter == nullptr)
				return;
		}

		// Clip and render the patches

		Rect& clip = m_clip;

		for (int i = 0; i < nBlits; i++)
		{
			Rect	dest = pBlits[i].dest;
			CoordF	src = pBlits[i].src;
			int		mtxIdx = pBlits[i].transformIdx;

			// Do Clipping

			if ((clip.x > dest.x) || (clip.x + clip.w < dest.x + dest.w) ||
				(clip.y > dest.y) || (clip.y + clip.h < dest.y + dest.h))
			{

				if ((clip.x > dest.x + dest.w) || (clip.x + clip.w < dest.x) ||
					(clip.y > dest.y + dest.h) || (clip.y + clip.h < dest.y))
					return;																					// Totally outside clip-rect.

				if (dest.x < clip.x)
				{
					int xDiff = clip.x - dest.x;
					dest.w -= xDiff;
					dest.x = clip.x;

					src.x += xDiff * complexTransform[mtxIdx][0][0];
					src.y += xDiff * complexTransform[mtxIdx][0][1];
				}

				if (dest.y < clip.y)
				{
					int yDiff = clip.y - dest.y;
					dest.h -= yDiff;
					dest.y = clip.y;

					src.x += yDiff * complexTransform[mtxIdx][1][0];
					src.y += yDiff * complexTransform[mtxIdx][1][1];
				}

				if (dest.x + dest.w > clip.x + clip.w)
					dest.w = (clip.x + clip.w) - dest.x;

				if (dest.y + dest.h > clip.y + clip.h)
					dest.h = (clip.y + clip.h) - dest.y;
			}

			// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

			if (scaleMode == ScaleMode::Interpolate)
			{
				src.x -= 0.5f;
				src.y -= 0.5f;
			}

			// Do render

			(this->*m_pComplexBlitOp)(dest, src, complexTransform[mtxIdx]);
		}
	}
|*/


	//____ _onePassSimpleBlit() _____________________________________________

	void SoftGfxDevice::_onePassSimpleBlit(const Rect& dest, Coord src, const int simpleTransform[2][2])
	{
		const SoftSurface * pSource = m_pBlitSource;

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitches;

		pitches.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
		pitches.dstX = dstPixelBytes;
		pitches.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitches.srcX*dest.w;
		pitches.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;
		uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + src.x * srcPixelBytes;

		m_pSimpleBlitOnePassOp(pSrc, pDst, pSource->m_pClut, pitches, dest.h, dest.w, m_colTrans);
	}

	//____ _twoPassSimpleBlit() _____________________________________________

	void SoftGfxDevice::_twoPassSimpleBlit(const Rect& dest, Coord src, const int simpleTransform[2][2])
	{
		SoftSurface * pSource = m_pBlitSource;

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass1, pitchesPass2;

		pitchesPass1.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
		pitchesPass1.dstX = 4;
		pitchesPass1.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitchesPass1.srcX*dest.w;
		pitchesPass1.dstY = 0;

		pitchesPass2.srcX = 4;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		int chunkLines;

		if (dest.w >= 2048)
			chunkLines = 1;
		else if (dest.w*dest.h <= 2048)
			chunkLines = dest.h;
		else
			chunkLines = 2048 / dest.w;

		int memBufferSize = chunkLines * dest.w*4;

		uint8_t * pChunkBuffer = (uint8_t*) Base::memStackAlloc(memBufferSize);

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y+line) * m_canvasPitch + dest.x * dstPixelBytes;
			uint8_t * pSrc = pSource->m_pData + (src.y+line) * pSource->m_pitch + src.x * srcPixelBytes;

			m_pSimpleBlitFirstPassOp(pSrc, pChunkBuffer, pSource->m_pClut, pitchesPass1, thisChunkLines, dest.w, m_colTrans);
			m_pBlitSecondPassOp(pChunkBuffer, pDst, nullptr, pitchesPass2, thisChunkLines, dest.w, m_colTrans);

			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _onePassComplexBlit() ____________________________________________

	void SoftGfxDevice::_onePassComplexBlit(const Rect& dest, CoordF pos, const float transformMatrix[2][2])
	{
		const SoftSurface * pSource = m_pBlitSource;

		// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

		if (pSource->m_scaleMode == ScaleMode::Interpolate)
		{
			pos.x -= 0.5f;
			pos.y -= 0.5f;
		}

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;

		m_pComplexBlitOnePassOp(pSource, pos, transformMatrix, pDst, dstPixelBytes, m_canvasPitch - dstPixelBytes * dest.w, dest.h, dest.w, m_colTrans);
	}


	//____ _twoPassComplexBlit() ____________________________________________

	void SoftGfxDevice::_twoPassComplexBlit(const Rect& dest, CoordF pos, const float transformMatrix[2][2])
	{
		const SoftSurface * pSource = m_pBlitSource;

		// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

		if (pSource->m_scaleMode == ScaleMode::Interpolate)
		{
			pos.x -= 0.5f;
			pos.y -= 0.5f;
		}


		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass2;

		pitchesPass2.srcX = 4;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		int chunkLines;

		if (dest.w >= 2048)
			chunkLines = 1;
		else if (dest.w*dest.h <= 2048)
			chunkLines = dest.h;
		else
			chunkLines = 2048 / dest.w;

		int memBufferSize = chunkLines * dest.w * 4;

		uint8_t * pChunkBuffer = (uint8_t*)Base::memStackAlloc(memBufferSize);

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y + line) * m_canvasPitch + dest.x * dstPixelBytes;

			m_pComplexBlitFirstPassOp(pSource, pos, transformMatrix, pChunkBuffer, 4, 0, thisChunkLines, dest.w, m_colTrans);
			m_pBlitSecondPassOp(pChunkBuffer, pDst, nullptr, pitchesPass2, thisChunkLines, dest.w, m_colTrans);

			pos.x += transformMatrix[1][0] * thisChunkLines;
			pos.y += transformMatrix[1][1] * thisChunkLines;

			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _dummySimpleBlit() _________________________________________________

	void SoftGfxDevice::_dummySimpleBlit(const Rect& dest, Coord pos, const int simpleTransform[2][2])
	{
		// Do nothing but prevent crashing or need to check for nullpointer ;)
	}

	//____ _dummyComplexBlit() ________________________________________________

	void SoftGfxDevice::_dummyComplexBlit(const Rect& dest, CoordF pos, const float matrix[2][2])
	{
		// Do nothing but prevent crashing or need to check for nullpointer ;)
	}


	//____ _updateBlitFunctions() _____________________________________________

	void SoftGfxDevice::_updateBlitFunctions()
	{
		// Sanity checking...

		if (!m_pCanvas || !m_pBlitSource || !m_pCanvasPixels || !m_pBlitSource->m_pData)
		{
			m_pSimpleBlitOp = &SoftGfxDevice::_dummySimpleBlit;
			m_pComplexBlitOp = &SoftGfxDevice::_dummyComplexBlit;
			return;
		}

		// 

		int				tintMode = m_colTrans.baseTint == Color::White ? 0 : 1;
		ScaleMode		scaleMode = m_pBlitSource->scaleMode();
		PixelFormat		srcFormat = m_pBlitSource->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pCanvas->pixelFormat();

		// Add fallback back to two-pass rendering.

		m_pSimpleBlitOnePassOp = nullptr;
		m_pComplexBlitOnePassOp = nullptr;

		m_pSimpleBlitFirstPassOp = s_moveTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
		m_pComplexBlitFirstPassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
		m_pBlitSecondPassOp = s_pass2OpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		// Try to find a suitable one-pass operation

		if (m_blendMode == BlendMode::Blend)
		{
			if (dstFormat == PixelFormat::BGRA_8)
			{
				m_pSimpleBlitOnePassOp = s_blendTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
				m_pComplexBlitOnePassOp = s_transformBlendTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			}
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
			{
				m_pSimpleBlitOnePassOp = s_blendTo_BGR_8_OpTab[(int)srcFormat][tintMode];
				m_pComplexBlitOnePassOp = s_transformBlendTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			}
		}
		else if (m_blendMode == BlendMode::Replace)
		{
			if (dstFormat == PixelFormat::BGRA_8)
			{
				m_pSimpleBlitOnePassOp = s_moveTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
				m_pComplexBlitOnePassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			}
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
			{
				m_pSimpleBlitOnePassOp = s_moveTo_BGR_8_OpTab[(int)srcFormat][tintMode];
				m_pComplexBlitOnePassOp = s_transformTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][tintMode];
			}
		}

		// Decide on Proxy function depending on what blit operations we got.

		if (m_pSimpleBlitOnePassOp)
			m_pSimpleBlitOp = &SoftGfxDevice::_onePassSimpleBlit;
		else if (m_pSimpleBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pSimpleBlitOp = &SoftGfxDevice::_twoPassSimpleBlit;
		else
			m_pSimpleBlitOp = &SoftGfxDevice::_dummySimpleBlit;

		if (m_pComplexBlitOnePassOp)
			m_pComplexBlitOp = &SoftGfxDevice::_onePassComplexBlit;
		else if (m_pComplexBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pComplexBlitOp = &SoftGfxDevice::_twoPassComplexBlit;
		else
			m_pComplexBlitOp = &SoftGfxDevice::_dummyComplexBlit;
	}

	//____ _initTables() ___________________________________________________________
	
	void SoftGfxDevice::_initTables()
	{	
		// Init mulTab

		for (int i = 0; i < 256; i++)
			s_mulTab[i] = i * 256 + i + 1;

		// Init lineThicknessTable
		
		for( int i = 0 ; i < 17 ; i++ )
		{
			double b = i/16.0;
			m_lineThicknessTable[i] = (int) (Util::squareRoot( 1.0 + b*b ) * 65536);
		}

		// Clear Op tables

		for (int i = 0; i < BlendMode_size; i++)
		{
			for (int j = 0; j < PixelFormat_size; j++)
			{
				for (int k = 0; k < TintMode_size; k++)
				{
					s_fillOpTab[i][k][j] = nullptr;
				}

				s_plotOpTab[i][j] = nullptr;

				s_pass2OpTab[i][j] = nullptr;
				s_LineOpTab[i][j] = nullptr;
				s_clipLineOpTab[i][j] = nullptr;
				s_plotListOpTab[i][j] = nullptr;
				s_waveOpTab[i][j] = nullptr;
			}
		}

		for (int i = 0; i < PixelFormat_size; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				s_moveTo_BGRA_8_OpTab[i][j] = nullptr;
				s_moveTo_BGR_8_OpTab[i][j] = nullptr;

				s_blendTo_BGRA_8_OpTab[i][j] = nullptr;
				s_blendTo_BGR_8_OpTab[i][j] = nullptr;

				s_transformTo_BGRA_8_OpTab[i][0][j] = nullptr;
				s_transformTo_BGRA_8_OpTab[i][1][j] = nullptr;
				s_transformTo_BGR_8_OpTab[i][0][j] = nullptr;
				s_transformTo_BGR_8_OpTab[i][1][j] = nullptr;

				s_transformBlendTo_BGRA_8_OpTab[i][0][j] = nullptr;
				s_transformBlendTo_BGRA_8_OpTab[i][1][j] = nullptr;
				s_transformBlendTo_BGR_8_OpTab[i][0][j] = nullptr;
				s_transformBlendTo_BGR_8_OpTab[i][1][j] = nullptr;
			}
		}


		// Init Plot Operation Table

		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Replace, 0, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Replace, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _plot<BlendMode::Replace, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _plot<BlendMode::Replace, 0, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Replace, 0, PixelFormat::BGRA_4>;

		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _plot<BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _plot<BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _plot<BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _plot<BlendMode::Add, 0, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Add, 0, PixelFormat::BGRA_4>;

		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _plot<BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _plot<BlendMode::Subtract, 0, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Subtract, 0, PixelFormat::BGRA_4>;

		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _plot<BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _plot<BlendMode::Multiply, 0, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Multiply, 0, PixelFormat::BGRA_4>;

		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _plot<BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _plot<BlendMode::Invert, 0, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Invert, 0, PixelFormat::BGRA_4>;

		// Init Fill Operation Table

		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::None][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::None][(int)PixelFormat::BGRX_8] = _fill<BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::None][(int)PixelFormat::BGR_8] = _fill<BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::None][(int)PixelFormat::BGR_565] = _fill<BlendMode::Replace, TintMode::None, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::None][(int)PixelFormat::BGRA_4] = _fill<BlendMode::Replace, TintMode::None, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::None][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::None][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::None][(int)PixelFormat::BGR_8] = _fill <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::None][(int)PixelFormat::BGR_565] = _fill <BlendMode::Blend, TintMode::None, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::None][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::None][(int) PixelFormat::BGRA_8] = _fill <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::None][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::None][(int)PixelFormat::BGR_8] = _fill <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::None][(int)PixelFormat::BGR_565] = _fill <BlendMode::Add, TintMode::None, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::None][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Add, TintMode::None, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::None][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::None][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::None][(int)PixelFormat::BGR_8] = _fill <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::None][(int)PixelFormat::BGR_565] = _fill <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::None][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::None][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::None][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::None][(int)PixelFormat::BGR_8] = _fill <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::None][(int)PixelFormat::BGR_565] = _fill <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::None][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::None][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::None][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::None][(int)PixelFormat::BGR_8] = _fill <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::None][(int)PixelFormat::BGR_565] = _fill <BlendMode::Invert, TintMode::None, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::None][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::Color][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Replace, TintMode::Color, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::Color][(int)PixelFormat::BGRX_8] = _fill<BlendMode::Replace, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::Color][(int)PixelFormat::BGR_8] = _fill<BlendMode::Replace, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::Color][(int)PixelFormat::BGR_565] = _fill<BlendMode::Replace, TintMode::Color, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::Color][(int)PixelFormat::BGRA_4] = _fill<BlendMode::Replace, TintMode::Color, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::Color][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Blend, TintMode::Color, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::Color][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Blend, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::Color][(int)PixelFormat::BGR_8] = _fill <BlendMode::Blend, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::Color][(int)PixelFormat::BGR_565] = _fill <BlendMode::Blend, TintMode::Color, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::Color][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Blend, TintMode::Color, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::Color][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Add, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::Color][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Add, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::Color][(int)PixelFormat::BGR_8] = _fill <BlendMode::Add, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::Color][(int)PixelFormat::BGR_565] = _fill <BlendMode::Add, TintMode::Color, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::Color][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Add, TintMode::Color, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::Color][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Subtract, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::Color][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Subtract, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::Color][(int)PixelFormat::BGR_8] = _fill <BlendMode::Subtract, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::Color][(int)PixelFormat::BGR_565] = _fill <BlendMode::Subtract, TintMode::Color, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::Color][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Subtract, TintMode::Color, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::Color][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Multiply, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::Color][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Multiply, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::Color][(int)PixelFormat::BGR_8] = _fill <BlendMode::Multiply, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::Color][(int)PixelFormat::BGR_565] = _fill <BlendMode::Multiply, TintMode::Color, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::Color][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Multiply, TintMode::Color, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::Color][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Invert, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::Color][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Invert, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::Color][(int)PixelFormat::BGR_8] = _fill <BlendMode::Invert, TintMode::Color, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::Color][(int)PixelFormat::BGR_565] = _fill <BlendMode::Invert, TintMode::Color, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::Color][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Invert, TintMode::Color, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapX][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Replace, TintMode::MapX, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapX][(int)PixelFormat::BGRX_8] = _fill<BlendMode::Replace, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapX][(int)PixelFormat::BGR_8] = _fill<BlendMode::Replace, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapX][(int)PixelFormat::BGR_565] = _fill<BlendMode::Replace, TintMode::MapX, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapX][(int)PixelFormat::BGRA_4] = _fill<BlendMode::Replace, TintMode::MapX, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapX][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Blend, TintMode::MapX, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapX][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Blend, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapX][(int)PixelFormat::BGR_8] = _fill <BlendMode::Blend, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapX][(int)PixelFormat::BGR_565] = _fill <BlendMode::Blend, TintMode::MapX, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapX][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Blend, TintMode::MapX, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapX][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Add, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapX][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Add, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapX][(int)PixelFormat::BGR_8] = _fill <BlendMode::Add, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapX][(int)PixelFormat::BGR_565] = _fill <BlendMode::Add, TintMode::MapX, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapX][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Add, TintMode::MapX, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapX][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Subtract, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapX][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Subtract, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapX][(int)PixelFormat::BGR_8] = _fill <BlendMode::Subtract, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapX][(int)PixelFormat::BGR_565] = _fill <BlendMode::Subtract, TintMode::MapX, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapX][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Subtract, TintMode::MapX, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapX][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Multiply, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapX][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Multiply, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapX][(int)PixelFormat::BGR_8] = _fill <BlendMode::Multiply, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapX][(int)PixelFormat::BGR_565] = _fill <BlendMode::Multiply, TintMode::MapX, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapX][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Multiply, TintMode::MapX, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapX][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Invert, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapX][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Invert, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapX][(int)PixelFormat::BGR_8] = _fill <BlendMode::Invert, TintMode::MapX, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapX][(int)PixelFormat::BGR_565] = _fill <BlendMode::Invert, TintMode::MapX, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapX][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Invert, TintMode::MapX, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Replace, TintMode::MapY, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapY][(int)PixelFormat::BGRX_8] = _fill<BlendMode::Replace, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapY][(int)PixelFormat::BGR_8] = _fill<BlendMode::Replace, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapY][(int)PixelFormat::BGR_565] = _fill<BlendMode::Replace, TintMode::MapY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapY][(int)PixelFormat::BGRA_4] = _fill<BlendMode::Replace, TintMode::MapY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Blend, TintMode::MapY, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Blend, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Blend, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Blend, TintMode::MapY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Blend, TintMode::MapY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Add, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Add, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Add, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Add, TintMode::MapY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Add, TintMode::MapY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Subtract, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Subtract, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Subtract, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Subtract, TintMode::MapY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Subtract, TintMode::MapY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Multiply, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Multiply, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Multiply, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Multiply, TintMode::MapY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Multiply, TintMode::MapY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Invert, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Invert, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Invert, TintMode::MapY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Invert, TintMode::MapY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Invert, TintMode::MapY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapXY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Replace, TintMode::MapXY, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapXY][(int)PixelFormat::BGRX_8] = _fill<BlendMode::Replace, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapXY][(int)PixelFormat::BGR_8] = _fill<BlendMode::Replace, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapXY][(int)PixelFormat::BGR_565] = _fill<BlendMode::Replace, TintMode::MapXY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Replace][(int)TintMode::MapXY][(int)PixelFormat::BGRA_4] = _fill<BlendMode::Replace, TintMode::MapXY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapXY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Blend, TintMode::MapXY, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapXY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Blend, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapXY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Blend, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapXY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Blend, TintMode::MapXY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Blend][(int)TintMode::MapXY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Blend, TintMode::MapXY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapXY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Add, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapXY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Add, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapXY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Add, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapXY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Add, TintMode::MapXY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Add][(int)TintMode::MapXY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Add, TintMode::MapXY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapXY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Subtract, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapXY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Subtract, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapXY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Subtract, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapXY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Subtract, TintMode::MapXY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)TintMode::MapXY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Subtract, TintMode::MapXY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapXY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Multiply, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapXY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Multiply, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapXY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Multiply, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapXY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Multiply, TintMode::MapXY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)TintMode::MapXY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Multiply, TintMode::MapXY, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapXY][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Invert, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapXY][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Invert, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapXY][(int)PixelFormat::BGR_8] = _fill <BlendMode::Invert, TintMode::MapXY, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapXY][(int)PixelFormat::BGR_565] = _fill <BlendMode::Invert, TintMode::MapXY, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Invert][(int)TintMode::MapXY][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Invert, TintMode::MapXY, PixelFormat::BGRA_4>;


		// Init Line Operation Table

		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Add, 0, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Add, 0, PixelFormat::BGRA_4>;

		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Subtract, 0, PixelFormat::BGRA_4>;

		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Multiply, 0, PixelFormat::BGRA_4>;

		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Invert, 0, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Invert, 0, PixelFormat::BGRA_4>;

		// Init ClipLine Operation Table

		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Add, 0, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Add, 0, PixelFormat::BGRA_4>;

		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Subtract, 0, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Subtract, 0, PixelFormat::BGRA_4>;

		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Multiply, 0, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Multiply, 0, PixelFormat::BGRA_4>;

		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Invert, 0, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Invert, 0, PixelFormat::BGRA_4>;


		// Init PlotList Operation Table

		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Add, 0, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Add, 0, PixelFormat::BGRA_4>;

		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Subtract, 0, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Subtract, 0, PixelFormat::BGRA_4>;

		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Multiply, 0, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Multiply, 0, PixelFormat::BGRA_4>;

		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Invert, 0, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Invert, 0, PixelFormat::BGRA_4>;



		// Init Blit Pass 2 Operation Table

		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGRA_4>;


		// Init straight move to BGRA_8 Operation Table
		// (also used as ass 1 Operation Table for straight blit)

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1] =_simple_blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0] =_simple_blit < PixelFormat::BGR_565, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1] =_simple_blit < PixelFormat::BGR_565, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0] =_simple_blit < PixelFormat::BGRA_4, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1] =_simple_blit < PixelFormat::BGRA_4, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::I8][0] =_simple_blit < PixelFormat::I8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::I8][1] =_simple_blit < PixelFormat::I8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::A8][0] =_simple_blit < PixelFormat::A8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::A8][1] =_simple_blit < PixelFormat::A8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		// Init straight blend to BGRA_8 Operation Table

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1] =_simple_blit < PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0] =_simple_blit < PixelFormat::BGR_565, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1] =_simple_blit < PixelFormat::BGR_565, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0] =_simple_blit < PixelFormat::BGRA_4, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1] =_simple_blit < PixelFormat::BGRA_4, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0] =_simple_blit < PixelFormat::I8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1] =_simple_blit < PixelFormat::I8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0] =_simple_blit < PixelFormat::A8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1] =_simple_blit < PixelFormat::A8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;



		// Init straight move to BGR_8 Operation Table

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1] =_simple_blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0] =_simple_blit < PixelFormat::BGR_565, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1] =_simple_blit < PixelFormat::BGR_565, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0] =_simple_blit < PixelFormat::BGRA_4, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1] =_simple_blit < PixelFormat::BGRA_4, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::I8][0] =_simple_blit < PixelFormat::I8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::I8][1] =_simple_blit < PixelFormat::I8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::A8][0] =_simple_blit < PixelFormat::A8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::A8][1] =_simple_blit < PixelFormat::A8, 1, BlendMode::Replace, PixelFormat::BGR_8>;


		// Init straight blend to BGR_8 Operation Table

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0] =_simple_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1] =_simple_blit < PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0] =_simple_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1] =_simple_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0] =_simple_blit < PixelFormat::BGR_565, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1] =_simple_blit < PixelFormat::BGR_565, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0] =_simple_blit < PixelFormat::BGRA_4, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1] =_simple_blit < PixelFormat::BGRA_4, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::I8][0] =_simple_blit < PixelFormat::I8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::I8][1] =_simple_blit < PixelFormat::I8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::A8][0] =_simple_blit < PixelFormat::A8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::A8][1] =_simple_blit < PixelFormat::A8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		// Init stretch move to BGRA_8 Operation Table

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGRA_8>;


		// Init straight blend to BGRA_8 Operation Table

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8 > ;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8 >;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGRA_8>;


		// Init stretch move to BGR_8 Operation Table

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 1, BlendMode::Replace, PixelFormat::BGR_8>;


		// Init stretch blend to BGR_8 Operation Table

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, 1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, 1, BlendMode::Blend, PixelFormat::BGR_8>;


		// Init Wave Operation Table

		s_waveOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _clip_wave_blend_32;
		s_waveOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _clip_wave_blend_24;

	}

	//____ _clearCustomFunctionTable() ________________________________________

	void SoftGfxDevice::_clearCustomFunctionTable()
	{
		int * p = reinterpret_cast<int*>(&m_customFunctions);
		
		for( int i = 0 ; i < sizeof(CustomFunctionTable)/4 ; i++ )
			p[i] = 0;
	}


	//____ _scaleLineThickness() ___________________________________________________
	
	int SoftGfxDevice::_scaleLineThickness( float thickness, int slope )
	{
		slope = std::abs(slope);
		
		int scale = m_lineThicknessTable[slope>>12];
		
		if( slope < (1 << 16) )
		{
			int scale2 = m_lineThicknessTable[(slope>>12)+1];
			scale += ((scale2-scale)*(slope & 0xFFF)) >> 12;
		}
		
		return (int) (thickness * scale);
	}
	




} // namespace wg
