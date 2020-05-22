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

	const TypeInfo SoftGfxDevice::TYPEINFO = { "SoftGfxDevice", &GfxDevice::TYPEINFO };

	int SoftGfxDevice::s_mulTab[256];

	SoftGfxDevice::PlotOp_p		SoftGfxDevice::s_plotOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::FillOp_p		SoftGfxDevice::s_fillOpTab[TintMode_size][BlendMode_size][PixelFormat_size];
	SoftGfxDevice::LineOp_p		SoftGfxDevice::s_LineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::ClipLineOp_p SoftGfxDevice::s_clipLineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::PlotListOp_p SoftGfxDevice::s_plotListOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::SegmentOp_p	SoftGfxDevice::s_segmentOpTab[2][BlendMode_size][PixelFormat_size];		// bVerticalTint, BlendMode, PixelFormat

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_pass2OpTab[TintMode_size][BlendMode_size][PixelFormat_size];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGRA_8_OpTab[PixelFormat_size];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGR_8_OpTab[PixelFormat_size];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGRA_8_OpTab[PixelFormat_size];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGR_8_OpTab[PixelFormat_size];

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

		if (format == PixelFormat::A8)
		{
			pPixel[0] = a;
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

			outB = limitUint8(backB - (srcB * alpha >> 16));
			outG = limitUint8(backG - (srcG * alpha >> 16));
			outR = limitUint8(backR - (srcR * alpha >> 16));
			outA = backA;
		}

		if (mode == BlendMode::Multiply)
		{
			outB = (s_mulTab[backB] * srcB) >> 16;
			outG = (s_mulTab[backG] * srcG) >> 16;
			outR = (s_mulTab[backR] * srcR) >> 16;
			outA = backA;
		}

		if (mode == BlendMode::Min)
		{
			outB = std::min(backB,srcB);
			outG = std::min(backG,srcG);
			outR = std::min(backR,srcR);
			outA = std::min(backA, srcA);
		}

		if (mode == BlendMode::Max)
		{
			outB = std::max(backB, srcB);
			outG = std::max(backG, srcG);
			outR = std::max(backR, srcR);
			outA = std::max(backA, srcA);
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


	//____ _color_tint_init() _________________________________________________

	inline void SoftGfxDevice::_color_tint_init(TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA,
												uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA,
												uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
												uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA, 
												uint32_t& leftIncB, uint32_t& leftIncG, uint32_t& leftIncR, uint32_t& leftIncA,
												uint32_t& rightIncB, uint32_t& rightIncG, uint32_t& rightIncR, uint32_t& rightIncA,
												uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA, CoordI patchPos)
	{
		if (tintMode == TintMode::Flat)
		{
/*
			outB = (inB * tint.flatTintColor.b)/255;
			outG = (inG * tint.flatTintColor.g)/255;
			outR = (inR * tint.flatTintColor.r)/255;
			outA = (inA * tint.flatTintColor.a)/255;

*/

			outB = (inB * s_mulTab[tint.flatTintColor.b]) >> 16;
			outG = (inG * s_mulTab[tint.flatTintColor.g]) >> 16;
			outR = (inR * s_mulTab[tint.flatTintColor.r]) >> 16;
			outA = (inA * s_mulTab[tint.flatTintColor.a]) >> 16;
		}
		
		if( tintMode == TintMode::None)
		{
			outB = inB;
			outG = inG;
			outR = inR;
			outA = inA;
		}
		
		if (tintMode == TintMode::GradientX || tintMode == TintMode::GradientY || tintMode == TintMode::GradientXY)
		{
			uint32_t topLeftB = (inB * tint.topLeftB) >> 8;
			uint32_t topLeftG = (inG * tint.topLeftG) >> 8;
			uint32_t topLeftR = (inR * tint.topLeftR) >> 8;
			uint32_t topLeftA = (inA * tint.topLeftA) >> 8;

			uint32_t topRightB = (inB * tint.topRightB) >> 8;
			uint32_t topRightG = (inG * tint.topRightG) >> 8;
			uint32_t topRightR = (inR * tint.topRightR) >> 8;
			uint32_t topRightA = (inA * tint.topRightA) >> 8;

			uint32_t bottomLeftB = (inB * (tint.topLeftB + tint.leftIncB * tint.tintRect.h)) >> 8;
			uint32_t bottomLeftG = (inG * (tint.topLeftG + tint.leftIncG * tint.tintRect.h)) >> 8;
			uint32_t bottomLeftR = (inR * (tint.topLeftR + tint.leftIncR * tint.tintRect.h)) >> 8;
			uint32_t bottomLeftA = (inA * (tint.topLeftA + tint.leftIncA * tint.tintRect.h)) >> 8;

			uint32_t bottomRightB = (inB * (tint.topRightB + tint.rightIncB * tint.tintRect.h)) >> 8;
			uint32_t bottomRightG = (inG * (tint.topRightG + tint.rightIncG * tint.tintRect.h)) >> 8;
			uint32_t bottomRightR = (inR * (tint.topRightR + tint.rightIncR * tint.tintRect.h)) >> 8;
			uint32_t bottomRightA = (inA * (tint.topRightA + tint.rightIncA * tint.tintRect.h)) >> 8;

			if (tintMode == TintMode::GradientX)
			{
				xIncB = int(topRightB - topLeftB) / tint.tintRect.w;
				xIncG = int(topRightG - topLeftG) / tint.tintRect.w;
				xIncR = int(topRightR - topLeftR) / tint.tintRect.w;
				xIncA = int(topRightA - topLeftA) / tint.tintRect.w;

				int patchOfsX = patchPos.x - tint.tintRect.x;

				leftB = topLeftB + xIncB * patchOfsX;
				leftG = topLeftG + xIncG * patchOfsX;
				leftR = topLeftR + xIncR * patchOfsX;
				leftA = topLeftA + xIncA * patchOfsX;
			}

			if (tintMode == TintMode::GradientY || tintMode == TintMode::GradientXY)
			{
				leftIncB = int(bottomLeftB - topLeftB) / tint.tintRect.h;
				leftIncG = int(bottomLeftG - topLeftG) / tint.tintRect.h;
				leftIncR = int(bottomLeftR - topLeftR) / tint.tintRect.h;
				leftIncA = int(bottomLeftA - topLeftA) / tint.tintRect.h;

				int patchOfsY = patchPos.y - tint.tintRect.y;

				leftB = topLeftB + patchOfsY * leftIncB;
				leftG = topLeftG + patchOfsY * leftIncG;
				leftR = topLeftR + patchOfsY * leftIncR;
				leftA = topLeftA + patchOfsY * leftIncA;

				if (tintMode == TintMode::GradientXY)
				{
					rightIncB = int(bottomRightB - topRightB) / tint.tintRect.h;
					rightIncG = int(bottomRightG - topRightG) / tint.tintRect.h;
					rightIncR = int(bottomRightR - topRightR) / tint.tintRect.h;
					rightIncA = int(bottomRightA - topRightA) / tint.tintRect.h;

					rightB = topRightB + patchOfsY * rightIncB;
					rightG = topRightG + patchOfsY * rightIncG;
					rightR = topRightR + patchOfsY * rightIncR;
					rightA = topRightA + patchOfsY * rightIncA;
				}

			}
		}
	}

	//____ _color_tint_line() _________________________________________________

	inline void SoftGfxDevice::_color_tint_line(TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA,
												uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA,
												uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
												uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA,
												uint32_t& leftIncB, uint32_t& leftIncG, uint32_t& leftIncR, uint32_t& leftIncA,
												uint32_t& rightIncB, uint32_t& rightIncG, uint32_t& rightIncR, uint32_t& rightIncA,
												uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
												uint32_t& pixelB, uint32_t& pixelG, uint32_t& pixelR, uint32_t& pixelA, CoordI patchPos)
	{
		if (tintMode == TintMode::GradientX)
		{
			pixelB = leftB;
			pixelG = leftG;
			pixelR = leftR;
			pixelA = leftA;
		}

		if (tintMode == TintMode::GradientY)
		{
			outB = leftB >> 16;
			outG = leftG >> 16;
			outR = leftR >> 16;
			outA = leftA >> 16;

			leftB += leftIncB;
			leftG += leftIncG;
			leftR += leftIncR;
			leftA += leftIncA;
		}

		if (tintMode == TintMode::GradientXY)
		{
			xIncB = int(rightB - leftB) / tint.tintRect.w;
			xIncG = int(rightG - leftG) / tint.tintRect.w;
			xIncR = int(rightR - leftR) / tint.tintRect.w;
			xIncA = int(rightA - leftA) / tint.tintRect.w;

			int patchOfsX = patchPos.x - tint.tintRect.x;

			pixelB = leftB + xIncB * patchOfsX;
			pixelG = leftG + xIncG * patchOfsX;
			pixelR = leftR + xIncR * patchOfsX;
			pixelA = leftA + xIncA * patchOfsX;

			leftB += leftIncB;
			leftG += leftIncG;
			leftR += leftIncR;
			leftA += leftIncA;

			rightB += rightIncB;
			rightG += rightIncG;
			rightR += rightIncR;
			rightA += rightIncA;

		}
	}

	//____ _color_tint_pixel() ______________________________________________________

	inline void SoftGfxDevice::_color_tint_pixel(TintMode tintMode,
												uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA,
												uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
												uint32_t& pixelB, uint32_t& pixelG, uint32_t& pixelR, uint32_t& pixelA)
	{
		if (tintMode == TintMode::GradientX || tintMode == TintMode::GradientXY)
		{
			outB = pixelB >> 16;
			outG = pixelG >> 16;
			outR = pixelR >> 16;
			outA = pixelA >> 16;

			pixelB += xIncB;
			pixelG += xIncG;
			pixelR += xIncR;
			pixelA += xIncA;

		}
	}

	//____ _texel_tint_init() _________________________________________________

	inline void SoftGfxDevice::_texel_tint_init(TintMode tintMode, const ColTrans& tint,
		uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
		uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA,
		uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
		uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA, CoordI patchPos)
	{
		if (tintMode == TintMode::None)
		{

		}

		if (tintMode == TintMode::Flat)
		{
			tintB = s_mulTab[tint.flatTintColor.b] << 8;
			tintG = s_mulTab[tint.flatTintColor.g] << 8;
			tintR = s_mulTab[tint.flatTintColor.r] << 8;
			tintA = s_mulTab[tint.flatTintColor.a] << 8;
		}

		if (tintMode == TintMode::GradientX || tintMode == TintMode::GradientY || tintMode == TintMode::GradientXY)
		{
			uint32_t topLeftB = tint.topLeftB;
			uint32_t topLeftG = tint.topLeftG;
			uint32_t topLeftR = tint.topLeftR;
			uint32_t topLeftA = tint.topLeftA;

			uint32_t topRightB = tint.topRightB;
			uint32_t topRightG = tint.topRightG;
			uint32_t topRightR = tint.topRightR;
			uint32_t topRightA = tint.topRightA;

			if (tintMode == TintMode::GradientX)
			{
				xIncB = int(topRightB - topLeftB) / tint.tintRect.w;
				xIncG = int(topRightG - topLeftG) / tint.tintRect.w;
				xIncR = int(topRightR - topLeftR) / tint.tintRect.w;
				xIncA = int(topRightA - topLeftA) / tint.tintRect.w;

				int patchOfsX = patchPos.x - tint.tintRect.x;

				leftB = topLeftB + xIncB * patchOfsX;
				leftG = topLeftG + xIncG * patchOfsX;
				leftR = topLeftR + xIncR * patchOfsX;
				leftA = topLeftA + xIncA * patchOfsX;
			}

			if (tintMode == TintMode::GradientY)
			{
				int patchOfsY = patchPos.y - tint.tintRect.y;

				tintB = topLeftB + int(tint.leftIncB) * patchOfsY;
				tintG = topLeftG + int(tint.leftIncG) * patchOfsY;
				tintR = topLeftR + int(tint.leftIncR) * patchOfsY;
				tintA = topLeftA + int(tint.leftIncA) * patchOfsY;
			}

			if (tintMode == TintMode::GradientXY)
			{
				int patchOfsY = patchPos.y - tint.tintRect.y;

				leftB = topLeftB + patchOfsY * int(tint.leftIncB);
				leftG = topLeftG + patchOfsY * int(tint.leftIncG);
				leftR = topLeftR + patchOfsY * int(tint.leftIncR);
				leftA = topLeftA + patchOfsY * int(tint.leftIncA);

				rightB = topRightB + patchOfsY * int(tint.rightIncB);
				rightG = topRightG + patchOfsY * int(tint.rightIncG);
				rightR = topRightR + patchOfsY * int(tint.rightIncR);
				rightA = topRightA + patchOfsY * int(tint.rightIncA);
			}
		}
	}

	//____ _texel_tint_line() _________________________________________________

	inline void SoftGfxDevice::_texel_tint_line(TintMode tintMode, const ColTrans& tint,
		uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
		uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA,
		uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
		uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA, CoordI patchPos)
	{
		if (tintMode == TintMode::GradientX)
		{
			tintB = leftB;
			tintG = leftG;
			tintR = leftR;
			tintA = leftA;
		}

		if (tintMode == TintMode::GradientY)
		{
			tintB += tint.leftIncB;
			tintG += tint.leftIncG;
			tintR += tint.leftIncR;
			tintA += tint.leftIncA;
		}

		if (tintMode == TintMode::GradientXY)
		{
			xIncB = int(rightB - leftB) / tint.tintRect.w;
			xIncG = int(rightG - leftG) / tint.tintRect.w;
			xIncR = int(rightR - leftR) / tint.tintRect.w;
			xIncA = int(rightA - leftA) / tint.tintRect.w;

			int patchOfsX = patchPos.x - tint.tintRect.x;

			tintB = leftB + int(xIncB) * patchOfsX;
			tintG = leftG + int(xIncG) * patchOfsX;
			tintR = leftR + int(xIncR) * patchOfsX;
			tintA = leftA + int(xIncA) * patchOfsX;

			leftB += tint.leftIncB;
			leftG += tint.leftIncG;
			leftR += tint.leftIncR;
			leftA += tint.leftIncA;

			rightB += tint.rightIncB;
			rightG += tint.rightIncG;
			rightR += tint.rightIncR;
			rightA += tint.rightIncA;
		}
	}

	//____ _texel_tint_pixel() ________________________________________________

	inline void SoftGfxDevice::_texel_tint_pixel(TintMode tintMode, uint8_t& pixelB, uint8_t& pixelG, uint8_t& pixelR, uint8_t& pixelA,
		uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
		uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA)
	{
		if (tintMode == TintMode::None)
		{
		}
		else
		{
			pixelB = (pixelB * tintB) >> 24;
			pixelG = (pixelG * tintG) >> 24;
			pixelR = (pixelR * tintR) >> 24;
			pixelA = (pixelA * tintA) >> 24;

			if (tintMode == TintMode::GradientX || tintMode == TintMode::GradientXY)
			{
				tintB += xIncB;
				tintG += xIncG;
				tintR += xIncR;
				tintA += xIncA;
			}
		}

	}

	//____ _plot() ____________________________________________________________

	template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_plot(uint8_t * pDst, Color col, const ColTrans& tint, CoordI patchPos)
	{
		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;

		srcB = col.b;
		srcG = col.g;
		srcR = col.r;
		srcA = col.a;

		// Step 1.5: Apply any tint to source

		if (TINT != TintMode::None)
		{
			int tintB, tintG, tintR, tintA;

			tintB = s_mulTab[tint.flatTintColor.b];
			tintG = s_mulTab[tint.flatTintColor.g];
			tintR = s_mulTab[tint.flatTintColor.r];
			tintA = s_mulTab[tint.flatTintColor.a];

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

	template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_plot_list(const RectI& clip, int nCoords, const CoordI * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint)
	{
		int tintB, tintG, tintR, tintA;

		if (TINT != TintMode::None)
		{
			tintB = s_mulTab[tint.flatTintColor.b];
			tintG = s_mulTab[tint.flatTintColor.g];
			tintR = s_mulTab[tint.flatTintColor.r];
			tintA = s_mulTab[tint.flatTintColor.a];
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

				if (TINT != TintMode::None)
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

	template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_draw_line(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint, CoordI patchPos)
	{
		const BlendMode EdgeBlendMode = BLEND == BlendMode::Replace ? BlendMode::Blend : BLEND;

		if (BLEND == BlendMode::Replace)
			color.a = 255;						// Needed since we still blend the edges.

		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;

		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;

		// Step 1.5: Apply any flatTintColor

		if (TINT != TintMode::None)
		{
			srcB = (srcB * s_mulTab[tint.flatTintColor.b]) >> 16;
			srcG = (srcG * s_mulTab[tint.flatTintColor.g]) >> 16;
			srcR = (srcR * s_mulTab[tint.flatTintColor.r]) >> 16;
			srcA = (srcA * s_mulTab[tint.flatTintColor.a]) >> 16;
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
				_blend_pixels(EdgeBlendMode, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

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
				_blend_pixels(EdgeBlendMode, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

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
					_blend_pixels(EdgeBlendMode, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
			}

			pRow += rowInc;
			pos += slope;
		}
	}

	//____ _clip_draw_line() __________________________________________________

	template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_clip_draw_line(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint, CoordI patchPos)
	{
		const BlendMode EdgeBlendMode = (BLEND == BlendMode::Replace) ? BlendMode::Blend : BLEND;

		if (BLEND == BlendMode::Replace)
			color.a = 255;						// Needed since we still blend the edges.

		// Step 1: Read source pixels

		uint8_t srcB, srcG, srcR, srcA;

		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;

		// Step 1.5: Apply any flatTintColor

		if (TINT != TintMode::None)
		{
			srcB = (srcB * s_mulTab[tint.flatTintColor.b]) >> 16;
			srcG = (srcG * s_mulTab[tint.flatTintColor.g]) >> 16;
			srcR = (srcR * s_mulTab[tint.flatTintColor.r]) >> 16;
			srcA = (srcA * s_mulTab[tint.flatTintColor.a]) >> 16;
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
				_blend_pixels(EdgeBlendMode, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				// First anti-aliased pixel of column

				int alpha = (srcA * (65536 - (clippedPos & 0xFFFF))) >> 16;		// Special AA

				uint8_t backB, backG, backR, backA;
				_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

				uint8_t outB, outG, outR, outA;
				_blend_pixels(EdgeBlendMode, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

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
					_blend_pixels(EdgeBlendMode, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);

					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
			}

			pRow += rowInc;
			pos += slope;
		}
	}


	//____ _fill() ____________________________________________________________

	template<TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_fill(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint, CoordI patchPos)
	{
		// Step 1: Read source pixels and prepare tint

		uint8_t srcB, srcG, srcR, srcA;

		uint32_t	leftB, leftG, leftR, leftA;						// Left side colors when tinting Y
		uint32_t	rightB, rightG, rightR, rightA;					// Right side colors when tinting X
		uint32_t	leftIncB, leftIncG, leftIncR, leftIncA;
		uint32_t	rightIncB, rightIncG, rightIncR, rightIncA;
		uint32_t	xIncB, xIncG, xIncR, xIncA;
		uint32_t	pixelB, pixelG, pixelR, pixelA;

		_color_tint_init(	TINT, tint, col.b, col.g, col.r, col.a, srcB, srcG, srcR, srcA,
							leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
							leftIncB, leftIncG, leftIncR, leftIncA, rightIncB, rightIncG, rightIncR, rightIncA,
							xIncB, xIncG, xIncR, xIncA, patchPos );

		for (int y = 0; y < nLines; y++)
		{

			// Step 2: Apply any vertical tint gradient

			_color_tint_line(	TINT, tint, col.b, col.g, col.r, col.a, srcB, srcG, srcR, srcA,
								leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
								leftIncB, leftIncG, leftIncR, leftIncA, rightIncB, rightIncG, rightIncR, rightIncA,
								xIncB, xIncG, xIncR, xIncA, pixelB, pixelG, pixelR, pixelA, patchPos );

			for (int x = 0; x < lineLength; x++)
			{
				// Step 3: Apply any horizontal tint gradient

				_color_tint_pixel(	TINT, srcB, srcG, srcR, srcA, xIncB, xIncG, xIncR, xIncA, 
									pixelB, pixelG, pixelR, pixelA);

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

	//____ _add_segment_color() _______________________________________________

	inline void SoftGfxDevice::_add_segment_color(bool GRADIENT, int blendFraction, int offset, const Color * pSegmentColor, const SegmentGradient * pSegmentGradient, int& accB, int& accG, int& accR, int& accA)
	{
		if (GRADIENT)
		{
			accB += blendFraction * ((pSegmentGradient->begB + pSegmentGradient->incB * offset) >> 16);
			accG += blendFraction * ((pSegmentGradient->begG + pSegmentGradient->incG * offset) >> 16);
			accR += blendFraction * ((pSegmentGradient->begR + pSegmentGradient->incR * offset) >> 16);
			accA += blendFraction * ((pSegmentGradient->begA + pSegmentGradient->incA * offset) >> 16);
		}
		else
		{
			accB += blendFraction * pSegmentColor->b;
			accG += blendFraction * pSegmentColor->g;
			accR += blendFraction * pSegmentColor->r;
			accA += blendFraction * pSegmentColor->a;
		}
	}

	//____ _segment_alpha() _______________________________________________

	inline int SoftGfxDevice::_segment_alpha(bool GRADIENT, int offset, const Color * pSegmentColor, const SegmentGradient * pSegmentGradient)
	{
		if (GRADIENT)
			return (pSegmentGradient->begA + pSegmentGradient->incA * offset) >> 16;
		else
			return pSegmentColor->a;
	}


	//____ _draw_segment_strip() _______________________________________________

	template<bool GRADIENT, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_draw_segment_strip(int colBeg, int colEnd, uint8_t * pStripStart, int pixelPitch, int nEdges, SegmentEdge * pEdges, const Color * pSegmentColors, const SoftGfxDevice::SegmentGradient * pSegmentGradients, const bool * pTransparentSegments)
	{
		// Render the column

		int offset = colBeg;				// 24.8 format, but binals cleared (always pointing at beginning of full pixel).
		uint8_t * pDst = pStripStart + (offset >> 8)*pixelPitch;

		while (offset < colEnd)
		{
			if (nEdges == 0 || offset + 255 < pEdges[0].begin)
			{
				// We are fully inside a segment, no need to take any edge into account.

				int end = nEdges == 0 ? colEnd : pEdges[0].begin;
				Color segmentColor = *pSegmentColors;

				if (*pTransparentSegments)									// This test is still valid in GRADIENT mode.
				{
					pDst = pStripStart + (end >> 8) * pixelPitch;
					offset = end & 0xFFFFFF00;												// Just skip segment since it is transparent
				} 
				else
				{
					uint8_t inB, inG, inR, inA;

					if (GRADIENT == false)
					{
						inB = pSegmentColors->b;
						inG = pSegmentColors->g;
						inR = pSegmentColors->r;
						inA = pSegmentColors->a;
					}

					while (offset + 255 < end)
					{
						if (GRADIENT)
						{
							inB = uint8_t((pSegmentGradients->begB + pSegmentGradients->incB * (offset >> 8)) >> 16);
							inG = uint8_t((pSegmentGradients->begG + pSegmentGradients->incG * (offset >> 8)) >> 16);
							inR = uint8_t((pSegmentGradients->begR + pSegmentGradients->incR * (offset >> 8)) >> 16);
							inA = uint8_t((pSegmentGradients->begA + pSegmentGradients->incA * (offset >> 8)) >> 16);
						}

						uint8_t backB, backG, backR, backA;
						_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

						uint8_t outB, outG, outR, outA;
						_blend_pixels(BLEND, inB, inG, inR, inA, backB, backG, backR, backA, outB, outG, outR, outA);

						_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);

						pDst += pixelPitch;
						offset += 256;
					}
				}
			}
			else
			{
				{
					int edge = 0;

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

						segmentFractions[edge] = remainingFractions - frac;

//						if (segmentFractions[edge] < 0 || segmentFractions[edge] > 65536)
//							int err = 1;

						remainingFractions = frac;
						edge++;
					}

					segmentFractions[edge] = remainingFractions;

					uint8_t backB, backG, backR, backA;

					_read_pixel(pDst, DSTFORMAT, nullptr, backB, backG, backR, backA);

					uint8_t outB = 0, outG = 0, outR = 0, outA = 0;

					int accB = 0;
					int accG = 0;
					int accR = 0;
					int accA = 0;

					if (BLEND == BlendMode::Replace)
					{
						for (int i = 0; i <= edge; i++)
						{
							int blendFraction = segmentFractions[i];
							_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i], accB, accG, accR, accA);
						}

						outB = accB >> 16;
						outG = accG >> 16;
						outR = accR >> 16;
						outA = accA >> 16;
					}

					if (BLEND == BlendMode::Blend)
					{
						int backFraction = 65536;

						for (int i = 0; i <= edge; i++)
						{
							int alpha = _segment_alpha(GRADIENT, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i]);
							int blendFraction = ((segmentFractions[i] * alpha) / 255);
							backFraction -= blendFraction;
							_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i], accB, accG, accR, accA);
						}

						outB = (accB + backB * backFraction) >> 16;
						outG = (accG + backG * backFraction) >> 16;
						outR = (accR + backR * backFraction) >> 16;
						outA = 255;													//TODO: Handle alpha correctly when writing to destination with alpha channel.
					}

					if (BLEND == BlendMode::Add)
					{
						for (int i = 0; i <= edge; i++)
						{
							int alpha = _segment_alpha(GRADIENT, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i]);
							int blendFraction = ((segmentFractions[i] * alpha) / 255);
							_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i], accB, accG, accR, accA);
						}

						outB = limitUint8(backB + (accB >> 16));
						outG = limitUint8(backG + (accG >> 16));
						outR = limitUint8(backR + (accR >> 16));
						outA = backA;
					}

					if (BLEND == BlendMode::Subtract)
					{
						for (int i = 0; i <= edge; i++)
						{
							int alpha = _segment_alpha(GRADIENT, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i]);
							int blendFraction = ((segmentFractions[i] * alpha) / 255);
							_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i], accB, accG, accR, accA);
						}

						outB = limitUint8(backB - (accB >> 16));
						outG = limitUint8(backG - (accG >> 16));
						outR = limitUint8(backR - (accR >> 16));
						outA = backA;
					}

					if (BLEND == BlendMode::Multiply)
					{
						for (int i = 0; i <= edge; i++)
						{
							int blendFraction = segmentFractions[i];
							_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i], accB, accG, accR, accA);
						}

						outB = (s_mulTab[backB] * (accB >> 16) ) >> 16;
						outG = (s_mulTab[backG] * (accG >> 16)) >> 16;
						outR = (s_mulTab[backR] * (accR >> 16)) >> 16;
						outA = backA;
					}

					if (BLEND == BlendMode::Invert)
					{
						for (int i = 0; i <= edge; i++)
						{
							int blendFraction = segmentFractions[i];
							_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i], &pSegmentGradients[i], accB, accG, accR, accA);
						}

						int srcB2 = s_mulTab[accB>>16];
						int srcG2 = s_mulTab[accG>>16];
						int srcR2 = s_mulTab[accR>>16];

						outB = (srcB2 * (255 - backB) + backB * (65536 - srcB2)) >> 16;
						outG = (srcG2 * (255 - backG) + backG * (65536 - srcG2)) >> 16;
						outR = (srcR2 * (255 - backR) + backR * (65536 - srcR2)) >> 16;
						outA = backA;
					}

					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
				pDst += pixelPitch;
				offset += 256;
			}

			while (nEdges > 0 && offset >= pEdges[0].end)
			{
				pEdges++;
				nEdges--;

				pTransparentSegments++;
				if (GRADIENT)
					pSegmentGradients++;
				else
					pSegmentColors++;

			}

		}

	}

	//_____ _simple_blit() ____________________________________________________________

	template<PixelFormat SRCFORMAT, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_simple_blit(const uint8_t * pSrc, uint8_t * pDst, const Color * pClut, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint, CoordI patchPos)
	{
		// Step 1: Prepare any tint gradient

		uint32_t	tintB, tintG, tintR, tintA;

		uint32_t	leftB, leftG, leftR, leftA;				// Left side colors when tinting Y
		uint32_t	rightB, rightG, rightR, rightA;			// Right side colors when tinting X
		uint32_t	xIncB, xIncG, xIncR, xIncA;

		_texel_tint_init(	TINT, tint, leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
							xIncB, xIncG, xIncR, xIncA, tintB, tintG, tintR, tintA, patchPos);

		for (int y = 0; y < nLines; y++)
		{
			// Step 2: Prepare tint for any vertical gradient

			_texel_tint_line(TINT, tint, leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
				xIncB, xIncG, xIncR, xIncA, tintB, tintG, tintR, tintA, patchPos);

			for (int x = 0; x < lineLength; x++)
			{
				// Step 3: Read source pixels

				uint8_t srcB, srcG, srcR, srcA;
				_read_pixel(pSrc, SRCFORMAT, pClut, srcB, srcG, srcR, srcA);

				// Step 3: Apply any tint

				_texel_tint_pixel(TINT, srcB, srcG, srcR, srcA, xIncB, xIncG, xIncR, xIncA,
					tintB, tintG, tintR, tintA);

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

	template<PixelFormat SRCFORMAT, ScaleMode SCALEMODE, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT, bool SRCCLIP >
	void SoftGfxDevice::_complex_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
	{
		int srcPixelBytes = pSrcSurf->m_pixelDescription.bits / 8;
		int	srcPitch = pSrcSurf->m_pitch;
		SizeI srcMax = pSrcSurf->m_size * 32768;

		int pixelIncX = (int)(matrix[0][0] * 32768);
		int pixelIncY = (int)(matrix[0][1] * 32768);

		int lineIncX = (int)(matrix[1][0] * 32768);
		int lineIncY = (int)(matrix[1][1] * 32768);

		/*
		int tintB, tintG, tintR, tintA;

		if (TINT != TintMode::None)
		{
			tintB = s_mulTab[tint.flatTintColor.b];
			tintG = s_mulTab[tint.flatTintColor.g];
			tintR = s_mulTab[tint.flatTintColor.r];
			tintA = s_mulTab[tint.flatTintColor.a];
		}
*/
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


					if (SRCCLIP && ((ofsX | ofsY | (srcMax.w - (ofsX+32768+1)) | (srcMax.h - (ofsY+32768+1))) < 0))
					{
						if( ofsX > srcMax.w || ofsY > srcMax.h || ofsX < -32768 || ofsY < -32768 )
						{
							// Totally outside, no need to process pixel

							ofsX += pixelIncX;
							ofsY += pixelIncY;
							pDst += dstPitchX;
							continue;
						}
						else
						{
							// At least one of the four sample pixels is within source surface

							if(ofsX >= 0 && ofsY >= 0 && ofsX < srcMax.w && ofsY < srcMax.h )
								_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, src11_b, src11_g, src11_r, src11_a);
							else
								src11_b = src11_g = src11_r = src11_a = 0;

							if ((ofsX+32768) >= 0 && ofsY >= 0 && (ofsX+32768) < srcMax.w && ofsY < srcMax.h)
								_read_pixel(p + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src12_b, src12_g, src12_r, src12_a);
							else
								src12_b = src12_g = src12_r = src12_a = 0;

							if (ofsX >= 0 && (ofsY+32768) >= 0 && ofsX < srcMax.w && (ofsY+32768) < srcMax.h)
								_read_pixel(p + srcPitch, SRCFORMAT, pSrcSurf->m_pClut, src21_b, src21_g, src21_r, src21_a);
							else
								src21_b = src21_g = src21_r = src21_a = 0;

							if ((ofsX + 32768) >= 0 && (ofsY+32768) >= 0 && (ofsX + 32768) < srcMax.w && (ofsY+32768) < srcMax.h)
								_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, pSrcSurf->m_pClut, src22_b, src22_g, src22_r, src22_a);
							else
								src22_b = src22_g = src22_r = src22_a = 0;
						}
					}
					else
					{
						assert((ofsX | ofsY | (srcMax.w - (ofsX + 32768)) | (srcMax.h - (ofsY + 32768))) >= 0);

						int nextX = (ofsX & 0x7FFF) == 0 ? 0 : srcPixelBytes;
						int nextY = (ofsY & 0x7FFF) == 0 ? 0 : srcPitch;

						_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, src11_b, src11_g, src11_r, src11_a);
						_read_pixel(p + nextX, SRCFORMAT, pSrcSurf->m_pClut, src12_b, src12_g, src12_r, src12_a);
						_read_pixel(p + nextY, SRCFORMAT, pSrcSurf->m_pClut, src21_b, src21_g, src21_r, src21_a);
						_read_pixel(p + nextX + nextY, SRCFORMAT, pSrcSurf->m_pClut, src22_b, src22_g, src22_r, src22_a);
					}

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
					if (SRCCLIP && ((ofsX | ofsY | (srcMax.w - 1- ofsX) | (srcMax.h - 1 - ofsY)) < 0))
					{
						ofsX += pixelIncX;
						ofsY += pixelIncY;
						pDst += dstPitchX;
						continue;
					}
					else
					{
						assert((ofsX | ofsY | (srcMax.w - 1 - ofsX) | (srcMax.h - 1 - ofsY)) >= 0);

						_read_pixel(p, SRCFORMAT, pSrcSurf->m_pClut, srcB, srcG, srcR, srcA);
					}
				}


				// Step 3.5: Apply any tinting
/*
				if (TINT != TintMode::None)
				{
					srcB = (srcB*tintB) >> 16;
					srcG = (srcG*tintG) >> 16;
					srcR = (srcR*tintR) >> 16;
					srcA = (srcA*tintA) >> 16;
				}
*/
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

	template<PixelFormat SRCFORMAT, ScaleMode SCALEMODE, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT>
	void SoftGfxDevice::_stretch_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
	{
		int srcPixelBytes = pSrcSurf->m_pixelDescription.bits / 8;
		int	srcPitch = pSrcSurf->m_pitch;

		int pixelIncX = (int)(matrix[0][0] * 32768);
		int lineIncY = (int)(matrix[1][1] * 32768);

		int tintB, tintG, tintR, tintA;

		if (TINT != TintMode::None)
		{
			tintB = s_mulTab[tint.flatTintColor.b];
			tintG = s_mulTab[tint.flatTintColor.g];
			tintR = s_mulTab[tint.flatTintColor.r];
			tintA = s_mulTab[tint.flatTintColor.a];
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

				if (TINT != TintMode::None)
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


	//____ constructor _____________________________________________________________

	SoftGfxDevice::SoftGfxDevice() : GfxDevice(SizeI(0,0))
	{
		m_pCanvas = nullptr;
		m_pCanvasPixels = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		_initTables();

	}

	SoftGfxDevice::SoftGfxDevice( Surface * pCanvas ) : GfxDevice( pCanvas?pCanvas->size():SizeI() )
	{
		m_pCanvas = pCanvas;
		m_pCanvasPixels = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		_initTables();
	}

	//____ Destructor ______________________________________________________________

	SoftGfxDevice::~SoftGfxDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SoftGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& SoftGfxDevice::surfaceType( void ) const
	{
		return SoftSurface::TYPEINFO;
	}

	//____ surfaceFactory() _______________________________________________________

	SurfaceFactory_p SoftGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = SoftSurfaceFactory::create();

		return m_pSurfaceFactory;
	}

	//____ setCanvas() _______________________________________________________________

	bool SoftGfxDevice::setCanvas( Surface * pCanvas, bool bResetClipRects )
	{
		if (m_pCanvas == pCanvas)
			return true;			// Not an error.

		if( !pCanvas )
		{
			m_pCanvas		= nullptr;
			m_canvasSize	= SizeI();
			m_clipCanvas	= RectI();
			m_clipBounds	= RectI();
			m_pClipRects	= &m_clipCanvas;
			m_nClipRects	= 1;

			// Make sure this also is cleared, in case we are rendering.

			m_pCanvasPixels = nullptr;
			m_canvasPixelBits = 0;
			m_canvasPitch = 0;
			return true;
		}


		PixelFormat format = pCanvas->pixelFormat();
		if( format != PixelFormat::BGRA_8 && format != PixelFormat::BGR_8 &&
			format != PixelFormat::BGRX_8 && format != PixelFormat::BGRA_4 &&
			format != PixelFormat::BGR_565 && format != PixelFormat::A8 )
			return false;

		if( m_pCanvasPixels )
			m_pCanvas->unlock();

		m_pCanvas		= pCanvas;
		m_canvasSize	= pCanvas->size();
		m_clipCanvas	= { 0,0,m_canvasSize };

		if( bResetClipRects )
		{
			m_clipBounds	= { 0,0,m_canvasSize };
			m_pClipRects = &m_clipCanvas;
			m_nClipRects = 1;
		}

		// Update stuff if we are rendering

		if( m_pCanvasPixels )
		{
			m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
			m_canvasPixelBits = m_pCanvas->pixelDescription()->bits;
			m_canvasPitch = m_pCanvas->pitch();

			_updateBlitFunctions();
		}

		return true;
	}

	//____ setTintColor() _____________________________________________________

	void SoftGfxDevice::setTintColor(Color color)
	{
		if (color == m_tintColor)
			return;

		GfxDevice::setTintColor(color);
		_updateTintSettings();

/*
		m_colTrans.flatTintColor = color;

		TintMode tintMode;

		if (m_nTintGradients == 0)
		{
			if (color == Color::White)
				tintMode = TintMode::None;
			else
				tintMode = TintMode::Flat;

			bool bOpacityChanged = (color.a != m_tintColor.a) && (color.a == 255 || m_tintColor.a == 255);

			if (bOpacityChanged || tintMode != m_tintModeSingle)
			{
				m_tintModeSingle = tintMode;
				m_tintModeMulti = tintMode;

				_updateBlitFunctions();
			}
		}
*/
	}

	//____ setTintGradient() __________________________________________________

	void SoftGfxDevice::setTintGradient(const RectI& rect, Color topLeft, Color topRight, Color bottomRight, Color bottomLeft)
	{
		GfxDevice::setTintGradient(rect, topLeft, topRight, bottomRight, bottomLeft);
		m_colTrans.tintRect = rect;
		_updateTintSettings();
	}

	//____ clearTintGradient() _______________________________________________

	void SoftGfxDevice::clearTintGradient()
	{
		GfxDevice::clearTintGradient();
		_updateTintSettings();
	}

	//____ _updateTintSettings() ______________________________________________

	void SoftGfxDevice::_updateTintSettings()
	{
		TintMode tintMode;

		if (!m_bTintGradient)
		{
			tintMode = (m_tintColor == Color::White) ? TintMode::None : TintMode::Flat;
			m_colTrans.flatTintColor = m_tintColor;
			m_bTintOpaque = (m_tintColor.a == 255);
		}
		else
		{
			// Update m_gradientData and generate diffMasks for X- and Y-gradients.

			int diffMaskX = 0, diffMaskY = 0;

			uint32_t	flatTintColorR = s_mulTab[m_tintColor.r]*256/255;
			uint32_t	flatTintColorG = s_mulTab[m_tintColor.g]*256/255;
			uint32_t	flatTintColorB = s_mulTab[m_tintColor.b]*256/255;
			uint32_t	flatTintColorA = s_mulTab[m_tintColor.a]*256/255;

			uint32_t	r[4], g[4], b[4], a[4];							// Scale: 0 -> (1 << 24)

			for (int i = 0; i < 4; i++)
			{
				r[i] = m_tintGradient[i].r * flatTintColorR;
				g[i] = m_tintGradient[i].g * flatTintColorG;
				b[i] = m_tintGradient[i].b * flatTintColorB;
				a[i] = m_tintGradient[i].a * flatTintColorA;
			}

			m_colTrans.topLeftR = r[0];
			m_colTrans.topLeftG = g[0];
			m_colTrans.topLeftB = b[0];
			m_colTrans.topLeftA = a[0];

			m_colTrans.topRightR = r[1];
			m_colTrans.topRightG = g[1];
			m_colTrans.topRightB = b[1];
			m_colTrans.topRightA = a[1];

			m_colTrans.leftIncR = uint32_t(int(r[3] - r[0]) / m_tintGradientRect.h);
			m_colTrans.leftIncG = uint32_t(int(g[3] - g[0]) / m_tintGradientRect.h);
			m_colTrans.leftIncB = uint32_t(int(b[3] - b[0]) / m_tintGradientRect.h);
			m_colTrans.leftIncA = uint32_t(int(a[3] - a[0]) / m_tintGradientRect.h);

			m_colTrans.rightIncR = uint32_t(int(r[2] - r[1]) / m_tintGradientRect.h);
			m_colTrans.rightIncG = uint32_t(int(g[2] - g[1]) / m_tintGradientRect.h);
			m_colTrans.rightIncB = uint32_t(int(b[2] - b[1]) / m_tintGradientRect.h);
			m_colTrans.rightIncA = uint32_t(int(a[2] - a[1]) / m_tintGradientRect.h);

			diffMaskX |= (m_tintGradient[0].argb - m_tintGradient[1].argb) | (m_tintGradient[2].argb - m_tintGradient[3].argb);
			diffMaskY |= (m_tintGradient[0].argb - m_tintGradient[3].argb) | (m_tintGradient[1].argb - m_tintGradient[2].argb);

			// Calculate tintMode

			if( (diffMaskX | diffMaskY) == 0 )
			{
				if (m_tintGradient[0] == Color::White)
					tintMode = TintMode::None;
				else
				{
					m_colTrans.flatTintColor = m_tintGradient[0] * m_tintColor;
					tintMode = TintMode::Flat;
				}
			}
			else
			{
				if (diffMaskY == 0)
					tintMode = TintMode::GradientX;
				else if (diffMaskX == 0)
					tintMode = TintMode::GradientY;
				else
					tintMode = TintMode::GradientXY;
			}

			// Update m_bTintOpaque

			int value = int(m_tintColor.a) + int(m_tintGradient[0].a) + int(m_tintGradient[1].a) 
						+ int(m_tintGradient[2].a) + int(m_tintGradient[3].a);

			m_bTintOpaque = (value == 255*5);
		}

		// Update tintMode and blitFunctions.

		if (tintMode != m_colTrans.mode)
		{
			m_colTrans.mode = tintMode;
			_updateBlitFunctions();
		}

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
		if( m_bRendering || !m_pCanvas)
			return false;

		m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
		m_canvasPixelBits = m_pCanvas->pixelDescription()->bits;
		m_canvasPitch = m_pCanvas->pitch();

		_updateBlitFunctions();

		if( !m_pCanvasPixels )
			return false;

		setTintColor(Color::White);
		setBlendMode(BlendMode::Blend);

		m_bRendering = true;
		return true;
	}

	//____ endRender() _________________________________________________________

	bool SoftGfxDevice::endRender()
	{
		if( !m_bRendering || !m_pCanvasPixels )
			return false;

		// Clean up.

		m_pCanvas->unlock();
		m_pCanvasPixels = 0;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;

		_updateBlitFunctions();		// Good to have dummies in place when we are not allowed to blit.
		m_bRendering = false;
		return true;
	}

	//____ fill() ______________________________________________________

	void SoftGfxDevice::fill(const RectI& rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels )
			return;

		// Clipping

		if( !m_clipBounds.intersectsWith(rect) )
			return;

		// Prepare colors


		// Skip calls that won't affect destination

		if ((col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract ) )
			return;

		// Optimize calls

		BlendMode blendMode = m_blendMode;
		if (blendMode == BlendMode::Blend && col.a == 255 && m_bTintOpaque)
		{
			blendMode = BlendMode::Replace;
		}

		//

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p pFunc = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pCanvas->pixelFormat()];
		if (pFunc == nullptr)
			return;

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI patch(m_pClipRects[i], rect);
			if (patch.w == 0 || patch.h == 0)
				continue;

			uint8_t * pDst = m_pCanvasPixels + patch.y *m_canvasPitch + patch.x * pixelBytes;
			pFunc(pDst, pixelBytes, m_canvasPitch - patch.w*pixelBytes, patch.h, patch.w, col, m_colTrans, patch.pos());
		}
	}

	void SoftGfxDevice::fill(const RectF& rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		// Clipping

		RectF clip(rect, RectF(m_clipBounds));
		if (clip.w == 0 || clip.h == 0)
			return;

		// Skip calls that won't affect destination

		if ((col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		// Optimize calls

		BlendMode blendMode = m_blendMode;
		if (blendMode == BlendMode::Blend && col.a == 255 && m_bTintOpaque)
		{
			blendMode = BlendMode::Replace;
		}


		BlendMode	edgeBlendMode = (m_blendMode == BlendMode::Replace) ? BlendMode::Blend : m_blendMode; // Need to blend edges and corners even if fill is replace

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p pFillOp = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pCanvas->pixelFormat()];
		FillOp_p pEdgeOp = s_fillOpTab[(int)m_colTrans.mode][(int)edgeBlendMode][(int)m_pCanvas->pixelFormat()];
		PlotOp_p pPlotOp = s_plotOpTab[(int)edgeBlendMode][(int)m_pCanvas->pixelFormat()];

		for (int i = 0; i < m_nClipRects; i++)
		{
			Color color = col;

			RectF  patch(rect, RectF(m_pClipRects[i]));
			if (patch.w == 0.f || patch.h == 0.f)
				continue;

			// Fill all but anti-aliased edges

			int x1 = (int)(patch.x + 0.999f);
			int y1 = (int)(patch.y + 0.999f);
			int x2 = (int)(patch.x + patch.w);
			int y2 = (int)(patch.y + patch.h);


			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x1 * pixelBytes;
			pFillOp(pDst, pixelBytes, m_canvasPitch - (x2 - x1)*pixelBytes, y2 - y1, x2 - x1, color, m_colTrans, { x1,y1 });

			// Draw the sides

			int aaLeft = (256 - (int)(patch.x * 256)) & 0xFF;
			int aaTop = (256 - (int)(patch.y * 256)) & 0xFF;
			int aaRight = ((int)((patch.x + patch.w) * 256)) & 0xFF;
			int aaBottom = ((int)((patch.y + patch.h) * 256)) & 0xFF;

			int aaTopLeft = aaTop * aaLeft / 256;
			int aaTopRight = aaTop * aaRight / 256;
			int aaBottomLeft = aaBottom * aaLeft / 256;
			int aaBottomRight = aaBottom * aaRight / 256;


			if (m_blendMode != BlendMode::Replace)
			{
				int alpha = s_mulTab[color.a];

				aaLeft = aaLeft * alpha >> 16;
				aaTop = aaTop * alpha >> 16;
				aaRight = aaRight * alpha >> 16;
				aaBottom = aaBottom * alpha >> 16;

				aaTopLeft = aaTopLeft * alpha >> 16;
				aaTopRight = aaTopRight * alpha >> 16;
				aaBottomLeft = aaBottomLeft * alpha >> 16;
				aaBottomRight = aaBottomRight * alpha >> 16;
			}


			if (aaTop != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + ((int)patch.y) * m_canvasPitch + x1 * pixelBytes;
				int length = x2 - x1;
				color.a = aaTop;
				pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,(int)patch.y });
			}

			if (aaBottom != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
				int length = x2 - x1;
				color.a = aaBottom;
				pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,y2 });
			}

			if (aaLeft != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + ((int)patch.x) * pixelBytes;
				int length = y2 - y1;
				color.a = aaLeft;
				pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { (int)patch.x, y1 });
			}

			if (aaRight != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
				int length = y2 - y1;
				color.a = aaRight;
				pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { x2, y1 });
			}

			// Draw corner pieces


			if (aaTopLeft != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + ((int)patch.y) * m_canvasPitch + ((int)patch.x) * pixelBytes;
				color.a = aaTopLeft;
				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, (int)patch.y } );
			}

			if (aaTopRight != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + ((int)patch.y) * m_canvasPitch + x2 * pixelBytes;
				color.a = aaTopRight;
				pPlotOp(pDst, color, m_colTrans, { x2, (int)patch.y });
			}

			if (aaBottomLeft != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + ((int)patch.x) * pixelBytes;
				color.a = aaBottomLeft;
				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, y2 } );
			}

			if (aaBottomRight != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
				color.a = aaBottomRight;
				pPlotOp(pDst, color, m_colTrans, { x2,y2 });
			}
		}
	}

	//____ drawLine() ____ [from/to] __________________________________________

	void SoftGfxDevice::drawLine(CoordI beg, CoordI end, Color color, float thickness)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		Color fillColor = color * m_tintColor;

		// Skip calls that won't affect destination

		if (fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		//

		ClipLineOp_p pOp = s_clipLineOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if (pOp == nullptr)
			return;

		uint8_t *	pRow;
		int		rowInc, pixelInc;
		int 	length, width;
		int		pos, slope;
		int		clipStart, clipEnd;

		if (std::abs(beg.x - end.x) > std::abs(beg.y - end.y))
		{
			// Prepare mainly horizontal line segment

			if (beg.x > end.x)
				swap(beg, end);

			length = end.x - beg.x;
			slope = ((end.y - beg.y) << 16) / length;

			width = _scaleLineThickness(thickness, slope);
			pos = (beg.y << 16) - width / 2 + 32768;

			rowInc = m_canvasPixelBits / 8;
			pixelInc = m_canvasPitch;

			pRow = m_pCanvasPixels + beg.x * rowInc;

			// Loop through patches

			for( int i = 0 ; i < m_nClipRects ; i++ )
			{
				// Do clipping

				const RectI& clip = m_pClipRects[i];

				int _length = length;
				int _pos = pos;
				uint8_t * _pRow = pRow;

				if (beg.x < clip.x)
				{
					int cut = clip.x - beg.x;
					_length -= cut;
					_pRow += rowInc * cut;
					_pos += slope * cut;
				}

				if (end.x > clip.x + clip.w)
					_length -= end.x - (clip.x + clip.w);

				clipStart = clip.y << 16;
				clipEnd = (clip.y + clip.h) << 16;

				//  Draw

				pOp(clipStart, clipEnd, _pRow, rowInc, pixelInc, _length, width, _pos, slope, fillColor, m_colTrans, { 0,0 });
			}
		}
		else
		{
			// Prepare mainly vertical line segment

			if (beg.y > end.y)
				swap(beg, end);

			length = end.y - beg.y;
			if (length == 0)
				return;											// TODO: Should stil draw the caps!

			slope = ((end.x - beg.x) << 16) / length;
			width = _scaleLineThickness(thickness, slope);
			pos = (beg.x << 16) - width / 2 + 32768;

			rowInc = m_canvasPitch;
			pixelInc = m_canvasPixelBits / 8;

			pRow = m_pCanvasPixels + beg.y * rowInc;

			// Loop through patches

			for (int i = 0; i < m_nClipRects; i++)
			{
				// Do clipping

				const RectI& clip = m_pClipRects[i];

				int _length = length;
				int _pos = pos;
				uint8_t * _pRow = pRow;

				if (beg.y < clip.y)
				{
					int cut = clip.y - beg.y;
					_length -= cut;
					_pRow += rowInc * cut;
					_pos += slope * cut;
				}

				if (end.y > clip.y + clip.h)
					_length -= end.y - (clip.y + clip.h);

				clipStart = clip.x << 16;
				clipEnd = (clip.x + clip.w) << 16;

				//  Draw

				pOp(clipStart, clipEnd, _pRow, rowInc, pixelInc, _length, width, _pos, slope, fillColor, m_colTrans, { 0,0 });
			}
		}
	}

	//____ drawLine() ____ [start/direction] ________________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void SoftGfxDevice::drawLine(CoordI _begin, Direction dir, int _length, Color _col, float thickness)
	{
		//TODO: Optimize!

		if (thickness <= 0.f)
			return;

		BlendMode edgeBlendMode = m_blendMode;
		if (m_blendMode == BlendMode::Replace)
		{
			_col.a = 255;						// Needed since we still blend the edges.
			edgeBlendMode = BlendMode::Blend;
		}

		_col = _col * m_tintColor;

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pCenterOp = s_fillOpTab[(int)m_blendMode][0][(int)m_pCanvas->pixelFormat()];
		FillOp_p	pEdgeOp = s_fillOpTab[(int)edgeBlendMode][0][(int)m_pCanvas->pixelFormat()];

		for (int i = 0; i < m_nClipRects; i++)
		{
			const RectI& clip = m_pClipRects[i];

			CoordI begin = _begin;
			int length = _length;

			switch (dir)
			{
			case Direction::Left:
				begin.x -= length;
			case Direction::Right:
			{
				if (begin.x > clip.x + clip.w)
					continue;

				if (begin.x < clip.x)
				{
					length -= clip.x - begin.x;
					if (length <= 0)
						continue;
					begin.x = clip.x;
				}

				if (begin.x + length > clip.x + clip.w)
				{
					length = clip.x + clip.w - begin.x;
					if (length <= 0)
						continue;
				}

				if (thickness <= 1.f)
				{
					if (begin.y < clip.y || begin.y >= clip.y + clip.h)
						continue;

					Color col = _col;
					col.a = (uint8_t)(thickness * col.a);

					uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
					pEdgeOp(pBegin, pixelBytes, 0, 1, length, col, m_colTrans, { 0,0 });
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					if (begin.y + expanse <= clip.y || begin.y - expanse >= clip.y + clip.h)
						continue;

					int beginY = begin.y - expanse;
					int endY = begin.y + expanse + 1;

					if (beginY < clip.y)
						beginY = clip.y - 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
						pEdgeOp(pBegin, pixelBytes, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
						//					_drawStraightLine({ begin.x, beginY }, Axis::X, length, edgeColor);
					}

					if (endY > clip.y + clip.h)
						endY = clip.y + clip.h + 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + (endY - 1) * m_canvasPitch + begin.x * pixelBytes;
						pEdgeOp(pBegin, pixelBytes, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}

					int bodyThickness = endY - beginY - 2;
					uint8_t * pBegin = m_pCanvasPixels + (beginY + 1) * m_canvasPitch + begin.x * pixelBytes;
					pCenterOp(pBegin, pixelBytes, m_canvasPitch - length * pixelBytes, bodyThickness, length, _col, m_colTrans, { 0,0 });
				}

				break;
			}
			case Direction::Up:
				begin.y -= length;
			case Direction::Down:
				if (begin.y > clip.y + clip.h)
					continue;

				if (begin.y < clip.y)
				{
					length -= clip.y - begin.y;
					if (length <= 0)
						continue;
					begin.y = clip.y;
				}

				if (begin.y + length > clip.y + clip.h)
				{
					length = clip.y + clip.h - begin.y;
					if (length <= 0)
						continue;
				}

				if (thickness <= 1.f)
				{
					if (begin.x < clip.x || begin.x >= clip.x + clip.w)
						continue;

					Color col = _col;
					col.a = (uint8_t)(thickness * col.a);

					uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
					pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, col, m_colTrans, { 0,0 });
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					if (begin.x + expanse <= clip.x || begin.x - expanse >= clip.x + clip.w)
						continue;

					int beginX = begin.x - expanse;
					int endX = begin.x + expanse + 1;

					if (beginX < clip.x)
						beginX = clip.x - 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
						pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}

					if (endX > clip.x + clip.w)
						endX = clip.x + clip.w + 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX - 1) * pixelBytes;
						pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}


					int bodyThickness = endX - beginX - 2;
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX + 1) * pixelBytes;
					pCenterOp(pBegin, m_canvasPitch, pixelBytes - length * m_canvasPitch, bodyThickness, length, _col, m_colTrans, { 0,0 });
				}

				break;
			}
		}
	}

	//____ _transformDrawSegments() _________________________________________

	void SoftGfxDevice::_transformDrawSegments(const RectI& _dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * _pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int _simpleTransform[2][2])
	{
		RectI dest = _dest;

		SegmentEdge edges[c_maxSegments-1];

		int xPitch = m_canvasPixelBits / 8;
		int yPitch = m_canvasPitch;

		// We need to modify our transform since we are moving the destination pointer, not the source pointer, according to the transform.

		int simpleTransform[2][2];

		simpleTransform[0][0] = _simpleTransform[0][0];
		simpleTransform[1][1] = _simpleTransform[1][1];

		if ((_simpleTransform[0][0] | _simpleTransform[1][1]) == 0)
		{
			simpleTransform[0][1] = _simpleTransform[1][0];
			simpleTransform[1][0] = _simpleTransform[0][1];
		}
		else
		{
			simpleTransform[0][1] = _simpleTransform[0][1];
			simpleTransform[1][0] = _simpleTransform[1][0];
		}

		// Calculate pitches

		int colPitch = simpleTransform[0][0] * xPitch + simpleTransform[0][1] * yPitch;
		int rowPitch = simpleTransform[1][0] * xPitch + simpleTransform[1][1] * yPitch;

		// Calculate start coordinate

		CoordI start = dest.pos();

		if (simpleTransform[0][0] + simpleTransform[1][0] < 0)
			start.x += dest.w - 1;

		if (simpleTransform[0][1] + simpleTransform[1][1] < 0)
			start.y += dest.h - 1;

		// Detect if strip columns are lined horizontally or verically

		bool bHorizontalColumns = (abs(colPitch) == xPitch);

		// Limit size of destination rect by number of edgestrips.

		if ( bHorizontalColumns )
		{
			if (dest.w > nEdgeStrips - 1)
			{
				if (colPitch < 0)
					dest.x += dest.w - nEdgeStrips - 1;

				dest.w = nEdgeStrips - 1;
			}
		}
		else
		{
			if (dest.h > nEdgeStrips - 1)
			{
				if (colPitch < 0)
					dest.y += dest.h - nEdgeStrips - 1;

				dest.h = nEdgeStrips - 1;
			}
		}

		// Apply simple tinting

		Color	colors[c_maxSegments];
		bool	transparentSegments[c_maxSegments];

		if (m_tintColor != Color::White)
		{
			for (int i = 0; i < nSegments; i++)
			{
				Color col = pSegmentColors[i] * m_tintColor;
				colors[i] = col;
				transparentSegments[i] = (col.a == 0);
			}
			pSegmentColors = colors;
		}

		// Set start position and clip dest

		uint8_t * pOrigo = m_pCanvasPixels + start.y * yPitch + start.x * xPitch;

		if (!dest.intersectsWith(m_clipBounds))
			return;

		SegmentOp_p	pOp = s_segmentOpTab[0][(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if (pOp == nullptr)
			return;

		// Loop through patches

		for (int patchIdx = 0; patchIdx < m_nClipRects; patchIdx++)
		{
			// Clip patch

			RectI patch(dest, m_pClipRects[patchIdx] );
			if (patch.w == 0 || patch.h == 0)
				continue;

			// Calculate stripstart, clipBeg/clipEnd and first edge for patch


			int columnOfs;
			int rowOfs;

			int columns;
			int rows;

			if (bHorizontalColumns)
			{
				columnOfs = colPitch > 0 ? patch.x - dest.x : dest.right() - patch.right();
				rowOfs = rowPitch > 0 ? patch.y - dest.y : dest.bottom() - patch.bottom();

				columns = patch.w;
				rows = patch.h;
			}
			else
			{
				columnOfs = colPitch > 0 ? patch.y - dest.y : dest.bottom() - patch.bottom();
				rowOfs = rowPitch > 0 ? patch.x - dest.x : dest.right() - patch.right();

				columns = patch.h;
				rows = patch.w;
			}

			const int *		pEdgeStrips		= _pEdgeStrips + columnOfs * edgeStripPitch;
			uint8_t *	pStripStart = pOrigo + columnOfs * colPitch;

			int clipBeg = rowOfs * 256;
			int clipEnd  = clipBeg + (rows * 256);

			for (int x = 0; x < columns; x++)
			{
				int nEdges = 0;
				int colOfs = 0;

				for (int y = 0; y < nSegments - 1; y++)
				{
					int beg = pEdgeStrips[y];
					int end = pEdgeStrips[y + edgeStripPitch];

					if (beg > end)
						swap(beg, end);

					if (beg >= clipEnd)
						break;

					if (end > clipBeg)
					{
						int coverageInc = (end == beg) ? 0 : (65536 * 256) / (end - beg);
						int coverage = 0;

						if (beg < clipBeg)
						{
							int cut = clipBeg - beg;
							beg = clipBeg;
							coverage += (coverageInc*cut) >> 8;
						}

						if (end > clipEnd)
							end = clipEnd;


						edges[nEdges].begin = beg;
						edges[nEdges].end = end;
						edges[nEdges].coverage = coverage;
						edges[nEdges].coverageInc = coverageInc;
						nEdges++;
					}
					else
						colOfs++;
				}

				const Color * pColors = pSegmentColors + colOfs;

				pOp(clipBeg, clipEnd, pStripStart, rowPitch, nEdges, edges, pColors, nullptr,nullptr);
				pEdgeStrips += edgeStripPitch;
				pStripStart += colPitch;
			}
		}

	}




	//____ _colTransRect() _____________________________________________________

	void SoftGfxDevice::_colTransRect( int outB[4], int outG[4], int outR[4], int outA[], const RectI& dest)
	{
		// Then we tint with our out tint

		if (m_colTrans.mode == TintMode::None)
		{
			for (int i = 0; i < 4; i++)
			{
				outB[i] = 0x10000;
				outG[i] = 0x10000;
				outR[i] = 0x10000;
				outA[i] = 0x10000;
			}
		}
		else if (m_colTrans.mode == TintMode::Flat)
		{
			int b = s_mulTab[m_colTrans.flatTintColor.b];
			int g = s_mulTab[m_colTrans.flatTintColor.g];
			int r = s_mulTab[m_colTrans.flatTintColor.r];
			int a = s_mulTab[m_colTrans.flatTintColor.a];

			for (int i = 0; i < 4; i++)
			{
				outB[i] = b;
				outG[i] = g;
				outR[i] = r;
				outA[i] = a;
			}
		}
		else if (m_colTrans.mode == TintMode::GradientX)
		{
			int ofsX = dest.x - m_colTrans.tintRect.x;
			int xIncB = int(m_colTrans.topRightB - m_colTrans.topLeftB) / m_colTrans.tintRect.w;

			int leftB = (m_colTrans.topLeftB + xIncB * ofsX) >> 8;
			int rightB = (m_colTrans.topLeftB + xIncB * (ofsX + dest.w)) >> 8;

			outB[0] = leftB;
			outB[1] = rightB;
			outB[2] = rightB;
			outB[3] = leftB;

			//

			int xIncG = int(m_colTrans.topRightG - m_colTrans.topLeftG) / m_colTrans.tintRect.w;

			int leftG = (m_colTrans.topLeftG + xIncG * ofsX) >> 8;
			int rightG = (m_colTrans.topLeftG + xIncG * (ofsX + dest.w)) >> 8;

			outG[0] = leftG;
			outG[1] = rightG;
			outG[2] = rightG;
			outG[3] = leftG;

			//

			int xIncR = int(m_colTrans.topRightR - m_colTrans.topLeftR) / m_colTrans.tintRect.w;

			int leftR = (m_colTrans.topLeftR + xIncR * ofsX) >> 8;
			int rightR = (m_colTrans.topLeftR + xIncR * (ofsX + dest.w)) >> 8;

			outR[0] = leftR;
			outR[1] = rightR;
			outR[2] = rightR;
			outR[3] = leftR;

			//

			int xIncA = int(m_colTrans.topRightA - m_colTrans.topLeftA) / m_colTrans.tintRect.w;

			int leftA = (m_colTrans.topLeftA + xIncA * ofsX) >> 8;
			int rightA = (m_colTrans.topLeftA + xIncA * (ofsX + dest.w)) >> 8;

			outA[0] = leftA;
			outA[1] = rightA;
			outA[2] = rightA;
			outA[3] = leftA;

		}
		else if (m_colTrans.mode == TintMode::GradientY)
		{
			int ofsY = dest.y - m_colTrans.tintRect.y;

			int topB = (m_colTrans.topLeftB + m_colTrans.leftIncB * ofsY) >> 8;
			int bottomB = (m_colTrans.topLeftB + m_colTrans.leftIncB * (ofsY + dest.h)) >> 8;

			outB[0] = topB;
			outB[1] = topB;
			outB[2] = bottomB;
			outB[3] = bottomB;

			//

			int topG = (m_colTrans.topLeftG + m_colTrans.leftIncG * ofsY) >> 8;
			int bottomG = (m_colTrans.topLeftG + m_colTrans.leftIncG * (ofsY + dest.h)) >> 8;

			outG[0] = topG;
			outG[1] = topG;
			outG[2] = bottomG;
			outG[3] = bottomG;

			//

			int topR = (m_colTrans.topLeftR + m_colTrans.leftIncR * ofsY) >> 8;
			int bottomR = (m_colTrans.topLeftR + m_colTrans.leftIncR * (ofsY + dest.h)) >> 8;

			outR[0] = topR;
			outR[1] = topR;
			outR[2] = bottomR;
			outR[3] = bottomR;

			//

			int topA = (m_colTrans.topLeftA + m_colTrans.leftIncA * ofsY) >> 8;
			int bottomA = (m_colTrans.topLeftA + m_colTrans.leftIncA * (ofsY + dest.h)) >> 8;

			outA[0] = topA;
			outA[1] = topA;
			outA[2] = bottomA;
			outA[3] = bottomA;
		}
		else if (m_colTrans.mode == TintMode::GradientXY)
		{
			CoordI ofs = dest.pos() - m_colTrans.tintRect.pos();

			int begB = m_colTrans.topLeftB + m_colTrans.leftIncB * ofs.y;
			int xIncB = (m_colTrans.topRightB + m_colTrans.rightIncB * ofs.y - begB) / m_colTrans.tintRect.w;

			outB[0] = (begB + xIncB * ofs.x) >> 8;
			outB[1] = (begB + xIncB * (ofs.x + dest.w)) >> 8;

			begB = m_colTrans.topLeftB + m_colTrans.leftIncB * (ofs.y + dest.h);
			xIncB = (m_colTrans.topRightB + m_colTrans.rightIncB * (ofs.y + dest.h) - begB) / m_colTrans.tintRect.w;

			outB[2] = (begB + xIncB * (ofs.x + dest.w)) >> 8;
			outB[3] = (begB + xIncB * ofs.x) >> 8;

			//

			int begG = m_colTrans.topLeftG + m_colTrans.leftIncG * ofs.y;
			int xIncG = (m_colTrans.topRightG + m_colTrans.rightIncG * ofs.y - begG) / m_colTrans.tintRect.w;

			outG[0] = (begG + xIncG * ofs.x) >> 8;
			outG[1] = (begG + xIncG * (ofs.x + dest.w)) >> 8;

			begG = m_colTrans.topLeftG + m_colTrans.leftIncG * (ofs.y + dest.h);
			xIncG = (m_colTrans.topRightG + m_colTrans.rightIncG * (ofs.y + dest.h) - begG) / m_colTrans.tintRect.w;

			outG[2] = (begG + xIncG * (ofs.x + dest.w)) >> 8;
			outG[3] = (begG + xIncG * ofs.x) >> 8;

			//

			int begR = m_colTrans.topLeftR + m_colTrans.leftIncR * ofs.y;
			int xIncR = (m_colTrans.topRightR + m_colTrans.rightIncR * ofs.y - begR) / m_colTrans.tintRect.w;

			outR[0] = (begR + xIncR * ofs.x) >> 8;
			outR[1] = (begR + xIncR * (ofs.x + dest.w)) >> 8;

			begR = m_colTrans.topLeftR + m_colTrans.leftIncR * (ofs.y + dest.h);
			xIncR = (m_colTrans.topRightR + m_colTrans.rightIncR * (ofs.y + dest.h) - begR) / m_colTrans.tintRect.w;

			outR[2] = (begR + xIncR * (ofs.x + dest.w)) >> 8;
			outR[3] = (begR + xIncR * ofs.x) >> 8;

			//

			int begA = m_colTrans.topLeftA + m_colTrans.leftIncA * ofs.y;
			int xIncA = (m_colTrans.topRightA + m_colTrans.rightIncA * ofs.y - begA) / m_colTrans.tintRect.w;

			outA[0] = (begA + xIncA * ofs.x) >> 8;
			outA[1] = (begA + xIncA * (ofs.x + dest.w)) >> 8;

			begA = m_colTrans.topLeftA + m_colTrans.leftIncA * (ofs.y + dest.h);
			xIncA = (m_colTrans.topRightA + m_colTrans.rightIncA * (ofs.y + dest.h) - begA) / m_colTrans.tintRect.w;

			outA[2] = (begA + xIncA * (ofs.x + dest.w)) >> 8;
			outA[3] = (begA + xIncA * ofs.x) >> 8;
		}
	}



	//____ drawSegments() ______________________________________________________

	void SoftGfxDevice::drawSegments(const RectI& _dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * _pEdgeStrips, int edgeStripPitch, TintMode tintMode)
	{
		RectI dest = _dest;

		SegmentEdge edges[c_maxSegments - 1];

		// Limit size of destination rect by number of edgestrips.

		if (dest.w > nEdgeStrips - 1)
			dest.w = nEdgeStrips - 1;

		// Apply tinting

		Color colors[c_maxSegments];
		bool	transparentSegments[c_maxSegments];

		SegmentGradient * pGradientsY = nullptr;
		SegmentGradient * pGradientsX = nullptr;
		int		gradientsYBufferSize = 0;
		int		gradientsXBufferSize = 0;

		// Determine combined tint-mode

		bool	bTintFlat = false;			// Set if we tint in any way, flat, x, y, or xy.
		bool	bTintX = false;
		bool	bTintY = false;

		if (tintMode != TintMode::None)
		{
			bTintFlat = m_tintColor != Color::White;

			if (tintMode == TintMode::GradientXY || m_colTrans.mode == TintMode::GradientXY)
			{
				bTintX = bTintY = true;
			}
			else
			{
				if (tintMode == TintMode::GradientY || m_colTrans.mode == TintMode::GradientY)
					bTintY = true;
				if (tintMode == TintMode::GradientX || m_colTrans.mode == TintMode::GradientX)
					bTintX = true;
			}
		}

		// If we have no tinting at all we still need to fill in transparentSegments

		if (!bTintFlat && !bTintX && !bTintY)
		{
			for (int i = 0; i < nSegments; i++)
				transparentSegments[i] = pSegmentColors[i].a == 0;
		}


		// If we just use flat tinting, we just tint our segment colors

		if (bTintFlat && !bTintX && !bTintY)
		{
			for (int i = 0; i < nSegments; i++)
			{
				Color col = pSegmentColors[i] * m_tintColor;
				colors[i] = col;
				transparentSegments[i] = (col.a == 0);
			}
			pSegmentColors = colors;
		}

		// If we instead have a gradient we have way more to do...

		else if (bTintX || bTintY)
		{
			// Generate the buffers that we will need

			if (bTintY)
			{
				gradientsYBufferSize = sizeof(SegmentGradient) * nSegments;
				pGradientsY = (SegmentGradient*)Base::memStackAlloc(gradientsYBufferSize);
			}

			if (bTintX)
			{
				// We use two gradients per segment in X, one for top and bottom of rectangle each.

				gradientsXBufferSize = sizeof(SegmentGradient) * nSegments * 2;
				pGradientsX = (SegmentGradient*)Base::memStackAlloc(gradientsXBufferSize);
			}


			// Calculate RGBA values from m_colTrans for our four corners.

			int		baseB[4], baseG[4], baseR[4], baseA[4];

			_colTransRect(baseB, baseG, baseR, baseA, _dest);

			// Lets process each segment

			for (int seg = 0; seg < nSegments; seg++)
			{

				int		segB[4], segG[4], segR[4], segA[4];

				// Calculate RGBA values for our four corners

				if (tintMode == TintMode::Flat)
				{
					Color col = pSegmentColors[seg];

					for (int i = 0; i < 4; i++)
					{
						segB[i] = baseB[i] * col.b;
						segG[i] = baseG[i] * col.g;
						segR[i] = baseR[i] * col.r;
						segA[i] = baseA[i] * col.a;
					}
				}
				else
				{
					Color col[4];

					if (tintMode == TintMode::GradientX)
					{
						col[0] = col[3] = pSegmentColors[seg * 2];
						col[1] = col[2] = pSegmentColors[seg * 2 + 1];
					}
					else if (tintMode == TintMode::GradientY)
					{
						col[0] = col[1] = pSegmentColors[seg * 2];
						col[2] = col[3] = pSegmentColors[seg * 2 + 1];
					}
					else
					{
						col[0] = pSegmentColors[seg * 4];
						col[1] = pSegmentColors[seg * 4 + 1];
						col[2] = pSegmentColors[seg * 4 + 2];
						col[3] = pSegmentColors[seg * 4 + 3];
					}

					for (int i = 0; i < 4; i++)
					{
						segB[i] = baseB[i] * col[i].b;
						segG[i] = baseG[i] * col[i].g;
						segR[i] = baseR[i] * col[i].r;
						segA[i] = baseA[i] * col[i].a;
					}
				}

				// We now have the segments corner colors. Let's save
				// that information in the correct format for future
				// processing depending on tint mode.

				// Filling in the x-gradient if we have one. Two SegmentGradient structs for
				// each segment, one for gradient along top and of dest rectangle and one along the bottom.

				if (bTintX)
				{
					// Fill in top gradient

					auto p = &pGradientsX[seg * 2];

					p->begB = segB[0];
					p->begG = segG[0];
					p->begR = segR[0];
					p->begA = segA[0];

					p->incB = (segB[1] - segB[0]) / dest.w;
					p->incG = (segG[1] - segG[0]) / dest.w;
					p->incR = (segR[1] - segR[0]) / dest.w;
					p->incA = (segA[1] - segA[0]) / dest.w;

					// Fill in bottom gradient

					p++;

					p->begB = segB[3];
					p->begG = segG[3];
					p->begR = segR[3];
					p->begA = segA[3];

					p->incB = (segB[2] - segB[3]) / dest.w;
					p->incG = (segG[2] - segG[3]) / dest.w;
					p->incR = (segR[2] - segR[3]) / dest.w;
					p->incA = (segA[2] - segA[3]) / dest.w;
				}

				// If we don't have any x-gradient we can fill in y-gradient once and for all

				if (bTintY && !bTintX)
				{
					auto p = &pGradientsY[seg];

					p->begB = segB[0];
					p->begG = segG[0];
					p->begR = segR[0];
					p->begA = segA[0];

					p->incB = (segB[3] - segB[0]) / dest.h;
					p->incG = (segG[3] - segG[0]) / dest.h;
					p->incR = (segR[3] - segR[0]) / dest.h;
					p->incA = (segA[3] - segA[0]) / dest.h;
				}

				// Possibly mark this segment as transparent

				transparentSegments[seg] = (segA[0] + segA[1] + segA[2] + segA[3] == 0);
			}
		}

		// Set start position and clip dest

		uint8_t * pOrigo = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * (m_canvasPixelBits / 8);

		if (!dest.intersectsWith(m_clipBounds))
			return;

		SegmentOp_p	pOp = s_segmentOpTab[(int)bTintY][(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if (pOp == nullptr)
			return;

		// Loop through patches

		for (int patchIdx = 0; patchIdx < m_nClipRects; patchIdx++)
		{
			// Clip patch

			RectI patch(dest, m_pClipRects[patchIdx]);
			if (patch.w == 0 || patch.h == 0)
				continue;

			// Calculate stripstart, clipBeg/clipEnd and first edge for patch

			int columnOfs = patch.x - dest.x;
			int rowOfs = patch.y - dest.y;

			const int *		pEdgeStrips = _pEdgeStrips + columnOfs * edgeStripPitch;
			uint8_t *		pStripStart = pOrigo + columnOfs * (m_canvasPixelBits / 8);

			int clipBeg = rowOfs * 256;
			int clipEnd = clipBeg + (patch.h * 256);

			for (int x = 0; x < patch.w; x++)
			{
				int nEdges = 0;
				int skippedSegments = 0;

				for (int y = 0; y < nSegments - 1; y++)
				{
					int beg = pEdgeStrips[y];
					int end = pEdgeStrips[y + edgeStripPitch];

					if (beg > end)
						swap(beg, end);

					if (beg >= clipEnd)
						break;

					if (end > clipBeg)
					{
						int coverageInc = (end == beg) ? 0 : (65536 * 256) / (end - beg);
						int coverage = 0;

						if (beg < clipBeg)
						{
							int cut = clipBeg - beg;
							beg = clipBeg;
							coverage += (coverageInc*cut) >> 8;
						}

						if (end > clipEnd)
							end = clipEnd;


						edges[nEdges].begin = beg;
						edges[nEdges].end = end;
						edges[nEdges].coverage = coverage;
						edges[nEdges].coverageInc = coverageInc;
						nEdges++;
					}
					else
						skippedSegments++;
				}

				// Update tinting if we have X-gradient

				const Color * pColors = pSegmentColors + skippedSegments;

				if (bTintX)
				{
					if (bTintY)
					{
						auto pOut = pGradientsY + skippedSegments;
						auto pIn = pGradientsX + skippedSegments * 2;

						for (int i = 0; i < nEdges + 1; i++)
						{
							int beg = pIn[0].begB + pIn[0].incB * columnOfs;
							int inc = (pIn[1].begB + pIn[1].incB * columnOfs - beg) / dest.h;
							pOut[i].begB = beg;
							pOut[i].incB = inc;

							beg = pIn[0].begG + pIn[0].incG * columnOfs;
							inc = (pIn[1].begG + pIn[1].incG * columnOfs - beg) / dest.h;
							pOut[i].begG = beg;
							pOut[i].incG = inc;

							beg = pIn[0].begR + pIn[0].incR * columnOfs;
							inc = (pIn[1].begR + pIn[1].incR * columnOfs - beg) / dest.h;
							pOut[i].begR = beg;
							pOut[i].incR = inc;

							beg = pIn[0].begA + pIn[0].incA * columnOfs;
							inc = (pIn[1].begA + pIn[1].incA * columnOfs - beg) / dest.h;
							pOut[i].begA = beg;
							pOut[i].incA = inc;

							pIn += 2;
						}
					}
					else
					{
						// We just use the color array and fill in correct color values for this column.

						auto pGrad = pGradientsX + skippedSegments * 2;

						for (int i = 0; i < nEdges + 1; i++ )
						{
							colors[i].b = (pGrad->begB + pGrad->incB * columnOfs) >> 16;
							colors[i].g = (pGrad->begG + pGrad->incG * columnOfs) >> 16;
							colors[i].r = (pGrad->begR + pGrad->incR * columnOfs) >> 16;
							colors[i].a = (pGrad->begA + pGrad->incA * columnOfs) >> 16;
							pGrad += 2;												// Skipping bottom gradient data since we don't need it.
						}

						pColors = colors;
					}
				}

				//

				pOp(clipBeg, clipEnd, pStripStart, m_canvasPitch, nEdges, edges, pColors, pGradientsY+skippedSegments, transparentSegments+skippedSegments);
				pEdgeStrips += edgeStripPitch;
				pStripStart += (m_canvasPixelBits / 8);
				columnOfs++;
			}
		}

		// Free what we have reserved on the memStack.

		if (gradientsXBufferSize > 0)
			Base::memStackRelease(gradientsXBufferSize);

		if (gradientsYBufferSize > 0)
			Base::memStackRelease(gradientsYBufferSize);
	}

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
				//				printf( "%d - %d - %d\n", i, brush[i], m_pCurveTab[0][(c_nCurveTabEntries - 1) - (i * c_nCurveTabEntries) / brushSteps]);
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

	//____ plotPixels() _________________________________________________

	void SoftGfxDevice::plotPixels(int nCoords, const CoordI * pCoords, const Color * pColors)
	{
		const int pitch = m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits / 8;

		PlotListOp_p pOp = s_plotListOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (pOp == nullptr )
			return;

		for (int i = 0; i < m_nClipRects; i++)
			pOp(m_pClipRects[i], nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch, m_colTrans);
	}

	//____ setBlitSource() ____________________________________________________

	bool SoftGfxDevice::setBlitSource(Surface * pSource)
	{
		if (pSource == m_pBlitSource)
			return true;

		if (!pSource || !pSource->isInstanceOf(SoftSurface::TYPEINFO))
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

	//____ _rotScaleBlit() ____________________________________________________

	void SoftGfxDevice::rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter )
	{
		m_bClipSource = true;
		GfxDevice::rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter);
		m_bClipSource = false;
	}


	//____ _transformBlit() [simple] ____________________________________

	void SoftGfxDevice::_transformBlit(const RectI& dest, CoordI _src, const int simpleTransform[2][2])
	{
		// Clip and render the patches

		if (!dest.intersectsWith(m_clipBounds))
			return;

		const RectI& clip = dest;

		// Step forward _src by half a pixel, so we start from correct pixel.

		_src.x = (_src.x * 2 + simpleTransform[0][0] + simpleTransform[1][0]) / 2;
		_src.y = (_src.y * 2 + simpleTransform[0][1] + simpleTransform[1][1]) / 2;

		//

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI  patch = m_pClipRects[i];

			CoordI src = _src;

			CoordI	patchOfs = patch.pos() - dest.pos();


			if ((clip.x > patch.x) || (clip.x + clip.w < patch.x + patch.w) ||
				(clip.y > patch.y) || (clip.y + clip.h < patch.y + patch.h))
			{

				if ((clip.x > patch.x + patch.w) || (clip.x + clip.w < patch.x) ||
					(clip.y > patch.y + patch.h) || (clip.y + clip.h < patch.y))
					continue;																					// Totally outside clip-rect.

				if (patch.x < clip.x)
				{
					int xDiff = clip.x - patch.x;
					patch.w -= xDiff;
					patch.x = clip.x;
					patchOfs.x += xDiff;
				}

				if (patch.y < clip.y)
				{
					int yDiff = clip.y - patch.y;
					patch.h -= yDiff;
					patch.y = clip.y;
					patchOfs.y += yDiff;
				}

				if (patch.x + patch.w > clip.x + clip.w)
					patch.w = (clip.x + clip.w) - patch.x;

				if (patch.y + patch.h > clip.y + clip.h)
					patch.h = (clip.y + clip.h) - patch.y;
			}

			//

			src.x += patchOfs.x * simpleTransform[0][0] + patchOfs.y * simpleTransform[1][0];
			src.y += patchOfs.x * simpleTransform[0][1] + patchOfs.y * simpleTransform[1][1];

			(this->*m_pSimpleBlitOp)(patch, src, simpleTransform, patch.pos());
		}
	}

	//____ _transformBlit() [complex] ____________________________________

	void SoftGfxDevice::_transformBlit(const RectI& dest, CoordF _src, const float complexTransform[2][2])
	{
		// Clip and render the patches

		if (!dest.intersectsWith(m_clipBounds))
			return;


		const RectI& clip = dest;

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI  patch = m_pClipRects[i];

			CoordF src = _src;

			CoordI	patchOfs = patch.pos() - dest.pos();


			if ((clip.x > patch.x) || (clip.x + clip.w < patch.x + patch.w) ||
				(clip.y > patch.y) || (clip.y + clip.h < patch.y + patch.h))
			{

				if ((clip.x > patch.x + patch.w) || (clip.x + clip.w < patch.x) ||
					(clip.y > patch.y + patch.h) || (clip.y + clip.h < patch.y))
					continue;																					// Totally outside clip-rect.

				if (patch.x < clip.x)
				{
					int xDiff = clip.x - patch.x;
					patch.w -= xDiff;
					patch.x = clip.x;
					patchOfs.x += xDiff;
				}

				if (patch.y < clip.y)
				{
					int yDiff = clip.y - patch.y;
					patch.h -= yDiff;
					patch.y = clip.y;
					patchOfs.y += yDiff;
				}

				if (patch.x + patch.w > clip.x + clip.w)
					patch.w = (clip.x + clip.w) - patch.x;

				if (patch.y + patch.h > clip.y + clip.h)
					patch.h = (clip.y + clip.h) - patch.y;
			}

			//

			src.x += patchOfs.x * complexTransform[0][0] + patchOfs.y * complexTransform[1][0];
			src.y += patchOfs.x * complexTransform[0][1] + patchOfs.y * complexTransform[1][1];

			// See if we can skip the expensive source clipping....

			bool bClipSource = false;
			if (m_bClipSource)
			{
				SizeF clipMax = static_cast<SizeF>(m_pBlitSource->m_size);

				if (m_pBlitSource->m_scaleMode == ScaleMode::Interpolate)
					clipMax -= SizeF(1.f, 1.f);

				CoordF src2 = src;		// Source pixel to read for top-right destination corner.
				CoordF src3 = src;		// Source pixel to read for bottom-right destination corner.
				CoordF src4 = src;		// Source pixel to read for bottom-left destination corner.

				src2.x += (patch.w - 1) * complexTransform[0][0];
				src2.y += (patch.w - 1) * complexTransform[0][1];

				src3.x += (patch.w - 1) * complexTransform[0][0] + (patch.h - 1) * complexTransform[1][0];
				src3.y += (patch.w - 1) * complexTransform[0][1] + (patch.h - 1) * complexTransform[1][1];

				src4.x += (patch.h - 1) * complexTransform[1][0];
				src4.y += (patch.h - 1) * complexTransform[1][1];

				if (src.x < 0 || src.y < 0 || src.x >= clipMax.w || src.y >= clipMax.h ||
					src2.x < 0 || src2.y < 0 || src2.x > clipMax.w || src2.y > clipMax.h ||
					src3.x < 0 || src3.y < 0 || src3.x > clipMax.w || src3.y > clipMax.h ||
					src4.x < 0 || src4.y < 0 || src4.x > clipMax.w || src4.y > clipMax.h )
				{
					bClipSource = true;
				}
			}

			//

			(this->*m_pComplexBlitOp)(patch, src, complexTransform,patch.pos(),bClipSource);
		}
	}


	//____ _onePassSimpleBlit() _____________________________________________

	void SoftGfxDevice::_onePassSimpleBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos)
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

		m_pSimpleBlitOnePassOp(pSrc, pDst, pSource->m_pClut, pitches, dest.h, dest.w, m_colTrans, patchPos);
	}

	//____ _twoPassSimpleBlit() _____________________________________________

	void SoftGfxDevice::_twoPassSimpleBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos)
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
			uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + line*(srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1]) + src.x * srcPixelBytes;
//			uint8_t * pSrc = pSource->m_pData + (src.y+line) * pSource->m_pitch + src.x * srcPixelBytes;

			m_pSimpleBlitFirstPassOp(pSrc, pChunkBuffer, pSource->m_pClut, pitchesPass1, thisChunkLines, dest.w, m_colTrans, { 0,0 });
			m_pBlitSecondPassOp(pChunkBuffer, pDst, nullptr, pitchesPass2, thisChunkLines, dest.w, m_colTrans, patchPos);

			patchPos.y += thisChunkLines;
			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _onePassComplexBlit() ____________________________________________

	void SoftGfxDevice::_onePassComplexBlit(const RectI& dest, CoordF pos, const float transformMatrix[2][2], CoordI patchPos, bool bClipSource)
	{
		const SoftSurface * pSource = m_pBlitSource;

		// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

		if (pSource->m_scaleMode == ScaleMode::Interpolate)
		{
//			pos.x -= 0.5f;
//			pos.y -= 0.5f;
		}

		int dstPixelBytes = m_canvasPixelBits / 8;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;

		ComplexBlitOp_p op = bClipSource ? m_pComplexBlitClipOnePassOp : m_pComplexBlitOnePassOp;

		op(pSource, pos, transformMatrix, pDst, dstPixelBytes, m_canvasPitch - dstPixelBytes * dest.w, dest.h, dest.w, m_colTrans, patchPos);
	}


	//____ _twoPassComplexBlit() ____________________________________________

	void SoftGfxDevice::_twoPassComplexBlit(const RectI& dest, CoordF pos, const float transformMatrix[2][2], CoordI patchPos, bool bClipSource)
	{
		const SoftSurface * pSource = m_pBlitSource;

		// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

		if (pSource->m_scaleMode == ScaleMode::Interpolate)
		{
//			pos.x -= 0.5f;
//			pos.y -= 0.5f;
		}


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

		ComplexBlitOp_p firstPassOp = bClipSource ? m_pComplexBlitClipFirstPassOp : m_pComplexBlitFirstPassOp;

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y + line) * m_canvasPitch + dest.x * dstPixelBytes;

			firstPassOp(pSource, pos, transformMatrix, pChunkBuffer, 4, 0, thisChunkLines, dest.w, m_colTrans, { 0,0 });
			m_pBlitSecondPassOp(pChunkBuffer, pDst, nullptr, pitchesPass2, thisChunkLines, dest.w, m_colTrans, patchPos);

			pos.x += transformMatrix[1][0] * thisChunkLines;
			pos.y += transformMatrix[1][1] * thisChunkLines;

			patchPos.y += thisChunkLines;
			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _dummySimpleBlit() _________________________________________________

	void SoftGfxDevice::_dummySimpleBlit(const RectI& dest, CoordI pos, const int simpleTransform[2][2], CoordI patchPos)
	{
		// Do nothing but prevent crashing or need to check for nullpointer ;)
	}

	//____ _dummyComplexBlit() ________________________________________________

	void SoftGfxDevice::_dummyComplexBlit(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, bool ClipSource)
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

		ScaleMode		scaleMode = m_pBlitSource->scaleMode();
		PixelFormat		srcFormat = m_pBlitSource->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pCanvas->pixelFormat();

		// Add fallback back to two-pass rendering.

		m_pSimpleBlitOnePassOp = nullptr;
		m_pComplexBlitOnePassOp = nullptr;

		m_pSimpleBlitFirstPassOp = s_moveTo_BGRA_8_OpTab[(int)srcFormat];
		m_pComplexBlitFirstPassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][false];
		m_pComplexBlitClipFirstPassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][true];
		m_pBlitSecondPassOp = s_pass2OpTab[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		// Try to find a suitable one-pass operation

		if (m_colTrans.mode == TintMode::None)
		{
			if (m_blendMode == BlendMode::Blend)
			{
				if (dstFormat == PixelFormat::BGRA_8)
				{
					m_pSimpleBlitOnePassOp = s_blendTo_BGRA_8_OpTab[(int)srcFormat];
					m_pComplexBlitOnePassOp = s_transformBlendTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][false];
					m_pComplexBlitClipOnePassOp = s_transformBlendTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][true];
				}
				else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				{
					m_pSimpleBlitOnePassOp = s_blendTo_BGR_8_OpTab[(int)srcFormat];
					m_pComplexBlitOnePassOp = s_transformBlendTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][false];
					m_pComplexBlitClipOnePassOp = s_transformBlendTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][true];
				}
			}
			else if (m_blendMode == BlendMode::Replace)
			{
				if (dstFormat == PixelFormat::BGRA_8)
				{
					m_pSimpleBlitOnePassOp = s_moveTo_BGRA_8_OpTab[(int)srcFormat];
					m_pComplexBlitOnePassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][false];
					m_pComplexBlitClipOnePassOp = s_transformTo_BGRA_8_OpTab[(int)srcFormat][(int)scaleMode][true];
				}
				else if (dstFormat == PixelFormat::BGR_8 || dstFormat == PixelFormat::BGRX_8)
				{
					m_pSimpleBlitOnePassOp = s_moveTo_BGR_8_OpTab[(int)srcFormat];
					m_pComplexBlitOnePassOp = s_transformTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][false];
					m_pComplexBlitClipOnePassOp = s_transformTo_BGR_8_OpTab[(int)srcFormat][(int)scaleMode][true];
				}
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

		//TODO: Both of these causes artefacts, but in different ways. What to do?

		for (int i = 0; i < 256; i++)
			s_mulTab[i] = 65536 * i / 255;

//		for (int i = 0; i < 256; i++)
//			s_mulTab[i] = i * 256 + i + 1;

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
					s_fillOpTab[k][i][j] = nullptr;
					s_pass2OpTab[k][i][j] = nullptr;
				}

				s_plotOpTab[i][j] = nullptr;
				s_LineOpTab[i][j] = nullptr;
				s_clipLineOpTab[i][j] = nullptr;
				s_plotListOpTab[i][j] = nullptr;
				s_segmentOpTab[0][i][j] = nullptr;
				s_segmentOpTab[1][i][j] = nullptr;
			}
		}

		for (int i = 0; i < PixelFormat_size; i++)
		{
			s_moveTo_BGRA_8_OpTab[i] = nullptr;
			s_moveTo_BGR_8_OpTab[i] = nullptr;

			s_blendTo_BGRA_8_OpTab[i] = nullptr;
			s_blendTo_BGR_8_OpTab[i] = nullptr;

			s_transformTo_BGRA_8_OpTab[i][0][0] = nullptr;
			s_transformTo_BGRA_8_OpTab[i][1][0] = nullptr;
			s_transformTo_BGR_8_OpTab[i][0][0] = nullptr;
			s_transformTo_BGR_8_OpTab[i][1][0] = nullptr;

			s_transformTo_BGRA_8_OpTab[i][0][1] = nullptr;
			s_transformTo_BGRA_8_OpTab[i][1][1] = nullptr;
			s_transformTo_BGR_8_OpTab[i][0][1] = nullptr;
			s_transformTo_BGR_8_OpTab[i][1][1] = nullptr;

			s_transformBlendTo_BGRA_8_OpTab[i][0][0] = nullptr;
			s_transformBlendTo_BGRA_8_OpTab[i][1][0] = nullptr;
			s_transformBlendTo_BGR_8_OpTab[i][0][0] = nullptr;
			s_transformBlendTo_BGR_8_OpTab[i][1][0] = nullptr;

			s_transformBlendTo_BGRA_8_OpTab[i][0][1] = nullptr;
			s_transformBlendTo_BGRA_8_OpTab[i][1][1] = nullptr;
			s_transformBlendTo_BGR_8_OpTab[i][0][1] = nullptr;
			s_transformBlendTo_BGR_8_OpTab[i][1][1] = nullptr;
		}


		// Init Plot Operation Table

		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Replace, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _plot<BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _plot<BlendMode::Replace, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Replace, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::A8] = _plot<BlendMode::Replace, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Blend, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _plot<BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _plot<BlendMode::Blend, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Blend, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::A8] = _plot<BlendMode::Blend, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Add, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _plot<BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _plot<BlendMode::Add, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Add, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::A8] = _plot<BlendMode::Add, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _plot<BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _plot<BlendMode::Subtract, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::A8] = _plot<BlendMode::Subtract, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _plot<BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _plot<BlendMode::Multiply, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::A8] = _plot<BlendMode::Multiply, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Min, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _plot<BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _plot<BlendMode::Min, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Min, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Min][(int)PixelFormat::A8] = _plot<BlendMode::Min, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Max, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _plot<BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _plot<BlendMode::Max, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Max, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Max][(int)PixelFormat::A8] = _plot<BlendMode::Max, TintMode::None, PixelFormat::A8>;

		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _plot<BlendMode::Invert, TintMode::None, PixelFormat::BGRA_8>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _plot<BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _plot<BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _plot<BlendMode::Invert, TintMode::None, PixelFormat::BGR_565>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _plot<BlendMode::Invert, TintMode::None, PixelFormat::BGRA_4>;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::A8] = _plot<BlendMode::Invert, TintMode::None, PixelFormat::A8>;

		// Init Fill Operation Table

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Replace, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Replace, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Blend, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Blend, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Add][(int) PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Add, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Add, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Add, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Add, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Subtract, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Multiply, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Min, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Min, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Min, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Min, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Max, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Max, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Max, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Max, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill<TintMode::None, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _fill<TintMode::None, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill<TintMode::None, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _fill<TintMode::None, BlendMode::Invert, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _fill<TintMode::None, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::A8] = _fill<TintMode::None, BlendMode::Invert, PixelFormat::A8>;


		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Replace, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Blend, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Add, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Add, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Subtract, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Multiply, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Min, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Min, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Max, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Max, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill<TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _fill<TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill<TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _fill<TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _fill<TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::A8] = _fill<TintMode::Flat, BlendMode::Invert, PixelFormat::A8>;


		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Replace, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Blend, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Add, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Subtract, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Multiply, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Min, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Max, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::A8] = _fill<TintMode::GradientX, BlendMode::Invert, PixelFormat::A8>;


		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Replace, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Blend, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Add, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Subtract, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Multiply, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Min, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Max, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::A8] = _fill<TintMode::GradientY, BlendMode::Invert, PixelFormat::A8>;


		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Replace, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Blend, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Add, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Subtract, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Multiply, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Min, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Max, PixelFormat::A8>;

		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill<TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _fill<TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill<TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _fill<TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_565>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _fill<TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_fillOpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::A8] = _fill<TintMode::GradientXY, BlendMode::Invert, PixelFormat::A8>;


		// Init Line Operation Table

		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Replace][(int)PixelFormat::A8] = _draw_line <BlendMode::Replace, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::A8] = _draw_line <BlendMode::Blend, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Add][(int)PixelFormat::A8] = _draw_line <BlendMode::Add, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::A8] = _draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::A8] = _draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Min][(int)PixelFormat::A8] = _draw_line <BlendMode::Min, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Max][(int)PixelFormat::A8] = _draw_line <BlendMode::Max, TintMode::None, PixelFormat::A8>;

		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_8>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGR_565>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_4>;
		s_LineOpTab[(int)BlendMode::Invert][(int)PixelFormat::A8] = _draw_line <BlendMode::Invert, TintMode::None, PixelFormat::A8>;

		// Init ClipLine Operation Table

		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Replace][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Replace, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Blend, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Add, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Add][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Add, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Subtract][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Subtract, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Multiply][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Multiply, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Min, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Min][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Min, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Max, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Max][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Max, TintMode::None, PixelFormat::A8>;

		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _clip_draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_8>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _clip_draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _clip_draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _clip_draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGR_565>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _clip_draw_line <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_4>;
		s_clipLineOpTab[(int)BlendMode::Invert][(int)PixelFormat::A8] = _clip_draw_line <BlendMode::Invert, TintMode::None, PixelFormat::A8>;


		// Init PlotList Operation Table

		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Replace, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Replace, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Replace, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Replace][(int)PixelFormat::A8] = _plot_list <BlendMode::Replace, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Blend, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Blend, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Blend, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::A8] = _plot_list <BlendMode::Blend, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Add, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Add, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Add, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Add, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Add][(int)PixelFormat::A8] = _plot_list <BlendMode::Add, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Subtract, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Subtract, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Subtract][(int)PixelFormat::A8] = _plot_list <BlendMode::Subtract, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Multiply, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Multiply, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Multiply][(int)PixelFormat::A8] = _plot_list <BlendMode::Multiply, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Min, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Min, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Min, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Min, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Min][(int)PixelFormat::A8] = _plot_list <BlendMode::Min, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Max, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Max, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Max, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Max, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Max][(int)PixelFormat::A8] = _plot_list <BlendMode::Max, TintMode::None, PixelFormat::A8>;

		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _plot_list <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_8>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _plot_list <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _plot_list <BlendMode::Invert, TintMode::None, PixelFormat::BGR_8>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _plot_list <BlendMode::Invert, TintMode::None, PixelFormat::BGR_565>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _plot_list <BlendMode::Invert, TintMode::None, PixelFormat::BGRA_4>;
		s_plotListOpTab[(int)BlendMode::Invert][(int)PixelFormat::A8] = _plot_list <BlendMode::Invert, TintMode::None, PixelFormat::A8>;



		// Init Blit Pass 2 Operation Table

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Add, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Add, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Add, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Subtract, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Multiply, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Min, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Min, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Min, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Min, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Max, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Max, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Max, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Max, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Invert, PixelFormat::A8>;


		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Replace, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Blend, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Add, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Subtract, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Multiply, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Min, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Max, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::Flat, BlendMode::Invert, PixelFormat::A8>;


		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Replace, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Blend, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Add, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Subtract, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Multiply, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Min, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Max, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientX, BlendMode::Invert, PixelFormat::A8>;


		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Replace, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Blend, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Add, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Subtract, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Multiply, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Min, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Max, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientY, BlendMode::Invert, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Replace, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Blend, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Add, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Min, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Max, PixelFormat::A8>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_565>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::A8] = _simple_blit < PixelFormat::BGRA_8, TintMode::GradientXY, BlendMode::Invert, PixelFormat::A8>;


		// Init straight move to BGRA_8 Operation Table
		// (also used as pass 1 Operation Table for straight blit)

		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGR_565, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_4, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::I8] =_simple_blit < PixelFormat::I8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_moveTo_BGRA_8_OpTab[(int)PixelFormat::A8] =_simple_blit < PixelFormat::A8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8>;


		// Init straight blend to BGRA_8 Operation Table

		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGR_565, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_4, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::I8] =_simple_blit < PixelFormat::I8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_blendTo_BGRA_8_OpTab[(int)PixelFormat::A8] =_simple_blit < PixelFormat::A8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8>;


		// Init straight move to BGR_8 Operation Table

		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGR_565, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_4, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::I8] =_simple_blit < PixelFormat::I8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;
		s_moveTo_BGR_8_OpTab[(int)PixelFormat::A8] =_simple_blit < PixelFormat::A8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8>;


		// Init straight blend to BGR_8 Operation Table

		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8] =_simple_blit < PixelFormat::BGRA_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8] =_simple_blit < PixelFormat::BGR_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565] =_simple_blit < PixelFormat::BGR_565, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4] =_simple_blit < PixelFormat::BGRA_4, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::I8] =_simple_blit < PixelFormat::I8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;
		s_blendTo_BGR_8_OpTab[(int)PixelFormat::A8] =_simple_blit < PixelFormat::A8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8>;


		// Init transform move to BGRA_8 Operation Table

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, false>;

		// Clipped versions

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;

		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;
		s_transformTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8, true>;


		// Init transform blend to BGRA_8 Operation Table

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8 , false>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, false>;

		// Clipped versions

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;

		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;
		s_transformBlendTo_BGRA_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8, true>;


		// Init transform move to BGR_8 Operation Table

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, false>;

		// Clipped versions

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;

		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;
		s_transformTo_BGR_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8, true>;


		// Init transform blend to BGR_8 Operation Table

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][0] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][0] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][0] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][0] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][0][0] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][1][0] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][0][0] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][1][0] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, false>;

		// Clipped versions

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][0][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_8][1][1] = _complex_blit < PixelFormat::BGRA_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRX_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][0][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_8][1][1] = _complex_blit < PixelFormat::BGR_8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][0][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGR_565][1][1] = _complex_blit < PixelFormat::BGR_565, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][0][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::BGRA_4][1][1] = _complex_blit < PixelFormat::BGRA_4, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][0][1] = _complex_blit < PixelFormat::I8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::I8][1][1] = _complex_blit < PixelFormat::I8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;

		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][0][1] = _complex_blit < PixelFormat::A8, ScaleMode::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;
		s_transformBlendTo_BGR_8_OpTab[(int)PixelFormat::A8][1][1] = _complex_blit < PixelFormat::A8, ScaleMode::Interpolate, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8, true>;



		// Init Segments Operation Table

		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::A8>;

		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_8>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_565>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::A8] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::A8>;

		// Versions with vertical tint

		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::A8>;

		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGRA_8>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_8>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGR_565] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_565>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGRA_4>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::A8] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::A8>;


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
