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
	SoftGfxDevice::FillOp_p		SoftGfxDevice::s_fillOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::LineOp_p		SoftGfxDevice::s_LineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::ClipLineOp_p SoftGfxDevice::s_clipLineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::PlotListOp_p SoftGfxDevice::s_plotListOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::WaveOp_p		SoftGfxDevice::s_waveOpTab[BlendMode_size][PixelFormat_size];

	SoftGfxDevice::BlitOp_p		SoftGfxDevice::s_pass2OpTab[BlendMode_size][PixelFormat_size];

	SoftGfxDevice::BlitOp_p		SoftGfxDevice::s_moveTo_BGRA_8_OpTab[PixelFormat_size][2];
	SoftGfxDevice::BlitOp_p		SoftGfxDevice::s_moveTo_BGR_8_OpTab[PixelFormat_size][2];

	SoftGfxDevice::BlitOp_p		SoftGfxDevice::s_blendTo_BGRA_8_OpTab[PixelFormat_size][2];
	SoftGfxDevice::BlitOp_p		SoftGfxDevice::s_blendTo_BGR_8_OpTab[PixelFormat_size][2]; 

	SoftGfxDevice::TransformOp_p		SoftGfxDevice::s_stretchTo_BGRA_8_OpTab[PixelFormat_size][2];
	SoftGfxDevice::TransformOp_p		SoftGfxDevice::s_stretchTo_BGR_8_OpTab[PixelFormat_size][2];

	SoftGfxDevice::TransformOp_p		SoftGfxDevice::s_stretchBlendTo_BGRA_8_OpTab[PixelFormat_size][2];
	SoftGfxDevice::TransformOp_p		SoftGfxDevice::s_stretchBlendTo_BGR_8_OpTab[PixelFormat_size][2];



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
			pPixel[1] = (g >> 5) | (a & 0xF8);

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

			outB = limitUint8(backB + (srcB * alpha << 16));
			outG = limitUint8(backG + (srcG * alpha << 16));
			outR = limitUint8(backR + (srcR * alpha << 16));
			outA = backA;
		}

		if (mode == BlendMode::Subtract)
		{
			int alpha = s_mulTab[srcA];

			outB = limitUint8(backB + (srcB * alpha << 16));
			outG = limitUint8(backG + (srcG * alpha << 16));
			outR = limitUint8(backR + (srcR * alpha << 16));
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

	//____ _plot() ____________________________________________________________

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	static void SoftGfxDevice::_plot(uint8_t * pDst, Color col, const ColTrans& tint)
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
	static void SoftGfxDevice::_plot_list(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint)
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
	static void SoftGfxDevice::_draw_line(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint)
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
	static void SoftGfxDevice::_clip_draw_line(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint)
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

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	static void SoftGfxDevice::_fill(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint)
	{
		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;
		srcB = col.b;
		srcG = col.g;
		srcR = col.r;
		srcA = col.a;

		// Step 1.5: Apply any baseTint

		if (TINTFLAGS & 0x1)
		{
			srcB = (srcB * s_mulTab[tint.baseTint.b]) >> 16;
			srcG = (srcG * s_mulTab[tint.baseTint.g]) >> 16;
			srcR = (srcR * s_mulTab[tint.baseTint.r]) >> 16;
			srcA = (srcA * s_mulTab[tint.baseTint.a]) >> 16;
		}

		for (int y = 0; y < nLines; y++)
		{
			// Step 2: Apply any vertical tint-map

			for (int x = 0; x < lineLength; x++)
			{
				// Step 3: Apply any horizontal tint-map

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

	template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
	static void SoftGfxDevice::_draw_wave_column(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch)
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




	//____ _blit() ____________________________________________________________

	template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
	static void	SoftGfxDevice::_blit(const uint8_t * pSrc, uint8_t * pDst, const Color * pClut, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint)
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


	//____ _transform_blit __________________________________________

	template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_transform_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint)
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
				// Step 1: Read separate source color components for our 2x2 pixel square.

				uint8_t * p = pSrcSurf->m_pData + (ofsY >> 15) * srcPitch + (ofsX >> 15)*srcPixelBytes;

				uint8_t src11_b, src11_g, src11_r, src11_a;
				uint8_t src12_b, src12_g, src12_r, src12_a;
				uint8_t src21_b, src21_g, src21_r, src21_a;
				uint8_t src22_b, src22_g, src22_r, src22_a;

				_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, src11_b, src11_g, src11_r, src11_a);
				_read_pixel(p+srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src12_b, src12_g, src12_r, src12_a);
				_read_pixel(p + srcPitch, SRCFORMAT, pSrcSurf->m_pClut, src21_b, src21_g, src21_r, src21_a);
				_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src22_b, src22_g, src22_r, src22_a);

				// Step 2: Interpolate our 2x2 source colors into one source color, srcX

				int fracX2 = ofsX & 0x7FFF;
				int fracX1 = 32768 - fracX2;

				int fracY2 = ofsY & 0x7FFF;
				int fracY1 = 32768 - fracY2;

				int mul11 = fracX1 * fracY1 >> 15;
				int mul12 = fracX2 * fracY1 >> 15;
				int mul21 = fracX1 * fracY2 >> 15;
				int mul22 = fracX2 * fracY2 >> 15;

				int srcB = (src11_b * mul11 + src12_b * mul12 + src21_b * mul21 + src22_b * mul22) >> 15;
				int srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> 15;
				int srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> 15;
				int srcA = (src11_a * mul11 + src12_a * mul12 + src21_a * mul21 + src22_a * mul22) >> 15;

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

	template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
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
				// Step 1: Separate source color components for our 2x2 pixel square.

				uint8_t * p = pSrc + (ofsX >> 15)*srcPixelBytes;

				uint8_t src11_b, src11_g, src11_r, src11_a;
				uint8_t src12_b, src12_g, src12_r, src12_a;
				uint8_t src21_b, src21_g, src21_r, src21_a;
				uint8_t src22_b, src22_g, src22_r, src22_a;

				_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, src11_b, src11_g, src11_r, src11_a);
				_read_pixel(p + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src12_b, src12_g, src12_r, src12_a);
				_read_pixel(p + srcPitch, SRCFORMAT, pSrcSurf->m_pClut, src21_b, src21_g, src21_r, src21_a);
				_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src22_b, src22_g, src22_r, src22_a);

				// Step 2: Interpolate our 2x2 source colors into one source color, srcX

				int fracX2 = ofsX & 0x7FFF;
				int fracX1 = 32768 - fracX2;

				int fracY2 = ofsY & 0x7FFF;
				int fracY1 = 32768 - fracY2;

				int mul11 = fracX1 * fracY1 >> 15;
				int mul12 = fracX2 * fracY1 >> 15;
				int mul21 = fracX1 * fracY2 >> 15;
				int mul22 = fracX2 * fracY2 >> 15;

				int srcB = (src11_b * mul11 + src12_b * mul12 + src21_b * mul21 + src22_b * mul22) >> 15;
				int srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> 15;
				int srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> 15;
				int srcA = (src11_a * mul11 + src12_a * mul12 + src21_a * mul21 + src22_a * mul22) >> 15;

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
		if( (pCanvas->pixelFormat() != PixelFormat::BGRA_8) && (pCanvas->pixelFormat() != PixelFormat::BGR_8) )
			return false;
	
		if( m_pCanvas == pCanvas )
			return true;			// Not an error.
	
		if( m_pCanvasPixels )
			m_pCanvas->unlock();

		m_pCanvas = pCanvas;
		if( pCanvas )
		{
			m_canvasSize = pCanvas->size();

			// Update stuff if we are rendering

			if( m_pCanvasPixels )
			{
				m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
				m_canvasPixelBits = m_pCanvas->pixelDescription()->bits;
				m_canvasPitch = m_pCanvas->pitch();

				// Call custom function, let it decide if it can render or not.

				if( m_bEnableCustomFunctions && m_customFunctions.setCanvas )
				{
					int retVal = m_customFunctions.setCanvas( m_pCanvasPixels, (int) pCanvas->pixelFormat(), m_canvasPitch );
					m_bUseCustomFunctions = retVal != 0;
				}
			}
		}
		else
		{
			m_canvasSize = Size();

			// Make sure this also is cleared, in case we are rendering.

			m_pCanvasPixels = 0;
			m_canvasPixelBits = 0;
			m_canvasPitch = 0;
		}
		return true;
	}

	//____ beginRender() _______________________________________________________

	bool SoftGfxDevice::beginRender()
	{
		if( !m_pCanvas)
			return false;

		m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
		m_canvasPixelBits = m_pCanvas->pixelDescription()->bits;
		m_canvasPitch = m_pCanvas->pitch();
		
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
		return true;
	}


	//____ fill() ____________________________________________________________________

	void SoftGfxDevice::fill(const Rect& rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

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

		FillOp_p pFunc = s_fillOpTab[(int)blendMode][(int)m_pCanvas->pixelFormat()];

		if (pFunc)
			pFunc(pDst, pixelBytes, m_canvasPitch - rect.w*pixelBytes, rect.h, rect.w, fillColor, colTrans);
	}

	//____ fillSubPixel() ____________________________________________________________________

	void SoftGfxDevice::fillSubPixel(const RectF& rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		Color fillColor = col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };


		// Skip calls that won't affect destination

		if (fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p pOp = s_fillOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		// Fill all but anti-aliased edges

		int x1 = (int)(rect.x + 0.999f);
		int y1 = (int)(rect.y + 0.999f);
		int x2 = (int)(rect.x + rect.w);
		int y2 = (int)(rect.y + rect.h);


		uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x1 * pixelBytes;
		pOp(pDst, pixelBytes, m_canvasPitch, y2 - y1, x2 - x1, col, colTrans);
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
			pOp = s_fillOpTab[(int)BlendMode::Blend][(int)m_pCanvas->pixelFormat()];		// Need to blend edges and corners anyway
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
//			_drawStraightLineAA(x1, (int)rect.y, x2 - x1, fillColor, blendMode, aaTop, Orientation::Horizontal);
		}

		if (aaBottom != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
			int length = x2 - x1;
			fillColor.a = aaBottom;
			pOp(pDst, pixelBytes, 0, 1, length, fillColor, colTrans);
//			_drawStraightLineAA(x1, (int)y2, x2 - x1, fillColor, blendMode, aaBottom, Orientation::Horizontal);
		}

		if (aaLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + ((int)rect.x) * pixelBytes;
			int length = y2 - y1;
			fillColor.a = aaLeft;
			pOp(pDst, m_canvasPitch, 0, 1, length, fillColor, colTrans);
//			_drawStraightLineAA((int)rect.x, y1, y2 - y1, fillColor, blendMode, aaLeft, Orientation::Vertical);
		}

		if (aaRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
			int length = y2 - y1;
			fillColor.a = aaRight;
			pOp(pDst, m_canvasPitch, 0, 1, length, fillColor, colTrans);
//			_drawStraightLineAA((int)x2, y1, y2 - y1, fillColor, blendMode, aaRight, Orientation::Vertical);
		}

		// Draw corner pieces

		PlotOp_p pPlotOp = s_plotOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (aaTopLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + ((int)rect.x) * pixelBytes;
			fillColor.a = aaTopLeft;
			pPlotOp(pDst, fillColor, colTrans);
//			_plotAA((int)rect.x, (int)rect.y, fillColor, blendMode, aaTopLeft);
		}

		if (aaTopRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + x2 * pixelBytes;
			fillColor.a = aaTopRight;
			pPlotOp(pDst, fillColor, colTrans);
//			_plotAA(x2, (int)rect.y, fillColor, blendMode, aaTopRight);
		}

		if (aaBottomLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + ((int)rect.x) * pixelBytes;
			fillColor.a = aaBottomLeft;
			pPlotOp(pDst, fillColor, colTrans);
//			_plotAA((int)rect.x, y2, fillColor, blendMode, aaBottomLeft);
		}

		if (aaBottomRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
			fillColor.a = aaBottomRight;
			pPlotOp(pDst, fillColor, colTrans);
//			_plotAA(x2, y2, fillColor, blendMode, aaBottomRight);
		}
	}
	
	//____ drawLine() ____ [from/to] __________________________________________

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
		}

		LineOp_p pOp = s_LineOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if( pOp )
			pOp( pRow, rowInc, pixelInc, length, width, pos, slope, fillColor, colTrans );
	}

	//____ drawLine() ____ [start/direction] __________________________________

	void SoftGfxDevice::drawLine(Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		//TODO: Check how much slower thick vertical lines gets than horizontal due to cache trashing caused by us drawing vertically.

		if (thickness <= 0.f)
			return;

		_col = _col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };


		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pOp = s_fillOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

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


	//____ clipDrawLine() ____ [from/to] ______________________________________

	void SoftGfxDevice::clipDrawLine( const Rect& clip, Coord beg, Coord end, Color color, float thickness )
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
			
			if( beg.x > clip.x + clip.w || end.x < clip.x )
				return;										// Segement not visible.
				
			if( beg.x < clip.x )
			{
				int cut = clip.x - beg.x;
				length -= cut;
				pRow += rowInc*cut;
				pos += slope*cut;
			}

			if( end.x > clip.x + clip.w )
				length -= end.x - (clip.x+clip.w);

			clipStart = clip.y << 16;
			clipEnd = (clip.y + clip.h) <<16;
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
			
			if( beg.y > clip.y + clip.h || end.y < clip.y )
				return;										// Segement not visible.
				
			if( beg.y < clip.y )
			{
				int cut = clip.y - beg.y;
				length -= cut;
				pRow += rowInc*cut;
				pos += slope*cut;
			}

			if( end.y > clip.y + clip.h )
				length -= end.y - (clip.y+clip.h);
				
			clipStart = clip.x << 16;
			clipEnd = (clip.x + clip.w) <<16;
		}

		ClipLineOp_p pOp = s_clipLineOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if( pOp )
			pOp( clipStart, clipEnd, pRow, rowInc, pixelInc, length, width, pos, slope, fillColor, colTrans );
	}

	//____ clipDrawLine() ____ [start/direction] ______________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void SoftGfxDevice::clipDrawLine(const Rect& clip, Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		if (thickness <= 0.f)
			return;

		_col = _col * m_tintColor;
		ColTrans	colTrans{ Color::White, nullptr, nullptr };

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pOp = s_fillOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		switch (dir)
		{
		case Direction::Left:
			begin.x -= length;
		case Direction::Right:
		{
			if (begin.x > clip.x + clip.w)
				return;

			if (begin.x < clip.x)
			{
				length -= clip.x - begin.x;
				if (length <= 0)
					return;
				begin.x = clip.x;
			}

			if (begin.x + length > clip.x + clip.w)
			{
				length = clip.x + clip.w - begin.x;
				if (length <= 0)
					return;
			}

			if (thickness <= 1.f)
			{
				if (begin.y < clip.y || begin.y >= clip.y + clip.h)
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

				if (begin.y + expanse <= clip.y || begin.y - expanse >= clip.y + clip.h)
					return;

				int beginY = begin.y - expanse;
				int endY = begin.y + expanse + 1;

				if (beginY < clip.y)
					beginY = clip.y - 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
					pOp(pBegin, pixelBytes, 0, 1, length, edgeColor, colTrans);
//					_drawStraightLine({ begin.x, beginY }, Orientation::Horizontal, length, edgeColor);
				}

				if (endY > clip.y + clip.h)
					endY = clip.y + clip.h + 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + (endY - 1) * m_canvasPitch + begin.x * pixelBytes;
					pOp(pBegin, pixelBytes, 0, 1, length, edgeColor, colTrans);
//					_drawStraightLine({ begin.x, endY - 1 }, Orientation::Horizontal, length, edgeColor);
				}

				int bodyThickness = endY - beginY - 2;
				uint8_t * pBegin = m_pCanvasPixels + (beginY + 1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, m_canvasPitch - bodyThickness * pixelBytes, bodyThickness, length, _col, colTrans);
//				fill({ begin.x, beginY + 1, length, endY - beginY - 2 }, _col);
			}

			break;
		}
		case Direction::Up:
			begin.y -= length;
		case Direction::Down:
			if (begin.y > clip.y + clip.h)
				return;

			if (begin.y < clip.y)
			{
				length -= clip.y - begin.y;
				if (length <= 0)
					return;
				begin.y = clip.y;
			}

			if (begin.y + length > clip.y + clip.h)
			{
				length = clip.y + clip.h - begin.y;
				if (length <= 0)
					return;
			}

			if (thickness <= 1.f)
			{
				if (begin.x < clip.x || begin.x >= clip.x + clip.w)
					return;

				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);

				uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, col, colTrans);
//				_drawStraightLine(begin, Orientation::Vertical, length, col);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				if (begin.x + expanse <= clip.x || begin.x - expanse >= clip.x + clip.w)
					return;

				int beginX = begin.x - expanse;
				int endX = begin.x + expanse + 1;

				if (beginX < clip.x)
					beginX = clip.x - 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
					pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, colTrans);
//					_drawStraightLine({ beginX, begin.y }, Orientation::Vertical, length, edgeColor);
				}

				if (endX > clip.x + clip.w)
					endX = clip.x + clip.w + 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX - 1) * pixelBytes;
					pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, colTrans);
//					_drawStraightLine({ endX - 1, begin.y }, Orientation::Vertical, length, edgeColor);
				}


				int bodyThickness = endX - beginX - 2;
				uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX + 1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, pixelBytes - m_canvasPitch - bodyThickness, bodyThickness, length, _col, colTrans);
//				fill({ beginX + 1, begin.y, endX - beginX - 2, length }, _col);
			}

			break;
		}
	}


	//____ clipDrawHorrWave() _____________________________________________________

	void SoftGfxDevice::clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		// Do early rough X-clipping with margin (need to trace lines with margin of thickest line).

		int ofs = 0;
		if (clip.x > begin.x || clip.x + clip.w < begin.x + length)
		{
			int margin = (int)(max(pTopBorder->thickness, pBottomBorder->thickness) / 2 + 0.99);

			if (clip.x > begin.x + margin )
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

		// Generate line traces

		int	bufferSize = (length+1) * 2 * sizeof(int) *2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
		char * pBuffer = Base::memStackAlloc(bufferSize);
		int * pTopBorderTrace = (int*)pBuffer;
		int * pBottomBorderTrace = (int*) (pBuffer + bufferSize/2);

		_traceLine(pTopBorderTrace, length + 1, pTopBorder, ofs);
		_traceLine(pBottomBorderTrace, length + 1, pBottomBorder, ofs);

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
		int pos[2][4];						// Startpositions for the 4 fields of the column (topline, fill, bottomline, line end) for left and right edge of pixel column. 16 binals.

		int clipBeg = clip.y - begin.y;
		int clipLen = clip.h;

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

		int columnBeg = (min(leftPos[0], rightPos[0]) & 0xFFFF0000) + 32768;		// Column starts in middle of first pixel

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

			int64_t startAmount = -((xInc * (yBeg-columnBeg)) >> 16);
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



	//____ _clipDrawSegmentColumn() _______________________________________________
/*
	void SoftGfxDevice::_clipDrawSegmentColumn(int clipBeg, int clipEnd, uint8_t * pColumn, int linePitch, int nEdges, SegmentEdge * pEdges, Color * pSegmentColors)
	{
		clipBeg <<= 8;
		clipEnd <<= 8;
		
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
				p->coverage += (p->coverageInc*cut) >> (8+8);
			}

			if (p->end > clipEnd)
				p->begin = clipEnd;
		}

		// Render the column

		uint8_t * pDst = pColumn;
		int offset = clipBeg;				// 24.8 format, but binals cleared (always pointing at beginning of full pixel).

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
					pDst = pColumn + (end >> 8) * linePitch;
					offset = end & 0xFFFFFF00;												// Just skip segment since it is transparent
				}
				else
				{
					int storedRed = ((int)segmentColor.r) * segmentColor.a;
					int storedGreen = ((int)segmentColor.g) * segmentColor.a;
					int storedBlue = ((int)segmentColor.b) * segmentColor.a;
					int invAlpha = 255 - segmentColor.a;

					while (offset + 255 < end)
					{
						pDst[0] = m_pDivTab[pDst[0] * invAlpha + storedBlue];
						pDst[1] = m_pDivTab[pDst[1] * invAlpha + storedGreen];
						pDst[2] = m_pDivTab[pDst[2] * invAlpha + storedRed];
						pDst += linePitch;
						offset+=256;
					}
				}
			}
			else
			{
				Color	* pCol = pSegmentColors;

				if (nEdges == 1 || offset + 255 < pEdges[1].begin)
				{

					int aFrac = 65536;
					int bFrac;

					if (offset + 255 < pEdges[0].end)
					{
						int bBeginHeight = 256 - (pEdges[0].begin & 0xFF);
						int bCoverageInc = (pEdges[0].coverageInc * bBeginHeight) >> 8;

						bFrac = ((pEdges[0].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[0].coverage += bCoverageInc;
						pEdges[0].begin = offset + 256;
					}
					else
					{
						bFrac = ((((pEdges[0].coverage + 65536) / 2) * (pEdges[0].end - pEdges[0].begin)) >> 8)
								+ (256 - pEdges[0].end & 0xFF) * 65536 / 256;
					}

					aFrac -= bFrac;

					aFrac = (aFrac*pCol[0].a) / 255;
					bFrac = (bFrac*pCol[1].a) / 255;

					int backFraction = 65536 - aFrac - bFrac;

					pDst[0] = (pDst[0] * backFraction + pCol[0].b * aFrac + pCol[1].b * bFrac) >> 16;
					pDst[1] = (pDst[1] * backFraction + pCol[0].g * aFrac + pCol[1].g * bFrac) >> 16;
					pDst[2] = (pDst[2] * backFraction + pCol[0].r * aFrac + pCol[1].r * bFrac) >> 16;

				}
				else if (nEdges == 2 || offset + 255 < pEdges[2].begin)
				{

					int aFrac = 65536;
					int bFrac;
					int cFrac;

					if (offset + 255 < pEdges[0].end)
					{
						int bBeginHeight = 256 - (pEdges[0].begin & 0xFF);
						int bCoverageInc = (pEdges[0].coverageInc * bBeginHeight) >> 8;

						bFrac = ((pEdges[0].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[0].coverage += bCoverageInc;
						pEdges[0].begin = offset + 256;
					}
					else
					{
						bFrac = ((((pEdges[0].coverage + 65536) / 2) * (pEdges[0].end - pEdges[0].begin)) >> 8)
							+ (256 - pEdges[0].end & 0xFF) * 65536 / 256;
					}

					if (offset + 255 < pEdges[1].end)
					{
						int bBeginHeight = 256 - (pEdges[1].begin & 0xFF);
						int bCoverageInc = (pEdges[1].coverageInc * bBeginHeight) >> 8;

						cFrac = ((pEdges[1].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[1].coverage += bCoverageInc;
						pEdges[1].begin = offset + 256;
					}
					else
					{
						cFrac = ((((pEdges[1].coverage + 65536) / 2) * (pEdges[1].end - pEdges[1].begin)) >> 8)
							+ (256 - pEdges[1].end & 0xFF) * 65536 / 256;
					}

					aFrac -= bFrac;
					bFrac -= cFrac;

					aFrac = (aFrac*pCol[0].a) / 255;
					bFrac = (bFrac*pCol[1].a) / 255;
					cFrac = (cFrac*pCol[2].a) / 255;

					int backFraction = 65536 - aFrac - bFrac - cFrac;

					pDst[0] = (pDst[0] * backFraction + pCol[0].b * aFrac + pCol[1].b * bFrac + pCol[2].b * cFrac) >> 16;
					pDst[1] = (pDst[1] * backFraction + pCol[0].g * aFrac + pCol[1].g * bFrac + pCol[2].g * cFrac) >> 16;
					pDst[2] = (pDst[2] * backFraction + pCol[0].r * aFrac + pCol[1].r * bFrac + pCol[2].r * cFrac) >> 16;

				}
				else if (nEdges == 3 || offset + 255 < pEdges[3].begin)
				{
					int aFrac = 65536;
					int bFrac;
					int cFrac;
					int dFrac;

					if (offset + 255 < pEdges[0].end)
					{
						int bBeginHeight = 256 - (pEdges[0].begin & 0xFF);
						int bCoverageInc = (pEdges[0].coverageInc * bBeginHeight) >> 8;

						bFrac = ((pEdges[0].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[0].coverage += bCoverageInc;
						pEdges[0].begin = offset + 256;
					}
					else
					{
						bFrac = ((((pEdges[0].coverage + 65536) / 2) * (pEdges[0].end - pEdges[0].begin)) >> 8)
							+ (256 - pEdges[0].end & 0xFF) * 65536 / 256;
					}

					if (offset + 255 < pEdges[1].end)
					{
						int bBeginHeight = 256 - (pEdges[1].begin & 0xFF);
						int bCoverageInc = (pEdges[1].coverageInc * bBeginHeight) >> 8;

						cFrac = ((pEdges[1].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[1].coverage += bCoverageInc;
						pEdges[1].begin = offset + 256;
					}
					else
					{
						cFrac = ((((pEdges[1].coverage + 65536) / 2) * (pEdges[1].end - pEdges[1].begin)) >> 8)
							+ (256 - pEdges[1].end & 0xFF) * 65536 / 256;
					}

					if (offset + 255 < pEdges[2].end)
					{
						int bBeginHeight = 256 - (pEdges[2].begin & 0xFF);
						int bCoverageInc = (pEdges[2].coverageInc * bBeginHeight) >> 8;

						dFrac = ((pEdges[2].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[2].coverage += bCoverageInc;
						pEdges[2].begin = offset + 256;
					}
					else
					{
						dFrac = ((((pEdges[2].coverage + 65536) / 2) * (pEdges[2].end - pEdges[2].begin)) >> 8)
							+ (256 - pEdges[2].end & 0xFF) * 65536 / 256;
					}

					aFrac -= bFrac;
					bFrac -= cFrac;
					cFrac -= dFrac;

					aFrac = (aFrac*pCol[0].a) / 255;
					bFrac = (bFrac*pCol[1].a) / 255;
					cFrac = (cFrac*pCol[2].a) / 255;
					dFrac = (dFrac*pCol[3].a) / 255;

					int backFraction = 65536 - aFrac - bFrac - cFrac - dFrac;

					pDst[0] = (pDst[0] * backFraction + pCol[0].b * aFrac + pCol[1].b * bFrac + pCol[2].b * cFrac + pCol[3].b * dFrac) >> 16;
					pDst[1] = (pDst[1] * backFraction + pCol[0].g * aFrac + pCol[1].g * bFrac + pCol[2].g * cFrac + pCol[3].g * dFrac) >> 16;
					pDst[2] = (pDst[2] * backFraction + pCol[0].r * aFrac + pCol[1].r * bFrac + pCol[2].r * cFrac + pCol[3].r * dFrac) >> 16;
				}
				else if (nEdges == 4 || offset + 255 < pEdges[4].begin)
				{
					int aFrac = 65536;
					int bFrac;
					int cFrac;
					int dFrac;
					int eFrac;

					if (offset + 255 < pEdges[0].end)
					{
						int bBeginHeight = 256 - (pEdges[0].begin & 0xFF);
						int bCoverageInc = (pEdges[0].coverageInc * bBeginHeight) >> 8;

						bFrac = ((pEdges[0].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[0].coverage += bCoverageInc;
						pEdges[0].begin = offset + 256;
					}
					else
					{
						bFrac = ((((pEdges[0].coverage + 65536) / 2) * (pEdges[0].end - pEdges[0].begin)) >> 8)
							+ (256 - pEdges[0].end & 0xFF) * 65536 / 256;
					}

					if (offset + 255 < pEdges[1].end)
					{
						int bBeginHeight = 256 - (pEdges[1].begin & 0xFF);
						int bCoverageInc = (pEdges[1].coverageInc * bBeginHeight) >> 8;

						cFrac = ((pEdges[1].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[1].coverage += bCoverageInc;
						pEdges[1].begin = offset + 256;
					}
					else
					{
						cFrac = ((((pEdges[1].coverage + 65536) / 2) * (pEdges[1].end - pEdges[1].begin)) >> 8)
							+ (256 - pEdges[1].end & 0xFF) * 65536 / 256;
					}

					if (offset + 255 < pEdges[2].end)
					{
						int bBeginHeight = 256 - (pEdges[2].begin & 0xFF);
						int bCoverageInc = (pEdges[2].coverageInc * bBeginHeight) >> 8;

						dFrac = ((pEdges[2].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[2].coverage += bCoverageInc;
						pEdges[2].begin = offset + 256;
					}
					else
					{
						dFrac = ((((pEdges[2].coverage + 65536) / 2) * (pEdges[2].end - pEdges[2].begin)) >> 8)
							+ (256 - pEdges[2].end & 0xFF) * 65536 / 256;
					}

					if (offset + 255 < pEdges[3].end)
					{
						int bBeginHeight = 256 - (pEdges[3].begin & 0xFF);
						int bCoverageInc = (pEdges[3].coverageInc * bBeginHeight) >> 8;

						eFrac = ((pEdges[3].coverage + bCoverageInc / 2) * bBeginHeight) >> 8;

						pEdges[3].coverage += bCoverageInc;
						pEdges[3].begin = offset + 256;
					}
					else
					{
						eFrac = ((((pEdges[3].coverage + 65536) / 2) * (pEdges[3].end - pEdges[3].begin)) >> 8)
							+ (256 - pEdges[3].end & 0xFF) * 65536 / 256;
					}

					aFrac -= bFrac;
					bFrac -= cFrac;
					cFrac -= dFrac;
					dFrac -= eFrac;

					aFrac = (aFrac*pCol[0].a) / 255;
					bFrac = (bFrac*pCol[1].a) / 255;
					cFrac = (cFrac*pCol[2].a) / 255;
					dFrac = (dFrac*pCol[3].a) / 255;
					eFrac = (eFrac*pCol[4].a) / 255;

					int backFraction = 65536 - aFrac - bFrac - cFrac - dFrac - eFrac;

					pDst[0] = (pDst[0] * backFraction + pCol[0].b * aFrac + pCol[1].b * bFrac + pCol[2].b * cFrac + pCol[3].b * dFrac + pCol[4].b * eFrac) >> 16;
					pDst[1] = (pDst[1] * backFraction + pCol[0].g * aFrac + pCol[1].g * bFrac + pCol[2].g * cFrac + pCol[3].g * dFrac + pCol[4].g * eFrac) >> 16;
					pDst[2] = (pDst[2] * backFraction + pCol[0].r * aFrac + pCol[1].r * bFrac + pCol[2].r * cFrac + pCol[3].r * dFrac + pCol[4].r * eFrac) >> 16;
				}

				pDst += linePitch;
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
|*/

	//____ clipPlotPixels() ____________________________________________________
	
	void SoftGfxDevice::clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors)
	{
		const int pitch =m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits/8;

		ColTrans	colTrans{ Color::White, nullptr, nullptr };

		PlotListOp_p pOp = s_plotListOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (pOp)
			pOp(clip, nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch, colTrans);
	}

	//____ _drawStraightLine() ________________________________________________
	
	void SoftGfxDevice::_drawStraightLine(Coord start, Orientation orientation, int _length, const Color& _col)
	{
		// Skipping this, only used by GfxDevice for drawLine() and clipDrawLine() which we have overloaded anyway.
	}
	

	
	//____ blit() __________________________________________________________________

	void SoftGfxDevice::blit(Surface * _pSrcSurf, const Rect& srcrect, Coord dest)
	{
		if (!_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME))
			return;

		SoftSurface * pSrcSurf = (SoftSurface*)_pSrcSurf;

		if (!m_pCanvasPixels || !pSrcSurf->m_pData)
			return;

		ColTrans			colTrans{ m_tintColor, nullptr, nullptr };

		int				tintMode = m_tintColor == Color::White ? 0 : 1;
		PixelFormat		srcFormat = pSrcSurf->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pCanvas->pixelFormat();

		// Try to find a suitable one-pass operation

		BlitOp_p	pOnePassOp = nullptr;

		if (m_blendMode == BlendMode::Blend)
		{
			if(dstFormat == PixelFormat::BGRA_8)
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

		if(pOnePassOp)
		{
			_onePassStraightBlit(pOnePassOp, static_cast<SoftSurface*>(_pSrcSurf), srcrect, dest, colTrans);
			return;
		}

		// Fall back to two-pass rendering.

		BlitOp_p pReader = s_moveTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
		BlitOp_p pWriter = s_pass2OpTab[(int)m_blendMode][(int)dstFormat];

		if (pReader == nullptr || pWriter == nullptr)
			return;

		_twoPassStraightBlit(pReader, pWriter, static_cast<SoftSurface*>(_pSrcSurf), srcrect, dest, colTrans);
	}

	//____ _onePassStraightBlit() _____________________________________________

	void SoftGfxDevice::_onePassStraightBlit(BlitOp_p pOp, const SoftSurface * pSource, const Rect& srcrect, Coord dest, const ColTrans& tint)
	{
		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitches;

		pitches.srcX = srcPixelBytes;
		pitches.dstX = dstPixelBytes;
		pitches.srcY = pSource->m_pitch - srcPixelBytes * srcrect.w;
		pitches.dstY = m_canvasPitch - dstPixelBytes * srcrect.w;


		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;
		uint8_t * pSrc = pSource->m_pData + srcrect.y * pSource->m_pitch + srcrect.x * srcPixelBytes;

		pOp(pSrc, pDst, pSource->m_pClut, pitches, srcrect.h, srcrect.w, tint);
	}



	//____ _twoPassStraightBlit() _____________________________________________

	void SoftGfxDevice::_twoPassStraightBlit(BlitOp_p pReader, BlitOp_p pWriter, const SoftSurface * pSource, const Rect& srcrect, Coord dest, const ColTrans& tint)
	{
		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass1, pitchesPass2;

		pitchesPass1.srcX = srcPixelBytes;
		pitchesPass1.dstX = 4;
		pitchesPass1.srcY = pSource->m_pitch - srcPixelBytes * srcrect.w;
		pitchesPass1.dstY = 0;

		pitchesPass2.srcX = 4;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * srcrect.w;

		int chunkLines;

		if (srcrect.w >= 2048)
			chunkLines = 1;
		else if (srcrect.w*srcrect.h <= 2048)
			chunkLines = srcrect.h;
		else
			chunkLines = 2048 / srcrect.w;

		int memBufferSize = chunkLines * srcrect.w*4;

		uint8_t * pChunkBuffer = (uint8_t*) Base::memStackAlloc(memBufferSize);

		int line = 0;

		while (line < srcrect.h)
		{
			int thisChunkLines = min(srcrect.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y+line) * m_canvasPitch + dest.x * dstPixelBytes;
			uint8_t * pSrc = pSource->m_pData + (srcrect.y+line) * pSource->m_pitch + srcrect.x * srcPixelBytes;

			pReader(pSrc, pChunkBuffer, pSource->m_pClut, pitchesPass1, thisChunkLines, srcrect.w, tint);
			pWriter(pChunkBuffer, pDst, nullptr, pitchesPass2, thisChunkLines, srcrect.w, tint);

			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _onePassTransformBlit() ____________________________________________

	void SoftGfxDevice::_onePassTransformBlit(TransformOp_p pOp, const SoftSurface * pSource, CoordF pos, const float transformMatrix[2][2], const Rect& dest, const ColTrans& tint)
	{
		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;

		pOp(pSource, pos, transformMatrix, pDst, dstPixelBytes, m_canvasPitch - dstPixelBytes * dest.w, dest.h, dest.w, tint);
	}


	//____ _twoPassTransformBlit() ____________________________________________

	void SoftGfxDevice::_twoPassTransformBlit(TransformOp_p pReader, BlitOp_p pWriter, const SoftSurface * pSource, CoordF pos, const float transformMatrix[2][2], const Rect& dest, const ColTrans& tint)
	{
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

			pReader(pSource, pos, transformMatrix, pChunkBuffer, 4, 0, thisChunkLines, dest.w, tint);
			pWriter(pChunkBuffer, pDst, nullptr, pitchesPass2, thisChunkLines, dest.w, tint);

			pos.x += transformMatrix[1][0] * thisChunkLines;
			pos.y += transformMatrix[1][1] * thisChunkLines;

			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ stretchBlit() ___________________________________________________

	void SoftGfxDevice::stretchBlit(Surface * _pSrcSurf, const RectF& source, const Rect& dest)
	{
		if (!_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME))
			return;

		SoftSurface * pSrcSurf = (SoftSurface*)_pSrcSurf;

		if (!m_pCanvasPixels || !pSrcSurf->m_pData)
			return;

		ColTrans			colTrans{ m_tintColor, nullptr, nullptr };

		int				tintMode = m_tintColor == Color::White ? 0 : 1;
		PixelFormat		srcFormat = pSrcSurf->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pCanvas->pixelFormat();

		float transform[2][2] = { { source.w / dest.w, 0.f },{ 0.f, source.h / dest.h } };

		// Try to find a suitable one-pass operation

		TransformOp_p	pOnePassOp = nullptr;

		if (m_blendMode == BlendMode::Blend)
		{
			if (dstFormat == PixelFormat::BGRA_8)
				pOnePassOp = s_stretchBlendTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				pOnePassOp = s_stretchBlendTo_BGR_8_OpTab[(int)srcFormat][tintMode];
		}
		else if (m_blendMode == BlendMode::Replace)
		{
			if (dstFormat == PixelFormat::BGRA_8)
				pOnePassOp = s_stretchTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
			else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				pOnePassOp = s_stretchTo_BGR_8_OpTab[(int)srcFormat][tintMode];
		}

		if (pOnePassOp)
		{
			_onePassTransformBlit(pOnePassOp, static_cast<SoftSurface*>(_pSrcSurf), source, transform, dest, colTrans);
			return;
		}

		// Fall back to two-pass rendering.

		TransformOp_p pReader = s_stretchTo_BGRA_8_OpTab[(int)srcFormat][tintMode];
		BlitOp_p pWriter = s_pass2OpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (pReader == nullptr || pWriter == nullptr)
			return;

		_twoPassTransformBlit(pReader, pWriter, static_cast<SoftSurface*>(_pSrcSurf), source, transform, dest, colTrans);
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
				s_plotOpTab[i][j] = nullptr;
				s_fillOpTab[i][j] = nullptr;
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

				s_stretchTo_BGRA_8_OpTab[i][j] = nullptr;
				s_stretchTo_BGR_8_OpTab[i][j] = nullptr;

				s_stretchBlendTo_BGRA_8_OpTab[i][j] = nullptr;
				s_stretchBlendTo_BGR_8_OpTab[i][j] = nullptr;
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

		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Replace, 0, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _fill<BlendMode::Replace, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill<BlendMode::Replace, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _fill<BlendMode::Replace, 0, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _fill<BlendMode::Replace, 0, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Blend, 0, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill <BlendMode::Blend, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _fill <BlendMode::Blend, 0, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Blend, 0, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Add][(int) PixelFormat::BGRA_8] = _fill <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill <BlendMode::Add, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _fill <BlendMode::Add, 0, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Add, 0, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill <BlendMode::Subtract, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _fill <BlendMode::Subtract, 0, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Subtract, 0, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill <BlendMode::Multiply, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _fill <BlendMode::Multiply, 0, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Multiply, 0, PixelFormat::BGRA_4>;

		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _fill <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill <BlendMode::Invert, 0, PixelFormat::BGR_8>;
		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _fill <BlendMode::Invert, 0, PixelFormat::BGR_565>;
		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _fill <BlendMode::Invert, 0, PixelFormat::BGRA_4>;

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

		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Add, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Subtract, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Multiply, PixelFormat::BGRA_4>;

		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Invert, PixelFormat::BGRA_4>;


		// Init straight move to BGRA_8 Operation Table
		// (also used as ass 1 Operation Table for straight blit)

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1] = _blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0] = _blit < PixelFormat::BGR_565, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1] = _blit < PixelFormat::BGR_565, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0] = _blit < PixelFormat::BGRA_4, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1] = _blit < PixelFormat::BGRA_4, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::I8][0] = _blit < PixelFormat::I8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::I8][1] = _blit < PixelFormat::I8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::A8][0] = _blit < PixelFormat::A8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::A8][1] = _blit < PixelFormat::A8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		// Init straight blend to BGRA_8 Operation Table

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1] = _blit < PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0] = _blit < PixelFormat::BGR_565, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1] = _blit < PixelFormat::BGR_565, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0] = _blit < PixelFormat::BGRA_4, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1] = _blit < PixelFormat::BGRA_4, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0] = _blit < PixelFormat::I8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1] = _blit < PixelFormat::I8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0] = _blit < PixelFormat::A8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1] = _blit < PixelFormat::A8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;



		// Init straight move to BGR_8 Operation Table

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1] = _blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0] = _blit < PixelFormat::BGR_565, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1] = _blit < PixelFormat::BGR_565, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0] = _blit < PixelFormat::BGRA_4, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1] = _blit < PixelFormat::BGRA_4, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::I8][0] = _blit < PixelFormat::I8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::I8][1] = _blit < PixelFormat::I8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::A8][0] = _blit < PixelFormat::A8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::A8][1] = _blit < PixelFormat::A8, 1, BlendMode::Replace, PixelFormat::BGR_8>;


		// Init straight blend to BGR_8 Operation Table

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0] = _blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1] = _blit < PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0] = _blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1] = _blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0] = _blit < PixelFormat::BGR_565, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1] = _blit < PixelFormat::BGR_565, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0] = _blit < PixelFormat::BGRA_4, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1] = _blit < PixelFormat::BGRA_4, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::I8][0] = _blit < PixelFormat::I8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::I8][1] = _blit < PixelFormat::I8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::A8][0] = _blit < PixelFormat::A8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::A8][1] = _blit < PixelFormat::A8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		// Init stretch move to BGRA_8 Operation Table

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0] = _stretch_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1] = _stretch_blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0] = _stretch_blit < PixelFormat::BGR_565, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1] = _stretch_blit < PixelFormat::BGR_565, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0] = _stretch_blit < PixelFormat::BGRA_4, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1] = _stretch_blit < PixelFormat::BGRA_4, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::I8][0] = _stretch_blit < PixelFormat::I8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::I8][1] = _stretch_blit < PixelFormat::I8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::A8][0] = _stretch_blit < PixelFormat::A8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_stretchTo_BGRA_8_OpTab[(int)PixelFormat::A8][1] = _stretch_blit < PixelFormat::A8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;

		// Init straight blend to BGRA_8 Operation Table

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0] = _stretch_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1] = _stretch_blit < PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0] = _stretch_blit < PixelFormat::BGR_565, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1] = _stretch_blit < PixelFormat::BGR_565, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0] = _stretch_blit < PixelFormat::BGRA_4, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1] = _stretch_blit < PixelFormat::BGRA_4, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0] = _stretch_blit < PixelFormat::I8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1] = _stretch_blit < PixelFormat::I8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;

		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0] = _stretch_blit < PixelFormat::A8, 0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_stretchBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1] = _stretch_blit < PixelFormat::A8, 1, BlendMode::Blend, PixelFormat::BGRA_8>;


		// Init stretch move to BGR_8 Operation Table

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0] = _stretch_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1] = _stretch_blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0] = _stretch_blit < PixelFormat::BGR_565, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1] = _stretch_blit < PixelFormat::BGR_565, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0] = _stretch_blit < PixelFormat::BGRA_4, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1] = _stretch_blit < PixelFormat::BGRA_4, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::I8][0] = _stretch_blit < PixelFormat::I8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::I8][1] = _stretch_blit < PixelFormat::I8, 1, BlendMode::Replace, PixelFormat::BGR_8>;

		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::A8][0] = _stretch_blit < PixelFormat::A8, 0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_stretchTo_BGR_8_OpTab[(int)PixelFormat::A8][1] = _stretch_blit < PixelFormat::A8, 1, BlendMode::Replace, PixelFormat::BGR_8>;


		// Init stretch blend to BGR_8 Operation Table

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0] = _stretch_blit < PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1] = _stretch_blit < PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0] = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1] = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0] = _stretch_blit < PixelFormat::BGR_565, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1] = _stretch_blit < PixelFormat::BGR_565, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0] = _stretch_blit < PixelFormat::BGRA_4, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1] = _stretch_blit < PixelFormat::BGRA_4, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][0] = _stretch_blit < PixelFormat::I8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][1] = _stretch_blit < PixelFormat::I8, 1, BlendMode::Blend, PixelFormat::BGR_8>;

		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][0] = _stretch_blit < PixelFormat::A8, 0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_stretchBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][1] = _stretch_blit < PixelFormat::A8, 1, BlendMode::Blend, PixelFormat::BGR_8>;



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
