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
	delete m_pCanvas;
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
				for( int x = 0 ; x < rect.w*pixelBytes ; x+=pixelBytes )
				{
					pDst[x] = fillColor.b;
					pDst[x+1] = fillColor.g;
					pDst[x+2] = fillColor.r;
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
				for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
				{
					pDst[x] = (Uint8) ((pDst[x]*invAlpha + storedBlue) >> 8);
					pDst[x+1] = (Uint8) ((pDst[x+1]*invAlpha + storedGreen) >> 8);
					pDst[x+2] = (Uint8) ((pDst[x+2]*invAlpha + storedRed) >> 8);
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
				for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
				{
					pDst[x] = m_limitTable[pDst[x] + storedBlue];
					pDst[x+1] = m_limitTable[pDst[x+1] + storedGreen];
					pDst[x+2] = m_limitTable[pDst[x+2] + storedRed];
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
				for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
				{
					pDst[x] = (pDst[x] * storedBlue) >> 8;
					pDst[x+1] = (pDst[x+1] * storedGreen) >> 8;
					pDst[x+2] = (pDst[x+2] * storedRed) >> 8;
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
				for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
				{
					pDst[x] = ((255-pDst[x]) * storedBlue + pDst[x] * invertRed) >> 8;
					pDst[x+1] = ((255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen) >> 8;
					pDst[x+2] = ((255-pDst[x+2]) * storedRed + pDst[x+2] * invertBlue) >> 8;
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

void WgGfxDeviceSoft::Blit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  )
{
	if( !_pSrcSurf || !m_pCanvas || _pSrcSurf->Type() != WgSurfaceSoft::GetMyType() )
		return;
		
	WgSurfaceSoft * pSrcSurf = (WgSurfaceSoft*) _pSrcSurf;
	
	if( !m_pCanvas->m_pData || !pSrcSurf->m_pData )
		return;

	int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;
	int dstPixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	
	int	srcPitchAdd = pSrcSurf->m_pitch - srcrect.w*srcPixelBytes;
	int	dstPitchAdd = m_pCanvas->m_pitch - srcrect.w*dstPixelBytes;
	
	Uint8 * pDst = m_pCanvas->m_pData + dy * m_pCanvas->m_pitch + dx * dstPixelBytes;
	Uint8 * pSrc = pSrcSurf->m_pData + srcrect.y * pSrcSurf->m_pitch + srcrect.x * srcPixelBytes;

	WgBlendMode		blendMode = m_blendMode;
	if( srcPixelBytes == 3 && blendMode == WG_BLENDMODE_BLEND && m_tintColor.a == 255 )
		blendMode = WG_BLENDMODE_OPAQUE;

	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			int tintRed = (int) m_tintColor.r;
			int tintGreen = (int) m_tintColor.g;
			int tintBlue = (int) m_tintColor.b;

			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					pDst[0] = (pSrc[0]*tintBlue) >> 8;
					pDst[1] = (pSrc[1]*tintGreen) >> 8;
					pDst[2] = (pSrc[2]*tintRed) >> 8;
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
						int alpha = (pSrc[3]*tintAlpha) >> 8;
						int invAlpha = (255-alpha) << 8;
						
						pDst[0] = (pDst[0]*invAlpha + pSrc[0]*tintBlue*alpha ) >> 16;
						pDst[1] = (pDst[1]*invAlpha + pSrc[1]*tintGreen*alpha ) >> 16;
						pDst[2] = (pDst[2]*invAlpha + pSrc[2]*tintRed*alpha ) >> 16;
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			else
			{
				int tintAlpha = (int) m_tintColor.a;
				int tintRed = (int) m_tintColor.r * tintAlpha;
				int tintGreen = (int) m_tintColor.g * tintAlpha;
				int tintBlue = (int) m_tintColor.b * tintAlpha;
				int invAlpha = (255-tintAlpha) << 8;
				
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = (pDst[0]*invAlpha + pSrc[0]*tintBlue ) >> 16;
						pDst[1] = (pDst[1]*invAlpha + pSrc[1]*tintGreen ) >> 16;
						pDst[2] = (pDst[2]*invAlpha + pSrc[2]*tintRed ) >> 16;
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			break;
		}
		case WG_BLENDMODE_ADD:
		{
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
						int alpha = (pSrc[3]*tintAlpha) >> 8;
						
						pDst[0] = m_limitTable[(pDst[0] + (pSrc[0]*tintBlue*alpha)>>16 )];
						pDst[1] = m_limitTable[(pDst[1] + (pSrc[1]*tintGreen*alpha)>>16 )];
						pDst[2] = m_limitTable[(pDst[2] + (pSrc[2]*tintRed*alpha)>>16 )];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			else
			{				
				int tintAlpha = (int) m_tintColor.a;
				int tintRed = (int) m_tintColor.r * tintAlpha;
				int tintGreen = (int) m_tintColor.g * tintAlpha;
				int tintBlue = (int) m_tintColor.b * tintAlpha;

				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = m_limitTable[(pDst[0] + (pSrc[0]*tintBlue)>>16 )];
						pDst[1] = m_limitTable[(pDst[1] + (pSrc[1]*tintGreen)>>16 )];
						pDst[2] = m_limitTable[(pDst[2] + (pSrc[2]*tintRed)>>16 )];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
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
					pDst[0] = (tintBlue*pDst[0]*pSrc[0]) >> 16;
					pDst[1] = (tintGreen*pDst[1]*pSrc[1]) >> 16;
					pDst[2] = (tintRed*pDst[2]*pSrc[2]) >> 16;
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
					int srcBlue = tintBlue*pSrc[0];
					int srcGreen = tintGreen*pSrc[1];
					int srcRed = tintRed*pSrc[2];

					pDst[0] = (srcBlue*(255-pDst[0]) + pDst[0]*(255-srcBlue)) >> 16;
					pDst[1] = (srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen)) >> 16;
					pDst[2] = (srcRed*(255-pDst[2]) + pDst[2]*(255-srcRed)) >> 16;
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

//____ StretchBlit() ___________________________________________________________

void WgGfxDeviceSoft::StretchBlit( const WgSurface * pSrc, bool bTriLinear, float mipmapBias )
{
	StretchBlit( pSrc, WgRect(0, 0, pSrc->Width(),pSrc->Height()), WgRect(0,0,m_canvasSize.w,m_canvasSize.h), bTriLinear, mipmapBias );
}

void WgGfxDeviceSoft::StretchBlit( const WgSurface * pSrc, const WgRect& dest, bool bTriLinear, float mipmapBias )
{
	StretchBlit( pSrc, WgRect(0, 0, pSrc->Width(),pSrc->Height()), dest, bTriLinear, mipmapBias );
}

void WgGfxDeviceSoft::StretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear, float mipmapBias )
{
	float srcW = src.w;
	float srcH = src.h;

	float destW = dest.w;
	float destH = dest.h;

	if( srcW < destW )
		srcW--;
		
	if( srcH < destH )
		srcH--;
	
	StretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, srcW, srcH, (float) dest.x, (float) dest.y, destW, destH, bTriLinear, mipmapBias );
}

//____ ClipStretchBlit() _______________________________________________________

void WgGfxDeviceSoft::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->Width(), pSrc->Height()), WgRect( 0,0,m_canvasSize), bTriLinear, mipBias );
}

void WgGfxDeviceSoft::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->Width(), pSrc->Height()), dest, bTriLinear, mipBias );
}

void WgGfxDeviceSoft::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, (float)src.x, (float)src.y, (float)src.w, (float)src.h, (float)dest.x, (float)dest.y, (float)dest.w, (float)dest.h, false );
}

void WgGfxDeviceSoft::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias)
{
	if( sw < dw )
		sw--; 
		
	if( sh < dh )
		sh--;

	float cx = std::max(float(clip.x), dx);
	float cy = std::max(float(clip.y), dy);
	float cw = std::min(float(clip.x + clip.w), dx + dw) - cx;
	float ch = std::min(float(clip.y + clip.h), dy + dh) - cy;

	if(cw <= 0 || ch <= 0)
		return;

	if( dw > cw )
	{
		float	sdxr = sw / dw;			// Source/Destination X Ratio.

		sw = sdxr * cw;

		if( dx < cx )
			sx += sdxr * (cx - dx);
	}

	if( dh > ch )
	{
		float	sdyr = sh / dh;			// Source/Destination Y Ratio.

		sh = sdyr * ch;

		if( dy < cy )
			sy += sdyr * (cy - dy);
	}

	StretchBlitSubPixel( pSrc, sx, sy, sw, sh, cx, cy, cw, ch, bTriLinear, mipBias );
}


//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceSoft::StretchBlitSubPixel( const WgSurface * _pSrcSurf, float sx, float sy, float sw, float sh,
						   		 float _dx, float _dy, float _dw, float _dh, bool bTriLinear, float mipBias )
{
	if( !_pSrcSurf || !m_pCanvas || _pSrcSurf->Type() != WgSurfaceSoft::GetMyType() )
		return;
		
	WgSurfaceSoft * pSrcSurf = (WgSurfaceSoft*) _pSrcSurf;
	
	if( !m_pCanvas->m_pData || !pSrcSurf->m_pData )
		return;

	int dx = (int) _dx;
	int dy = (int) _dy;
	int dw = (int) _dw;
	int dh = (int) _dh;

	int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;
	int dstPixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	
	int	srcPitch = pSrcSurf->m_pitch;
	int	dstPitch = m_pCanvas->m_pitch;
	
	//...
	int tintRed = (int) m_tintColor.r;
	int tintGreen = (int) m_tintColor.g;
	int tintBlue = (int) m_tintColor.b;	
	//
	
	int ofsY = (int) (sy*32768);		// We use 15 binals for all calculations
	int incY = (int) (sh*32768/dh);

	for( int y = 0 ; y < dh ; y++ )
	{		
		int fracY2 = ofsY & 0x7FFF;
		int fracY1 = 32768 - fracY2;

		int ofsX = (int) (sx*32768);
		int incX = (int) (sw*32768/dw);

		Uint8 * pDst = m_pCanvas->m_pData + (dy+y) * m_pCanvas->m_pitch + dx * dstPixelBytes;
		Uint8 * pSrc = pSrcSurf->m_pData + (ofsY>>15) * pSrcSurf->m_pitch;
		
		for( int x = 0 ; x < dw ; x++ )
		{
			int fracX2 = ofsX & 0x7FFF;
			int fracX1 = 32768 - fracX2;
			
			Uint8 * p = pSrc + (ofsX >> 15)*srcPixelBytes;

			int mul11 = fracX1*fracY1 >> 15;
			int mul12 = fracX2*fracY1 >> 15;
			int mul21 = fracX1*fracY2 >> 15;
			int mul22 = fracX2*fracY2 >> 15;
			
			int srcBlue = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;
			p++;
			int srcGreen = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;
			p++;
			int srcRed = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;
			p++;
			int srcAlpha = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;

			//...
			pDst[0] = (srcBlue*tintBlue) >> 8;
			pDst[1] = (srcGreen*tintGreen) >> 8;
			pDst[2] = (srcRed*tintRed) >> 8;
			//...		   
			
			ofsX += incX;
			pDst += dstPixelBytes;
		}		
		ofsY += incY;
	}
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
