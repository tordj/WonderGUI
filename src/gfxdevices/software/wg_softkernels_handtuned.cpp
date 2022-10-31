#include <wg_softgfxdevice.h>
#include <wg_base.h>
#include <wg_context.h>

using namespace wg;

//____ _fill_rgb565bigendian_noblend_notint() ____________________________________________________________

void _fill_rgb565bigendian_noblend_notint(uint8_t* pDst, int pitchX, int pitchY, int nLines, int lineLength, HiColor col, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	// Generate our 32-bit double-pixel.
	
	const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? HiColor::packSRGBTab : HiColor::packLinearTab;

	int16_t b = pPackTab[col.b];
	int16_t g = pPackTab[col.g];
	int16_t r = pPackTab[col.r];
	
	uint8_t lowerByte = (b & 0xF8) | (g >> 5);
	uint8_t higherByte = ((g & 0x1C) << 3) | (r >> 3);
	
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

//____ _straight_blit_rgb565bigendian_to_same_notint_noblend_notransform() ____________________________________________________________

void _straight_blit_rgb565bigendian_to_same_notint_noblend_notransform(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
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
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		if( bStartSingle )
		{
			* ((uint16_t*)pDst) = * ((uint16_t*) pSrc);
			pSrc += 2;
			pDst += 2;
		}

		for (int x = 0; x < loops; x++)
		{
			* ((uint32_t*)pDst) = * ((uint32_t*)pSrc);
			pSrc += 4;
			pDst += 4;
		}
		if( bEndSingle )
		{
			* ((uint16_t*)pDst) = * ((uint16_t*) pSrc);
			pSrc += 2;
			pDst += 2;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ _straight_blit_index8_to_rgb565bigendian_notint_noblend_notransform() ____________________________________________________________

void _straight_blit_index8_to_rgb565bigendian_notint_noblend_notransform(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{

	// Calculate number of loops and if we need extra start/end writes
	
	auto pPalette = pSrcSurf->clut();
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			uint32_t col = ((uint32_t*)pPalette)[*pSrc++];

			uint16_t out = ((col >> 19) & 0x1F) | ((col >> 5) & 0x7E0) | ((col & 0xF8) << 8);
			out = (out >> 8 | out << 8);
			* (uint16_t*)pDst = out;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ _straight_blit_index8_to_rgb565bigendian_notint_blend_notransform() ____________________________________________________________

void _straight_blit_index8_to_rgb565bigendian_notint_blend_notransform(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{

	// Calculate number of loops and if we need extra start/end writes
	
	auto pPalette = pSrcSurf->clut();
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			Color8 col = pPalette[*pSrc++];

			int alpha = col.a * 65536 / 255;
			int invAlpha = 65536 - alpha;

			uint16_t back = * ((uint16_t*)pDst);
	
			back = (back >> 8 | back << 8);
			int backR = back & 0x1F;
			int backG = (back >> 5) & 0x3F;
			int backB = (back >> 11) & 0x3F;
						
			int outB = (backB * invAlpha + col.b * alpha) >> 16;
			int outG = (backG * invAlpha + col.g * alpha) >> 16;
			int outR = (backR * invAlpha + col.r * alpha) >> 16;
			
			uint16_t out = outR | (outG << 5) | (outB << 11 );
			out = (out >> 8 | out << 8);

			* (uint16_t*)pDst = out;
			pDst += 2;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}



//____ _straight_blit_alpha8_to_rgb565bigendian_notint_blend_notransform() ____________________________________________________________

void _straight_blit_alpha8_to_rgb565bigendian_notint_blend_notransform(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			uint8_t a = * pSrc++;
			int alpha = a * 65536 / 255;
			int invAlpha = 65536 - alpha;

			uint16_t back = * ((uint16_t*)pDst);
	
			back = (back >> 8 | back << 8);
			int backR = back & 0x1F;
			int backG = (back >> 5) & 0x3F;
			int backB = (back >> 11) & 0x3F;
						
			int outB = (backB * invAlpha + 31 * alpha) >> 16;
			int outG = (backG * invAlpha + 63 * alpha) >> 16;
			int outR = (backR * invAlpha + 31 * alpha) >> 16;
			
			uint16_t out = outR | (outG << 5) | (outB << 11 );
			out = (out >> 8 | out << 8);

			* (uint16_t*)pDst = out;
			pDst += 2;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ _straight_blit_alpha8_to_rgb565bigendian_tint_blend_notransform() ____________________________________________________________

void _straight_blit_alpha8_to_rgb565bigendian_tint_blend_notransform(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{

	uint8_t srcR = HiColor::packLinearTab[tint.flatTintColor.r] >> 3;
	uint8_t srcG = HiColor::packLinearTab[tint.flatTintColor.g] >> 2;
	uint8_t srcB = HiColor::packLinearTab[tint.flatTintColor.b] >> 3;

	uint16_t tintA = tint.flatTintColor.a * 16;
	
	// Copy loop
	
	for (int y = 0; y < nLines; y++)
	{
		for (int x = 0; x < lineLength; x++)
		{
			uint8_t a = * pSrc++;
			int alpha = a * tintA / 255;
			int invAlpha = 65536 - alpha;

			
			uint16_t back = * ((uint16_t*)pDst);
	
			back = (back >> 8 | back << 8);
			int backR = back & 0x1F;
			int backG = (back >> 5) & 0x3F;
			int backB = (back >> 11) & 0x3F;
						
			
			int outB = (backB * invAlpha + srcB * alpha) >> 16;
			int outG = (backG * invAlpha + srcG * alpha) >> 16;
			int outR = (backR * invAlpha + srcR * alpha) >> 16;
			
			uint16_t out = outR | (outG << 5) | (outB << 11 );
			out = (out >> 8 | out << 8);

			* (uint16_t*)pDst = out;
			pDst += 2;
		}

		pSrc += pitches.srcY;
		pDst += pitches.dstY;
	}
}

//____ addHandtunedSoftGfxKernels() __________________________________

void addHandtunedSoftGfxKernels( SoftGfxDevice * pDevice )
{
	pDevice->setFillKernel( TintMode::None, BlendMode::Replace, PixelFormat::RGB_565_bigendian, &_fill_rgb565bigendian_noblend_notint );
	
	pDevice->setStraightBlitKernel( PixelFormat::RGB_565_bigendian, SoftGfxDevice::EdgeOp::None, TintMode::None, BlendMode::Replace, PixelFormat::RGB_565_bigendian, _straight_blit_rgb565bigendian_to_same_notint_noblend_notransform );
	
	pDevice->setStraightBlitKernel( PixelFormat::CLUT_8_linear, SoftGfxDevice::EdgeOp::None, TintMode::None, BlendMode::Replace, PixelFormat::RGB_565_bigendian, _straight_blit_index8_to_rgb565bigendian_notint_noblend_notransform );

	pDevice->setStraightBlitKernel( PixelFormat::CLUT_8_linear, SoftGfxDevice::EdgeOp::None, TintMode::None, BlendMode::Blend, PixelFormat::RGB_565_bigendian, _straight_blit_index8_to_rgb565bigendian_notint_blend_notransform );

	pDevice->setStraightBlitKernel( PixelFormat::A_8, SoftGfxDevice::EdgeOp::None, TintMode::None, BlendMode::Blend, PixelFormat::RGB_565_bigendian, _straight_blit_alpha8_to_rgb565bigendian_notint_blend_notransform );

	pDevice->setStraightBlitKernel( PixelFormat::A_8, SoftGfxDevice::EdgeOp::None, TintMode::Flat, BlendMode::Blend, PixelFormat::RGB_565_bigendian, _straight_blit_alpha8_to_rgb565bigendian_tint_blend_notransform );
};

