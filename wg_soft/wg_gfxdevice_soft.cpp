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
#include <math.h>

#include <assert.h>

#define NB_CURVETAB_ENTRIES	1024


//____ Constructor _____________________________________________________________

WgGfxDeviceSoft::WgGfxDeviceSoft() : WgGfxDevice(WgSize(0,0))
{
	m_bBilinearFiltering = true;
	m_pCanvas = 0;
	_initTables();
	_genCurveTab();
}

WgGfxDeviceSoft::WgGfxDeviceSoft( WgSurfaceSoft * pCanvas ) : WgGfxDevice( pCanvas?pCanvas->Size():WgSize() )
{
	m_bBilinearFiltering = true;
	m_pCanvas = pCanvas;
	_initTables();
	_genCurveTab();
}

//____ Destructor ______________________________________________________________

WgGfxDeviceSoft::~WgGfxDeviceSoft()
{
	delete m_pCanvas;
	delete m_pCurveTab;
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
					pDst[x] = ((255-pDst[x]) * storedBlue + pDst[x] * invertBlue) >> 8;
					pDst[x+1] = ((255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen) >> 8;
					pDst[x+2] = ((255-pDst[x+2]) * storedRed + pDst[x+2] * invertRed) >> 8;
				}
				pDst += m_pCanvas->m_pitch;
			}
			break;
		}
		default:
			break;
	}
}

//____ FillSubPixel() ____________________________________________________________________

void WgGfxDeviceSoft::FillSubPixel( const WgRectF& rect, const WgColor& col )
{
	if( !m_pCanvas || !m_pCanvas->m_pData )
		return;

	WgColor fillColor = col * m_tintColor;

	// Skip calls that won't affect destination

	if( fillColor.a == 0 && (m_blendMode == WG_BLENDMODE_BLEND || m_blendMode == WG_BLENDMODE_ADD) )
		return;

	// Fill all but anti-aliased edges

	int x1 = (int) (rect.x + 0.999f);
	int y1 = (int) (rect.y + 0.999f);
	int x2 = (int) (rect.x + rect.w);
	int y2 = (int) (rect.y + rect.h);

	Fill( WgRect( x1,y1,x2-x1,y2-y1 ), col );

	// Optimize calls

	WgBlendMode blendMode = m_blendMode;
	if( blendMode == WG_BLENDMODE_BLEND && fillColor.a == 255 )
		blendMode = WG_BLENDMODE_OPAQUE;

	// Draw the sides

	int aaLeft = (256 - (int)(rect.x * 256)) & 0xFF;
	int aaTop = (256 - (int)(rect.y * 256)) & 0xFF;
	int aaRight = ((int)((rect.x + rect.w) * 256)) & 0xFF;
	int aaBottom = ((int)((rect.y + rect.h) * 256)) & 0xFF;

	if( aaTop != 0 )
		_drawHorrVertLineAA( x1, (int) rect.y, x2-x1, fillColor, blendMode, aaTop, WG_HORIZONTAL );

	if( aaBottom != 0 )
		_drawHorrVertLineAA( x1, (int) y2, x2-x1, fillColor, blendMode, aaBottom, WG_HORIZONTAL );

	if( aaLeft != 0 )
		_drawHorrVertLineAA( (int) rect.x, y1, y2-y1, fillColor, blendMode, aaLeft, WG_VERTICAL );

	if( aaRight != 0 )
		_drawHorrVertLineAA( (int) x2, y1, y2-y1, fillColor, blendMode, aaRight, WG_VERTICAL );

	// Draw corner pieces

	int aaTopLeft = aaTop * aaLeft / 256;
	int aaTopRight = aaTop * aaRight / 256;
	int aaBottomLeft = aaBottom * aaLeft / 256;
	int aaBottomRight = aaBottom * aaRight / 256;

	if( aaTopLeft != 0 )
		_plotAA( (int) rect.x, (int) rect.y, fillColor, blendMode, aaTopLeft );

	if( aaTopRight != 0 )
		_plotAA( x2, (int) rect.y, fillColor, blendMode, aaTopRight );

	if( aaBottomLeft != 0 )
		_plotAA( (int) rect.x, y2, fillColor, blendMode, aaBottomLeft );

	if( aaBottomRight != 0 )
		_plotAA( x2, y2, fillColor, blendMode, aaBottomRight );
}

//____ ClipDrawHorrLine() _____________________________________________________

void WgGfxDeviceSoft::ClipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col )
{
	if( start.y < clip.y || start.y >= clip.y + clip.h || start.x >= clip.x + clip.w || start.x + length <= clip.x )
		return;

	int x = start.x;

	if( x < clip.x )
	{
		length = start.x + length - clip.x;
		x = clip.x;
	}

	if( x + length > clip.x + clip.w )
		length = clip.x + clip.w - x;

	_drawHorrVertLine( x, start.y, length, col, WG_HORIZONTAL );
}

//____ ClipDrawVertLine() _____________________________________________________

void WgGfxDeviceSoft::ClipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col )
{
	if( start.x < clip.x || start.x >= clip.x + clip.w || start.y >= clip.y + clip.h || start.y + length <= clip.y )
		return;

	int y = start.y;

	if( y < clip.y )
	{
		length = start.y + length - clip.y;
		y = clip.y;
	}

	if( y + length > clip.y + clip.h )
		length = clip.y + clip.h - y;

	_drawHorrVertLine( start.x, y, length, col, WG_VERTICAL );
}

//____ ClipPlotSoftPixels() _______________________________________________________

void WgGfxDeviceSoft::ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness )
{
	int pitch = m_pCanvas->m_pitch;
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;

	int offset[4];

	offset[0] = -pixelBytes;
	offset[1] = -pitch;
	offset[2] = pixelBytes;
	offset[3] = pitch;

	int alpha = (int) (256*(thickness - 1.f)/2);

	int storedRed = ((int)col.r) * alpha;
	int storedGreen = ((int)col.g) * alpha;
	int storedBlue = ((int)col.b) * alpha;
	int invAlpha = 255-alpha;

	int yp = pCoords[0].y;

	for( int i = 0 ; i < nCoords ; i++ )
	{
		int x = pCoords[i].x;
		int begY;
		int endY;

		if( yp > pCoords[i].y )
		{
			begY = pCoords[i].y;
			endY = yp-1;
		}
		else if( pCoords[i].y > yp )
		{
			begY = yp+1;
			endY = pCoords[i].y;
		}
		else
		{
			begY = endY = yp;
		}

		for( int y = begY ; y <= endY ; y++ )
		{
			Uint8 * pDst = m_pCanvas->m_pData + y * m_pCanvas->m_pitch + pCoords[i].x * pixelBytes;

			if( y > clip.y && y < clip.y + clip.h -1 && x > clip.x && x < clip.x + clip.w -1 )
			{
				pDst[0] = col.b;
				pDst[1] = col.g;
				pDst[2] = col.r;

				for( int x = 0 ; x < 4 ; x++ )
				{
					int ofs = offset[x];
					pDst[ofs] = (Uint8) ((pDst[ofs]*invAlpha + storedBlue) >> 8);
					pDst[ofs+1] = (Uint8) ((pDst[ofs+1]*invAlpha + storedGreen) >> 8);
					pDst[ofs+2] = (Uint8) ((pDst[ofs+2]*invAlpha + storedRed) >> 8);
				}
			}
		}

		yp = pCoords[i].y;
	}
}
/*
void WgGfxDeviceSoft::ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness )
{
	int pitch = m_pCanvas->m_pitch;
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;

	int offset[4];

	offset[0] = -pixelBytes;
	offset[1] = -pitch;
	offset[2] = pixelBytes;
	offset[3] = pitch;

	int alpha = (int) (256*(thickness - 1.f)/2);

	int storedRed = ((int)col.r) * alpha;
	int storedGreen = ((int)col.g) * alpha;
	int storedBlue = ((int)col.b) * alpha;
	int invAlpha = 255-alpha;


	for( int i = 0 ; i < nCoords ; i++ )
	{
		Uint8 * pDst = m_pCanvas->m_pData + pCoords[i].y * m_pCanvas->m_pitch + pCoords[i].x * pixelBytes;

		pDst[0] = col.b;
		pDst[1] = col.g;
		pDst[2] = col.r;

		for( int x = 0 ; x < 4 ; x++ )
		{
			int ofs = offset[x];
			pDst[ofs] = (Uint8) ((pDst[ofs]*invAlpha + storedBlue) >> 8);
			pDst[ofs+1] = (Uint8) ((pDst[ofs+1]*invAlpha + storedGreen) >> 8);
			pDst[ofs+2] = (Uint8) ((pDst[ofs+2]*invAlpha + storedRed) >> 8);
		}
	}
}
*/

//____ _drawHorrVertLine() ________________________________________________

void WgGfxDeviceSoft::_drawHorrVertLine( int _x, int _y, int _length, const WgColor& _col, WgOrientation orientation  )
{
	if( !m_pCanvas || !m_pCanvas->m_pData || _length <= 0  )
		return;

	WgColor fillColor = _col * m_tintColor;

	// Skip calls that won't affect destination

	if( fillColor.a == 0 && (m_blendMode == WG_BLENDMODE_BLEND || m_blendMode == WG_BLENDMODE_ADD) )
		return;

	// Optimize calls

	WgBlendMode blendMode = m_blendMode;
	if( blendMode == WG_BLENDMODE_BLEND && fillColor.a == 255 )
		blendMode = WG_BLENDMODE_OPAQUE;

	//

	int pitch = m_pCanvas->m_pitch;
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * pDst = m_pCanvas->m_pData + _y * m_pCanvas->m_pitch + _x * pixelBytes;

	int inc;

	if( orientation == WG_HORIZONTAL )
		inc = pixelBytes;
	else
		inc = pitch;

	//

	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			for( int x = 0 ; x < _length*inc ; x+=inc )
			{
				pDst[x] = fillColor.b;
				pDst[x+1] = fillColor.g;
				pDst[x+2] = fillColor.r;
			}
		}
		break;
		case WG_BLENDMODE_BLEND:
		{
			int storedRed = ((int)fillColor.r) * fillColor.a;
			int storedGreen = ((int)fillColor.g) * fillColor.a;
			int storedBlue = ((int)fillColor.b) * fillColor.a;
			int invAlpha = 255-fillColor.a;

			for( int x = 0 ; x < _length*inc ; x+=inc )
			{
				pDst[x] = (Uint8) ((pDst[x]*invAlpha + storedBlue) >> 8);
				pDst[x+1] = (Uint8) ((pDst[x+1]*invAlpha + storedGreen) >> 8);
				pDst[x+2] = (Uint8) ((pDst[x+2]*invAlpha + storedRed) >> 8);
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

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = m_limitTable[pDst[x] + storedBlue];
				pDst[x+1] = m_limitTable[pDst[x+1] + storedGreen];
				pDst[x+2] = m_limitTable[pDst[x+2] + storedRed];
			}
			break;
		}
		case WG_BLENDMODE_MULTIPLY:
		{
			int storedRed = (int)fillColor.r;
			int storedGreen = (int)fillColor.g;
			int storedBlue = (int)fillColor.b;

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = (pDst[x] * storedBlue) >> 8;
				pDst[x+1] = (pDst[x+1] * storedGreen) >> 8;
				pDst[x+2] = (pDst[x+2] * storedRed) >> 8;
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


			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = ((255-pDst[x]) * storedBlue + pDst[x] * invertBlue) >> 8;
				pDst[x+1] = ((255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen) >> 8;
				pDst[x+2] = ((255-pDst[x+2]) * storedRed + pDst[x+2] * invertRed) >> 8;
			}
			break;
		}
		default:
			break;
	}
}

//____ _drawHorrVertLineAA() ________________________________________________

void WgGfxDeviceSoft::_drawHorrVertLineAA( int _x, int _y, int _length, const WgColor& _col, WgBlendMode blendMode, int _aa, WgOrientation orientation )
{
	int pitch = m_pCanvas->m_pitch;
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * pDst = m_pCanvas->m_pData + _y * m_pCanvas->m_pitch + _x * pixelBytes;

	int inc;
	if( orientation == WG_HORIZONTAL )
		inc = pixelBytes;
	else
		inc = pitch;

	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			int storedRed = ((int)_col.r) * _aa;
			int storedGreen = ((int)_col.g) * _aa;
			int storedBlue = ((int)_col.b) * _aa;
			int invAlpha = 255- _aa;

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = (Uint8) ((pDst[x]*invAlpha + storedBlue) >> 8);
				pDst[x+1] = (Uint8) ((pDst[x+1]*invAlpha + storedGreen) >> 8);
				pDst[x+2] = (Uint8) ((pDst[x+2]*invAlpha + storedRed) >> 8);
			}
			break;
		}
		case WG_BLENDMODE_BLEND:
		{
			int aa = _col.a * _aa;

			int storedRed = (((int)_col.r) * aa) >> 8;
			int storedGreen = (((int)_col.g) * aa) >> 8;
			int storedBlue = (((int)_col.b) * aa) >> 8;
			int invAlpha = 255-(aa>>8);

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = (Uint8) ((pDst[x]*invAlpha + storedBlue) >> 8);
				pDst[x+1] = (Uint8) ((pDst[x+1]*invAlpha + storedGreen) >> 8);
				pDst[x+2] = (Uint8) ((pDst[x+2]*invAlpha + storedRed) >> 8);
			}
			break;
		}
		case WG_BLENDMODE_ADD:
		{
			int aa = _col.a * _aa;

			int storedRed = (((int)_col.r) * aa) >> 16;
			int storedGreen = (((int)_col.g) * aa) >> 16;
			int storedBlue = (((int)_col.b) * aa) >> 16;

			if( storedRed + storedGreen + storedBlue == 0 )
				return;

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = m_limitTable[pDst[x] + storedBlue];
				pDst[x+1] = m_limitTable[pDst[x+1] + storedGreen];
				pDst[x+2] = m_limitTable[pDst[x+2] + storedRed];
			}
			break;
		}
		case WG_BLENDMODE_MULTIPLY:
		{
			int storedRed = (int)_col.r;
			int storedGreen = (int)_col.g;
			int storedBlue = (int)_col.b;

			int invAlpha = (255 - _aa) << 8;

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = ( (pDst[x]*invAlpha) + (_aa * pDst[x] * storedBlue) ) >> 16;
				pDst[x+1] = ( (pDst[x+1]*invAlpha) + (_aa * pDst[x+1] * storedGreen) ) >> 16;
				pDst[x+2] = ( (pDst[x+2]*invAlpha) + (_aa * pDst[x+2] * storedRed) ) >> 16;
			}
			break;
		}
		case WG_BLENDMODE_INVERT:
		{
			int storedRed = (int)_col.r;
			int storedGreen = (int)_col.g;
			int storedBlue = (int)_col.b;

			int invertRed = 255 - (int)_col.r;
			int invertGreen = 255 - (int)_col.g;
			int invertBlue = 255 - (int)_col.b;

			int invAlpha = (255 - _aa) << 8;

			for( int x = 0 ; x < _length*inc ; x+= inc )
			{
				pDst[x] = ( (pDst[x]*invAlpha) + _aa * ((255-pDst[x]) * storedBlue + pDst[x] * invertBlue) ) >> 16;
				pDst[x+1] = ( (pDst[x+1]*invAlpha) + _aa * ((255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen) )  >> 16;
				pDst[x+2] = ( (pDst[x+2]*invAlpha) + _aa * ((255-pDst[x+2]) * storedRed + pDst[x+2] * invertRed) ) >> 16;
			}
			break;
		}
		default:
			break;
	}
}

//____ _plotAA() ________________________________________________

void WgGfxDeviceSoft::_plotAA( int _x, int _y, const WgColor& _col, WgBlendMode blendMode, int _aa )
{
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * pDst = m_pCanvas->m_pData + _y * m_pCanvas->m_pitch + _x * pixelBytes;

	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			int storedRed = ((int)_col.r) * _aa;
			int storedGreen = ((int)_col.g) * _aa;
			int storedBlue = ((int)_col.b) * _aa;
			int invAlpha = 255- _aa;

			pDst[0] = (Uint8) ((pDst[0]*invAlpha + storedBlue) >> 8);
			pDst[1] = (Uint8) ((pDst[1]*invAlpha + storedGreen) >> 8);
			pDst[2] = (Uint8) ((pDst[2]*invAlpha + storedRed) >> 8);
			break;
		}
		case WG_BLENDMODE_BLEND:
		{
			int aa = _col.a * _aa;

			int storedRed = (((int)_col.r) * aa) >> 8;
			int storedGreen = (((int)_col.g) * aa) >> 8;
			int storedBlue = (((int)_col.b) * aa) >> 8;
			int invAlpha = 255-(aa>>8);

			pDst[0] = (Uint8) ((pDst[0]*invAlpha + storedBlue) >> 8);
			pDst[1] = (Uint8) ((pDst[1]*invAlpha + storedGreen) >> 8);
			pDst[2] = (Uint8) ((pDst[2]*invAlpha + storedRed) >> 8);
			break;
		}
		case WG_BLENDMODE_ADD:
		{
			int aa = _col.a * _aa;

			int storedRed = (((int)_col.r) * aa) >> 16;
			int storedGreen = (((int)_col.g) * aa) >> 16;
			int storedBlue = (((int)_col.b) * aa) >> 16;

			if( storedRed + storedGreen + storedBlue == 0 )
				return;

			pDst[0] = m_limitTable[pDst[0] + storedBlue];
			pDst[1] = m_limitTable[pDst[1] + storedGreen];
			pDst[2] = m_limitTable[pDst[2] + storedRed];
			break;
		}
		case WG_BLENDMODE_MULTIPLY:
		{
			int storedRed = (int)_col.r;
			int storedGreen = (int)_col.g;
			int storedBlue = (int)_col.b;

			int invAlpha = (255 - _aa) << 8;

			pDst[0] = ( (pDst[0]*invAlpha) + (_aa * pDst[0] * storedBlue) ) >> 16;
			pDst[1] = ( (pDst[1]*invAlpha) + (_aa * pDst[1] * storedGreen) ) >> 16;
			pDst[2] = ( (pDst[2]*invAlpha) + (_aa * pDst[2] * storedRed) ) >> 16;
			break;
		}
		case WG_BLENDMODE_INVERT:
		{
			int storedRed = (int)_col.r;
			int storedGreen = (int)_col.g;
			int storedBlue = (int)_col.b;

			int invertRed = 255 - (int)_col.r;
			int invertGreen = 255 - (int)_col.g;
			int invertBlue = 255 - (int)_col.b;

			int invAlpha = (255 - _aa) << 8;

			pDst[0] = ( (pDst[0]*invAlpha) + _aa * ((255-pDst[0]) * storedBlue + pDst[0] * invertBlue) ) >> 16;
			pDst[1] = ( (pDst[1]*invAlpha) + _aa * ((255-pDst[1]) * storedGreen + pDst[1] * invertGreen) )  >> 16;
			pDst[2] = ( (pDst[2]*invAlpha) + _aa * ((255-pDst[2]) * storedRed + pDst[2] * invertRed) ) >> 16;
			break;
		}
		default:
			break;
	}
}


//____ _genCurveTab() ___________________________________________________________

void WgGfxDeviceSoft::_genCurveTab()
{
	m_pCurveTab = new int[NB_CURVETAB_ENTRIES];

	double factor = 3.14159265 / (2.0 * NB_CURVETAB_ENTRIES);

	for( int i = 0 ; i < NB_CURVETAB_ENTRIES ; i++ )
	{
		double y = 1.f - i/(double)NB_CURVETAB_ENTRIES;
		m_pCurveTab[i] = (int) (sqrt(1.f - y*y)*65536.f);
	}
}


//____ DrawElipse() _______________________________________________________________

void WgGfxDeviceSoft::DrawElipse( const WgRect& rect, WgColor color )
{
	if( rect.h < 2 || rect.w < 1 )
		return;

	int sectionHeight = rect.h/2;
	int maxWidth = rect.w/2;

	Uint8 * pLineBeg = m_pCanvas->m_pData + rect.y * m_pCanvas->m_pitch;
	int pitch = m_pCanvas->m_pitch;

	int center = (rect.x + rect.w/2) << 8;

	int sinOfsInc = (NB_CURVETAB_ENTRIES << 16) / sectionHeight;
	int sinOfs = 0;

	int begOfs = 0;
	int peakOfs = 0;
	int endOfs = 0;

	for( int i = 0 ; i < sectionHeight ; i++ )
	{
		peakOfs = ((m_pCurveTab[sinOfs>>16] * maxWidth) >> 8);
		endOfs = (m_pCurveTab[(sinOfs+(sinOfsInc-1))>>16] * maxWidth) >> 8;

		_drawHorrFadeLine( pLineBeg + i*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, color );
		_drawHorrFadeLine( pLineBeg + i*pitch, center - endOfs, center - peakOfs, center - begOfs +256, color );

		_drawHorrFadeLine( pLineBeg + (sectionHeight*2-i-1)*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, color );
		_drawHorrFadeLine( pLineBeg + (sectionHeight*2-i-1)*pitch, center - endOfs, center - peakOfs, center - begOfs +256, color );

		begOfs = peakOfs;
		sinOfs += sinOfsInc;
	}
}

//____ _clipDrawHorrFadeLine() _______________________________________________________________

void WgGfxDeviceSoft::_clipDrawHorrFadeLine( int clipX1, int clipX2, Uint8 * pLineStart, int begOfs, int peakOfs, int endOfs, WgColor color )
{
	int pitch = m_pCanvas->m_pitch;
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * p = pLineStart + (begOfs>>8) * pixelBytes;
	Uint8 * pClip1 = pLineStart + clipX1*pixelBytes;
	Uint8 * pClip2 = pLineStart + clipX2*pixelBytes;

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

void WgGfxDeviceSoft::_drawHorrFadeLine( Uint8 * pLineStart, int begOfs, int peakOfs, int endOfs, WgColor color )
{
	int pitch = m_pCanvas->m_pitch;
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;
	Uint8 * p = pLineStart + (begOfs>>8) * pixelBytes;

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

//____ ClipDrawElipse() _______________________________________________________________

void WgGfxDeviceSoft::ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color )
{
	if( rect.h < 2 || rect.w < 1 )
		return;

	if( !rect.IntersectsWith(clip) )
		return;

	if( clip.Contains(rect) )
		return DrawElipse(rect,color);

	int sectionHeight = rect.h/2;
	int maxWidth = rect.w/2;

	Uint8 * pLineBeg = m_pCanvas->m_pData + rect.y*m_pCanvas->m_pitch;
	int pitch = m_pCanvas->m_pitch;

	int center = (rect.x + rect.w/2) << 8;

	int sinOfsInc = (NB_CURVETAB_ENTRIES << 16) / sectionHeight;
	int sinOfs = 0;

	int begOfs = 0;
	int peakOfs = 0;
	int endOfs = 0;

	for( int i = 0 ; i < sectionHeight ; i++ )
	{
		peakOfs = ((m_pCurveTab[sinOfs>>16] * maxWidth) >> 8);
		endOfs = (m_pCurveTab[(sinOfs+(sinOfsInc-1))>>16] * maxWidth) >> 8;

		if( rect.y + i >= clip.y && rect.y + i < clip.y + clip.h )
		{
			_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + i*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, color );
			_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + i*pitch, center - endOfs, center - peakOfs, center - begOfs +256, color );
		}

		int y2 = sectionHeight*2-i-1;
		if( rect.y + y2 >= clip.y && rect.y + y2 < clip.y + clip.h )
		{
			_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + y2*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, color );
			_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + y2*pitch, center - endOfs, center - peakOfs, center - begOfs +256, color );
		}

		begOfs = peakOfs;
		sinOfs += sinOfsInc;
	}
}

//____ DrawFilledElipse() _____________________________________________________

void WgGfxDeviceSoft::DrawFilledElipse( const WgRect& rect, WgColor color )
{
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;

	Uint8 * pLineCenter = m_pCanvas->m_pData + rect.y * m_pCanvas->m_pitch + (rect.x+rect.w/2) * pixelBytes;

	int sinOfsInc = (NB_CURVETAB_ENTRIES << 16) / (rect.h/2);
	int sinOfs = sinOfsInc >> 1;

	for( int j = 0 ; j < 2 ; j++ )
	{
		for( int i = 0 ; i < rect.h/2 ; i++ )
		{
			int lineLen = ((m_pCurveTab[sinOfs>>16] * rect.w/2 + 32768)>>16)*pixelBytes;
			Uint8 * pLineBeg = pLineCenter - lineLen;
			Uint8 * pLineEnd = pLineCenter + lineLen;

			for( Uint8 * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
			{
				p[0] = color.b;
				p[1] = color.g;
				p[2] = color.r;
			}

			sinOfs += sinOfsInc;
			pLineCenter += m_pCanvas->m_pitch;
		}
		sinOfsInc = -sinOfsInc;
		sinOfs = (NB_CURVETAB_ENTRIES << 16) + (sinOfsInc >> 1);
	}
}

//____ ClipDrawFilledElipse() _____________________________________________________

void WgGfxDeviceSoft::ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color )
{
	if( !rect.IntersectsWith(clip) )
		return;

	if( clip.Contains(rect) )
		return DrawFilledElipse(rect,color);

	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;

	Uint8 * pLine = m_pCanvas->m_pData + rect.y * m_pCanvas->m_pitch;

	int sinOfsInc = (NB_CURVETAB_ENTRIES << 16) / (rect.h/2);
	int sinOfs = sinOfsInc >> 1;

	for( int j = 0 ; j < 2 ; j++ )
	{
		for( int i = 0 ; i < rect.h/2 ; i++ )
		{
			if( rect.y + j*(rect.h/2) + i >= clip.y && rect.y + j*(rect.h/2) + i < clip.y + clip.h )
			{
				int lineLen = ((m_pCurveTab[sinOfs>>16] * rect.w/2 + 32768)>>16);

				int beg = rect.x + rect.w/2 - lineLen;
				int end = rect.x + rect.w/2 + lineLen;

				if( beg < clip.x )
					beg = clip.x;

				if( end > clip.x + clip.w )
					end = clip.x + clip.w;

				if( beg < end )
				{
					Uint8 * pLineBeg = pLine + beg * pixelBytes;
					Uint8 * pLineEnd = pLine + end * pixelBytes;

					for( Uint8 * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
					{
						p[0] = color.b;
						p[1] = color.g;
						p[2] = color.r;
					}
				}
			}

			sinOfs += sinOfsInc;
			pLine += m_pCanvas->m_pitch;
		}

		sinOfsInc = -sinOfsInc;
		sinOfs = (NB_CURVETAB_ENTRIES << 16) + (sinOfsInc >> 1);
	}
}


//____ DrawArcNE() ____________________________________________________________

void WgGfxDeviceSoft::DrawArcNE( const WgRect& rect, WgColor color )
{
	int pixelBytes = m_pCanvas->m_pixelFormat.bits/8;

	Uint8 * pLineBeg = m_pCanvas->m_pData + rect.y * m_pCanvas->m_pitch + rect.x * pixelBytes;

	int sinOfsInc = (NB_CURVETAB_ENTRIES << 16) / rect.h;
	int sinOfs = sinOfsInc >> 1;

	for( int i = 0 ; i < rect.h ; i++ )
	{
		Uint8 * pLineEnd = pLineBeg + ((m_pCurveTab[sinOfs>>16] * rect.w + 32768)>>16)*pixelBytes;

		for( Uint8 * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
		{
			p[0] = color.b;
			p[1] = color.g;
			p[2] = color.r;
		}

		sinOfs += sinOfsInc;
		pLineBeg += m_pCanvas->m_pitch;
	}

}

//____ ClipDrawArcNE() _________________________________________________________

void WgGfxDeviceSoft::ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color )
{
	//TODO: Implement!!!
}


//____ Blit() __________________________________________________________________

void WgGfxDeviceSoft::Blit( const WgSurface* pSrcSurf, const WgRect& srcrect, int dx, int dy  )
{
	if( m_tintColor.argb == 0xFFFFFFFF )
		_blit( pSrcSurf, srcrect, dx, dy );
	else
		_tintBlit( pSrcSurf, srcrect, dx, dy );
}

//____ _blit() _____________________________________________________________

void WgGfxDeviceSoft::_blit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  )
{
	if( !_pSrcSurf || !m_pCanvas || _pSrcSurf->Type() != WgSurfaceSoft::GetClass() )
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
	if( srcPixelBytes == 3 && blendMode == WG_BLENDMODE_BLEND )
		blendMode = WG_BLENDMODE_OPAQUE;

	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
		{
			if( srcPixelBytes == 4 && dstPixelBytes == 4 )
			{
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						* ((Uint32*)pDst) = * ((Uint32*)pSrc) & 0x00FFFFFF;
						pSrc += 4;
						pDst += 4;
					}

					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			else
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
			}

			break;
		}
		case WG_BLENDMODE_BLEND:
		{
			if( srcPixelBytes == 4 )
			{
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						int alpha = pSrc[3];
						int invAlpha = 255-alpha;

						pDst[0] = (pDst[0]*invAlpha + pSrc[0]*alpha ) >> 8;
						pDst[1] = (pDst[1]*invAlpha + pSrc[1]*alpha ) >> 8;
						pDst[2] = (pDst[2]*invAlpha + pSrc[2]*alpha ) >> 8;
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			else
			{
				// Should never get here, skips to blendmode opaque instead.
			}
			break;
		}
		case WG_BLENDMODE_ADD:
		{
			if( srcPixelBytes == 4 )
			{
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						int alpha = pSrc[3];

						pDst[0] = m_limitTable[pDst[0] + ((pSrc[0]*alpha)>>8)];
						pDst[1] = m_limitTable[pDst[1] + ((pSrc[1]*alpha)>>8)];
						pDst[2] = m_limitTable[pDst[2] + ((pSrc[2]*alpha)>>8)];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
			}
			else
			{
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = m_limitTable[pDst[0] + pSrc[0]];
						pDst[1] = m_limitTable[pDst[1] + pSrc[1]];
						pDst[2] = m_limitTable[pDst[2] + pSrc[2]];
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
			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					pDst[0] = (pDst[0]*pSrc[0]) >> 8;
					pDst[1] = (pDst[1]*pSrc[1]) >> 8;
					pDst[2] = (pDst[2]*pSrc[2]) >> 8;
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
			for( int y = 0 ; y < srcrect.h ; y++ )
			{
				for( int x = 0 ; x < srcrect.w ; x++ )
				{
					pDst[0] = (pSrc[0]*(255-pDst[0]) + pDst[0]*(255-pSrc[0])) >> 8;
					pDst[1] = (pSrc[1]*(255-pDst[1]) + pDst[1]*(255-pSrc[0])) >> 8;
					pDst[2] = (pSrc[2]*(255-pDst[2]) + pDst[2]*(255-pSrc[0])) >> 8;
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



//____ _tintBlit() _____________________________________________________________

void WgGfxDeviceSoft::_tintBlit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  )
{
	if( !_pSrcSurf || !m_pCanvas || _pSrcSurf->Type() != WgSurfaceSoft::GetClass() )
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

						pDst[0] = m_limitTable[pDst[0] + ((pSrc[0]*tintBlue*alpha)>>16)];
						pDst[1] = m_limitTable[pDst[1] + ((pSrc[1]*tintGreen*alpha)>>16)];
						pDst[2] = m_limitTable[pDst[2] + ((pSrc[2]*tintRed*alpha)>>16)];
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
						pDst[0] = m_limitTable[pDst[0] + ((pSrc[0]*tintBlue)>>16)];
						pDst[1] = m_limitTable[pDst[1] + ((pSrc[1]*tintGreen)>>16)];
						pDst[2] = m_limitTable[pDst[2] + ((pSrc[2]*tintRed)>>16)];
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

					pDst[0] = (srcBlue*(255-pDst[0]) + pDst[0]*(255*255-srcBlue)) >> 16;
					pDst[1] = (srcGreen*(255-pDst[1]) + pDst[1]*(255*255-srcGreen)) >> 16;
					pDst[2] = (srcRed*(255-pDst[2]) + pDst[2]*(255*255-srcRed)) >> 16;
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
	float srcW = (float) src.w;
	float srcH = (float) src.h;

	float destW = (float) dest.w;
	float destH = (float) dest.h;

	if( m_bBilinearFiltering )
	{
		if( srcW < destW )
			srcW--;

		if( srcH < destH )
			srcH--;
	}

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
	if( m_bBilinearFiltering )
	{
		if( sw < dw )
			sw--;

		if( sh < dh )
			sh--;
	}

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
	if( !_pSrcSurf || !m_pCanvas || _pSrcSurf->Type() != WgSurfaceSoft::GetClass() )
		return;

	WgSurfaceSoft * pSrcSurf = (WgSurfaceSoft*) _pSrcSurf;

	if( !m_pCanvas->m_pData || !pSrcSurf->m_pData )
		return;

	int dx = (int) _dx;
	int dy = (int) _dy;
	int dw = (int) _dw;
	int dh = (int) _dh;

	WgBlendMode		blendMode = m_blendMode;
	if( pSrcSurf->m_pixelFormat.bits == 24 && blendMode == WG_BLENDMODE_BLEND && m_tintColor.a == 255 )
		blendMode = WG_BLENDMODE_OPAQUE;

	if( m_tintColor == 0xFFFFFFFF )
	{
		switch( blendMode )
		{
			case WG_BLENDMODE_OPAQUE:
				_stretchBlitOpaque( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_BLEND:
				if( pSrcSurf->m_pixelFormat.bits == 24 )
					assert(0);							// SHOULD NEVER GET HERE!
				else
					_stretchBlitBlend32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_ADD:
				if( pSrcSurf->m_pixelFormat.bits == 24 )
					_stretchBlitAdd24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				else
					_stretchBlitAdd32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_MULTIPLY:
				_stretchBlitMultiply( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_INVERT:
				_stretchBlitInvert( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
		}
	}
	else
	{
		switch( blendMode )
		{
			case WG_BLENDMODE_OPAQUE:
				_stretchBlitTintedOpaque( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_BLEND:
				if( pSrcSurf->m_pixelFormat.bits == 24 )
					_stretchBlitTintedBlend24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				else
					_stretchBlitTintedBlend32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_ADD:
				if( pSrcSurf->m_pixelFormat.bits == 24 )
					_stretchBlitTintedAdd24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				else
					_stretchBlitTintedAdd32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_MULTIPLY:
				_stretchBlitTintedMultiply( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
			case WG_BLENDMODE_INVERT:
				_stretchBlitTintedInvert( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
				break;
		}
	}
}


#define STRETCHBLIT( _bReadAlpha_, _init_, _loop_ )										\
{																						\
	int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;									\
	int dstPixelBytes = m_pCanvas->m_pixelFormat.bits/8;								\
																						\
	int	srcPitch = pSrcSurf->m_pitch;													\
	int	dstPitch = m_pCanvas->m_pitch;													\
																						\
	_init_																				\
																						\
	if( m_bBilinearFiltering )															\
	{																					\
		int ofsY = (int) (sy*32768);		/* We use 15 binals for all calculations */	\
		int incY = (int) (sh*32768/dh);													\
																						\
		for( int y = 0 ; y < dh ; y++ )													\
		{																				\
			int fracY2 = ofsY & 0x7FFF;													\
			int fracY1 = 32768 - fracY2;												\
																						\
			int ofsX = (int) (sx*32768);												\
			int incX = (int) (sw*32768/dw);												\
																						\
			Uint8 * pDst = m_pCanvas->m_pData + (dy+y) * m_pCanvas->m_pitch + dx * dstPixelBytes;	\
			Uint8 * pSrc = pSrcSurf->m_pData + (ofsY>>15) * pSrcSurf->m_pitch;						\
																						\
			for( int x = 0 ; x < dw ; x++ )												\
			{																			\
				int fracX2 = ofsX & 0x7FFF;												\
				int fracX1 = 32768 - fracX2;											\
																						\
				Uint8 * p = pSrc + (ofsX >> 15)*srcPixelBytes;							\
																						\
				int mul11 = fracX1*fracY1 >> 15;										\
				int mul12 = fracX2*fracY1 >> 15;										\
				int mul21 = fracX1*fracY2 >> 15;										\
				int mul22 = fracX2*fracY2 >> 15;										\
																						\
				int srcBlue = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15; 	\
				p++;																												\
				int srcGreen = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;	\
				p++;																												\
				int srcRed = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;		\
				int srcAlpha;																										\
				if( _bReadAlpha_ )																									\
				{																													\
					p++;																											\
					srcAlpha = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;	\
				}																		\
																						\
				_loop_																	\
																						\
				ofsX += incX;															\
				pDst += dstPixelBytes;													\
			}																			\
			ofsY += incY;																\
		}																				\
	}																					\
	else	/* UNFILTERED */															\
	{																					\
		int ofsY = (int) (sy*32768);		/* We use 15 binals for all calculations */	\
		int incY = (int) (sh*32768/dh);													\
																						\
		for( int y = 0 ; y < dh ; y++ )													\
		{																				\
			int ofsX = (int) (sx*32768);												\
			int incX = (int) (sw*32768/dw);												\
																						\
			Uint8 * pDst = m_pCanvas->m_pData + (dy+y) * m_pCanvas->m_pitch + dx * dstPixelBytes;	\
			Uint8 * pSrc = pSrcSurf->m_pData + (ofsY>>15) * pSrcSurf->m_pitch;						\
																						\
			for( int x = 0 ; x < dw ; x++ )												\
			{																			\
				Uint8 * p = pSrc + (ofsX >> 15)*srcPixelBytes;							\
																						\
				int srcBlue = p[0]; 													\
				int srcGreen = p[1];													\
				int srcRed = p[2];														\
				int srcAlpha;															\
				if( _bReadAlpha_ )														\
					srcAlpha = p[3];													\
																						\
				_loop_																	\
																						\
				ofsX += incX;															\
				pDst += dstPixelBytes;													\
			}																			\
			ofsY += incY;																\
		}																				\
	}																					\
}																						\



//____ _stretchBlitTintedOpaque() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedOpaque( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
														int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	int tintRed = (int) m_tintColor.r;
	int tintGreen = (int) m_tintColor.g;
	int tintBlue = (int) m_tintColor.b;

	,

	pDst[0] = (srcBlue*tintBlue) >> 8;
	pDst[1] = (srcGreen*tintGreen) >> 8;
	pDst[2] = (srcRed*tintRed) >> 8;

	)
}

//____ _stretchBlitTintedBlend32() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedBlend32( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
														 int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( true,

	int tintAlpha = (int) m_tintColor.a;
	int tintRed = (int) m_tintColor.r;
	int tintGreen = (int) m_tintColor.g;
	int tintBlue = (int) m_tintColor.b;

	,

	int alpha = (srcAlpha*tintAlpha) >> 8;
	int invAlpha = (255-alpha) << 8;

	pDst[0] = (pDst[0]*invAlpha + srcBlue*tintBlue*alpha ) >> 16;
	pDst[1] = (pDst[1]*invAlpha + srcGreen*tintGreen*alpha ) >> 16;
	pDst[2] = (pDst[2]*invAlpha + srcRed*tintRed*alpha ) >> 16;

	)
}

//____ _stretchBlitTintedBlend24() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedBlend24( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
														 int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	int tintAlpha = (int) m_tintColor.a;
	int tintRed = (int) m_tintColor.r * tintAlpha;
	int tintGreen = (int) m_tintColor.g * tintAlpha;
	int tintBlue = (int) m_tintColor.b * tintAlpha;
	int invAlpha = (255-tintAlpha) << 8;

	,

	pDst[0] = (pDst[0]*invAlpha + srcBlue*tintBlue ) >> 16;
	pDst[1] = (pDst[1]*invAlpha + srcGreen*tintGreen ) >> 16;
	pDst[2] = (pDst[2]*invAlpha + srcRed*tintRed ) >> 16;

	)
}


//____ _stretchBlitTintedAdd32() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedAdd32( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
														int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( true,

	int tintAlpha = (int) m_tintColor.a;
	int tintRed = (int) m_tintColor.r;
	int tintGreen = (int) m_tintColor.g;
	int tintBlue = (int) m_tintColor.b;

	,

	int alpha = (srcAlpha*tintAlpha) >> 8;

	pDst[0] = m_limitTable[pDst[0] + ((srcBlue*tintBlue*alpha)>>16)];
	pDst[1] = m_limitTable[pDst[1] + ((srcGreen*tintGreen*alpha)>>16)];
	pDst[2] = m_limitTable[pDst[2] + ((srcRed*tintRed*alpha)>>16)];

	)
}


//____ _stretchBlitTintedAdd24() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedAdd24( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
														int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	int tintAlpha = (int) m_tintColor.a;
	int tintRed = (int) m_tintColor.r * tintAlpha;
	int tintGreen = (int) m_tintColor.g * tintAlpha;
	int tintBlue = (int) m_tintColor.b * tintAlpha;

	,

	pDst[0] = m_limitTable[pDst[0] + ((srcBlue*tintBlue)>>16)];
	pDst[1] = m_limitTable[pDst[1] + ((srcGreen*tintGreen)>>16)];
	pDst[2] = m_limitTable[pDst[2] + ((srcRed*tintRed)>>16)];

	)
}


//____ _stretchBlitTintedMultiply() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedMultiply( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
														  int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	int tintRed = (int) m_tintColor.r;
	int tintGreen = (int) m_tintColor.g;
	int tintBlue = (int) m_tintColor.b;

	,

	pDst[0] = (tintBlue*pDst[0]*srcBlue) >> 16;
	pDst[1] = (tintGreen*pDst[1]*srcGreen) >> 16;
	pDst[2] = (tintRed*pDst[2]*srcRed) >> 16;

	)
}

//____ _stretchBlitTintedInvert() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitTintedInvert( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
											      int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	int tintRed = (int) m_tintColor.r;
	int tintGreen = (int) m_tintColor.g;
	int tintBlue = (int) m_tintColor.b;

	,

	srcBlue = srcBlue * tintBlue;
	srcGreen = srcGreen * tintGreen;
	srcRed = srcRed * tintRed;

	pDst[0] = (srcBlue*(255-pDst[0]) + pDst[0]*(255*255-srcBlue)) >> 16;
	pDst[1] = (srcGreen*(255-pDst[1]) + pDst[1]*(255*255-srcGreen)) >> 16;
	pDst[2] = (srcRed*(255-pDst[2]) + pDst[2]*(255*255-srcRed)) >> 16;

	)
}


//____ _stretchBlitOpaque() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitOpaque( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
												  int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	,

	pDst[0] = srcBlue;
	pDst[1] = srcGreen;
	pDst[2] = srcRed;

	)
}

//____ _stretchBlitBlend32() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitBlend32( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
												   int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( true,

	,

	int invAlpha = 255-srcAlpha;

	pDst[0] = (pDst[0]*invAlpha + srcBlue*srcAlpha ) >> 8;
	pDst[1] = (pDst[1]*invAlpha + srcGreen*srcAlpha ) >> 8;
	pDst[2] = (pDst[2]*invAlpha + srcRed*srcAlpha ) >> 8;

	)
}

//____ _stretchBlitAdd32() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitAdd32( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
												 int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( true,

	,

	pDst[0] = m_limitTable[pDst[0] + ((srcBlue*srcAlpha)>>8 )];
	pDst[1] = m_limitTable[pDst[1] + ((srcGreen*srcAlpha)>>8 )];
	pDst[2] = m_limitTable[pDst[2] + ((srcRed*srcAlpha)>>8 )];

	)
}


//____ _stretchBlitAdd24() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitAdd24( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
												 int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	,

	pDst[0] = m_limitTable[pDst[0] + srcBlue];
	pDst[1] = m_limitTable[pDst[1] + srcGreen];
	pDst[2] = m_limitTable[pDst[2] + srcRed];

	)
}


//____ _stretchBlitMultiply() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitMultiply( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
												    int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	,

	pDst[0] = (pDst[0]*srcBlue) >> 8;
	pDst[1] = (pDst[1]*srcGreen) >> 8;
	pDst[2] = (pDst[2]*srcRed) >> 8;

	)
}

//____ _stretchBlitInvert() ____________________________________________

void WgGfxDeviceSoft::_stretchBlitInvert( const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
											      int dx, int dy, int dw, int dh )
{
	STRETCHBLIT( false,

	,

	pDst[0] = (srcBlue*(255-pDst[0]) + pDst[0]*(255-srcBlue)) >> 8;
	pDst[1] = (srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen)) >> 8;
	pDst[2] = (srcRed*(255-pDst[2]) + pDst[2]*(255-srcRed)) >> 8;

	)
}



//____ _initTables() ___________________________________________________________

void WgGfxDeviceSoft::_initTables()
{
	// Init limitTable

	for( int i = 0 ; i < 256 ; i++ )
		m_limitTable[i] = i;

	for( int i = 256 ; i < 512 ; i++ )
		m_limitTable[i] = 255;

}
