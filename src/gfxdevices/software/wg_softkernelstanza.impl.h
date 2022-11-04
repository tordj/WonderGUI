
#include <wg_softgfxdevice.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_context.h>

using namespace wg;
using namespace wg::Util;


	
//____ read_pixel_fast8() _______________________________________________________

inline void _read_pixel_fast8(const uint8_t* pPixel, PixelFormat format, const Color8* pClut, const HiColor* pClut4096, int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA)
{
	if (format == PixelFormat::Undefined)
	{
		const int16_t* p = (const int16_t*)pPixel;

		outB = HiColor::packLinearTab[p[0]];
		outG = HiColor::packLinearTab[p[1]];
		outR = HiColor::packLinearTab[p[2]];
		outA = HiColor::packLinearTab[p[3]];
	}

	if (format == PixelFormat::BGRA_8_linear || format == PixelFormat::BGRA_8_sRGB)
	{
		outB = pPixel[0];
		outG = pPixel[1];
		outR = pPixel[2];
		outA = pPixel[3];
	}

	if (format == PixelFormat::BGR_8_linear || format == PixelFormat::BGR_8_sRGB || format == PixelFormat::BGRX_8_linear || format == PixelFormat::BGRX_8_sRGB)
	{
		outB = pPixel[0];
		outG = pPixel[1];
		outR = pPixel[2];
		outA = 255;
	}

	if (format == PixelFormat::BGR_565_linear)
	{
		uint16_t pixel = *(uint16_t*)pPixel;
#if WG_IS_BIG_ENDIAN
		pixel = Util::endianSwap(pixel);
#endif
		outB = SoftGfxDevice::s_fast8_channel_5[pixel & 0x1F];
		outG = SoftGfxDevice::s_fast8_channel_6[(pixel >> 5) & 0x3F];
		outR = SoftGfxDevice::s_fast8_channel_5[pixel >> 11];
		outA = 255;
	}

	if (format == PixelFormat::RGB_565_bigendian)
	{
		uint16_t pixel = *(uint16_t*)pPixel;
#if WG_IS_LITTLE_ENDIAN
		pixel = Util::endianSwap(pixel);
#endif
		outR = SoftGfxDevice::s_fast8_channel_5[pixel & 0x1F];
		outG = SoftGfxDevice::s_fast8_channel_6[(pixel >> 5) & 0x3F];
		outB = SoftGfxDevice::s_fast8_channel_5[pixel >> 11];
		outA = 255;
	}


	if (format == PixelFormat::BGRA_4_linear)
	{
		outB = SoftGfxDevice::s_fast8_channel_4_1[pPixel[0]];
		outG = SoftGfxDevice::s_fast8_channel_4_2[pPixel[0]];
		outR = SoftGfxDevice::s_fast8_channel_4_1[pPixel[1]];
		outA = SoftGfxDevice::s_fast8_channel_4_2[pPixel[1]];
	}

	if (format == PixelFormat::A_8)
	{
		outB = 255;
		outG = 255;
		outR = 255;
		outA = *pPixel;
	}

	if (format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear)
	{
		Color8 c = pClut[*pPixel];
		outB = c.b;
		outG = c.g;
		outR = c.r;
		outA = c.a;
	}
}


//____ read_pixel() _______________________________________________________

inline void _read_pixel(const uint8_t* pPixel, PixelFormat format, const Color8* pClut, const HiColor* pClut4096, int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA)
{
	if (format == PixelFormat::Undefined)
	{
		const int16_t* p = (const int16_t*)pPixel;

		outB = p[0];
		outG = p[1];
		outR = p[2];
		outA = p[3];
	}

	if (format == PixelFormat::BGRA_8_linear)
	{
		outB = HiColor::unpackLinearTab[pPixel[0]];
		outG = HiColor::unpackLinearTab[pPixel[1]];
		outR = HiColor::unpackLinearTab[pPixel[2]];
		outA = HiColor::unpackLinearTab[pPixel[3]];
	}

	if (format == PixelFormat::BGRA_8_sRGB)
	{
		outB = HiColor::unpackSRGBTab[pPixel[0]];
		outG = HiColor::unpackSRGBTab[pPixel[1]];
		outR = HiColor::unpackSRGBTab[pPixel[2]];
		outA = HiColor::unpackLinearTab[pPixel[3]];
	}

	if (format == PixelFormat::BGR_8_linear || format == PixelFormat::BGRX_8_linear )
	{
		outB = HiColor::unpackLinearTab[pPixel[0]];
		outG = HiColor::unpackLinearTab[pPixel[1]];
		outR = HiColor::unpackLinearTab[pPixel[2]];
		outA = 4096;
	}

	if (format == PixelFormat::BGR_8_sRGB || format == PixelFormat::BGRX_8_sRGB)
	{
		outB = HiColor::unpackSRGBTab[pPixel[0]];
		outG = HiColor::unpackSRGBTab[pPixel[1]];
		outR = HiColor::unpackSRGBTab[pPixel[2]];
		outA = 4096;
	}

	if (format == PixelFormat::BGR_565_linear)
	{
		uint16_t pixel = *(uint16_t*)pPixel;
#if WG_IS_BIG_ENDIAN
		pixel = Util::endianSwap(pixel);
#endif
		outB = SoftGfxDevice::s_channel_5[pixel & 0x1F];
		outG = SoftGfxDevice::s_channel_6[(pixel >> 5) & 0x3F];
		outR = SoftGfxDevice::s_channel_5[pixel >> 11];
		outA = 4096;
	}

	if (format == PixelFormat::RGB_565_bigendian)
	{
		uint16_t pixel = *(uint16_t*)pPixel;
#if WG_IS_LITTLE_ENDIAN
		pixel = Util::endianSwap(pixel);
#endif
		outR = SoftGfxDevice::s_channel_5[pixel & 0x1F];
		outG = SoftGfxDevice::s_channel_6[(pixel >> 5) & 0x3F];
		outB = SoftGfxDevice::s_channel_5[pixel >> 11];
		outA = 4096;
	}

	if (format == PixelFormat::BGRA_4_linear)
	{
		outB = SoftGfxDevice::s_channel_4_1[pPixel[0]];
		outG = SoftGfxDevice::s_channel_4_2[pPixel[0]];
		outR = SoftGfxDevice::s_channel_4_1[pPixel[1]];
		outA = SoftGfxDevice::s_channel_4_2[pPixel[1]];
	}

	if (format == PixelFormat::A_8)
	{
		outB = 4096;
		outG = 4096;
		outR = 4096;
		outA = HiColor::unpackLinearTab[*pPixel];
	}

	if (format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear)
	{
		const HiColor* p = &pClut4096[(*pPixel)];
		outB = p->b;
		outG = p->g;
		outR = p->r;
		outA = p->a;
	}
}

//____ write_pixel_fast8() _______________________________________________________

inline void _write_pixel_fast8(uint8_t* pPixel, PixelFormat format, int16_t b, int16_t g, int16_t r, int16_t a)
{
	//TODO: support BigEndian systems!

	if (format == PixelFormat::Undefined)
	{
		int16_t* p = (int16_t*)pPixel;

		p[0] = HiColor::unpackLinearTab[b];
		p[1] = HiColor::unpackLinearTab[g];
		p[2] = HiColor::unpackLinearTab[r];
		p[3] = HiColor::unpackLinearTab[a];
	}

	if (format == PixelFormat::BGRA_8_linear || format == PixelFormat::BGRA_8_sRGB)
	{
		pPixel[0] = (uint8_t)b;
		pPixel[1] = (uint8_t)g;
		pPixel[2] = (uint8_t)r;
		pPixel[3] = (uint8_t)a;
	}

	if (format == PixelFormat::BGR_8_linear || format == PixelFormat::BGR_8_sRGB || format == PixelFormat::BGRX_8_linear || format == PixelFormat::BGRX_8_sRGB)
	{
		pPixel[0] = (uint8_t)b;
		pPixel[1] = (uint8_t)g;
		pPixel[2] = (uint8_t)r;
	}

	if (format == PixelFormat::BGR_565_linear)
	{
		pPixel[0] = (b >> 3) | ((g & 0xFC) << 3);
		pPixel[1] = (g >> 5) | (r & 0xF8);

	}

	if (format == PixelFormat::RGB_565_bigendian)
	{
		pPixel[0] = (b & 0xF8) | (g >> 5);
		pPixel[1] = ((g & 0x1C) << 3) | (r >> 3);
	}



	if (format == PixelFormat::BGRA_4_linear)
	{
		pPixel[0] = (b >> 4) | (g & 0xF0);
		pPixel[1] = (r >> 4) | (a & 0xF0);
	}

	if (format == PixelFormat::A_8)
	{
		pPixel[0] = (uint8_t)a;
	}

}


//____ write_pixel() _______________________________________________________

inline void _write_pixel(uint8_t* pPixel, PixelFormat format, int16_t b, int16_t g, int16_t r, int16_t a)
{
	//TODO: support BigEndian systems!

	if (format == PixelFormat::Undefined)
	{
		int16_t* p = (int16_t*)pPixel;

		p[0] = b;
		p[1] = g;
		p[2] = r;
		p[3] = a;
	}

	if (format == PixelFormat::BGRA_8_linear)
	{
		pPixel[0] = HiColor::packLinearTab[b];
		pPixel[1] = HiColor::packLinearTab[g];
		pPixel[2] = HiColor::packLinearTab[r];
		pPixel[3] = HiColor::packLinearTab[a];
	}

	if (format == PixelFormat::BGRA_8_sRGB)
	{
		pPixel[0] = HiColor::packSRGBTab[b];
		pPixel[1] = HiColor::packSRGBTab[g];
		pPixel[2] = HiColor::packSRGBTab[r];
		pPixel[3] = HiColor::packLinearTab[a];
	}

	if (format == PixelFormat::BGR_8_linear || format == PixelFormat::BGRX_8_linear)
	{
		pPixel[0] = HiColor::packLinearTab[b];
		pPixel[1] = HiColor::packLinearTab[g];
		pPixel[2] = HiColor::packLinearTab[r];
	}

	if (format == PixelFormat::BGR_8_sRGB || format == PixelFormat::BGRX_8_sRGB)
	{
		pPixel[0] = HiColor::packSRGBTab[b];
		pPixel[1] = HiColor::packSRGBTab[g];
		pPixel[2] = HiColor::packSRGBTab[r];
	}

	if (format == PixelFormat::BGR_565_linear)
	{
		pPixel[0] = (HiColor::packLinearTab[b] >> 3) | ((HiColor::packLinearTab[g] & 0xFC) << 3);
		pPixel[1] = (HiColor::packLinearTab[g] >> 5) | (HiColor::packLinearTab[r] & 0xF8);
	}

	if (format == PixelFormat::RGB_565_bigendian)
	{
		pPixel[0] = (HiColor::packLinearTab[b] & 0xF8) | (HiColor::packLinearTab[g] >> 5);
		pPixel[1] = ((HiColor::packLinearTab[g] & 0x1C) << 3) | (HiColor::packLinearTab[r] >> 3);
	}

	if (format == PixelFormat::BGRA_4_linear)
	{
		pPixel[0] = (HiColor::packLinearTab[b] >> 4) | (HiColor::packLinearTab[g] & 0xF0);
		pPixel[1] = (HiColor::packLinearTab[r] >> 4) | (HiColor::packLinearTab[a] & 0xF0);
	}

	if (format == PixelFormat::A_8)
	{
		pPixel[0] = HiColor::packLinearTab[a];
	}
}

//____ _blend_pixels_fast8() ____________________________________________________

inline void	_blend_pixels_fast8(BlendMode mode, int morphFactor, PixelFormat destFormat,
	int16_t srcB, int16_t srcG, int16_t srcR, int16_t srcA,
	int16_t backB, int16_t backG, int16_t backR, int16_t backA,
	int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA)
{
	// Note: We use if-statements instead of switch/case here since some compilers
	// won't fully eliminate the switch/case statements when this code is inlined
	// into templates where only one BlendMode is possible. With if-statements,
	// the statements are properly detected as never occuring and completely removed.

	if (destFormat == PixelFormat::A_8)
	{
		// Alpha only destination. We treat alpha differently when there is no other channel.

		if (mode == BlendMode::Replace)
			outA = srcA;

		if (mode == BlendMode::Morph)
		{
			int invMorph = 4096 - morphFactor;
			outA = (backA * invMorph + srcA * morphFactor) >> 12;
		}

		if (mode == BlendMode::Blend)
		{
			int alpha = SoftGfxDevice::s_mulTab[srcA];
			int invAlpha = 65536 - alpha;

			outA = (backA * invAlpha + 255 * alpha) >> 16;
		}

		if (mode == BlendMode::Add)
			outA = limitUint8(backA + srcA);

		if (mode == BlendMode::Subtract)
			outA = limitUint8(backA - srcA);

		if (mode == BlendMode::Multiply)
			outA = (SoftGfxDevice::s_mulTab[backA] * srcA) >> 16;

		if (mode == BlendMode::Min)
			outA = srcA - limitUint8(srcA - backA);

		if (mode == BlendMode::Max)
			outA = backA + limitUint8(srcA - backA);

		if (mode == BlendMode::Invert)
		{
			int srcA2 = SoftGfxDevice::s_mulTab[srcA];
			outA = (srcA2 * (255 - backA) + backA * (65536 - srcA2)) >> 16;
		}
	}
	else
	{
		if (mode == BlendMode::Replace)
		{
			outB = srcB;
			outG = srcG;
			outR = srcR;
			outA = srcA;
		}

		if (mode == BlendMode::Morph)
		{
			int invMorph = 4096 - morphFactor;

			outB = (backB * invMorph + srcB * morphFactor) >> 12;
			outG = (backG * invMorph + srcG * morphFactor) >> 12;
			outR = (backR * invMorph + srcR * morphFactor) >> 12;
			outA = (backA * invMorph + srcA * morphFactor) >> 12;
		}

		if (mode == BlendMode::Blend)
		{
			int alpha = SoftGfxDevice::s_mulTab[srcA];
			int invAlpha = 65536 - alpha;

			outB = (backB * invAlpha + srcB * alpha) >> 16;
			outG = (backG * invAlpha + srcG * alpha) >> 16;
			outR = (backR * invAlpha + srcR * alpha) >> 16;
			outA = (backA * invAlpha + 255 * alpha) >> 16;
		}

		if (mode == BlendMode::Add)
		{
			int alpha = SoftGfxDevice::s_mulTab[srcA];

			outB = limitUint8(backB + (srcB * alpha >> 16));
			outG = limitUint8(backG + (srcG * alpha >> 16));
			outR = limitUint8(backR + (srcR * alpha >> 16));
			outA = backA;
		}

		if (mode == BlendMode::Subtract)
		{
			int alpha = SoftGfxDevice::s_mulTab[srcA];

			outB = limitUint8(backB - (srcB * alpha >> 16));
			outG = limitUint8(backG - (srcG * alpha >> 16));
			outR = limitUint8(backR - (srcR * alpha >> 16));
			outA = backA;
		}

		if (mode == BlendMode::Multiply)
		{
			outB = (SoftGfxDevice::s_mulTab[backB] * srcB) >> 16;
			outG = (SoftGfxDevice::s_mulTab[backG] * srcG) >> 16;
			outR = (SoftGfxDevice::s_mulTab[backR] * srcR) >> 16;
			outA = backA;
		}

		if (mode == BlendMode::Min)
		{
			outB = srcB - limitUint8(srcB - backB);
			outG = srcG - limitUint8(srcG - backG);
			outR = srcR - limitUint8(srcR - backR);
			outA = backA;
		}

		if (mode == BlendMode::Max)
		{
			outB = backB + limitUint8(srcB - backB);
			outG = backG + limitUint8(srcG - backG);
			outR = backR + limitUint8(srcR - backR);
			outA = backA;
		}

		if (mode == BlendMode::Invert)
		{
			int srcB2 = SoftGfxDevice::s_mulTab[srcB];
			int srcG2 = SoftGfxDevice::s_mulTab[srcG];
			int srcR2 = SoftGfxDevice::s_mulTab[srcR];

			outB = (srcB2 * (255 - backB) + backB * (65536 - srcB2)) >> 16;
			outG = (srcG2 * (255 - backG) + backG * (65536 - srcG2)) >> 16;
			outR = (srcR2 * (255 - backR) + backR * (65536 - srcR2)) >> 16;
			outA = backA;
		}
	}
}


//____ _blend_pixels() ____________________________________________________

inline void	_blend_pixels(BlendMode mode, int morphFactor, PixelFormat destFormat,
	int16_t srcB, int16_t srcG, int16_t srcR, int16_t srcA,
	int16_t backB, int16_t backG, int16_t backR, int16_t backA,
	int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA)
{
	// Note: We use if-statements instead of switch/case here since some compilers
	// won't fully eliminate the switch/case statements when this code is inlined
	// into templates where only one BlendMode is possible. With if-statements,
	// the statements are properly detected as never occuring and completely removed.

	if (destFormat == PixelFormat::A_8)
	{
		// Alpha only destination. We treat alpha differently when there is no other channel.

		if (mode == BlendMode::Replace)
			outA = srcA;

		if (mode == BlendMode::Morph)
		{
			int invMorph = 4096 - morphFactor;
			outA = (backA * invMorph + srcA * morphFactor) >> 12;
		}

		if (mode == BlendMode::Blend)
		{
			int alpha = srcA;
			int invAlpha = 4096 - alpha;

			outA = (backA * invAlpha + 4096 * alpha) >> 12;
		}

		if (mode == BlendMode::Add)
			outA = SoftGfxDevice::s_limit4096Tab[4097 + backA + srcA];

		if (mode == BlendMode::Subtract)
			outA = SoftGfxDevice::s_limit4096Tab[4097 + backA - srcA];

		if (mode == BlendMode::Multiply)
			outA = (backA * srcA) >> 12;

		if (mode == BlendMode::Min)
			outA = std::min(backA, srcA);

		if (mode == BlendMode::Max)
			outA = std::max(backA, srcA);

		if (mode == BlendMode::Invert)
			outA = (srcA * (4096 - backA) + backA * (4096 - srcA)) >> 12;

	}
	else
	{
		if (mode == BlendMode::Replace)
		{
			outB = srcB;
			outG = srcG;
			outR = srcR;
			outA = srcA;
		}

		if (mode == BlendMode::Morph)
		{
			int invMorph = 4096 - morphFactor;

			outB = (backB * invMorph + srcB * morphFactor) >> 12;
			outG = (backG * invMorph + srcG * morphFactor) >> 12;
			outR = (backR * invMorph + srcR * morphFactor) >> 12;
			outA = (backA * invMorph + srcA * morphFactor) >> 12;
		}

		if (mode == BlendMode::Blend)
		{
			int alpha = srcA;
			int invAlpha = 4096 - alpha;

			outB = (backB * invAlpha + srcB * alpha) >> 12;
			outG = (backG * invAlpha + srcG * alpha) >> 12;
			outR = (backR * invAlpha + srcR * alpha) >> 12;
			outA = (backA * invAlpha + 4096 * alpha) >> 12;
		}

		if (mode == BlendMode::Add)
		{
			int alpha = srcA;

			outB = SoftGfxDevice::s_limit4096Tab[4097 + backB + (srcB * alpha >> 12)];
			outG = SoftGfxDevice::s_limit4096Tab[4097 + backG + (srcG * alpha >> 12)];
			outR = SoftGfxDevice::s_limit4096Tab[4097 + backR + (srcR * alpha >> 12)];
			outA = backA;
		}

		if (mode == BlendMode::Subtract)
		{
			int alpha = srcA;

			outB = SoftGfxDevice::s_limit4096Tab[4097 + backB - (srcB * alpha >> 12)];
			outG = SoftGfxDevice::s_limit4096Tab[4097 + backG - (srcG * alpha >> 12)];
			outR = SoftGfxDevice::s_limit4096Tab[4097 + backR - (srcR * alpha >> 12)];
			outA = backA;
		}

		if (mode == BlendMode::Multiply)
		{
			outB = (backB * srcB) >> 12;
			outG = (backG * srcG) >> 12;
			outR = (backR * srcR) >> 12;
			outA = backA;
		}

		if (mode == BlendMode::Min)
		{
			outB = std::min(backB, srcB);
			outG = std::min(backG, srcG);
			outR = std::min(backR, srcR);
			outA = backA;
		}

		if (mode == BlendMode::Max)
		{
			outB = std::max(backB, srcB);
			outG = std::max(backG, srcG);
			outR = std::max(backR, srcR);
			outA = backA;
		}

		if (mode == BlendMode::Invert)
		{
			outB = (srcB * (4096 - backB) + backB * (4096 - srcB)) >> 12;
			outG = (srcG * (4096 - backG) + backG * (4096 - srcG)) >> 12;
			outR = (srcR * (4096 - backR) + backR * (4096 - srcR)) >> 12;
			outA = backA;
		}
	}
}


//____ _color_tint_init() _________________________________________________

inline void _color_tint_init(TintMode tintMode, const SoftGfxDevice::ColTrans& tint, int bits, int16_t inB, int16_t inG, int16_t inR, int16_t inA,
	int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA,
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

		outR = (inR * tint.flatTintColor.r) >> 12;
		outG = (inG * tint.flatTintColor.g) >> 12;
		outB = (inB * tint.flatTintColor.b) >> 12;
		outA = (inA * tint.flatTintColor.a) >> 12;
	}

	if (tintMode == TintMode::None)
	{
		outB = inB;
		outG = inG;
		outR = inR;
		outA = inA;
	}

	if (tintMode == TintMode::GradientX || tintMode == TintMode::GradientY || tintMode == TintMode::GradientXY)
	{
		uint32_t topLeftB = (inB * tint.topLeftB) >> bits;
		uint32_t topLeftG = (inG * tint.topLeftG) >> bits;
		uint32_t topLeftR = (inR * tint.topLeftR) >> bits;
		uint32_t topLeftA = (inA * tint.topLeftA) >> bits;

		uint32_t topRightB = (inB * tint.topRightB) >> bits;
		uint32_t topRightG = (inG * tint.topRightG) >> bits;
		uint32_t topRightR = (inR * tint.topRightR) >> bits;
		uint32_t topRightA = (inA * tint.topRightA) >> bits;

		uint32_t bottomLeftB = (inB * (tint.topLeftB + tint.leftIncB * tint.tintRect.h)) >> bits;
		uint32_t bottomLeftG = (inG * (tint.topLeftG + tint.leftIncG * tint.tintRect.h)) >> bits;
		uint32_t bottomLeftR = (inR * (tint.topLeftR + tint.leftIncR * tint.tintRect.h)) >> bits;
		uint32_t bottomLeftA = (inA * (tint.topLeftA + tint.leftIncA * tint.tintRect.h)) >> bits;

		uint32_t bottomRightB = (inB * (tint.topRightB + tint.rightIncB * tint.tintRect.h)) >> bits;
		uint32_t bottomRightG = (inG * (tint.topRightG + tint.rightIncG * tint.tintRect.h)) >> bits;
		uint32_t bottomRightR = (inR * (tint.topRightR + tint.rightIncR * tint.tintRect.h)) >> bits;
		uint32_t bottomRightA = (inA * (tint.topRightA + tint.rightIncA * tint.tintRect.h)) >> bits;

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

inline void _color_tint_line(TintMode tintMode, const SoftGfxDevice::ColTrans& tint, int bits, int16_t inB, int16_t inG, int16_t inR, int16_t inA,
	int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA,
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
		outB = leftB >> (18 - bits);
		outG = leftG >> (18 - bits);
		outR = leftR >> (18 - bits);
		outA = leftA >> (18 - bits);

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

inline void _color_tint_pixel(TintMode tintMode, int bits,
	int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA,
	uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
	uint32_t& pixelB, uint32_t& pixelG, uint32_t& pixelR, uint32_t& pixelA)
{
	if (tintMode == TintMode::GradientX || tintMode == TintMode::GradientXY)
	{
		outB = pixelB >> (18 - bits);
		outG = pixelG >> (18 - bits);
		outR = pixelR >> (18 - bits);
		outA = pixelA >> (18 - bits);

		pixelB += xIncB;
		pixelG += xIncG;
		pixelR += xIncR;
		pixelA += xIncA;

	}
}

//____ _texel_tint_init() _________________________________________________

inline void _texel_tint_init(TintMode tintMode, const SoftGfxDevice::ColTrans& tint, int bits,
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
		tintR = tint.flatTintColor.r << 6;
		tintG = tint.flatTintColor.g << 6;
		tintB = tint.flatTintColor.b << 6;
		tintA = tint.flatTintColor.a << 6;
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

			tintB = topLeftB + tint.leftIncB * patchOfsY;
			tintG = topLeftG + tint.leftIncG * patchOfsY;
			tintR = topLeftR + tint.leftIncR * patchOfsY;
			tintA = topLeftA + tint.leftIncA * patchOfsY;
		}

		if (tintMode == TintMode::GradientXY)
		{
			int patchOfsY = patchPos.y - tint.tintRect.y;

			leftB = topLeftB + patchOfsY * tint.leftIncB;
			leftG = topLeftG + patchOfsY * tint.leftIncG;
			leftR = topLeftR + patchOfsY * tint.leftIncR;
			leftA = topLeftA + patchOfsY * tint.leftIncA;

			rightB = topRightB + patchOfsY * tint.rightIncB;
			rightG = topRightG + patchOfsY * tint.rightIncG;
			rightR = topRightR + patchOfsY * tint.rightIncR;
			rightA = topRightA + patchOfsY * tint.rightIncA;
		}
	}
}

//____ _texel_tint_line() _________________________________________________

inline void _texel_tint_line(TintMode tintMode, const SoftGfxDevice::ColTrans& tint, int bits,
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

inline void _texel_tint_pixel(TintMode tintMode, int bits, int16_t& pixelB, int16_t& pixelG, int16_t& pixelR, int16_t& pixelA,
	uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
	uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA)
{
	if (tintMode == TintMode::None)
	{
	}
	else
	{
		pixelB = (pixelB * tintB) >> 18;
		pixelG = (pixelG * tintG) >> 18;
		pixelR = (pixelR * tintR) >> 18;
		pixelA = (pixelA * tintA) >> 18;

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
void _plot(uint8_t* pDst, HiColor color, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	bool bFast8 = false;

	if (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear)
	{
		bFast8 = true;
	}

	// Step 1: Read source pixels

	int16_t srcB, srcG, srcR, srcA;

	if (bFast8)
	{
		const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? HiColor::packSRGBTab : HiColor::packLinearTab;

		srcB = pPackTab[color.b];
		srcG = pPackTab[color.g];
		srcR = pPackTab[color.r];
		srcA = HiColor::packLinearTab[color.a];
	}
	else
	{
		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;
	}

	// Step 1.5: Apply any tint to source

	if (TINT != TintMode::None)
	{
		srcR = (srcR * tint.flatTintColor.r) >> 12;
		srcG = (srcG * tint.flatTintColor.g) >> 12;
		srcB = (srcB * tint.flatTintColor.b) >> 12;
		srcA = (srcA * tint.flatTintColor.a) >> 12;
	}

	// Step 2: Get color components of background pixel blending into backX
	// Step 3: Blend srcX and backX into outX
	// Step 4: Write resulting pixel to destination

	int16_t backB, backG, backR, backA;
	int16_t outB, outG, outR, outA;

	if (bFast8)
	{
		_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
		_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
		_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
	}
	else
	{
		_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
		_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
		_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
	}
}

//____ plot_list() ________________________________________________________

template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
void _plot_list(const RectI& clip, int nCoords, const CoordI* pCoords, const HiColor* pColors, uint8_t* pCanvas, int pitchX, int pitchY, const SoftGfxDevice::ColTrans& tint)
{
	bool bFast8 = false;

	if (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear)
	{
		bFast8 = true;
	}

	int tintB, tintG, tintR, tintA;

	if (TINT != TintMode::None)
	{
		tintR = tint.flatTintColor.r;
		tintG = tint.flatTintColor.g;
		tintB = tint.flatTintColor.b;
		tintA = tint.flatTintColor.a;
	}

	const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? HiColor::packSRGBTab : HiColor::packLinearTab;

	RectI pixelClip = clip / 64;

	for (int i = 0; i < nCoords; i++)
	{
		const int x = (pCoords[i].x + 32) >> 6;
		const int y = (pCoords[i].y + 32) >> 6;

		if (y >= pixelClip.y && y <= pixelClip.y + pixelClip.h - 1 && x >= pixelClip.x && x <= pixelClip.x + pixelClip.w - 1)
		{
			uint8_t* pDst = pCanvas + y * pitchY + x * pitchX;

			// Step 1: Read source pixels

			int16_t srcB, srcG, srcR, srcA;

			if (bFast8)
			{

				srcB = pPackTab[pColors[i].b];
				srcG = pPackTab[pColors[i].g];
				srcR = pPackTab[pColors[i].r];
				srcA = HiColor::packLinearTab[pColors[i].a];
			}
			else
			{
				srcB = pColors[i].b;
				srcG = pColors[i].g;
				srcR = pColors[i].r;
				srcA = pColors[i].a;
			}

			// Step 1.5: Apply any tint to source

			if (TINT != TintMode::None)
			{
				srcB = (srcB * tintB) >> 12;
				srcG = (srcG * tintG) >> 12;
				srcR = (srcR * tintR) >> 12;
				srcA = (srcA * tintA) >> 12;
			}

			// Step 2: Get color components of background pixel blending into backX

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
		}
	}
}

//____ _draw_line() _______________________________________________________

template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
void _draw_line(uint8_t* pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, HiColor color, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	bool bFast8 = false;

	if (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear)
	{
		bFast8 = true;
	}

	const BlendMode EdgeBlendMode = BLEND == BlendMode::Replace ? BlendMode::Blend : BLEND;

	if (BLEND == BlendMode::Replace)
		color.a = 4096;						// Needed since we still blend the edges.

	// Step 1: Read source pixels

	int16_t srcB, srcG, srcR, srcA;

	if (bFast8)
	{
		const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? HiColor::packSRGBTab : HiColor::packLinearTab;

		srcB = pPackTab[color.b];
		srcG = pPackTab[color.g];
		srcR = pPackTab[color.r];
		srcA = HiColor::packLinearTab[color.a];
	}
	else
	{
		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;
	}


	// Step 1.5: Apply any flatTintColor

	if (TINT != TintMode::None)
	{
		srcR = (srcR * tint.flatTintColor.r) >> 12;
		srcG = (srcG * tint.flatTintColor.g) >> 12;
		srcB = (srcB * tint.flatTintColor.b) >> 12;
		srcA = (srcA * tint.flatTintColor.a) >> 12;
	}

	for (int i = 0; i < length; i++)
	{
		int beg = pos >> 16;
		int end = (pos + width) >> 16;
		int ofs = beg;

		uint8_t* pDst = pRow + ofs * pixelInc;

		if (beg == end)
		{
			// Special case, one pixel wide row

			int alpha = (color.a * width) >> 16;

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
		}
		else
		{
			// First anti-aliased pixel of column

			int alpha = (srcA * (65536 - (pos & 0xFFFF))) >> 16;		// Special AA

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}

			ofs++;
			pDst += pixelInc;

			// All non-antialiased middle pixels of column

			if (ofs < end)
			{
				do
				{
					int16_t backB, backG, backR, backA;
					int16_t outB, outG, outR, outA;

					if (bFast8)
					{
						_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
						_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
						_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
					}
					else
					{
						_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
						_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
						_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
					}
					ofs++;
					pDst += pixelInc;

				} while (ofs < end);
			}

			// Last anti-aliased pixel of column

			int overflow = (pos + width) & 0xFFFF;
			if (overflow > 0)
			{
				int alpha = srcA * overflow >> 16;		// Special AA

				int16_t backB, backG, backR, backA;
				int16_t outB, outG, outR, outA;

				if (bFast8)
				{
					_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
					_blend_pixels_fast8(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
					_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
				else
				{
					_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
					_blend_pixels(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}

			}
		}

		pRow += rowInc;
		pos += slope;
	}
}

//____ _clip_draw_line() __________________________________________________

template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
void _clip_draw_line(int clipStart, int clipEnd, uint8_t* pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, HiColor color, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	bool bFast8 = false;

	const BlendMode EdgeBlendMode = (BLEND == BlendMode::Replace) ? BlendMode::Blend : BLEND;

	if (BLEND == BlendMode::Replace)
		color.a = 4096;						// Needed since we still blend the edges.

	// Step 1: Read source pixels

	int16_t srcB, srcG, srcR, srcA;

	if (bFast8)
	{
		const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? HiColor::packSRGBTab : HiColor::packLinearTab;

		srcB = pPackTab[color.b];
		srcG = pPackTab[color.g];
		srcR = pPackTab[color.r];
		srcA = HiColor::packLinearTab[color.a];
	}
	else
	{
		srcB = color.b;
		srcG = color.g;
		srcR = color.r;
		srcA = color.a;
	}

	// Step 1.5: Apply any flatTintColor

	if (TINT != TintMode::None)
	{
		srcR = (srcR * tint.flatTintColor.r) >> 12;
		srcG = (srcG * tint.flatTintColor.g) >> 12;
		srcB = (srcB * tint.flatTintColor.b) >> 12;
		srcA = (srcA * tint.flatTintColor.a) >> 12;
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

		uint8_t* pDst = pRow + ofs * pixelInc;

		if (beg == end)
		{
			// Special case, one pixel wide row


			//TODO: Width might be more than a pixel, needs to be clipped as well!

			int alpha = (color.a * width) >> 16;

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
		}
		else
		{
			// First anti-aliased pixel of column

			int alpha = (srcA * (65536 - (clippedPos & 0xFFFF))) >> 16;		// Special AA

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}

			ofs++;
			pDst += pixelInc;

			// All non-antialiased middle pixels of column

			if (ofs < end)
			{
				do
				{
					// Step 2: Get color components of background pixel blending into backX
					// Step 3: Blend srcX and backX into outX
					// Step 4: Write resulting pixel to destination

					int16_t backB, backG, backR, backA;
					int16_t outB, outG, outR, outA;

					if (bFast8)
					{
						_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
						_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
						_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
					}
					else
					{
						_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
						_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
						_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
					}

					ofs++;
					pDst += pixelInc;

				} while (ofs < end);
			}

			// Last anti-aliased pixel of column

			int overflow = (clippedPos + clippedWidth) & 0xFFFF;
			if (overflow > 0)
			{
				int alpha = srcA * overflow >> 16;		// Special AA

				int16_t backB, backG, backR, backA;
				int16_t outB, outG, outR, outA;

				if (bFast8)
				{
					_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
					_blend_pixels_fast8(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
					_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
				else
				{
					_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
					_blend_pixels(EdgeBlendMode, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, alpha, backB, backG, backR, backA, outB, outG, outR, outA);
					_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
				}
			}
		}

		pRow += rowInc;
		pos += slope;
	}
}


//____ _fill() ____________________________________________________________

template<TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT>
void _fill(uint8_t* pDst, int pitchX, int pitchY, int nLines, int lineLength, HiColor col, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	bool bFast8 = false;
	int		bits = 12;

	if (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear)
	{
		bFast8 = true;
		bits = 8;
	}

	// Step 1: Read source pixels and prepare tint

	uint32_t	leftB, leftG, leftR, leftA;						// Left side colors when tinting Y
	uint32_t	rightB, rightG, rightR, rightA;					// Right side colors when tinting X
	uint32_t	leftIncB, leftIncG, leftIncR, leftIncA;
	uint32_t	rightIncB, rightIncG, rightIncR, rightIncA;
	uint32_t	xIncB, xIncG, xIncR, xIncA;
	uint32_t	pixelB, pixelG, pixelR, pixelA;

	int16_t srcB, srcG, srcR, srcA;
	int16_t tintedB, tintedG, tintedR, tintedA;

	if (bFast8)
	{
		const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? HiColor::packSRGBTab : HiColor::packLinearTab;

		srcB = pPackTab[col.b];
		srcG = pPackTab[col.g];
		srcR = pPackTab[col.r];
		srcA = HiColor::packLinearTab[col.a];
	}
	else
	{
		srcB = col.b;
		srcG = col.g;
		srcR = col.r;
		srcA = col.a;
	}


	_color_tint_init(TINT, tint, bits, srcB, srcG, srcR, srcA, tintedB, tintedG, tintedR, tintedA,
		leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
		leftIncB, leftIncG, leftIncR, leftIncA, rightIncB, rightIncG, rightIncR, rightIncA,
		xIncB, xIncG, xIncR, xIncA, patchPos);

	for (int y = 0; y < nLines; y++)
	{

		// Step 2: Apply any vertical tint gradient

		_color_tint_line(TINT, tint, bits, srcB, srcG, srcR, srcA, tintedB, tintedG, tintedR, tintedA,
			leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
			leftIncB, leftIncG, leftIncR, leftIncA, rightIncB, rightIncG, rightIncR, rightIncA,
			xIncB, xIncG, xIncR, xIncA, pixelB, pixelG, pixelR, pixelA, patchPos);

		for (int x = 0; x < lineLength; x++)
		{
			// Step 3: Apply any horizontal tint gradient

			_color_tint_pixel(TINT, bits, tintedB, tintedG, tintedR, tintedA, xIncB, xIncG, xIncR, xIncA,
				pixelB, pixelG, pixelR, pixelA);

			// Step 4: Get color components of background pixel blending into backX
			// Step 5: Blend srcX and backX into outX
			// Step 6: Write resulting pixel to destination

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, tintedB, tintedG, tintedR, tintedA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, tintedB, tintedG, tintedR, tintedA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}

			// Step 7: Increment destination pointers

			pDst += pitchX;
		}
		pDst += pitchY;
	}

}

//____ _add_segment_color() _______________________________________________

inline void _add_segment_color(bool GRADIENT, int blendFraction, int offset, const int16_t* pSegmentColor, const SoftGfxDevice::SegmentGradient* pSegmentGradients, int gradientIndex, int& accB, int& accG, int& accR, int& accA)
{
	if (GRADIENT)
	{
		auto pSegmentGradient = &pSegmentGradients[gradientIndex];
		
		accB += (blendFraction >> 4) * ((pSegmentGradient->begB + pSegmentGradient->incB * offset) >> 12);
		accG += (blendFraction >> 4) * ((pSegmentGradient->begG + pSegmentGradient->incG * offset) >> 12);
		accR += (blendFraction >> 4) * ((pSegmentGradient->begR + pSegmentGradient->incR * offset) >> 12);
		accA += blendFraction << 8;
	}
	else
	{
		accR += (blendFraction * pSegmentColor[0]) >> 4;
		accG += (blendFraction * pSegmentColor[1]) >> 4;
		accB += (blendFraction * pSegmentColor[2]) >> 4;
		accA += blendFraction << 8;
	}
}


//____ _draw_segment_strip() _______________________________________________

template<bool GRADIENT, BlendMode BLEND, PixelFormat DSTFORMAT>
void _draw_segment_strip(int colBeg, int colEnd, uint8_t* pStripStart, int pixelPitch, int nEdges, SegmentEdge* pEdges, const int16_t* pSegmentColors, const SoftGfxDevice::SegmentGradient* pSegmentGradients, const bool* pTransparentSegments, const bool* pOpaqueSegments, int morphFactor)
{

	bool bFast8 = false;
	int		bits = 12;

	if (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear)
	{
		bFast8 = true;
		bits = 8;
	}

	// Render the column

	int offset = colBeg;				// 24.8 format, but binals cleared (always pointing at beginning of full pixel).
	uint8_t* pDst = pStripStart + (offset >> 8) * pixelPitch;

	while (offset < colEnd)
	{
		if (nEdges == 0 || offset + 255 < pEdges[0].begin)
		{
			// We are fully inside a segment, no need to take any edge into account.

			int end = nEdges == 0 ? colEnd : pEdges[0].begin;

			if (*pTransparentSegments)									// This test is still valid in GRADIENT mode.
			{
				pDst = pStripStart + (end >> 8) * pixelPitch;
				offset = end & 0xFFFFFF00;												// Just skip segment since it is transparent
			}
			else
			{
				int16_t inB, inG, inR, inA;

				if (GRADIENT == false)
				{
					if (bFast8)
					{
						inR = HiColor::packLinearTab[pSegmentColors[0]];
						inG = HiColor::packLinearTab[pSegmentColors[1]];
						inB = HiColor::packLinearTab[pSegmentColors[2]];
						inA = HiColor::packLinearTab[pSegmentColors[3]];
					}
					else
					{
						inR = pSegmentColors[0];
						inG = pSegmentColors[1];
						inB = pSegmentColors[2];
						inA = pSegmentColors[3];
					}
				}

				if (*pOpaqueSegments)
				{
					while (offset + 255 < end)
					{
						if (GRADIENT)
						{
							if (bFast8)
							{
								inB = HiColor::packLinearTab[((pSegmentGradients->begB + pSegmentGradients->incB * (offset >> 8)) >> 12)];
								inG = HiColor::packLinearTab[((pSegmentGradients->begG + pSegmentGradients->incG * (offset >> 8)) >> 12)];
								inR = HiColor::packLinearTab[((pSegmentGradients->begR + pSegmentGradients->incR * (offset >> 8)) >> 12)];
								inA = HiColor::packLinearTab[((pSegmentGradients->begA + pSegmentGradients->incA * (offset >> 8)) >> 12)];
							}
							else
							{
								inB = int32_t((pSegmentGradients->begB + pSegmentGradients->incB * (offset >> 8)) >> 12);
								inG = int32_t((pSegmentGradients->begG + pSegmentGradients->incG * (offset >> 8)) >> 12);
								inR = int32_t((pSegmentGradients->begR + pSegmentGradients->incR * (offset >> 8)) >> 12);
								inA = int32_t((pSegmentGradients->begA + pSegmentGradients->incA * (offset >> 8)) >> 12);
							}
						}

						if (bFast8)
						{
							_write_pixel_fast8(pDst, DSTFORMAT, inB, inG, inR, inA);
						}
						else
						{
							_write_pixel(pDst, DSTFORMAT, inB, inG, inR, inA);
						}

						pDst += pixelPitch;
						offset += 256;
					}
				}
				else
				{
					while (offset + 255 < end)
					{
						if (GRADIENT)
						{
							if (bFast8)
							{
								inB = HiColor::packLinearTab[((pSegmentGradients->begB + pSegmentGradients->incB * (offset >> 8)) >> 12)];
								inG = HiColor::packLinearTab[((pSegmentGradients->begG + pSegmentGradients->incG * (offset >> 8)) >> 12)];
								inR = HiColor::packLinearTab[((pSegmentGradients->begR + pSegmentGradients->incR * (offset >> 8)) >> 12)];
								inA = HiColor::packLinearTab[((pSegmentGradients->begA + pSegmentGradients->incA * (offset >> 8)) >> 12)];
							}
							else
							{
								inB = int32_t((pSegmentGradients->begB + pSegmentGradients->incB * (offset >> 8)) >> 12);
								inG = int32_t((pSegmentGradients->begG + pSegmentGradients->incG * (offset >> 8)) >> 12);
								inR = int32_t((pSegmentGradients->begR + pSegmentGradients->incR * (offset >> 8)) >> 12);
								inA = int32_t((pSegmentGradients->begA + pSegmentGradients->incA * (offset >> 8)) >> 12);
							}
						}

						int16_t backB, backG, backR, backA;
						int16_t outB, outG, outR, outA;

						if (bFast8)
						{
							_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
							_blend_pixels_fast8(BLEND, morphFactor, DSTFORMAT, inB, inG, inR, inA, backB, backG, backR, backA, outB, outG, outR, outA);
							_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
						}
						else
						{
							_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
							_blend_pixels(BLEND, morphFactor, DSTFORMAT, inB, inG, inR, inA, backB, backG, backR, backA, outB, outG, outR, outA);
							_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
						}

						pDst += pixelPitch;
						offset += 256;
					}

				}
			}
		}
		else
		{
			{
				int edge = 0;

				int	segmentFractions[SoftGfxDevice::maxSegments()];
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

				int16_t backB, backG, backR, backA;

				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);

				int16_t outB = 0, outG = 0, outR = 0, outA = 0;

				int accB = 0;
				int accG = 0;
				int accR = 0;
				int accA = 0;

				if (BLEND == BlendMode::Replace)
				{
					for (int i = 0; i <= edge; i++)
					{
						int blendFraction = segmentFractions[i];
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					outB = accB >> 12;
					outG = accG >> 12;
					outR = accR >> 12;
					outA = accA >> 12;
				}

				if (BLEND == BlendMode::Blend)
				{
					int backFraction = 65536;

					for (int i = 0; i <= edge; i++)
					{
						int alpha = GRADIENT ? (pSegmentGradients[i].begA + pSegmentGradients[i].incA * (offset >> 8)) >> 12 : pSegmentColors[i * 4 + 3];
						int blendFraction = ((segmentFractions[i] * alpha) / 4096);
						backFraction -= blendFraction;
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					outB = (accB >> 12) + ((backB * backFraction) >> 16);
					outG = (accG >> 12) + ((backG * backFraction) >> 16);
					outR = (accR >> 12) + ((backR * backFraction) >> 16);
					outA = (accA >> 12) + ((backA * backFraction) >> 16);	// This should be correct... 							
				}

				if (BLEND == BlendMode::Add)
				{
					for (int i = 0; i <= edge; i++)
					{
						int alpha = GRADIENT ? (pSegmentGradients[i].begA + pSegmentGradients[i].incA * (offset >> 8)) >> 12 : pSegmentColors[i * 4 + 3];
						int blendFraction = ((segmentFractions[i] * alpha) / 4096);
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					outB = SoftGfxDevice::s_limit4096Tab[4097 + backB + (accB >> 12)];
					outG = SoftGfxDevice::s_limit4096Tab[4097 + backG + (accG >> 12)];
					outR = SoftGfxDevice::s_limit4096Tab[4097 + backR + (accR >> 12)];
					outA = backA;
				}

				if (BLEND == BlendMode::Subtract)
				{
					for (int i = 0; i <= edge; i++)
					{
						int alpha = GRADIENT ? (pSegmentGradients[i].begA + pSegmentGradients[i].incA * (offset >> 8)) >> 12 : pSegmentColors[i * 4 + 3];
						int blendFraction = ((segmentFractions[i] * alpha) / 4096);
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					outB = SoftGfxDevice::s_limit4096Tab[4097 + backB - (accB >> 12)];
					outG = SoftGfxDevice::s_limit4096Tab[4097 + backG - (accG >> 12)];
					outR = SoftGfxDevice::s_limit4096Tab[4097 + backR - (accR >> 12)];
					outA = backA;
				}

				if (BLEND == BlendMode::Multiply)
				{
					for (int i = 0; i <= edge; i++)
					{
						int blendFraction = segmentFractions[i];
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					outB = (backB * (accB >> 12)) >> 12;
					outG = (backG * (accG >> 12)) >> 12;
					outR = (backR * (accR >> 12)) >> 12;
					outA = backA;
				}

				if (BLEND == BlendMode::Invert)
				{
					for (int i = 0; i <= edge; i++)
					{
						int blendFraction = segmentFractions[i];
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					int srcB2 = accB >> 12;
					int srcG2 = accG >> 12;
					int srcR2 = accR >> 12;

					outB = (srcB2 * (4096 - backB) + backB * (4096 - srcB2)) >> 12;
					outG = (srcG2 * (4096 - backG) + backG * (4096 - srcG2)) >> 12;
					outR = (srcR2 * (4096 - backR) + backR * (4096 - srcR2)) >> 12;
					outA = backA;
				}

				if (BLEND == BlendMode::Min)
				{
					int backFraction = 65536;

					for (int i = 0; i <= edge; i++)
					{
						int alpha = GRADIENT ? (pSegmentGradients[i].begA + pSegmentGradients[i].incA * (offset >> 8)) >> 12 : pSegmentColors[i * 4 + 3];
						int blendFraction = ((segmentFractions[i] * alpha) / 4096);
						backFraction -= blendFraction;
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					int16_t srcB = (accB >> 12) + ((backB * backFraction) >> 16);
					int16_t srcG = (accG >> 12) + ((backG * backFraction) >> 16);
					int16_t srcR = (accR >> 12) + ((backR * backFraction) >> 16);

					outB = std::min(backB, srcB);
					outG = std::min(backG, srcG);
					outR = std::min(backR, srcR);
				}

				if (BLEND == BlendMode::Max)
				{
					int backFraction = 65536;

					for (int i = 0; i <= edge; i++)
					{
						int alpha = GRADIENT ? (pSegmentGradients[i].begA + pSegmentGradients[i].incA * (offset >> 8)) >> 12 : pSegmentColors[i * 4 + 3];
						int blendFraction = ((segmentFractions[i] * alpha) / 4096);
						backFraction -= blendFraction;
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					int16_t srcB = (accB >> 12) + ((backB * backFraction) >> 16);
					int16_t srcG = (accG >> 12) + ((backG * backFraction) >> 16);
					int16_t srcR = (accR >> 12) + ((backR * backFraction) >> 16);

					outB = std::max(backB, srcB);
					outG = std::max(backG, srcG);
					outR = std::max(backR, srcR);
				}

				if (BLEND == BlendMode::Morph)
				{
					int backFraction = 65536;

					for (int i = 0; i <= edge; i++)
					{
						int alpha = GRADIENT ? (pSegmentGradients[i].begA + pSegmentGradients[i].incA * (offset >> 8)) >> 12 : pSegmentColors[i * 4 + 3];
						int blendFraction = ((segmentFractions[i] * alpha) / 4096);
						backFraction -= blendFraction;
						_add_segment_color(GRADIENT, blendFraction, offset >> 8, &pSegmentColors[i * 4], pSegmentGradients, i, accB, accG, accR, accA);
					}

					int invMorph = 4096 - morphFactor;

					outB = (backB * invMorph + (accB >> 12) * morphFactor) >> 12;
					outG = (backG * invMorph + (accG >> 12) * morphFactor) >> 12;
					outR = (backR * invMorph + (accR >> 12) * morphFactor) >> 12;
					outA = (backA * invMorph + (backFraction >> 4) * morphFactor) >> 12;
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
			pOpaqueSegments++;
			if (GRADIENT)
				pSegmentGradients++;
			else
				pSegmentColors += 4;

		}
	}
}

//_____ _straight_blit() ____________________________________________________________

template<PixelFormat SRCFORMAT, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT, bool TILE>
void _straight_blit(const uint8_t* pSrc, uint8_t* pDst, const SoftSurface* pSrcSurf, const SoftGfxDevice::Pitches& pitches, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2])
{
	bool	bFast8 = false;
	int		bits = 12;

	bool srcIsLinear = (SRCFORMAT == PixelFormat::A_8 || SRCFORMAT == PixelFormat::BGRA_4_linear ||
		SRCFORMAT == PixelFormat::BGRA_8_linear || SRCFORMAT == PixelFormat::BGRX_8_linear ||
		SRCFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		SRCFORMAT == PixelFormat::BGR_8_linear);

	bool dstIsLinear = (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear);

	if ((srcIsLinear && dstIsLinear) || (!srcIsLinear && !dstIsLinear && TINT == TintMode::None && BLEND == BlendMode::Replace))
	{
		if (SRCFORMAT != PixelFormat::Undefined && DSTFORMAT != PixelFormat::Undefined)
		{
			bFast8 = true;
			bits = 8;
		}
	}

	// Preapare tiling

	int srcX, srcY;
	int maskX, maskY;
	int xPitch, yPitch;


	if (TILE)
	{
		xPitch = pSrcSurf->pixelBytes();
		yPitch = pSrcSurf->pitch();

		int ofs = int(pSrc - pSrcSurf->pixels());
		srcY = ofs / yPitch;
		srcX = (ofs % yPitch) / xPitch;
		pSrc = pSrcSurf->pixels();
		maskX = pSrcSurf->tileMaskX();
		maskY = pSrcSurf->tileMaskY();
	}

	const Color8 * pClut = pSrcSurf->clut();
	const HiColor * pClut4096 = pSrcSurf->clut4096();


	// Step 1: Prepare any tint gradient

	uint32_t	tintB, tintG, tintR, tintA;

	uint32_t	leftB, leftG, leftR, leftA;				// Left side colors when tinting Y
	uint32_t	rightB, rightG, rightR, rightA;			// Right side colors when tinting X
	uint32_t	xIncB, xIncG, xIncR, xIncA;

	_texel_tint_init(TINT, tint, bits, leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
		xIncB, xIncG, xIncR, xIncA, tintB, tintG, tintR, tintA, patchPos);

	for (int y = 0; y < nLines; y++)
	{
		// Step 2: Prepare tint for any vertical gradient

		_texel_tint_line(TINT, tint, bits, leftB, leftG, leftR, leftA, rightB, rightG, rightR, rightA,
			xIncB, xIncG, xIncR, xIncA, tintB, tintG, tintR, tintA, patchPos);

		for (int x = 0; x < lineLength; x++)
		{
			// Step 3: Read source pixels

			if (TILE)
			{
				srcX &= maskX;
				srcY &= maskY;
				pSrc = pSrcSurf->pixels() + srcY * yPitch + srcX * xPitch;
			}

			int16_t srcB, srcG, srcR, srcA;
			if (bFast8)
				_read_pixel_fast8(pSrc, SRCFORMAT, pClut, pClut4096, srcB, srcG, srcR, srcA);
			else
				_read_pixel(pSrc, SRCFORMAT, pClut, pClut4096, srcB, srcG, srcR, srcA);

			// Step 3: Apply any tint

			_texel_tint_pixel(TINT, bits, srcB, srcG, srcR, srcA, xIncB, xIncG, xIncR, xIncA,
				tintB, tintG, tintR, tintA);

			// Step 2: Get color components of background pixel blending into backX
			// Step 3: Blend srcX and backX into outX
			// Step 4: Write resulting pixel to destination

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}

			// Step 6: Increment source and destination pointers

			if (TILE)
			{
				srcX += simpleTransform[0][0];
				srcY += simpleTransform[0][1];
			}
			else
				pSrc += pitches.srcX;

			pDst += pitches.dstX;
		}

		if (TILE)
		{
			srcX += simpleTransform[1][0];
			srcY += simpleTransform[1][1];
		}
		else
			pSrc += pitches.srcY;

		pDst += pitches.dstY;
	}
}

//____ _transform_blit __________________________________________

template<PixelFormat SRCFORMAT, SampleMethod SAMPLEMETHOD, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT, SoftGfxDevice::EdgeOp EDGEOP >
void _transform_blit(const SoftSurface* pSrcSurf, BinalCoord pos, const binalInt matrix[2][2], uint8_t* pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos)
{
	bool	bFast8 = false;
	int		bits = 12;

	bool srcIsLinear = (SRCFORMAT == PixelFormat::A_8 || SRCFORMAT == PixelFormat::BGRA_4_linear ||
		SRCFORMAT == PixelFormat::BGRA_8_linear || SRCFORMAT == PixelFormat::BGRX_8_linear ||
		SRCFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		SRCFORMAT == PixelFormat::BGR_8_linear);

	bool dstIsLinear = (DSTFORMAT == PixelFormat::A_8 || DSTFORMAT == PixelFormat::BGRA_4_linear ||
		DSTFORMAT == PixelFormat::BGRA_8_linear || DSTFORMAT == PixelFormat::BGRX_8_linear ||
		DSTFORMAT == PixelFormat::BGR_565_linear || DSTFORMAT == PixelFormat::RGB_565_bigendian ||
		DSTFORMAT == PixelFormat::BGR_8_linear);

	if ((srcIsLinear && dstIsLinear) || (!srcIsLinear && !dstIsLinear && TINT == TintMode::None && BLEND == BlendMode::Replace))
	{
		if (SRCFORMAT != PixelFormat::Undefined && DSTFORMAT != PixelFormat::Undefined)
		{
			bFast8 = true;
			bits = 8;
		}
	}


	int srcPixelBytes = pSrcSurf->pixelBytes();
	int	srcPitch = pSrcSurf->pitch();
	binalInt srcMax_w = pSrcSurf->pixelSize().w * BINAL_MUL;
	binalInt srcMax_h = pSrcSurf->pixelSize().h * BINAL_MUL;

	binalInt pixelIncX = matrix[0][0];
	binalInt pixelIncY = matrix[0][1];

	binalInt lineIncX = matrix[1][0];
	binalInt lineIncY = matrix[1][1];

	binalInt	srcPosMaskX = pSrcSurf->tileMaskX();
	binalInt	srcPosMaskY = pSrcSurf->tileMaskY();

	binalInt	srcPosMaskX_binals = (srcPosMaskX << BINAL_SHIFT) | BINAL_MASK;
	binalInt	srcPosMaskY_binals = (srcPosMaskY << BINAL_SHIFT) | BINAL_MASK;

	const Color8 * pClut = pSrcSurf->clut();
	const HiColor * pClut4096 = pSrcSurf->clut4096();

	
	for (int y = 0; y < nLines; y++)
	{
		binalInt ofsX = (binalInt)(pos.x + lineIncX * y);
		binalInt ofsY = (binalInt)(pos.y + lineIncY * y);		// We use 15 binals for all calculations


		for (int x = 0; x < lineLength; x++)
		{
			if (EDGEOP == SoftGfxDevice::EdgeOp::Tile)
			{
				ofsX &= srcPosMaskX_binals;
				ofsY &= srcPosMaskY_binals;
			}

			// Step 1: Read source color.

			int16_t srcB, srcG, srcR, srcA;
			uint8_t* p = pSrcSurf->pixels() + (ofsY >> BINAL_SHIFT) * srcPitch + (ofsX >> BINAL_SHIFT) * srcPixelBytes;

			if (SAMPLEMETHOD == SampleMethod::Bilinear)
			{
				// Read separate source color components for our 2x2 pixel square.

				int16_t src11_b, src11_g, src11_r, src11_a;
				int16_t src12_b, src12_g, src12_r, src12_a;
				int16_t src21_b, src21_g, src21_r, src21_a;
				int16_t src22_b, src22_g, src22_r, src22_a;

				if (EDGEOP == SoftGfxDevice::EdgeOp::Clip && ((ofsX | ofsY | (srcMax_w - (ofsX + BINAL_MUL + 1)) | (srcMax_h - (ofsY + BINAL_MUL + 1))) < 0))
				{
					if (ofsX > srcMax_w || ofsY > srcMax_h || ofsX < -BINAL_MUL || ofsY < -BINAL_MUL)
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

						if (bFast8)
						{
							if (ofsX >= 0 && ofsY >= 0 && ofsX < srcMax_w && ofsY < srcMax_h)
								_read_pixel_fast8(p, SRCFORMAT, pClut, pClut4096, src11_b, src11_g, src11_r, src11_a);
							else
								src11_b = src11_g = src11_r = src11_a = 0;

							if ((ofsX + BINAL_MUL) >= 0 && ofsY >= 0 && (ofsX + BINAL_MUL) < srcMax_w && ofsY < srcMax_h)
								_read_pixel_fast8(p + srcPixelBytes, SRCFORMAT, pClut, pClut4096, src12_b, src12_g, src12_r, src12_a);
							else
								src12_b = src12_g = src12_r = src12_a = 0;

							if (ofsX >= 0 && (ofsY + BINAL_MUL) >= 0 && ofsX < srcMax_w && (ofsY + BINAL_MUL) < srcMax_h)
								_read_pixel_fast8(p + srcPitch, SRCFORMAT, pClut, pClut4096, src21_b, src21_g, src21_r, src21_a);
							else
								src21_b = src21_g = src21_r = src21_a = 0;

							if ((ofsX + 32768) >= 0 && (ofsY + BINAL_MUL) >= 0 && (ofsX + BINAL_MUL) < srcMax_w && (ofsY + BINAL_MUL) < srcMax_h)
								_read_pixel_fast8(p + srcPitch + srcPixelBytes, SRCFORMAT, pClut, pClut4096, src22_b, src22_g, src22_r, src22_a);
							else
								src22_b = src22_g = src22_r = src22_a = 0;
						}
						else
						{
							if (ofsX >= 0 && ofsY >= 0 && ofsX < srcMax_w && ofsY < srcMax_h)
								_read_pixel(p, SRCFORMAT, pClut, pClut4096, src11_b, src11_g, src11_r, src11_a);
							else
								src11_b = src11_g = src11_r = src11_a = 0;

							if ((ofsX + BINAL_MUL) >= 0 && ofsY >= 0 && (ofsX + BINAL_MUL) < srcMax_w && ofsY < srcMax_h)
								_read_pixel(p + srcPixelBytes, SRCFORMAT, pClut, pClut4096, src12_b, src12_g, src12_r, src12_a);
							else
								src12_b = src12_g = src12_r = src12_a = 0;

							if (ofsX >= 0 && (ofsY + BINAL_MUL) >= 0 && ofsX < srcMax_w && (ofsY + BINAL_MUL) < srcMax_h)
								_read_pixel(p + srcPitch, SRCFORMAT, pClut, pClut4096, src21_b, src21_g, src21_r, src21_a);
							else
								src21_b = src21_g = src21_r = src21_a = 0;

							if ((ofsX + BINAL_MUL) >= 0 && (ofsY + BINAL_MUL) >= 0 && (ofsX + BINAL_MUL) < srcMax_w && (ofsY + BINAL_MUL) < srcMax_h)
								_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, pClut, pClut4096, src22_b, src22_g, src22_r, src22_a);
							else
								src22_b = src22_g = src22_r = src22_a = 0;
						}
					}
				}
				else
				{
					uint8_t* p2, * p3, * p4;

					if (EDGEOP == SoftGfxDevice::EdgeOp::Tile)
					{
						binalInt x = (ofsX >> BINAL_SHIFT), y = (ofsY >> BINAL_SHIFT);

						p2 = pSrcSurf->pixels() + y * srcPitch + ((x + 1) & srcPosMaskX) * srcPixelBytes;
						p3 = pSrcSurf->pixels() + ((y + 1) & srcPosMaskY) * srcPitch + x * srcPixelBytes;
						p4 = pSrcSurf->pixels() + ((y + 1) & srcPosMaskY) * srcPitch + ((x + 1) & srcPosMaskX) * srcPixelBytes;
					}
					else
					{
						//							assert((ofsX | ofsY | (srcMax.w - (ofsX + 32768)) | (srcMax.h - (ofsY + 32768))) >= 0);

						binalInt nextX = (ofsX & BINAL_MASK) == 0 ? 0 : srcPixelBytes;
						binalInt nextY = (ofsY & BINAL_MASK) == 0 ? 0 : srcPitch;

						p2 = p + nextX;
						p3 = p + nextY;
						p4 = p + nextX + nextY;
					}

					if (bFast8)
					{
						_read_pixel_fast8(p, SRCFORMAT, pClut, pClut4096, src11_b, src11_g, src11_r, src11_a);
						_read_pixel_fast8(p2, SRCFORMAT, pClut, pClut4096, src12_b, src12_g, src12_r, src12_a);
						_read_pixel_fast8(p3, SRCFORMAT, pClut, pClut4096, src21_b, src21_g, src21_r, src21_a);
						_read_pixel_fast8(p4, SRCFORMAT, pClut, pClut4096, src22_b, src22_g, src22_r, src22_a);
					}
					else
					{
						_read_pixel(p, SRCFORMAT, pClut, pClut4096, src11_b, src11_g, src11_r, src11_a);
						_read_pixel(p2, SRCFORMAT, pClut, pClut4096, src12_b, src12_g, src12_r, src12_a);
						_read_pixel(p3, SRCFORMAT, pClut, pClut4096, src21_b, src21_g, src21_r, src21_a);
						_read_pixel(p4, SRCFORMAT, pClut, pClut4096, src22_b, src22_g, src22_r, src22_a);
					}
				}

				// Interpolate our 2x2 source colors into one source color, srcX

				binalInt fracX2 = ofsX & BINAL_MASK;
				binalInt fracX1 = BINAL_MUL - fracX2;

				binalInt fracY2 = ofsY & BINAL_MASK;
				binalInt fracY1 = BINAL_MUL - fracY2;

				binalInt mul11 = fracX1 * fracY1 >> BINAL_SHIFT;
				binalInt mul12 = fracX2 * fracY1 >> BINAL_SHIFT;
				binalInt mul21 = fracX1 * fracY2 >> BINAL_SHIFT;
				binalInt mul22 = fracX2 * fracY2 >> BINAL_SHIFT;

				srcB = (src11_b * mul11 + src12_b * mul12 + src21_b * mul21 + src22_b * mul22) >> BINAL_SHIFT;
				srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> BINAL_SHIFT;
				srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> BINAL_SHIFT;
				srcA = (src11_a * mul11 + src12_a * mul12 + src21_a * mul21 + src22_a * mul22) >> BINAL_SHIFT;

			}
			else
			{
				if (EDGEOP == SoftGfxDevice::EdgeOp::Clip && ((ofsX | ofsY | (srcMax_w - 1 - ofsX) | (srcMax_h - 1 - ofsY)) < 0))
				{
					ofsX += pixelIncX;
					ofsY += pixelIncY;
					pDst += dstPitchX;
					continue;
				}
				else
				{
					assert((ofsX | ofsY | (srcMax_w - 1 - ofsX) | (srcMax_h - 1 - ofsY)) >= 0);
					if (bFast8)
						_read_pixel_fast8(p, SRCFORMAT, pClut, pClut4096, srcB, srcG, srcR, srcA);
					else
						_read_pixel(p, SRCFORMAT, pClut, pClut4096, srcB, srcG, srcR, srcA);
				}
			}

			// Step 3: Get color components of background pixel blending into backX
			// Step 4: Blend srcX and backX into outX
			// Step 5: Write resulting pixel to destination

			int16_t backB, backG, backR, backA;
			int16_t outB, outG, outR, outA;

			if (bFast8)
			{
				_read_pixel_fast8(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels_fast8(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel_fast8(pDst, DSTFORMAT, outB, outG, outR, outA);
			}
			else
			{
				_read_pixel(pDst, DSTFORMAT, nullptr, nullptr, backB, backG, backR, backA);
				_blend_pixels(BLEND, tint.morphFactor, DSTFORMAT, srcB, srcG, srcR, srcA, backB, backG, backR, backA, outB, outG, outR, outA);
				_write_pixel(pDst, DSTFORMAT, outB, outG, outR, outA);
			}

			// Step 6: Increment source and destination pointers

			ofsX += pixelIncX;
			ofsY += pixelIncY;
			pDst += dstPitchX;
		}
		pDst += dstPitchY;
	}
}
