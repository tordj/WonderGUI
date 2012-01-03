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

//____ Constructor _____________________________________________________________

WgGfxDeviceSoft::WgGfxDeviceSoft() : WgGfxDevice(WgSize(0,0))
{
	m_pCanvas = 0;
}

WgGfxDeviceSoft::WgGfxDeviceSoft( WgSurfaceSoft * pCanvas ) : WgGfxDevice( pCanvas?pCanvas->Size():WgSize(); )
{
	m_pCanvas = pCanvas;
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
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * pDst = m_pCanvas->m_pData + rect.y * m_pCanvas->m_pitch + rect.x * pixelBytes;
	
	
	switch( m_blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w ; x++ )
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
			for( int y = 0 ; y < rect.h ; y++ )
			{
				for( int x = 0 ; x < rect.w ; x++ )
				{
					pDst[x] = ((((int)pDst[x])*(255-fillColor.a) + (((int)fillColor.r) * fillColor.a)) >> 8;
					pDst[x+1] = fillColor.g;
					pDst[x+2] = fillColor.b;
					x+= pixelBytes;
				}
				pDst += m_pCanvas->m_pitch;
			}
			break;
		case WG_BLENDMODE_ADD:
			break;
		case WG_BLENDMODE_MULTIPLY:
			break;
		case WG_BLENDMODE_INVERT:
			break;
		default:
			break;
	}
	
}




//____ Blit() __________________________________________________________________

void WgGfxDeviceSoft::Blit( const WgSurface* src, const WgRect& srcrect, int dx, int dy  )
{
}

//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceSoft::StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
{
}
