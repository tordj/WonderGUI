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

#include <wg2_oscilloscope.h>
#include <wg2_gfxdevice.h>
#include <wg2_base.h>


static const char	c_widgetType[] = {"Oscilloscope"};

#include <stdlib.h>
#include <math.h>
#include <cmath>
#define round(x) floorf((x)+0.5f)

//____ Constructor ____________________________________________________________

WgOscilloscope::WgOscilloscope()
{
	m_gridColor = WgColor::Black;
	m_lineColor = WgColor::White;
	m_lineThickness = 2.f;

	m_nVGridLines = 0;
	m_pVGridLines = 0;
	m_nHGridLines = 0;
	m_pHGridLines = 0;

	m_nLinePoints = 0;
	m_pLinePoints = 0;

	m_nDisplayPoints = 0;
	m_pDisplayPoints = 0;

	m_nMarkers = 0;
	m_pMarkers = 0;

	m_nRenderSegments = 1;
}

//____ Destructor _____________________________________________________________

WgOscilloscope::~WgOscilloscope()
{
	delete [] m_pVGridLines;
	delete [] m_pHGridLines;
	delete [] m_pLinePoints;
	delete [] m_pDisplayPoints;
	delete [] m_pMarkers;
}

//____ Type() _________________________________________________________________

const char * WgOscilloscope::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgOscilloscope::GetClass()
{
	return c_widgetType;
}

//____ PreferredPixelSize() ________________________________________________________

WgSize WgOscilloscope::PreferredPixelSize() const
{
	return WgSize(80,64);
}


//____ SetBackground() ________________________________________________________

void WgOscilloscope::SetBackground( wg::Skin * skin )
{
	if( m_pBG != skin )
	{
		m_pBG = skin;
		if( m_pBG )
			m_bOpaque = m_pBG->isOpaque();
		else
			m_bOpaque = false;

		_requestRender();
	}
}

//____ SetGridColor() _________________________________________________________

void WgOscilloscope::SetGridColor( WgColor color )
{
	if( m_gridColor != color )
	{
		m_gridColor = color;
		if( m_nVGridLines != 0 || m_nHGridLines != 0 )
			_requestRender();
	}
}

//____ SetVGridLines() ________________________________________________________

void WgOscilloscope::SetVGridLines( int nLines, float pLines[] )
{
	if( nLines != m_nVGridLines )
	{
		delete [] m_pVGridLines;
		if( nLines == 0 )
			m_pVGridLines = 0;
		else
			m_pVGridLines = new float[nLines];
		m_nVGridLines = nLines;
	}

	for( int i = 0 ; i < nLines ; i++ )
		m_pVGridLines[i] = pLines[i];

	_requestRender();
}

//____ SetHGridLines() ________________________________________________________

void WgOscilloscope::SetHGridLines( int nLines, float pLines[] )
{
	if( nLines != m_nHGridLines )
	{
		delete [] m_pHGridLines;
		if( nLines == 0 )
			m_pHGridLines = 0;
		else
			m_pHGridLines = new float[nLines];
		m_nHGridLines = nLines;
	}

	for( int i = 0 ; i < nLines ; i++ )
		m_pHGridLines[i] = pLines[i];

	_requestRender();
}

//____ SetLineColor() ________________________________________________________

void WgOscilloscope::SetLineColor( WgColor color )
{
	if( wg::HiColor(color) != m_lineColor )
	{
		m_lineColor = color;
		_requestRender();
	}
}

//____ SetLineThickness() _____________________________________________________

void WgOscilloscope::SetLineThickness( float thickness )
{
	wg::limit( thickness, 1.f, 2.f );
	if( thickness != m_lineThickness )
	{
		m_lineThickness = thickness;
		_requestRender();
	}
}

//____ SetRenderSegments()______________________________________________________

void WgOscilloscope::SetRenderSegments( int nSegments )
{
	if( nSegments > 0 )
		m_nRenderSegments = nSegments;
}


//____ SetLinePoints() ________________________________________________________

void WgOscilloscope::SetLinePoints( int nPoints, float pPointValues[] )
{
	// Handle special case of no points (no line will be displayed)

	if( nPoints == 0 )
	{
		if( m_nLinePoints != 0 )
		{
			delete [] m_pLinePoints;
			m_pLinePoints = 0;
			m_nLinePoints = 0;
			_resampleLinePoints( PixelSize() );
			_requestRender();
		}
		return;
	}

	WgSize sz = PixelSize();

	// Resize array if needed

	if( nPoints != m_nLinePoints )
	{
		delete [] m_pLinePoints;
		m_pLinePoints = new float[nPoints];
		m_nLinePoints = nPoints;
	}

	// Fill in array

	for( int i = 0 ; i < nPoints ; i++ )
		m_pLinePoints[i] = pPointValues[i];

	// Generate and prepare a list of render segments

	int nSegments = m_nRenderSegments;
	int segWidth = (sz.w+m_nRenderSegments-1)/m_nRenderSegments;
	if( segWidth < 16 )
	{
		segWidth = 16;
		nSegments = (sz.w+15)/segWidth;
	}

	int allocSize = sizeof(WgRect)*nSegments;
	WgRect * pSegments = reinterpret_cast<WgRect*>(wg::GfxBase::memStackAlloc(allocSize));


	int ofs = 0;
	for( int i = 0 ; i < nSegments ; i++ )
	{
		pSegments[i].x = ofs;
		pSegments[i].y = 0;
		pSegments[i].w = segWidth;
		pSegments[i].h = 0;
		ofs += segWidth;
	}
	pSegments[nSegments-1].w = sz.w - pSegments[nSegments-1].x;

	// Resample the line points and update the render segments to only re-render as much as needed.

	if( m_nDisplayPoints != 0 )
		_updateRenderSegments( nSegments, pSegments );		// Get dirty rects for erasing previous line
	_resampleLinePoints( sz );
	_updateRenderSegments( nSegments, pSegments );			// Expand dirty rects to include current line

	for( int i = 0 ; i < nSegments ; i++ )
		_requestRender( pSegments[i] );

	// Return memory allocated for render segments

	wg::GfxBase::memStackFree(allocSize);
}

//____ _updateRenderSegments()__________________________________________________

void WgOscilloscope::_updateRenderSegments( int nSegments, WgRect * pSegments )
{

	WgRect * pSeg = pSegments;

	for( int seg = 0 ; seg < nSegments ; seg++ )
	{
		int point = pSeg->x;

		float min = m_pDisplayPoints[point > 0 ? point - 1 : 0];
		float max = min;

		for( int i = 0 ; i < pSeg->w+1 ; i++ )		// include one extra measure for the aa-algorithm
		{
			float v = m_pDisplayPoints[point+i];
			if( v < min ) min = v;
			if( v > max ) max = v;
		}

		int yBeg = ((int)min) - 2;		// Two pixels margin for the line drawing
		int yEnd = ((int)max) + 3;		// Two pixels margin for the line drawing (+1 for cropping)

		if( pSeg->h == 0 )
		{
			pSeg->y = yBeg;
			pSeg->h = yEnd - yBeg;
		}
		else
		{
			if( yBeg < pSeg->y )
			{
				pSeg->h += pSeg->y - yBeg;
				pSeg->y = yBeg;
			}
			if( yEnd > pSeg->y + pSeg->h )
				pSeg->h = yEnd - pSeg->y;
		}

		pSeg++;
	}
}


//____ _resampleLinePoints()____________________________________________________

void WgOscilloscope::_resampleLinePoints( WgSize sz )
{
	if( m_nLinePoints == 0 || sz.w == 0 )
	{
		if( m_nDisplayPoints != 0 )
		{
			delete [] m_pDisplayPoints;
			m_pDisplayPoints = 0;
			m_nDisplayPoints = 0;
		}
		return;
	}

	if( m_nDisplayPoints != sz.w )
	{
		delete [] m_pDisplayPoints;
		m_pDisplayPoints = new float[sz.w+2]; // Need two extra points for anti-aliasing
		m_nDisplayPoints = sz.w;
	}

	// Recalculate values for our display, possibly resample if display width != m_nLinePoints

	float centerY = sz.h/2.f;
	float scaleY = (sz.h-1)/2.f;


	if (m_nLinePoints != sz.w )
	{
		int ip = 0;
		float lam = 0;
		float point = 0;
		float ratio = m_nLinePoints/(float)sz.w;

		for( int i=0 ; i < sz.w ; i++ )
		{
			point = i * ratio;
			ip = (int)point;
			lam = point - (float)ip;

			if(ip+1 < m_nLinePoints)
				m_pDisplayPoints[i] = centerY + (m_pLinePoints[ip] * (1-lam) + m_pLinePoints[ip+1] * lam) * scaleY;
			else
				m_pDisplayPoints[i] = centerY + m_pLinePoints[m_nLinePoints-1] * scaleY;
		}
	}
	else
	{
		// Straight copy

		for( int i = 0 ; i < m_nLinePoints ; i++ )
			m_pDisplayPoints[i] = centerY + m_pLinePoints[i] * scaleY;
	}

	// Just fill out our extras

	m_pDisplayPoints[m_nDisplayPoints] = centerY + m_pLinePoints[m_nLinePoints-1] * scaleY;
	m_pDisplayPoints[m_nDisplayPoints+1] = centerY + m_pLinePoints[m_nLinePoints-1] * scaleY;

}

//____ AddMarker() ____________________________________________________________

void WgOscilloscope::AddMarker( int xOfs, float yOfs )
{
	Marker * pNew = new Marker[m_nMarkers+1];
	for( int i = 0 ; i < m_nMarkers ; i++ )
		pNew[i] = m_pMarkers[i];
	delete [] m_pMarkers;

	pNew[m_nMarkers].x = xOfs;
	pNew[m_nMarkers].y = yOfs;

	m_nMarkers++;
	m_pMarkers = pNew;
	_requestRender();
}

//____ SetMarkerGfx() _________________________________________________________

void WgOscilloscope::SetMarkerGfx( wg::Skin * pSkin )
{
	if( m_pMarkerSkin != pSkin )
	{
		m_pMarkerSkin = pSkin;
		_requestRender();
	}
}

//____ _onNewSize()_____________________________________________________________

void WgOscilloscope::_onNewSize( const WgSize& size )
{
	_resampleLinePoints( size );
}


//____ _onCloneContent() ______________________________________________________

void WgOscilloscope::_onCloneContent( const WgWidget * _pOrg )
{

}

//____ _onRender() ____________________________________________________________

void WgOscilloscope::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
	// Render background
	if( m_pBG )
		_renderSkin( m_pBG, pDevice, WgStateEnum::Default, _canvas, m_scale );

	float centerX = _canvas.x + _canvas.w/2.f;
	float centerY = _canvas.y + _canvas.h/2.f;
	float scaleX = (_canvas.w-1)/2.f;
	float scaleY = (_canvas.h-1)/2.f;

	// Draw HGridLines
	for( int i = 0; i < m_nHGridLines; i++ )
	{
		int ofsY = (int) (m_pHGridLines[i] * scaleY + centerY);
		pDevice->drawLine( WgCoord(_canvas.x,ofsY)*64, WgDirection::Right, _canvas.w*64, m_gridColor );
	}

	// Draw VGridLines
	for( int i = 0; i < m_nVGridLines; i++ )
	{
		int ofsX = (int) (m_pVGridLines[i] * scaleX + centerX);
		pDevice->drawLine( WgCoord(ofsX,_canvas.y)*64, WgDirection::Down, _canvas.h*64, m_gridColor );
	}

	// Nothing to draw (yet)
	if(m_nDisplayPoints == 0)
		return;

	// Draw the oscilloscope line

	m_iNextPixel = 0;

//	antiAlias(_clip.w, _clip.x, m_pDisplayPoints + _clip.x - _canvas.x);

/*
	if( _clip.x > _canvas.x )
		_antiAlias(_clip.w+1, m_pDisplayPoints + _clip.x - _canvas.x-1, WgCoord( _clip.x-1, _canvas.y ) );
	else
		_antiAlias(_clip.w, m_pDisplayPoints + _clip.x - _canvas.x, WgCoord( _clip.x, _canvas.y ) );
*/

	_antiAlias(_canvas.w, m_pDisplayPoints, _canvas.pos() );
	pDevice->plotPixels(m_iNextPixel, m_pAAPix, m_pAACol);

	// Blit markers

	for( int i = 0; i < m_nMarkers; i++ )
	{
		WgRect dest;

		int x = m_pMarkers[i].x;
		int y = (int) (m_pMarkers[i].y*scaleY+centerY);

		dest.setSize( _skinPreferredSize(m_pMarkerSkin, m_scale) );
		dest.x = x - dest.w / 2;
		dest.y = y - dest.h / 2;

		_renderSkin(m_pMarkerSkin, pDevice, wg::StateEnum::Default, dest, m_scale);
	}

}

void WgOscilloscope::plot(const int x, const int y, const float alpha)
{
  if(m_iNextPixel < WG2_OSC_PIXEL_BUFFER_SIZE-1)
  {
	  m_pAAPix[m_iNextPixel] = WgCoord(x, y)*64;
	  m_pAACol[m_iNextPixel] = m_lineColor;
	  m_pAACol[m_iNextPixel].a = (4096*alpha);

	  ++m_iNextPixel;
  }
  else
  {
	  //DBGM(DBG_FFT, ("-----WARNING!------- m_iNextPixel=%i", m_iNextPixel));
  }

  //DBG_ASSERT(m_iNextPixel < WG2_OSC_PIXEL_BUFFER_SIZE);
}


// Xiaolin Wu's line algorithm
void WgOscilloscope::_antiAlias(const int nPoints, const float *pYval, WgCoord ofs )
{
	int   x0i,x1i;
	float x0,x1,y0,y1,yprev;
	float dx;
	float dy;
	float intery, gradient;
	int xpxl1, xpxl2;
	int ypxl1, ypxl2;
	float xgap;
	float xend;
	float yend;

	yprev = pYval[0];

	for(x0i = 0; x0i < nPoints; x0i++)
	{
		x1i = x0i + 1;
		y0 = yprev;
		y1 = pYval[x1i];
		yprev = y1;

		x0 = float(x0i);
		x1 = float(x1i);

		// Check for NaN
		if(!( y1 > 0.0f || y1 <= 0.0f ))
			y1 = 0.0f;


		bool steep = fabsf(y1 - y0) > std::abs(x1 - x0);

		if(steep) {
			std::swap(x0,y0);
			std::swap(x1,y1);
		}

		if (x0>x1) {
			std::swap(x0,x1);
			std::swap(y0,y1);
		}

		dx = (float)x1 - (float)x0;
		dy = (float)y1 - (float)y0;
		gradient = dy/dx;

		xend = round(x0);
		yend = y0 + gradient * (xend - x0);
		xgap =  rfpart(x0 + 0.5f);
		xpxl1 = (int) xend;
		ypxl1 = ipart(yend);

		if (steep) {
			plot(ypxl1 +     ofs.x, xpxl1+ ofs.y, rfpart(yend) * xgap);
			plot(ypxl1 + 1 + ofs.x, xpxl1+ ofs.y,  fpart(yend) * xgap);
		} else {
			plot(xpxl1 +     ofs.x, ypxl1 + ofs.y,     rfpart(yend) * xgap);
			plot(xpxl1 +     ofs.x, ypxl1 + 1 + ofs.y,  fpart(yend) * xgap);
		}
		intery = yend + gradient;

		// Second end point
		xend = round(x1);
		yend = y1 + gradient * (xend - x1);
		xgap = fpart(x1 + 0.5f);
		xpxl2 = (int) xend;
		ypxl2 = ipart(yend);

		if (steep) {
			plot(ypxl2 +     ofs.x, xpxl2 + ofs.y, rfpart(yend) * xgap);
			plot(ypxl2 + 1 + ofs.x, xpxl2 + ofs.y,  fpart(yend) * xgap);
		} else {
			plot(xpxl2 +     ofs.x, ypxl2 + ofs.y,     rfpart(yend) * xgap);
			plot(xpxl2 +     ofs.x, ypxl2 + 1 + ofs.y,  fpart(yend) * xgap);
		}

		for (int x=xpxl1+1; x<xpxl2; x++) {
			if (steep) {
				plot(ipart(intery) +     ofs.x, x + ofs.y, rfpart(intery));
				plot(ipart(intery) + 1 + ofs.x, x + ofs.y,  fpart(intery));
			} else {
				plot(x + ofs.x, ipart (intery) + ofs.y,  rfpart(intery));
				plot(x + ofs.x, ipart (intery)+1 + ofs.y, fpart(intery));
			}
			intery = intery + gradient;
		}
	}

	return;
}

