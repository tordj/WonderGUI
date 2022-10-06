#include <wg_softgfxdevice.h>
#include <wg_base.h>
#include <wg_context.h>

namespace wg
{

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

	//____ _populateOpTabWithHandTunedKernels() __________________________________

	void SoftGfxDevice::_populateOpTabWithHandTunedKernels()
	{
		s_fillOpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = &_fill_rgb565bigendian_noblend_notint;
	}

};
