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
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <wg_base.h>

#include <assert.h>

using namespace std;

namespace wg 
{
	using namespace Util;
	
	const char SoftGfxDevice::CLASSNAME[] = {"SoftGfxDevice"};
	
	int SoftGfxDevice::s_mulTab[256];

	SoftGfxDevice::PlotOp_p		SoftGfxDevice::s_plotOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::FillOp_p		SoftGfxDevice::s_fillOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::BlitOp_p		SoftGfxDevice::s_pass2OpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::LineOp_p		SoftGfxDevice::s_LineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::PlotListOp_p SoftGfxDevice::s_plotListOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::WaveOp_p		SoftGfxDevice::s_waveOpTab[BlendMode_size][PixelFormat_size];

	SoftGfxDevice::ClipLineOp_p SoftGfxDevice::s_clipLineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::ClipPlotListOp_p SoftGfxDevice::s_clipPlotListOpTab[BlendMode_size][PixelFormat_size];

	//____ read_pixel() _______________________________________________________

	inline void SoftGfxDevice::_read_pixel(const uint8_t * pPixel, PixelFormat format, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA)
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

				_read_pixel(p, SRCFORMAT, src11_b, src11_g, src11_r, src11_a);
				_read_pixel(p+srcPixelBytes, SRCFORMAT, src12_b, src12_g, src12_r, src12_a);
				_read_pixel(p + srcPitch, SRCFORMAT, src21_b, src21_g, src21_r, src21_a);
				_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, src22_b, src22_g, src22_r, src22_a);

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
				p++;
				int srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> 15;
				p++;
				int srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> 15;
				p++;
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

				_read_pixel(pDst, DSTFORMAT, backB, backG, backR, backA);

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

				_read_pixel(p, SRCFORMAT, src11_b, src11_g, src11_r, src11_a);
				_read_pixel(p + srcPixelBytes, SRCFORMAT, src12_b, src12_g, src12_r, src12_a);
				_read_pixel(p + srcPitch, SRCFORMAT, src21_b, src21_g, src21_r, src21_a);
				_read_pixel(p + srcPitch + srcPixelBytes, SRCFORMAT, src22_b, src22_g, src22_r, src22_a);

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
				p++;
				int srcG = (src11_g * mul11 + src12_g * mul12 + src21_g * mul21 + src22_g * mul22) >> 15;
				p++;
				int srcR = (src11_r * mul11 + src12_r * mul12 + src21_r * mul21 + src22_r * mul22) >> 15;
				p++;
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

				_read_pixel(pDst, DSTFORMAT, backB, backG, backR, backA );

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




	template void SoftGfxDevice::_transform_blit<PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_transform_blit<PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);

	template void SoftGfxDevice::_transform_blit<PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_transform_blit<PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);


	template void SoftGfxDevice::_transform_blit<PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_transform_blit<PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);

	template void SoftGfxDevice::_transform_blit<PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_transform_blit<PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);


	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);

	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);


	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGRA_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGR_8, 0, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);

	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGRA_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);
	template void SoftGfxDevice::_stretch_blit<PixelFormat::BGR_8, 1, BlendMode::Blend, PixelFormat::BGRA_8>(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& dummy);



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
			pFunc(pDst, pixelBytes, m_canvasPitch - rect.w*pixelBytes, rect.h, rect.w, fillColor);
	}

	//____ fillSubPixel() ____________________________________________________________________

	void SoftGfxDevice::fillSubPixel(const RectF& rect, const Color& col)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		Color fillColor = col * m_tintColor;

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
		pOp(pDst, pixelBytes, m_canvasPitch, y2 - y1, x2 - x1, col);
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
			pOp(pDst, pixelBytes, 0, 1, length, fillColor);
//			_drawStraightLineAA(x1, (int)rect.y, x2 - x1, fillColor, blendMode, aaTop, Orientation::Horizontal);
		}

		if (aaBottom != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
			int length = x2 - x1;
			fillColor.a = aaBottom;
			pOp(pDst, pixelBytes, 0, 1, length, fillColor);
//			_drawStraightLineAA(x1, (int)y2, x2 - x1, fillColor, blendMode, aaBottom, Orientation::Horizontal);
		}

		if (aaLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + ((int)rect.x) * pixelBytes;
			int length = y2 - y1;
			fillColor.a = aaLeft;
			pOp(pDst, m_canvasPitch, 0, 1, length, fillColor);
//			_drawStraightLineAA((int)rect.x, y1, y2 - y1, fillColor, blendMode, aaLeft, Orientation::Vertical);
		}

		if (aaRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
			int length = y2 - y1;
			fillColor.a = aaRight;
			pOp(pDst, m_canvasPitch, 0, 1, length, fillColor);
//			_drawStraightLineAA((int)x2, y1, y2 - y1, fillColor, blendMode, aaRight, Orientation::Vertical);
		}

		// Draw corner pieces

		PlotOp_p pPlotOp = s_plotOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (aaTopLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + ((int)rect.x) * pixelBytes;
			fillColor.a = aaTopLeft;
			pPlotOp(pDst, fillColor);
//			_plotAA((int)rect.x, (int)rect.y, fillColor, blendMode, aaTopLeft);
		}

		if (aaTopRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + ((int)rect.y) * m_canvasPitch + x2 * pixelBytes;
			fillColor.a = aaTopRight;
			pPlotOp(pDst, fillColor);
//			_plotAA(x2, (int)rect.y, fillColor, blendMode, aaTopRight);
		}

		if (aaBottomLeft != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + ((int)rect.x) * pixelBytes;
			fillColor.a = aaBottomLeft;
			pPlotOp(pDst, fillColor);
//			_plotAA((int)rect.x, y2, fillColor, blendMode, aaBottomLeft);
		}

		if (aaBottomRight != 0)
		{
			uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
			fillColor.a = aaBottomRight;
			pPlotOp(pDst, fillColor);
//			_plotAA(x2, y2, fillColor, blendMode, aaBottomRight);
		}
	}

	//____ _plot_move_32() ________________________________________________________

	void SoftGfxDevice::_plot_move_32(uint8_t * pDst, Color col)
	{
		*((uint32_t*)(pDst)) = col.argb;
	}

	//____ _plot_blend_32() ________________________________________________________

	void SoftGfxDevice::_plot_blend_32(uint8_t * pDst, Color col)
	{
		int a = s_mulTab[col.a];
		int invAlpha = 65536 - a;

		pDst[0] = (pDst[0] * invAlpha + col.b * a) >> 16;
		pDst[1] = (pDst[1] * invAlpha + col.g * a) >> 16;
		pDst[2] = (pDst[2] * invAlpha + col.r * a) >> 16;
		pDst[3] = (pDst[3] * invAlpha + col.a * 65536) >> 16;
	}

	//____ _plot_move_24() ________________________________________________________

	void SoftGfxDevice::_plot_move_24(uint8_t * pDst, Color col)
	{
		pDst[0] = col.b;
		pDst[1] = col.g;
		pDst[2] = col.r;
	}

	//____ _plot_blend_24() ________________________________________________________

	void SoftGfxDevice::_plot_blend_24(uint8_t * pDst, Color col)
	{
		int a = s_mulTab[col.a];
		int invAlpha = 65536 - a;

		pDst[0] = (pDst[0] * invAlpha + col.b * a) >> 16;
		pDst[1] = (pDst[1] * invAlpha + col.g * a) >> 16;
		pDst[2] = (pDst[2] * invAlpha + col.r * a) >> 16;
	}

	//____ _plot_add_24() _________________________________________________________

	void SoftGfxDevice::_plot_add_24(uint8_t * pDst, Color col)
	{
		pDst[0] = limitUint8(pDst[0] + col.b);
		pDst[1] = limitUint8(pDst[1] + col.g);
		pDst[2] = limitUint8(pDst[2] + col.r);
	}

	//____ _plot_sub_24() _________________________________________________________

	void SoftGfxDevice::_plot_sub_24(uint8_t * pDst, Color col)
	{
		pDst[0] = limitUint8(pDst[0] - col.b);
		pDst[1] = limitUint8(pDst[1] - col.g);
		pDst[2] = limitUint8(pDst[2] - col.r);
	}

	//____ _plot_mul_24() _________________________________________________________

	void SoftGfxDevice::_plot_mul_24(uint8_t * pDst, Color col)
	{
		pDst[0] = (pDst[0] * s_mulTab[col.b]) >> 16;
		pDst[1] = (pDst[1] * s_mulTab[col.g]) >> 16;
		pDst[2] = (pDst[2] * s_mulTab[col.r]) >> 16;
	}

	//____ _plot_invert_24() ______________________________________________________

	void SoftGfxDevice::_plot_invert_24(uint8_t * pDst, Color col)
	{
		int storedRed = s_mulTab[col.r];
		int storedGreen = s_mulTab[col.g];
		int storedBlue = s_mulTab[col.b];

		int invertRed = 65536 - storedRed;
		int invertGreen = 65536 - storedGreen;
		int invertBlue = 65536 - storedBlue;

		pDst[0] = ((255 - pDst[0]) * storedBlue + pDst[0] * invertBlue) >> 16;
		pDst[1] = ((255 - pDst[1]) * storedGreen + pDst[1] * invertGreen) >> 16;
		pDst[2] = ((255 - pDst[2]) * storedRed + pDst[2] * invertRed) >> 16;

	}

	//____ _fill_move_32() ____________________________________________________

	void SoftGfxDevice::_fill_move_32(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		for (int y = 0; y < nLines; y++)
		{
			uint32_t fillValue = ((int)col.b) | (((int)col.g) << 8) | (((int)col.r) << 16) | (((int)col.a) << 24);

			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				*((uint32_t*)(pDst)) = fillValue;
				pDst += 4;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_blend_32() ___________________________________________________

	void SoftGfxDevice::_fill_blend_32(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		int a = s_mulTab[col.a];

		int storedRed = col.r * a;
		int storedGreen = col.g * a;
		int storedBlue = col.b * a;
		int invAlpha = 65536 - a;

		int storedAlpha = col.a * 65536;

		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				pDst[0] = (pDst[0] * invAlpha + storedBlue) >> 16;
				pDst[1] = (pDst[1] * invAlpha + storedGreen) >> 16;
				pDst[2] = (pDst[2] * invAlpha + storedRed) >> 16;
				pDst[3] = (pDst[3] * invAlpha + storedAlpha) >> 16;
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_move_24() __________________________________________________

	void SoftGfxDevice::_fill_move_24(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				pDst[0] = col.b;
				pDst[1] = col.g;
				pDst[2] = col.r;
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_blend_24() ___________________________________________________

	void SoftGfxDevice::_fill_blend_24(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		int a = s_mulTab[col.a];

		int storedRed = col.r * a;
		int storedGreen = col.g * a;
		int storedBlue = col.b * a;
		int invAlpha = 65536 - a;

		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				pDst[0] = (pDst[0] * invAlpha + storedBlue) >> 16;
				pDst[1] = (pDst[1] * invAlpha + storedGreen) >> 16;
				pDst[2] = (pDst[2] * invAlpha + storedRed) >> 16;
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_add_24() _____________________________________________________

	void SoftGfxDevice::_fill_add_24(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		int storedRed = (int)col.r;
		int storedGreen = (int)col.g;
		int storedBlue = (int)col.b;

		if (storedRed + storedGreen + storedBlue == 0)
			return;

		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				pDst[0] = limitUint8(pDst[0] + storedBlue);
				pDst[1] = limitUint8(pDst[1] + storedGreen);
				pDst[2] = limitUint8(pDst[2] + storedRed);
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_sub_24() _____________________________________________________

	void SoftGfxDevice::_fill_sub_24(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		int storedRed = (int)col.r;
		int storedGreen = (int)col.g;
		int storedBlue = (int)col.b;

		if (storedRed + storedGreen + storedBlue == 0)
			return;

		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				pDst[0] = limitUint8(pDst[0] - storedBlue);
				pDst[1] = limitUint8(pDst[1] - storedGreen);
				pDst[2] = limitUint8(pDst[2] - storedRed);
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_mul_24() _____________________________________________________

	void SoftGfxDevice::_fill_mul_24(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		int storedRed = s_mulTab[col.r];
		int storedGreen = s_mulTab[col.g];
		int storedBlue = s_mulTab[col.b];

		if (storedRed + storedGreen + storedBlue == 65536*3)
			return;

		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while (pDst < pEnd)
			{
				pDst[0] = (pDst[0] * storedBlue) >> 16;
				pDst[1] = (pDst[1] * storedGreen) >> 16;
				pDst[2] = (pDst[2] * storedRed) >> 16;
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}

	//____ _fill_invert_24() __________________________________________________

	void SoftGfxDevice::_fill_invert_24(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col)
	{
		int storedRed = s_mulTab[col.r];
		int storedGreen = s_mulTab[col.g];
		int storedBlue = s_mulTab[col.b];

		if (storedRed + storedGreen + storedBlue <= 3)
			return;

		int invertRed = 65536 - storedRed;
		int invertGreen = 65536 - storedGreen;
		int invertBlue = 65536 - storedBlue;

		for (int y = 0; y < nLines; y++)
		{
			uint8_t * pEnd = pDst + pitchX * lineLength;
			while(pDst < pEnd)
			{
				pDst[0] = ((255 - pDst[0]) * storedBlue + pDst[0] * invertBlue) >> 16;
				pDst[1] = ((255 - pDst[1]) * storedGreen + pDst[1] * invertGreen) >> 16;
				pDst[2] = ((255 - pDst[2]) * storedRed + pDst[2] * invertRed) >> 16;
				pDst += pitchX;
			}
			pDst += pitchY;
		}
	}
	
	//____ drawLine() ____ [from/to] __________________________________________

	void SoftGfxDevice::drawLine( Coord beg, Coord end, Color color, float thickness )
	{
		if( !m_pCanvas || !m_pCanvasPixels )
			return;
	
		Color fillColor = color * m_tintColor;
	
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
			pOp( pRow, rowInc, pixelInc, length, width, pos, slope, fillColor );
	}

	//____ drawLine() ____ [start/direction] __________________________________

	void SoftGfxDevice::drawLine(Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		//TODO: Check how much slower thick vertical lines gets than horizontal due to cache trashing caused by us drawing vertically.

		if (thickness <= 0.f)
			return;

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
				pOp(pBegin, pixelBytes, 0, 1, length, col);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				int beginY = begin.y - expanse;
				int endY = begin.y + expanse + 1;

				uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, 0, 1, length, edgeColor);

				pBegin = m_pCanvasPixels + (endY-1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, 0, 1, length, edgeColor);

				int bodyThickness = endY - beginY - 2;
				pBegin = m_pCanvasPixels + (beginY+1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, m_canvasPitch-bodyThickness*pixelBytes, bodyThickness, length, _col);

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
				pOp(pBegin, m_canvasPitch, 0, 1, length, col);
			}
			else
			{
				int expanse = (int)(1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				int beginX = begin.x - expanse;
				int endX = begin.x + expanse + 1;

				uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor);

				pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX-1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor);

				int bodyThickness = endX - beginX - 2;
				pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX+1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, pixelBytes - m_canvasPitch-bodyThickness, bodyThickness, length, _col);

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
			pOp( clipStart, clipEnd, pRow, rowInc, pixelInc, length, width, pos, slope, fillColor );
	}

	//____ clipDrawLine() ____ [start/direction] ______________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void SoftGfxDevice::clipDrawLine(const Rect& clip, Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		if (thickness <= 0.f)
			return;

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
				pOp(pBegin, pixelBytes, 0, 1, length, col);
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
					pOp(pBegin, pixelBytes, 0, 1, length, edgeColor);
//					_drawStraightLine({ begin.x, beginY }, Orientation::Horizontal, length, edgeColor);
				}

				if (endY > clip.y + clip.h)
					endY = clip.y + clip.h + 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + (endY - 1) * m_canvasPitch + begin.x * pixelBytes;
					pOp(pBegin, pixelBytes, 0, 1, length, edgeColor);
//					_drawStraightLine({ begin.x, endY - 1 }, Orientation::Horizontal, length, edgeColor);
				}

				int bodyThickness = endY - beginY - 2;
				uint8_t * pBegin = m_pCanvasPixels + (beginY + 1) * m_canvasPitch + begin.x * pixelBytes;
				pOp(pBegin, pixelBytes, m_canvasPitch - bodyThickness * pixelBytes, bodyThickness, length, _col);
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
				pOp(pBegin, m_canvasPitch, 0, 1, length, col);
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
					pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor);
//					_drawStraightLine({ beginX, begin.y }, Orientation::Vertical, length, edgeColor);
				}

				if (endX > clip.x + clip.w)
					endX = clip.x + clip.w + 1;
				else
				{
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX - 1) * pixelBytes;
					pOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor);
//					_drawStraightLine({ endX - 1, begin.y }, Orientation::Vertical, length, edgeColor);
				}


				int bodyThickness = endX - beginX - 2;
				uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX + 1) * pixelBytes;
				pOp(pBegin, m_canvasPitch, pixelBytes - m_canvasPitch - bodyThickness, bodyThickness, length, _col);
//				fill({ beginX + 1, begin.y, endX - beginX - 2, length }, _col);
			}

			break;
		}
	}


	//____ _drawLineSegment_blend_24() ______________________________________________________

	void SoftGfxDevice::_drawLineSegment_blend_24( uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color )
	{
		for( int i = 0 ; i < length ; i++ )
		{
			
			int beg = pos >> 16;
			int end = (pos + width) >> 16;
			int ofs = beg;
			
			if( beg == end )
			{
				// Special case, one pixel wide row
				
				int alpha = s_mulTab[color.a] * (width >> 4) >> 12;

				int invAlpha = 65536 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = (pDst[0]*invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1]*invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2]*invAlpha + color.r*alpha) >> 16;			
			}
			else
			{
				// First anti-aliased pixel of column
				
				int alpha = s_mulTab[color.a * (65536 - (pos & 0xFFFF)) >> 16];
				
				int invAlpha = 655365 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = (pDst[0]*invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1]*invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2]*invAlpha + color.r*alpha) >> 16;			
				ofs++;
				
				// All non-antialiased middle pixels of column
						
				if( ofs < end )
				{					
					alpha = s_mulTab[color.a];	
					invAlpha = 65536 - alpha;

					int storedRed = color.r * alpha;
					int storedGreen = color.g * alpha;
					int storedBlue = color.b * alpha;

					do 
					{
						pDst = pRow + ofs*pixelInc;						
						pDst[0] = (pDst[0]*invAlpha + storedBlue) >> 16;
						pDst[1] = (pDst[1]*invAlpha + storedGreen) >> 16;
						pDst[2] = (pDst[2]*invAlpha + storedRed) >> 16;			
						ofs++;
						
					} while( ofs < end );
				}

				// Last anti-aliased pixel of column

				int overflow = (pos+width) & 0xFFFF;
				if( overflow > 0 )
				{
					alpha = s_mulTab[color.a * overflow >> 16];
					
					invAlpha = 65536 - alpha;
					pDst = pRow + ofs*pixelInc;
					
					pDst[0] = (pDst[0]*invAlpha + color.b*alpha) >> 16;
					pDst[1] = (pDst[1]*invAlpha + color.g*alpha) >> 16;
					pDst[2] = (pDst[2]*invAlpha + color.r*alpha) >> 16;			
				}
			}
			
			pRow += rowInc;
			pos += slope;
		}
	}

	//____ _clipDrawLineSegment_blend_24() ______________________________________________________

	void SoftGfxDevice::_clipDrawLineSegment_blend_24( int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color )
	{
		for( int i = 0 ; i < length ; i++ )
		{
			
			if( pos >= clipEnd || pos + width <= clipStart )
			{
				pRow += rowInc;
				pos += slope;
				continue;
			}
			
			int clippedPos = pos;
			int clippedWidth = width;
			
			if( clippedPos < clipStart )
			{
				clippedWidth -= clipStart - clippedPos;
				clippedPos = clipStart;
			}
			
			if( clippedPos + clippedWidth > clipEnd )
				clippedWidth = clipEnd - clippedPos;
			
			
			int beg = clippedPos >> 16;
			int end = (clippedPos + clippedWidth) >> 16;
			int ofs = beg;
			
			if( beg == end )
			{
				// Special case, one pixel wide row
				
				int alpha = s_mulTab[color.a] * (width >> 4) >> 12;

				int invAlpha = 65536 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = (pDst[0]*invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1]*invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2]*invAlpha + color.r*alpha) >> 16;
			}
			else
			{
				// First anti-aliased pixel of column
				
				int alpha = s_mulTab[color.a * (65536 - (clippedPos & 0xFFFF)) >> 16];
				
				int invAlpha = 65536 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = (pDst[0]*invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1]*invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2]*invAlpha + color.r*alpha) >> 16;
				ofs++;
				
				// All non-antialiased middle pixels of column
								
				if( ofs < end )
				{					
					alpha = s_mulTab[color.a];	
					invAlpha = 65536 - alpha;

					int storedRed = color.r * alpha;
					int storedGreen = color.g * alpha;
					int storedBlue = color.b * alpha;

					do 
					{
						pDst = pRow + ofs*pixelInc;						
						pDst[0] = (pDst[0]*invAlpha + storedBlue) >> 16;
						pDst[1] = (pDst[1]*invAlpha + storedGreen) >> 16;
						pDst[2] = (pDst[2]*invAlpha + storedRed) >> 16;
						ofs++;
						
					} while( ofs < end );
				}

				// Last anti-aliased pixel of column

				int overflow = (clippedPos+clippedWidth) & 0xFFFF;
				if( overflow > 0 )
				{
					alpha = s_mulTab[color.a * overflow >> 16];
					invAlpha = 65536 - alpha;
					pDst = pRow + ofs*pixelInc;
				
					pDst[0] = (pDst[0]*invAlpha + color.b*alpha) >> 16;
					pDst[1] = (pDst[1]*invAlpha + color.g*alpha) >> 16;
					pDst[2] = (pDst[2]*invAlpha + color.r*alpha) >> 16;
				}
			}
			
			pRow += rowInc;
			pos += slope;
		}
	}
	 
	//____ _drawLineSegment_blend_32() ______________________________________________________

	void SoftGfxDevice::_drawLineSegment_blend_32(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color)
	{
		for (int i = 0; i < length; i++)
		{

			int beg = pos >> 16;
			int end = (pos + width) >> 16;
			int ofs = beg;

			if (beg == end)
			{
				// Special case, one pixel wide row

				int alpha = (s_mulTab[color.a] * (width >> 4)) >> 12;

				int invAlpha = 65536 - alpha;
				uint8_t * pDst = pRow + ofs * pixelInc;

				pDst[0] = (pDst[0] * invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1] * invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2] * invAlpha + color.r*alpha) >> 16;
				pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
			}
			else
			{
				// First anti-aliased pixel of column

				int alpha = s_mulTab[(color.a * (65536 - (pos & 0xFFFF))) >> 16];

				int invAlpha = 655365 - alpha;
				uint8_t * pDst = pRow + ofs * pixelInc;

				pDst[0] = (pDst[0] * invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1] * invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2] * invAlpha + color.r*alpha) >> 16;
				pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
				ofs++;

				// All non-antialiased middle pixels of column

				if (ofs < end)
				{
					alpha = s_mulTab[color.a];
					invAlpha = 65536 - alpha;

					int storedRed = color.r * alpha;
					int storedGreen = color.g * alpha;
					int storedBlue = color.b * alpha;
					int storedAlpha = 255 * alpha;

					do
					{
						pDst = pRow + ofs * pixelInc;
						pDst[0] = (pDst[0] * invAlpha + storedBlue) >> 16;
						pDst[1] = (pDst[1] * invAlpha + storedGreen) >> 16;
						pDst[2] = (pDst[2] * invAlpha + storedRed) >> 16;
						pDst[3] = (pDst[3] * invAlpha + storedAlpha) >> 16;
						ofs++;

					} while (ofs < end);
				}

				// Last anti-aliased pixel of column

				int overflow = (pos + width) & 0xFFFF;
				if (overflow > 0)
				{
					alpha = s_mulTab[color.a * overflow >> 16];

					invAlpha = 65536 - alpha;
					pDst = pRow + ofs * pixelInc;

					pDst[0] = (pDst[0] * invAlpha + color.b*alpha) >> 16;
					pDst[1] = (pDst[1] * invAlpha + color.g*alpha) >> 16;
					pDst[2] = (pDst[2] * invAlpha + color.r*alpha) >> 16;
					pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
				}
			}

			pRow += rowInc;
			pos += slope;
		}
	}

	//____ _clipDrawLineSegment_blend_32() ______________________________________________________

	void SoftGfxDevice::_clipDrawLineSegment_blend_32(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color)
	{
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

			if (beg == end)
			{
				// Special case, one pixel wide row

				int alpha = (s_mulTab[color.a] * (width >> 4)) >> 12;

				int invAlpha = 65536 - alpha;
				uint8_t * pDst = pRow + ofs * pixelInc;

				pDst[0] = (pDst[0] * invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1] * invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2] * invAlpha + color.r*alpha) >> 16;
				pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
			}
			else
			{
				// First anti-aliased pixel of column

				int alpha = s_mulTab[(color.a * (65536 - (clippedPos & 0xFFFF))) >> 16];

				int invAlpha = 65536 - alpha;
				uint8_t * pDst = pRow + ofs * pixelInc;

				pDst[0] = (pDst[0] * invAlpha + color.b*alpha) >> 16;
				pDst[1] = (pDst[1] * invAlpha + color.g*alpha) >> 16;
				pDst[2] = (pDst[2] * invAlpha + color.r*alpha) >> 16;
				pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
				ofs++;

				// All non-antialiased middle pixels of column

				if (ofs < end)
				{
					alpha = s_mulTab[color.a];
					invAlpha = 65536 - alpha;

					int storedRed = color.r * alpha;
					int storedGreen = color.g * alpha;
					int storedBlue = color.b * alpha;
					int storedAlpha = 255 * alpha;

					do
					{
						pDst = pRow + ofs * pixelInc;
						pDst[0] = (pDst[0] * invAlpha + storedBlue) >> 16;
						pDst[1] = (pDst[1] * invAlpha + storedGreen) >> 16;
						pDst[2] = (pDst[2] * invAlpha + storedRed) >> 16;
						pDst[3] = (pDst[3] * invAlpha + storedAlpha) >> 16;
						ofs++;

					} while (ofs < end);
				}

				// Last anti-aliased pixel of column

				int overflow = (clippedPos + clippedWidth) & 0xFFFF;
				if (overflow > 0)
				{
					alpha = s_mulTab[color.a * overflow >> 16];
					invAlpha = 65536 - alpha;
					pDst = pRow + ofs * pixelInc;

					pDst[0] = (pDst[0] * invAlpha + color.b*alpha) >> 16;
					pDst[1] = (pDst[1] * invAlpha + color.g*alpha) >> 16;
					pDst[2] = (pDst[2] * invAlpha + color.r*alpha) >> 16;
					pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
				}
			}

			pRow += rowInc;
			pos += slope;
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

		ClipPlotListOp_p pOp = s_clipPlotListOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (pOp)
			pOp(clip, nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch);
	}

	//____ plotPixels() ____________________________________________________

	void SoftGfxDevice::plotPixels(int nCoords, const Coord * pCoords, const Color * pColors)
	{
		//TODO: Support blend modes and tint

		const int pitch = m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits / 8;

		PlotListOp_p pOp = s_plotListOpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];

		if (pOp)
			pOp(nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch);
	}

	//____ _clip_plotlist_blend_32() ______________________________________________

	void SoftGfxDevice::_clip_plotlist_blend_32(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY)
	{
		for (int i = 0; i < nCoords; i++)
		{
			const int x = pCoords[i].x;
			const int y = pCoords[i].y;

			if (y >= clip.y && y <= clip.y + clip.h - 1 && x >= clip.x && x <= clip.x + clip.w - 1)
			{
				uint8_t * pDst = pCanvas + y * pitchY + x * pitchX;

				const int alpha = s_mulTab[pColors[i].a];
				const int invAlpha = 65536 - alpha;

				pDst[0] = (uint8_t)((pDst[0] * invAlpha + (int)pColors[i].b*alpha) >> 16);
				pDst[1] = (uint8_t)((pDst[1] * invAlpha + (int)pColors[i].g*alpha) >> 16);
				pDst[2] = (uint8_t)((pDst[2] * invAlpha + (int)pColors[i].r*alpha) >> 16);
				pDst[3] = (uint8_t)((pDst[3] * invAlpha + 255*alpha) >> 16);
			}
		}
	}


	//____ _clip_plotlist_blend_24() ______________________________________________

	void SoftGfxDevice::_clip_plotlist_blend_24(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY )
	{
		for (int i = 0; i < nCoords; i++)
		{
			const int x = pCoords[i].x;
			const int y = pCoords[i].y;

			if (y >= clip.y && y <= clip.y + clip.h - 1 && x >= clip.x && x <= clip.x + clip.w - 1)
			{
				uint8_t * pDst = pCanvas + y * pitchY + x * pitchX;

				const int alpha = s_mulTab[pColors[i].a];
				const int invAlpha = 65536 - alpha;

				pDst[0] = (uint8_t)((pDst[0] * invAlpha + (int)pColors[i].b*alpha) >> 16);
				pDst[1] = (uint8_t)((pDst[1] * invAlpha + (int)pColors[i].g*alpha) >> 16);
				pDst[2] = (uint8_t)((pDst[2] * invAlpha + (int)pColors[i].r*alpha) >> 16);
			}
		}
	}

	//____ _plotlist_blend_32() ______________________________________________

	void SoftGfxDevice::_plotlist_blend_32(int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY)
	{
		for (int i = 0; i < nCoords; i++)
		{
			const int x = pCoords[i].x;
			const int y = pCoords[i].y;

			uint8_t * pDst = pCanvas + y * pitchY + x * pitchX;

			const int alpha = s_mulTab[pColors[i].a];
			const int invAlpha = 65536 - alpha;

			pDst[0] = (uint8_t)((pDst[0] * invAlpha + (int)pColors[i].b*alpha) >> 16);
			pDst[1] = (uint8_t)((pDst[1] * invAlpha + (int)pColors[i].g*alpha) >> 16);
			pDst[2] = (uint8_t)((pDst[2] * invAlpha + (int)pColors[i].r*alpha) >> 16);
			pDst[3] = (uint8_t)((pDst[3] * invAlpha + 255 * alpha) >> 16);
		}
	}

	//____ _plotlist_blend_24() ______________________________________________

	void SoftGfxDevice::_plotlist_blend_24(int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY)
	{
		for (int i = 0; i < nCoords; i++)
		{
			const int x = pCoords[i].x;
			const int y = pCoords[i].y;

			uint8_t * pDst = pCanvas + y * pitchY + x * pitchX;

			const int alpha = s_mulTab[pColors[i].a];
			const int invAlpha = 65536 - alpha;

			pDst[0] = (uint8_t)((pDst[0] * invAlpha + (int)pColors[i].b*alpha) >> 16);
			pDst[1] = (uint8_t)((pDst[1] * invAlpha + (int)pColors[i].g*alpha) >> 16);
			pDst[2] = (uint8_t)((pDst[2] * invAlpha + (int)pColors[i].r*alpha) >> 16);
		}
	}

	//____ _drawStraightLine() ________________________________________________
	
	void SoftGfxDevice::_drawStraightLine(Coord start, Orientation orientation, int _length, const Color& _col)
	{
		// Skipping this, only used by GfxDevice for drawLine() and clipDrawLine() which we have overloaded anyway.
	}
	
	
	//____ drawElipse() _______________________________________________________________
	
	void SoftGfxDevice::drawElipse( const Rect& rect, Color color )
	{
		if( !m_pCanvas || !m_pCanvasPixels || rect.h < 2 || rect.w < 1 )
			return;
	
		Color fillColor = color * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;
		
			
		int sectionHeight = rect.h/2;
		int maxWidth = rect.w/2;
	
		uint8_t * pLineBeg = m_pCanvasPixels + rect.y *m_canvasPitch;
		int pitch =m_canvasPitch;
	
		int center = (rect.x + rect.w/2) << 8;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / sectionHeight;
		int sinOfs = 0;
	
		int begOfs = 0;
		int peakOfs = 0;
		int endOfs = 0;
	
		for( int i = 0 ; i < sectionHeight ; i++ )
		{
			peakOfs = ((s_pCurveTab[sinOfs>>16] * maxWidth) >> 8);
			endOfs = (s_pCurveTab[(sinOfs+(sinOfsInc-1))>>16] * maxWidth) >> 8;
	
			_drawHorrFadeLine( pLineBeg + i*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
			_drawHorrFadeLine( pLineBeg + i*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
	
			_drawHorrFadeLine( pLineBeg + (sectionHeight*2-i-1)*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
			_drawHorrFadeLine( pLineBeg + (sectionHeight*2-i-1)*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
	
			begOfs = peakOfs;
			sinOfs += sinOfsInc;
		}
	}
	
	//____ _clipDrawHorrFadeLine() _______________________________________________________________
	
	void SoftGfxDevice::_clipDrawHorrFadeLine( int clipX1, int clipX2, uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color )
	{
		//TODO: Translate to use m_pDivTab
	
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * p = pLineStart + (begOfs>>8) * pixelBytes;
		uint8_t * pClip1 = pLineStart + clipX1*pixelBytes;
		uint8_t * pClip2 = pLineStart + clipX2*pixelBytes;
	
		int alphaInc, alpha, len;
	
		if( (peakOfs>>8) == (begOfs>>8) )
		{
			alphaInc = 0;
			alpha = (256-(peakOfs&0xff) + (peakOfs-begOfs)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (peakOfs - begOfs);			// alpha inc per pixel with 14 binals.
			alpha = ((256 - (begOfs&0xff)) * alphaInc) >> 9;		// alpha for ramp up start pixel with 14 binals.
			len = ((peakOfs+256) >> 8) - (begOfs >> 8);
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			if( p >= pClip1 && p < pClip2 )
			{
				int invAlpha = (255 << 14) - alpha;
	
				p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
				p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
				p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			}
			alpha += alphaInc;
			if( alpha > 255 << 14 )
				alpha = 255 << 14;
	
			p += pixelBytes;
		}
	
		if( (endOfs>>8) == ((peakOfs + 256)>>8) )
		{
			alphaInc = 0;
			alpha = ((peakOfs&0xff)+(endOfs-peakOfs-256)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (endOfs - (peakOfs+256));						// alpha dec per pixel with 14 binals.
			alpha = (255 << 14) - (((256 - (peakOfs&0xff)) * alphaInc) >> 9);	// alpha for ramp down start pixel with 14 binals.
			len = (endOfs >> 8) - ((peakOfs+256) >> 8);
			alphaInc = -alphaInc;
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			if( p >= pClip1 && p < pClip2 )
			{
				int invAlpha = (255 << 14) - alpha;
	
				p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
				p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
				p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			}
			alpha += alphaInc;
			p += pixelBytes;
		}
	}
	
	
	//____ _drawHorrFadeLine() _______________________________________________________________
	
	void SoftGfxDevice::_drawHorrFadeLine( uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color )
	{
		//TODO: Translate to use m_pDivTab
	
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * p = pLineStart + (begOfs>>8) * pixelBytes;
	
		int alphaInc, alpha, len;
	
		if( (peakOfs>>8) == (begOfs>>8) )
		{
			alphaInc = 0;
			alpha = (256-(peakOfs&0xff) + (peakOfs-begOfs)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (peakOfs - begOfs);			// alpha inc per pixel with 14 binals.
			alpha = ((256 - (begOfs&0xff)) * alphaInc) >> 9;		// alpha for ramp up start pixel with 14 binals.
			len = ((peakOfs+256) >> 8) - (begOfs >> 8);
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			int invAlpha = (255 << 14) - alpha;
	
			p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
			p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
			p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			alpha += alphaInc;
			if( alpha > 255 << 14 )
				alpha = 255 << 14;
	
			p += pixelBytes;
		}
	
		if( (endOfs>>8) == ((peakOfs + 256)>>8) )
		{
			alphaInc = 0;
			alpha = ((peakOfs&0xff)+(endOfs-peakOfs-256)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (endOfs - (peakOfs+256));						// alpha dec per pixel with 14 binals.
			alpha = (255 << 14) - (((256 - (peakOfs&0xff)) * alphaInc) >> 9);	// alpha for ramp down start pixel with 14 binals.
			len = (endOfs >> 8) - ((peakOfs+256) >> 8);
			alphaInc = -alphaInc;
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			int invAlpha = (255 << 14) - alpha;
	
			p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
			p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
			p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			alpha += alphaInc;
			p += pixelBytes;
		}
	
	}
	
	//____ clipDrawElipse() _______________________________________________________________
	
	void SoftGfxDevice::clipDrawElipse( const Rect& clip, const Rect& rect, Color color )
	{
		if( !m_pCanvas || !m_pCanvasPixels || rect.h < 2 || rect.w < 1 )
			return;
	
		Color fillColor = color * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;

	
		if( !rect.intersectsWith(clip) )
			return;
	
		if( clip.contains(rect) )
			return drawElipse(rect,color);
	
		int sectionHeight = rect.h/2;
		int maxWidth = rect.w/2;
	
		uint8_t * pLineBeg = m_pCanvasPixels + rect.y*m_canvasPitch;
		int pitch =m_canvasPitch;
	
		int center = (rect.x + rect.w/2) << 8;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / sectionHeight;
		int sinOfs = 0;
	
		int begOfs = 0;
		int peakOfs = 0;
		int endOfs = 0;
	
		for( int i = 0 ; i < sectionHeight ; i++ )
		{
			peakOfs = ((s_pCurveTab[sinOfs>>16] * maxWidth) >> 8);
			endOfs = (s_pCurveTab[(sinOfs+(sinOfsInc-1))>>16] * maxWidth) >> 8;
	
			if( rect.y + i >= clip.y && rect.y + i < clip.y + clip.h )
			{
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + i*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + i*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
			}
	
			int y2 = sectionHeight*2-i-1;
			if( rect.y + y2 >= clip.y && rect.y + y2 < clip.y + clip.h )
			{
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + y2*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + y2*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
			}
	
			begOfs = peakOfs;
			sinOfs += sinOfsInc;
		}
	}
	
	//____ drawFilledElipse() _____________________________________________________
	
	void SoftGfxDevice::drawFilledElipse( const Rect& rect, Color color )
	{
		int pixelBytes = m_canvasPixelBits/8;
	
		uint8_t * pLineCenter = m_pCanvasPixels + rect.y *m_canvasPitch + (rect.x+rect.w/2) * pixelBytes;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / (rect.h/2);
		int sinOfs = sinOfsInc >> 1;
	
		for( int j = 0 ; j < 2 ; j++ )
		{
			for( int i = 0 ; i < rect.h/2 ; i++ )
			{
				int lineLen = ((s_pCurveTab[sinOfs>>16] * rect.w/2 + 32768)>>16)*pixelBytes;
				uint8_t * pLineBeg = pLineCenter - lineLen;
				uint8_t * pLineEnd = pLineCenter + lineLen;
	
				for( uint8_t * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
				{
					p[0] = color.b;
					p[1] = color.g;
					p[2] = color.r;
				}
	
				sinOfs += sinOfsInc;
				pLineCenter +=m_canvasPitch;
			}
			sinOfsInc = -sinOfsInc;
			sinOfs = (c_nCurveTabEntries << 16) + (sinOfsInc >> 1);
		}
	}
	
	//____ clipDrawFilledElipse() _____________________________________________________
	
	void SoftGfxDevice::clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color )
	{
		if( !rect.intersectsWith(clip) )
			return;
	
		if( clip.contains(rect) )
			return drawFilledElipse(rect,color);
	
		int pixelBytes = m_canvasPixelBits/8;
	
		uint8_t * pLine = m_pCanvasPixels + rect.y *m_canvasPitch;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / (rect.h/2);
		int sinOfs = sinOfsInc >> 1;
	
		for( int j = 0 ; j < 2 ; j++ )
		{
			for( int i = 0 ; i < rect.h/2 ; i++ )
			{
				if( rect.y + j*(rect.h/2) + i >= clip.y && rect.y + j*(rect.h/2) + i < clip.y + clip.h )
				{
					int lineLen = ((s_pCurveTab[sinOfs>>16] * rect.w/2 + 32768)>>16);
	
					int beg = rect.x + rect.w/2 - lineLen;
					int end = rect.x + rect.w/2 + lineLen;
	
					if( beg < clip.x )
						beg = clip.x;
	
					if( end > clip.x + clip.w )
						end = clip.x + clip.w;
	
					if( beg < end )
					{
						uint8_t * pLineBeg = pLine + beg * pixelBytes;
						uint8_t * pLineEnd = pLine + end * pixelBytes;
	
						for( uint8_t * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
						{
							p[0] = color.b;
							p[1] = color.g;
							p[2] = color.r;
						}
					}
				}
	
				sinOfs += sinOfsInc;
				pLine +=m_canvasPitch;
			}
	
			sinOfsInc = -sinOfsInc;
			sinOfs = (c_nCurveTabEntries << 16) + (sinOfsInc >> 1);
		}
	}
	
	
	//____ drawArcNE() ____________________________________________________________
	
	void SoftGfxDevice::drawArcNE( const Rect& rect, Color color )
	{
		int pixelBytes = m_canvasPixelBits/8;
	
		uint8_t * pLineBeg = m_pCanvasPixels + rect.y *m_canvasPitch + rect.x * pixelBytes;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / rect.h;
		int sinOfs = sinOfsInc >> 1;
	
		for( int i = 0 ; i < rect.h ; i++ )
		{
			uint8_t * pLineEnd = pLineBeg + ((s_pCurveTab[sinOfs>>16] * rect.w + 32768)>>16)*pixelBytes;
	
			for( uint8_t * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
			{
				p[0] = color.b;
				p[1] = color.g;
				p[2] = color.r;
			}
	
			sinOfs += sinOfsInc;
			pLineBeg +=m_canvasPitch;
		}
	
	}
	
	//____ clipDrawArcNE() _________________________________________________________
	
	void SoftGfxDevice::clipDrawArcNE( const Rect& clip, const Rect& rect, Color color )
	{
		//TODO: Implement!!!
	}
	
	//____ _tint_32_to_32() ____________________________________________________

	void SoftGfxDevice::_tint_32_to_32(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint)
	{
		int tintB = s_mulTab[tint.baseTint.b];
		int tintG = s_mulTab[tint.baseTint.g];
		int tintR = s_mulTab[tint.baseTint.r];
		int tintA = s_mulTab[tint.baseTint.a];

		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				pDst[0] = (pSrc[0] * tintB) >> 16;
				pDst[1] = (pSrc[1] * tintG) >> 16;
				pDst[2] = (pSrc[2] * tintR) >> 16;
				pDst[3] = (pSrc[3] * tintA) >> 16;
				pSrc += pitches.srcX; 
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}

	//____ _move_24_to_32() ____________________________________________________

	void SoftGfxDevice::_move_24_to_32(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for( int x = 0 ; x < lineLength ; x++ )
			{
				pDst[0] = pSrc[0];
				pDst[1] = pSrc[1];
				pDst[2] = pSrc[2];
				pDst[3] = 255;
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}

	//____ _tint_24_to_32() ____________________________________________________

	void SoftGfxDevice::_tint_24_to_32(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint)
	{
		int tintB = s_mulTab[tint.baseTint.b];
		int tintG = s_mulTab[tint.baseTint.g];
		int tintR = s_mulTab[tint.baseTint.r];

		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				pDst[0] = (pSrc[0] * tintB) >> 16;
				pDst[1] = (pSrc[1] * tintG) >> 16;
				pDst[2] = (pSrc[2] * tintR) >> 16;
				pDst[3] = tint.baseTint.a;
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}



	//____ _move_32_to_32() ____________________________________________________

	void SoftGfxDevice::_move_32_to_32(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		uint32_t * pS = (uint32_t*)pSrc;
		uint32_t * pD = (uint32_t*)pDst;

		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				*pD = *pS;
				pS += pitches.srcX / 4;
				pD += pitches.dstX / 4;
			}
			pS += pitches.srcY/4;
			pD += pitches.dstY/4;
		}
	}

	//____ _blend_32_to_32() ____________________________________________________

	void SoftGfxDevice::_blend_32_to_32(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				int alpha = s_mulTab[pSrc[3]];
				int invAlpha = 65536 - alpha;

				pDst[0] = (pDst[0] * invAlpha + pSrc[0] * alpha) >> 16;
				pDst[1] = (pDst[1] * invAlpha + pSrc[1] * alpha) >> 16;
				pDst[2] = (pDst[2] * invAlpha + pSrc[2] * alpha) >> 16;
				pDst[3] = (pDst[3] * invAlpha + 255 * alpha) >> 16;
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY; 
			pDst += pitches.dstY;
		}
	}

	//____ _move_32_to_24() ____________________________________________________

	void SoftGfxDevice::_move_32_to_24(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				pDst[0] = pSrc[0];
				pDst[1] = pSrc[1];
				pDst[2] = pSrc[2];
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}

	//____ _blend_32_to_24() ____________________________________________________

	void SoftGfxDevice::_blend_32_to_24(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				int alpha = s_mulTab[pSrc[3]];
				int invAlpha = 65536 - alpha;

				pDst[0] = (pDst[0] * invAlpha + pSrc[0] * alpha) >> 16;
				pDst[1] = (pDst[1] * invAlpha + pSrc[1] * alpha) >> 16;
				pDst[2] = (pDst[2] * invAlpha + pSrc[2] * alpha) >> 16;
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}

	//____ _add_32_to_24() ____________________________________________________

	void SoftGfxDevice::_add_32_to_24(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				int alpha = s_mulTab[pSrc[3]];

				pDst[0] = limitUint8(pDst[0] + (pSrc[0]*alpha << 16) );
				pDst[1] = limitUint8(pDst[1] + (pSrc[1]*alpha << 16) );
				pDst[2] = limitUint8(pDst[2] + (pSrc[2]*alpha << 16) );
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}

	//____ _sub_32_to_24() ____________________________________________________

	void SoftGfxDevice::_sub_32_to_24(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				int alpha = s_mulTab[pSrc[3]];

				pDst[0] = limitUint8(pDst[0] - (pSrc[0] * alpha << 16));
				pDst[1] = limitUint8(pDst[1] - (pSrc[1] * alpha << 16));
				pDst[2] = limitUint8(pDst[2] - (pSrc[2] * alpha << 16));
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}

	}

	//____ _mul_32_to_24() ____________________________________________________

	void SoftGfxDevice::_mul_32_to_24(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				pDst[0] = (s_mulTab[pDst[0]] * pSrc[0]) >> 16;
				pDst[1] = (s_mulTab[pDst[1]] * pSrc[1]) >> 16;
				pDst[2] = (s_mulTab[pDst[2]] * pSrc[2]) >> 16;
				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}

	//____ _invert_32_to_24() ____________________________________________________

	void SoftGfxDevice::_invert_32_to_24(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& dummy)
	{
		for (int y = 0; y < nLines; y++)
		{
			for (int x = 0; x < lineLength; x++)
			{
				int srcB = s_mulTab[pSrc[0]];
				int srcG = s_mulTab[pSrc[1]];
				int srcR = s_mulTab[pSrc[2]];

				pDst[0] = (srcB * (255 - pDst[0]) + pDst[0] * (65536 - srcB)) >> 16;
				pDst[1] = (srcG * (255 - pDst[1]) + pDst[1] * (65536 - srcG)) >> 16;
				pDst[2] = (srcR * (255 - pDst[2]) + pDst[2] * (65536 - srcR)) >> 16;

				pSrc += pitches.srcX;
				pDst += pitches.dstX;
			}
			pSrc += pitches.srcY;
			pDst += pitches.dstY;
		}
	}


	
	//____ blit() __________________________________________________________________

	void SoftGfxDevice::blit(Surface * _pSrcSurf, const Rect& srcrect, Coord dest)
	{
		if (!_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME))
			return;

		SoftSurface * pSrcSurf = (SoftSurface*)_pSrcSurf;

		if (!m_pCanvasPixels || !pSrcSurf->m_pData)
			return;

		BlitOp_p	pReader = nullptr;
		BlitOp_p	pWriter = nullptr;
		
		ColTrans			colTrans{ m_tintColor, nullptr, nullptr };

		switch (pSrcSurf->m_pixelDescription.format)
		{
			case PixelFormat::BGR_8:
			{
				if (m_tintColor == Color::White)
					pReader = _move_24_to_32;
				else
					pReader = _tint_24_to_32;
			}
			break;

			case PixelFormat::BGRA_8:
			{
				if (m_tintColor == Color::White)
					pReader = _move_32_to_32;
				else
					pReader = _tint_32_to_32;
			}
			break;

			default:
				return;			// ERROR: Unsupported source pixel format!
		}

		pWriter = s_pass2OpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if (pWriter == nullptr)
			return;

		if (pReader == _move_32_to_32)
		{
			_onePassStraightBlit(pWriter, static_cast<SoftSurface*>(_pSrcSurf), srcrect, dest, colTrans);
			return;
		}

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

		pOp(pSrc, pDst, pitches, srcrect.h, srcrect.w, tint);
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

			pReader(pSrc, pChunkBuffer, pitchesPass1, thisChunkLines, srcrect.w, tint);
			pWriter(pChunkBuffer, pDst, pitchesPass2, thisChunkLines, srcrect.w, tint);

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
			pWriter(pChunkBuffer, pDst, pitchesPass2, thisChunkLines, dest.w, tint);

			pos.x += transformMatrix[1][0] * thisChunkLines;
			pos.y += transformMatrix[1][1] * thisChunkLines;

			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}


	
	//____ _tintBlit() _____________________________________________________________
	/*
	void SoftGfxDevice::_tintBlit( const Surface* _pSrcSurf, const Rect& srcrect, int dx, int dy  )
	{
		if( !_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME) )
			return;
	
		SoftSurface * pSrcSurf = (SoftSurface*) _pSrcSurf;
	
		if( !m_pCanvasPixels || !pSrcSurf->m_pData )
			return;
	
		int srcPixelBytes = pSrcSurf->m_pixelDescription.bits/8;
		int dstPixelBytes = m_canvasPixelBits/8;
	
		int	srcPitchAdd = pSrcSurf->m_pitch - srcrect.w*srcPixelBytes;
		int	dstPitchAdd =m_canvasPitch - srcrect.w*dstPixelBytes;
	
		uint8_t * pDst = m_pCanvasPixels + dy *m_canvasPitch + dx * dstPixelBytes;
		uint8_t * pSrc = pSrcSurf->m_pData + srcrect.y * pSrcSurf->m_pitch + srcrect.x * srcPixelBytes;
	
		BlendMode		blendMode = m_blendMode;
		if( srcPixelBytes == 3 && blendMode == BlendMode::Blend && m_tintColor.a == 255 )
			blendMode = BlendMode::Replace;

        
        
		switch( blendMode )
		{
			case BlendMode::Replace:
			{
				if( m_bUseCustomFunctions && m_customFunctions.tintBlitReplace )
				{
					m_customFunctions.tintBlitReplace( pSrcSurf->m_pData, (int) pSrcSurf->pixelFormat(), pSrcSurf->m_pitch, srcrect.x, srcrect.y, srcrect.w, srcrect.h, dx, dy, m_tintColor.argb );
					break;
				}
								
				int tintRed = (int) m_tintColor.r;
				int tintGreen = (int) m_tintColor.g;
				int tintBlue = (int) m_tintColor.b;
	
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = m_pDivTab[pSrc[0]*tintBlue];
						pDst[1] = m_pDivTab[pSrc[1]*tintGreen];
						pDst[2] = m_pDivTab[pSrc[2]*tintRed];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += pitches.srcY;
					pDst += pitches.dstY;
				}
				break;
			}
			case BlendMode::Blend:
			{
                if( m_tintColor.a == 0 )
                    break;
                
				if( m_bUseCustomFunctions && m_customFunctions.tintBlitBlend )
				{
					m_customFunctions.tintBlitBlend( pSrcSurf->m_pData, (int) pSrcSurf->pixelFormat(), pSrcSurf->m_pitch, srcrect.x, srcrect.y, srcrect.w, srcrect.h, dx, dy, m_tintColor.argb );
					break;
				}
	
				
                if( srcPixelBytes == 4 )
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_tintColor.r;
					int tintGreen = (int) m_tintColor.g;
					int tintBlue = (int) m_tintColor.b;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = m_pDivTab[pSrc[3]*tintAlpha];
							int invAlpha = 255-alpha;
	
							int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
							int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
							int srcRed		= m_pDivTab[pSrc[2] * tintRed];
							
	
							pDst[0] = m_pDivTab[ pDst[0]*invAlpha + srcBlue*alpha ];
							pDst[1] = m_pDivTab[ pDst[1]*invAlpha + srcGreen*alpha ];
							pDst[2] = m_pDivTab[ pDst[2]*invAlpha + srcRed*alpha ];
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += pitches.srcY;
						pDst += pitches.dstY;
					}
				}
				else
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_pDivTab[ m_tintColor.r * tintAlpha ];
					int tintGreen = (int) m_pDivTab[ m_tintColor.g * tintAlpha ];
					int tintBlue = (int) m_pDivTab[ m_tintColor.b * tintAlpha ];
					int invAlpha = 255-tintAlpha;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = m_pDivTab[ pDst[0]*invAlpha + pSrc[0]*tintBlue ];
							pDst[1] = m_pDivTab[ pDst[1]*invAlpha + pSrc[1]*tintGreen ];
							pDst[2] = m_pDivTab[ pDst[2]*invAlpha + pSrc[2]*tintRed ];
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += pitches.srcY;
						pDst += pitches.dstY;
					}
				}
				break;
			}
			case BlendMode::Add:
			{
				if( m_bUseCustomFunctions && m_customFunctions.tintBlitAdd )
				{
					m_customFunctions.tintBlitAdd( pSrcSurf->m_pData, (int) pSrcSurf->pixelFormat(), pSrcSurf->m_pitch, srcrect.x, srcrect.y, srcrect.w, srcrect.h, dx, dy, m_tintColor.argb );
					break;
				}
				
                if( m_tintColor.a == 0 )
                    break;
                
                if( srcPixelBytes == 4 )
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_tintColor.r;
					int tintGreen = (int) m_tintColor.g;
					int tintBlue = (int) m_tintColor.b;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = m_pDivTab[ pSrc[3]*tintAlpha ];
	
							int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
							int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
							int srcRed		= m_pDivTab[pSrc[2] * tintRed];
	
							pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[srcBlue*alpha]);
							pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[srcGreen*alpha]);
							pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[ srcRed*alpha]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += pitches.srcY;
						pDst += pitches.dstY;
					}
				}
				else
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
					int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
					int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[pSrc[0]*tintBlue]);
							pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[pSrc[1]*tintGreen]);
							pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[pSrc[2]*tintRed]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += pitches.srcY;
						pDst += pitches.dstY;
					}
				}
				break;
			}
			case BlendMode::Subtract:
			{
				if( m_bUseCustomFunctions && m_customFunctions.tintBlitSubtract )
				{
					m_customFunctions.tintBlitSubtract( pSrcSurf->m_pData, (int) pSrcSurf->pixelFormat(), pSrcSurf->m_pitch, srcrect.x, srcrect.y, srcrect.w, srcrect.h, dx, dy, m_tintColor.argb );
					break;
				}
				
                if( m_tintColor.a == 0 )
                    break;
                
                if( srcPixelBytes == 4 )
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_tintColor.r;
					int tintGreen = (int) m_tintColor.g;
					int tintBlue = (int) m_tintColor.b;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = m_pDivTab[ pSrc[3]*tintAlpha ];
	
							int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
							int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
							int srcRed		= m_pDivTab[pSrc[2] * tintRed];
	
							pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[srcBlue*alpha]);
							pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[srcGreen*alpha]);
							pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[ srcRed*alpha]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += pitches.srcY;
						pDst += pitches.dstY;
					}
				}
				else
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
					int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
					int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[pSrc[0]*tintBlue]);
							pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[pSrc[1]*tintGreen]);
							pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[pSrc[2]*tintRed]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += pitches.srcY;
						pDst += pitches.dstY;
					}
				}
				break;
			}
			case BlendMode::Multiply:
			{
				if( m_bUseCustomFunctions && m_customFunctions.tintBlitMultiply )
				{
					m_customFunctions.tintBlitMultiply( pSrcSurf->m_pData, (int) pSrcSurf->pixelFormat(), pSrcSurf->m_pitch, srcrect.x, srcrect.y, srcrect.w, srcrect.h, dx, dy, m_tintColor.argb );
					break;
				}

				int tintRed = (int) m_tintColor.r;
				int tintGreen = (int) m_tintColor.g;
				int tintBlue = (int) m_tintColor.b;
	
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
						int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
						int srcRed		= m_pDivTab[pSrc[2] * tintRed];
	
	
						pDst[0] = m_pDivTab[srcBlue*pDst[0]];
						pDst[1] = m_pDivTab[srcGreen*pDst[1]];
						pDst[2] = m_pDivTab[srcRed*pDst[2]];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += pitches.srcY;
					pDst += pitches.dstY;
				}
				break;
			}
			case BlendMode::Invert:
			{
				if( m_bUseCustomFunctions && m_customFunctions.tintBlitInvert )
				{
					m_customFunctions.tintBlitInvert( pSrcSurf->m_pData, (int) pSrcSurf->pixelFormat(), pSrcSurf->m_pitch, srcrect.x, srcrect.y, srcrect.w, srcrect.h, dx, dy, m_tintColor.argb );
					break;
				}
				
				int tintRed = (int) m_tintColor.r;
				int tintGreen = (int) m_tintColor.g;
				int tintBlue = (int) m_tintColor.b;
	
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						int srcBlue = m_pDivTab[tintBlue*pSrc[0]];
						int srcGreen = m_pDivTab[tintGreen*pSrc[1]];
						int srcRed = m_pDivTab[tintRed*pSrc[2]];
	
						pDst[0] = m_pDivTab[srcBlue*(255-pDst[0]) + pDst[0]*(255-srcBlue)];
						pDst[1] = m_pDivTab[srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen)];
						pDst[2] = m_pDivTab[srcRed*(255-pDst[2]) + pDst[2]*(255-srcRed)];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += pitches.srcY;
					pDst += pitches.dstY;
				}
				break;
			}
			default:
				break;
		}
	}
	*/
	
	//____ stretchBlit() ___________________________________________________

	void SoftGfxDevice::stretchBlit(Surface * _pSrcSurf, const RectF& source, const Rect& dest)
	{
		if (!_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME))
			return;

		SoftSurface * pSrcSurf = (SoftSurface*)_pSrcSurf;

		if (!m_pCanvasPixels || !pSrcSurf->m_pData)
			return;

		TransformOp_p	pReader = nullptr;
		BlitOp_p	pWriter = nullptr;

		ColTrans			colTrans{ m_tintColor, nullptr, nullptr };

		switch (pSrcSurf->m_pixelDescription.format)
		{
		case PixelFormat::BGR_8:
		{
			if (m_tintColor == Color::White)
				pReader = _stretch_blit < PixelFormat::BGR_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
			else
				pReader = _stretch_blit < PixelFormat::BGR_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		}
		break;

		case PixelFormat::BGRA_8:
		{
			if (m_tintColor == Color::White)
				pReader = _stretch_blit < PixelFormat::BGRA_8, 0, BlendMode::Replace, PixelFormat::BGRA_8>;
			else
				pReader = _stretch_blit < PixelFormat::BGRA_8, 1, BlendMode::Replace, PixelFormat::BGRA_8>;
		}
		break;

		default:
			return;			// ERROR: Unsupported source pixel format!
		}

		pWriter = s_pass2OpTab[(int)m_blendMode][(int)m_pCanvas->pixelFormat()];
		if (pWriter == nullptr)
			return;

		float transform[2][2] = { { source.w / dest.w, 0.f },{ 0.f, source.h / dest.h } };

//		if (pWriter == _move_32_to_32)
		{
			_onePassTransformBlit(pReader, static_cast<SoftSurface*>(_pSrcSurf), source, transform, dest, colTrans);
			return;
		}

//		_twoPassTransformBlit(pReader, pWriter, static_cast<SoftSurface*>(_pSrcSurf), source, transform, dest, colTrans);
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
				s_plotListOpTab[i][j] = nullptr;
				s_waveOpTab[i][j] = nullptr;

				s_clipLineOpTab[i][j] = nullptr;
				s_clipPlotListOpTab[i][j] = nullptr;
			}
		}

		// Init Plot Operation Table

		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _plot_move_32;
		s_plotOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _plot_move_24;

		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _plot_blend_32;
		s_plotOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _plot_blend_24;

		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _plot_add_24;
		s_plotOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _plot_add_24;

		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _plot_sub_24;
		s_plotOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _plot_sub_24;

		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _plot_mul_24;
		s_plotOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _plot_mul_24;

		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _plot_invert_24;
		s_plotOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _plot_invert_24;

		// Init Fill Operation Table

		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _fill_move_32;
		s_fillOpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _fill_move_24;

		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _fill_blend_32;
		s_fillOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _fill_blend_24;

		s_fillOpTab[(int) BlendMode::Add][(int) PixelFormat::BGRA_8] = _fill_add_24;
		s_fillOpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _fill_add_24;

		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _fill_sub_24;
		s_fillOpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _fill_sub_24;

		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _fill_mul_24;
		s_fillOpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _fill_mul_24;

		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _fill_invert_24;
		s_fillOpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _fill_invert_24;

		// Init Straight Blit Pass 2 Operation Table

		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGRA_8] = _move_32_to_32;
		s_pass2OpTab[(int)BlendMode::Replace][(int)PixelFormat::BGR_8] = _move_32_to_24;

		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _blend_32_to_32;
		s_pass2OpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _blend_32_to_24;

		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGRA_8] = _add_32_to_24;
		s_pass2OpTab[(int)BlendMode::Add][(int)PixelFormat::BGR_8] = _add_32_to_24;

		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8] = _sub_32_to_24;
		s_pass2OpTab[(int)BlendMode::Subtract][(int)PixelFormat::BGR_8] = _sub_32_to_24;

		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8] = _mul_32_to_24;
		s_pass2OpTab[(int)BlendMode::Multiply][(int)PixelFormat::BGR_8] = _mul_32_to_24;

		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGRA_8] = _invert_32_to_24;
		s_pass2OpTab[(int)BlendMode::Invert][(int)PixelFormat::BGR_8] = _invert_32_to_24;

		// Init Line Operation Table

		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _drawLineSegment_blend_32;
		s_LineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _drawLineSegment_blend_24;

		// Init ClipLine Operation Table

		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _clipDrawLineSegment_blend_32;
		s_clipLineOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _clipDrawLineSegment_blend_24;		

		// Init PlotList Operation Table

		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _plotlist_blend_32;
		s_plotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _plotlist_blend_24;

		// Init ClipPlotList Operation Table

		s_clipPlotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGRA_8] = _clip_plotlist_blend_32;
		s_clipPlotListOpTab[(int)BlendMode::Blend][(int)PixelFormat::BGR_8] = _clip_plotlist_blend_24;

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
