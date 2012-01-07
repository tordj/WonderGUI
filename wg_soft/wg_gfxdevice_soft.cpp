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

#include <wg_gfxdevice_soft.h>
#include <wg_surface_soft.h>

//____ Constructor _____________________________________________________________

WgGfxDeviceSoft::WgGfxDeviceSoft() : WgGfxDevice(WgSize(0,0))
{
	m_pCanvas = 0;
	_initTables();
}

WgGfxDeviceSoft::WgGfxDeviceSoft( WgSurfaceSoft * pCanvas ) : WgGfxDevice( pCanvas?pCanvas->Size():WgSize() )
{
	m_pCanvas = pCanvas;
	_initTables();
}

//____ Destructor ______________________________________________________________

WgGfxDeviceSoft::~WgGfxDeviceSoft()
{
}

//____ SetCanvas() _______________________________________________________________

void WgGfxDeviceSoft::SetCanvas( WgSurfaceSoft * pCanvas )
{
	m_pCanvas = pCanvas;
	if( pCanvas )
		m_canvasSize = pCanvas->Size();
	else
		m_canvasSize = WgSize();
}

//____ Fill() ____________________________________________________________________

void WgGfxDeviceSoft::Fill( const WgRect& rect, const WgColor& col )
{
	if( !m_pCanvas || !m_pCanvas->m_pData )
		return;

	WgColor fillColor = col * m_tintColor;

	// Skip calls that won't affect destination
	
	if( fillColor.a == 0 && (m_blendMode == WG_BLENDMODE_BLEND || m_blendMode == WG_BLENDMODE_ADD) )
		return;
	
	// Optimize calls
	
	WgBlendMode blendMode = m_blendMode;
	if( blendMode == WG_BLENDMODE_BLEND && fillColor.a == 255 )
		blendMode = WG_BLENDMODE_OPAQUE;
		
	//
	
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * pDst = m_pCanvas->m_pData + rect.y * m_pCanvas->m_pitch + rect.x * pixelBytes;
	
	
	
	
	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w*pixelBytes ; x++ )
				{
					pDst[x] = fillColor.r;
					pDst[x+1] = fillColor.g;
					pDst[x+2] = fillColor.b;
					x+= pixelBytes;
				}
				pDst += m_pCanvas->m_pitch;
			}
		}
		break;
		case WG_BLENDMODE_BLEND:
		{
			int storedRed = ((int)fillColor.r) * fillColor.a;
			int storedGreen = ((int)fillColor.g) * fillColor.a;
			int storedBlue = ((int)fillColor.b) * fillColor.a;
			int invAlpha = 255-fillColor.a;

			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w*pixelBytes ; x++ )
				{
					pDst[x] = (Uint8) ((pDst[x]*invAlpha + storedRed) >> 8);
					pDst[x+1] = (Uint8) ((pDst[x+1]*invAlpha + storedGreen) >> 8);
					pDst[x+2] = (Uint8) ((pDst[x+2]*invAlpha + storedBlue) >> 8);
					x+= pixelBytes;
				}
				pDst += m_pCanvas->m_pitch;
			}
			break;
		}
		case WG_BLENDMODE_ADD:
		{
			int storedRed = (((int)fillColor.r) * fillColor.a) >> 8;
			int storedGreen = (((int)fillColor.g) * fillColor.a) >> 8;
			int storedBlue = (((int)fillColor.b) * fillColor.a) >> 8;

			if( storedRed + storedGreen + storedBlue == 0 )
				return;
				
			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w*pixelBytes ; x++ )
				{
					pDst[x] = m_limitTable[pDst[x] + storedRed];
					pDst[x+1] = m_limitTable[pDst[x+1] + storedGreen];
					pDst[x+2] = m_limitTable[pDst[x+2] + storedBlue];
					x+= pixelBytes;
				}
				pDst += m_pCanvas->m_pitch;			
			}
			break;
		}
		case WG_BLENDMODE_MULTIPLY:
		{
			int storedRed = (int)fillColor.r;
			int storedGreen = (int)fillColor.g;
			int storedBlue = (int)fillColor.b;

			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w*pixelBytes ; x++ )
				{
					pDst[x] = (pDst[x] * storedRed) >> 8;
					pDst[x+1] = (pDst[x+1] * storedGreen) >> 8;
					pDst[x+2] = (pDst[x+2] * storedBlue) >> 8;
					x+= pixelBytes;
				}
				pDst += m_pCanvas->m_pitch;			
			}
			break;
		}
		case WG_BLENDMODE_INVERT:
		{
			int storedRed = (int)fillColor.r;
			int storedGreen = (int)fillColor.g;
			int storedBlue = (int)fillColor.b;

			int invertRed = 255 - (int)fillColor.r;
			int invertGreen = 255 - (int)fillColor.g;
			int invertBlue = 255 - (int)fillColor.b;


			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w*pixelBytes ; x++ )
				{
					pDst[x] = ((255-pDst[x]) * storedRed + pDst[x] * invertRed) >> 8;
					pDst[x+1] = ((255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen) >> 8;
					pDst[x+2] = ((255-pDst[x+2]) * storedBlue + pDst[x+2] * invertBlue) >> 8;
					x+= pixelBytes;
				}
				pDst += m_pCanvas->m_pitch;			
			}
			break;
		}
		default:
			break;
	}
}

//____ Blit() __________________________________________________________________

void WgGfxDeviceSoft::Blit( const WgSurface* pSrcSurf, const WgRect& srcrect, int dx, int dy  )
{
	
	
	if( !m_pCanvas || !m_pCanvas->m_pData || !pSrcSurf || !pSrcSurf->m_pData )
		return;

	int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;
	int dstPixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	
	int	srcPitchAdd = pSrcSurf->m_pitch - srcrect.w*srcPixelBytes;
	int	dstPitchAdd = m_pCanvas->m_pitch - srcrect.w*dstPixelBytes;
	
	Uint8 * pDst = m_pCanvas->m_pData + dy * m_pCanvas->m_pitch + dx * pixelBytes;
	Uint8 * pSrc = pSrcSurf->m_pData + srcrect.y * pSrcSurf->m_pitch + srcrect.x * pixelBytes;

	switch( m_blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					pDst[0] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[2];
					pSrc += srcPixelBytes;
					pDst += dstPixelBytes;
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}
		case WG_BLENDMODE_BLEND:
		{
			int tintAlpha = (int) m_tintColor.a;
			int tintRed = (int) m_tintColor.r;
			int tintGreen = (int) m_tintColor.g;
			int tintBlue = (int) m_tintColor.b;
			
			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					int alpha = (pSrc[3]*tintAlpha) >> 8;
					int invAlpha = 255-alpha;
					
					pDst[0] = (pDst[0]*invAlpha + (pSrc[0]*tintRed*alpha)>>8 ) >> 8;
					pDst[1] = (pDst[1]*invAlpha + (pSrc[1]*tintGreen*alpha)>>8 ) >> 8;
					pDst[2] = (pDst[2]*invAlpha + (pSrc[2]*tintBlue*alpha)>>8 ) >> 8;
					pSrc += srcPixelBytes;
					pDst += dstPixelBytes;
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}
		case WG_BLENDMODE_ADD:
		{
			int tintAlpha = (int) m_tintColor.a;
			int tintRed = (int) m_tintColor.r;
			int tintGreen = (int) m_tintColor.g;
			int tintBlue = (int) m_tintColor.b;
			
			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					int alpha = (pSrc[3]*tintAlpha) >> 8;
					
					pDst[0] = m_limitTable[(pDst[0] + (pSrc[0]*tintRed*alpha)>>16 )];
					pDst[1] = m_limitTable[(pDst[1] + (pSrc[1]*tintGreen*alpha)>>16 )];
					pDst[2] = m_limitTable[(pDst[2] + (pSrc[2]*tintBlue*alpha)>>16 )];
					pSrc += srcPixelBytes;
					pDst += dstPixelBytes;
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}
		case WG_BLENDMODE_MULTIPLY:
		{
			int tintRed = (int) m_tintColor.r;
			int tintGreen = (int) m_tintColor.g;
			int tintBlue = (int) m_tintColor.b;
			
			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					pDst[0] = (tintRed*pDst[0]*pSrc[0]) >> 16;
					pDst[1] = (tintGreen*pDst[1]*pSrc[1]) >> 16;
					pDst[2] = (tintBlue*pDst[2]*pSrc[2]) >> 16;
					pSrc += srcPixelBytes;
					pDst += dstPixelBytes;
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}
		case WG_BLENDMODE_INVERT:
		{
			int tintRed = (int) m_tintColor.r;
			int tintGreen = (int) m_tintColor.g;
			int tintBlue = (int) m_tintColor.b;

			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					int srcRed = tintRed*pSrc[0];
					int srcGreen = tintGreen*pSrc[1];
					int srcBlue = tintBlue*pSrc[2];

					pDst[0] = (srcRed*(255-pDst[0]) + pDst[0]*(255-srcRed)) >> 16;
					pDst[1] = (srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen)) >> 16;
					pDst[2] = (srcBlue*(255-pDst[2]) + pDst[2]*(255-srcBlue)) >> 16;
					pSrc += srcPixelBytes;
					pDst += dstPixelBytes;
				}
				pSrc += srcPitchAdd;
				pDst += dstPitchAdd;
			}
			break;
		}
		default:
			break;
	}
}

//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceSoft::StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
{
}

//____ _initTables() ___________________________________________________________

void WgGfxDeviceSoft::_initTables()
{
	// Init limitTable
	
	for( int i = 0 ; i < 256 ; i++ )
		m_limitTable[i] = i;

	for( int i = 0 ; i < 256 ; i++ )
		m_limitTable[i] = 255;
		
}
