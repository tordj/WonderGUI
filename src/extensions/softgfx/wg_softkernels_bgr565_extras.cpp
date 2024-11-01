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

#include <wg_softkernels_bgr565_extras.h>
#include <wg_c_softgfx.h>

using namespace wg;

//____ _fill_bgr565linear_noblend_notint() ____________________________________________________________

static void _fill_bgr565linear_noblend_notint(uint8_t* pDst, int pitchX, int pitchY, int nLines, int lineLength, HiColor col, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	// Generate our 32-bit double-pixel.
	
	const uint8_t* pPackTab = HiColor::packLinearTab;

	int16_t b = pPackTab[col.b];
	int16_t g = pPackTab[col.g];
	int16_t r = pPackTab[col.r];
	

	uint8_t higherByte = (r & 0xF8) | (g >> 5);
	uint8_t lowerByte = ((g & 0x1C) << 3) | (b >> 3);
	
	uint32_t doublePixel = lowerByte | (uint32_t(higherByte) << 8);
	doublePixel |= doublePixel << 16;
	
	// Calculate number of loops and if we need extra start/end writes
	
	int loops = lineLength;
	bool bStartSingle = false;
	
	if( (intptr_t(pDst) & 0x3) != 0 )
	{
		bStartSingle = true;
		loops--;
	}
	
	bool bEndSingle = loops % 2;
	loops /= 2;
	
	// Fill loop
	
	for (int y = 0; y < nLines; y++)
	{
		if( bStartSingle )
		{
			* ((uint16_t*)pDst) = (uint16_t) doublePixel;
			pDst += 2;
		}

		for (int x = 0; x < loops; x++)
		{
			* ((uint32_t*)pDst) = doublePixel;
			pDst += 4;
		}
		if( bEndSingle )
		{
			* ((uint16_t*)pDst) = (uint16_t) doublePixel;
			pDst += 2;
		}

		pDst += pitchY;
	}
}

//____ _straight_blit_bgr565linear_to_same_notint_noblend() ____________________________________________________________

static void _straight_blit_bgr565linear_to_same_notint_noblend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	if( pitches.srcX == 2 && pitches.dstX == 2 )
	{
		if((intptr_t(pSrc) & 0x3) == (intptr_t(pDst) & 0x3))
		{
			// Calculate number of loops and if we need extra start/end writes
			
			int loops = lineLength;
			bool bStartSingle = false;
			
			if( (intptr_t(pDst) & 0x3) != 0 )
			{
				bStartSingle = true;
				loops--;
			}
			
			bool bEndSingle = loops % 2;
			loops /= 2;

			for (int y = 0; y < nLines; y++)
			{
				if( bStartSingle )
				{
					* ((uint16_t*)pDst) = * ((uint16_t*)pSrc);
					pSrc += 2;
					pDst += 2;
				}
								
				for (int x = 0; x < loops; x++)
				{
					* ((uint32_t*)pDst) = * ((uint32_t*) pSrc);
					pSrc += 4;
					pDst += 4;
				}

				if( bEndSingle )
				{
					* ((uint16_t*)pDst) = * ((uint16_t*)pSrc);
					pSrc += 2;
					pDst += 2;
				}
				
				pSrc += pitches.srcY;
				pDst += pitches.dstY;
			}
			
			
		}
		else
		{
			for (int y = 0; y < nLines; y++)
			{
				for (int x = 0; x < lineLength; x++)
				{
					* ((uint16_t*)pDst) = * ((uint16_t*) pSrc);
					pSrc += 2;
					pDst += 2;
				}

				pSrc += pitches.srcY;
				pDst += pitches.dstY;
			}
		}
	}
	else
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				* ((uint16_t*)pDst) = * ((uint16_t*) pSrc);
				pSrc += pitches.srcX;
				pDst += pitches.srcY;
			}

			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}
}

//____ _straight_blit_index8linear_to_bgr565linear_notint_noblend() ____________________________________________________________

static void _straight_blit_index8linear_to_bgr565linear_notint_noblend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	auto pPalette = pSrcSurf->palette();
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			uint32_t col = ((uint32_t*)pPalette)[*pSrc];

			uint16_t out = ((col >> 3) & 0x001F) | ((col >> 5) & 0x07E0) | ((col >> 8 ) & 0xF800);
			* (uint16_t*)pDst = out;
			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ _straight_blit_index8linear_to_bgr565linear_notint_blend() ____________________________________________________________

static void _straight_blit_index8linear_to_bgr565linear_notint_blend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	auto pPalette = pSrcSurf->palette();
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			Color8 col = pPalette[*pSrc];

			int alpha = SoftGfxDevice::s_mulTab[col.a];
			int invAlpha = 65536 - alpha;

			uint16_t back = * ((uint16_t*)pDst);
	
			int backR = (back >> 11);
			int backG = (back >> 5) & 0x3F;
			int backB = back & 0x1F;
						
			int outB = (backB * invAlpha + col.b * (alpha>>3)) >> 16;
			int outG = (backG * invAlpha + col.g * (alpha>>2)) >> 16;
			int outR = (backR * invAlpha + col.r * (alpha>>3)) >> 16;
			
			uint16_t out = outB | (outG << 5) | (outR << 11 );

			* (uint16_t*)pDst = out;
			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}



//____ _straight_blit_alpha8_to_bgr565linear_notint_blend() ____________________________________________________________

static void _straight_blit_alpha8_to_bgr565linear_notint_blend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			int alpha = SoftGfxDevice::s_mulTab[* pSrc];
			int invAlpha = 65536 - alpha;

			uint16_t back = * ((uint16_t*)pDst);
	
			int backR = (back >> 11);
			int backG = (back >> 5) & 0x3F;
			int backB = back & 0x1F;
						
			int outB = (backB * invAlpha + 31 * alpha) >> 16;
			int outG = (backG * invAlpha + 63 * alpha) >> 16;
			int outR = (backR * invAlpha + 31 * alpha) >> 16;
			
			uint16_t out = outB | (outG << 5) | (outR << 11 );

			* (uint16_t*)pDst = out;
			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}




//____ _straight_blit_alpha8_to_bgr565linear_flattint_blend() ____________________________________________________________

static void _straight_blit_alpha8_to_bgr565linear_flattint_blend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{

	uint8_t srcR = HiColor::packLinearTab[tint.flatTintColor.r] >> 3;
	uint8_t srcG = HiColor::packLinearTab[tint.flatTintColor.g] >> 2;
	uint8_t srcB = HiColor::packLinearTab[tint.flatTintColor.b] >> 3;
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			int alpha = (SoftGfxDevice::s_mulTab[* pSrc] * tint.flatTintColor.a) >> 12;
			int invAlpha = 65536 - alpha;

			uint16_t back = * ((uint16_t*)pDst);
	
			int backR = (back >> 11);
			int backG = (back >> 5) & 0x3F;
			int backB = back & 0x1F;
									
			int outB = (backB * invAlpha + srcB * alpha) >> 16;
			int outG = (backG * invAlpha + srcG * alpha) >> 16;
			int outR = (backR * invAlpha + srcR * alpha) >> 16;
			
			uint16_t out = outB | (outG << 5) | (outR << 11 );

			* (uint16_t*)pDst = out;
			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ _straight_blit_bgrxa8linear_to_bgr565linear_notint_noblend() ____________________________________________________________

static void _straight_blit_bgrxa8linear_to_bgr565linear_notint_noblend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			uint32_t col = *((uint32_t*)pSrc);

			uint16_t out = ((col >> 3) & 0x001F) | ((col >> 5) & 0x07E0) | ((col >> 8 ) & 0xF800);
			* (uint16_t*)pDst = out;

			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ _straight_blit_bgra8linear_to_bgr565linear_notint_blend() ____________________________________________________________

static void _straight_blit_bgra8linear_to_bgr565linear_notint_blend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			Color8 col = * (Color8*) pSrc;

			int alpha = SoftGfxDevice::s_mulTab[col.a];
			int invAlpha = 65536 - alpha;

			uint16_t back = * ((uint16_t*)pDst);
	
			int backR = (back >> 11);
			int backG = (back >> 5) & 0x3F;
			int backB = back & 0x1F;
						
			int outB = (backB * invAlpha + col.b * (alpha>>3)) >> 16;
			int outG = (backG * invAlpha + col.g * (alpha>>2)) >> 16;
			int outR = (backR * invAlpha + col.r * (alpha>>3)) >> 16;
			
			uint16_t out = outB | (outG << 5) | (outR << 11 );

			* (uint16_t*)pDst = out;
			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}


static uint16_t s_fixedBlendCache[256];
static HiColor	s_fixedBlendCacheFgColor = HiColor::Undefined;
static HiColor	s_fixedBlendCacheBgColor = HiColor::Undefined;

//____ updateFixedBlendCache() ___________________________________________

static void updateFixedBlendCache( HiColor bgCol, HiColor fgCol )
{
	s_fixedBlendCacheBgColor = bgCol;
	s_fixedBlendCacheFgColor = bgCol;

	int backB = bgCol.b;
	int backG = bgCol.g;
	int backR = bgCol.r;

	for( int i = 0 ; i < 256 ; i++ )
	{
		int alpha = (fgCol.a * i) / 4096;
		int invAlpha = 255 - alpha;

		int outB = (backB * invAlpha + fgCol.b * alpha) >> 15;
		int outG = (backG * invAlpha + fgCol.g * alpha) >> 15;
		int outR = (backR * invAlpha + fgCol.r * alpha) >> 15;
		
		uint16_t out = outB | (outG << 5) | (outR << 11 );
		s_fixedBlendCache[i] = out;
	}
}



//____ _straight_blit_alpha8_to_bgr565linear_notint_fixedblend() ____________________________________________________________

static void _straight_blit_alpha8_to_bgr565linear_no_or_flat_tint_fixedblend(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	// Setup conversion table

	if( s_fixedBlendCacheBgColor != tint.fixedBlendColor || s_fixedBlendCacheFgColor != tint.flatTintColor )
		updateFixedBlendCache(tint.fixedBlendColor, tint.flatTintColor );
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			* (uint16_t*)pDst = s_fixedBlendCache[* pSrc];
			pSrc += pitches.srcX;
			pDst += pitches.dstX;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}


//____ wg_addExtraSoftKernelsForRGB555BECanvas() __________________________________

#ifdef __cplusplus
extern "C" {
#endif
int	wg_addExtraSoftKernelsForRGB555BECanvas( wg_obj device )
{
	auto pDevice = static_cast<SoftGfxDevice*>(reinterpret_cast<Object*>(device));

	return addExtraSoftKernelsForRGB555BECanvas(pDevice);
}
#ifdef __cplusplus
}
#endif

//____ addExtraSoftKernelsForBGR565Canvas() __________________________________

bool wg::addExtraSoftKernelsForBGR565Canvas( SoftGfxDevice * pDevice )
{
	pDevice->setFillKernel( TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, &_fill_bgr565linear_noblend_notint );

	
	pDevice->setStraightBlitKernel( PixelFormat::BGR_565_linear, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, _straight_blit_bgr565linear_to_same_notint_noblend );

	
	pDevice->setStraightBlitKernel( PixelFormat::Index_8_linear, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, _straight_blit_index8linear_to_bgr565linear_notint_noblend );

	pDevice->setStraightBlitKernel( PixelFormat::Index_8_linear, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Blend, PixelFormat::BGR_565_linear, _straight_blit_index8linear_to_bgr565linear_notint_blend );
	
	
	pDevice->setStraightBlitKernel( PixelFormat::Alpha_8, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Blend, PixelFormat::BGR_565_linear, _straight_blit_alpha8_to_bgr565linear_notint_blend );

	pDevice->setStraightBlitKernel( PixelFormat::Alpha_8, SoftGfxDevice::ReadOp::Normal, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_565_linear, _straight_blit_alpha8_to_bgr565linear_flattint_blend );

	
	pDevice->setStraightBlitKernel( PixelFormat::Alpha_8, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::BlendFixedColor, PixelFormat::BGR_565_linear, _straight_blit_alpha8_to_bgr565linear_no_or_flat_tint_fixedblend );

	pDevice->setStraightBlitKernel( PixelFormat::Alpha_8, SoftGfxDevice::ReadOp::Normal, TintMode::Flat, BlendMode::BlendFixedColor, PixelFormat::BGR_565_linear, _straight_blit_alpha8_to_bgr565linear_no_or_flat_tint_fixedblend );

	
	pDevice->setStraightBlitKernel( PixelFormat::BGRA_8_linear, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, _straight_blit_bgrxa8linear_to_bgr565linear_notint_noblend );

	pDevice->setStraightBlitKernel( PixelFormat::BGRX_8_linear, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, _straight_blit_bgrxa8linear_to_bgr565linear_notint_noblend );
	
	pDevice->setStraightBlitKernel( PixelFormat::BGRA_8_linear, SoftGfxDevice::ReadOp::Normal, TintMode::None, BlendMode::Blend, PixelFormat::BGR_565_linear, _straight_blit_bgra8linear_to_bgr565linear_notint_blend );
	
	return true;
};

