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


#include <wg2_simpleknob.h>
#include <wg2_gfxdevice.h>
#include <wg2_base.h>

#include <algorithm>

static const char	c_widgetType[] = {"Knob"};

#include <math.h>
#define round(x) floorf((x)+0.5f)

//____ Constructor ____________________________________________________________

WgSimpleKnob::WgSimpleKnob()
{
	m_bOpaque = false;
	m_iNextPixel = 0;
	m_lineColor = WgColor::White;
	m_fValue = 0.0f;
	m_preferredSize = WgSize(45,45);

}

//____ Destructor _____________________________________________________________

WgSimpleKnob::~WgSimpleKnob()
{
}

//____ Type() _________________________________________________________________

const char * WgSimpleKnob::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgSimpleKnob::GetClass()
{
	return c_widgetType;
}

//____ SetValue() _____________________________________________________________

void WgSimpleKnob::SetValue( float fValue )
{
	if(m_fValue == fValue)
		return;

	m_fValue = fValue;
  	_requestRender();
}

//____ PreferredPixelSize() __________________________________________________________
void WgSimpleKnob::SetPreferredPixelSize(WgSize size)
{
	m_preferredSize = size;
}

WgSize WgSimpleKnob::PreferredPixelSize() const
{
	return m_preferredSize;
}


//____ _onCloneContent() _______________________________________________________

void WgSimpleKnob::_onCloneContent( const WgWidget * _pOrg )
{
	const WgSimpleKnob * pOrg = static_cast<const WgSimpleKnob*>(_pOrg);
}

//____ _onRender() _____________________________________________________________
#define PI 3.141592653f
void WgSimpleKnob::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
//    pDevice->Fill( _clip, WgColor::black );

	int rad = (std::min( _canvas.w,_canvas.h ) - 1) / 2;
	int sz = 2*rad + 1; // Rad is the radius (even). Total size will be 2*radius + 1 middle point

	if( sz > 1 )
	{
		m_iNextPixel = 0;
		drawCircle(_canvas.x + sz/2, _canvas.y + sz/2, (float)sz/2, (float)sz/2);
		pDevice->plotPixels(m_iNextPixel, m_pAAPix, m_pAACol);
//        pDevice->ClipPlotPixels(_clip, m_iNextPixel, m_pAAPix, m_pAACol);

		const float d = 45.0f;
		// Draw the index line
		const float x = -sin(d*PI/180.0f + m_fValue*(360.0f-d*2)*PI/180.0f)*(0.92f*float(sz)/2.0f);
		const float y =  cos(d*PI/180.0f + m_fValue*(360.0f-d*2)*PI/180.0f)*(0.92f*float(sz)/2.0f);

		m_iNextPixel = 0;
		drawLine( float(_canvas.x + rad), float(_canvas.y + rad), _canvas.x + rad + x, _canvas.y + rad + y);
		pDevice->plotPixels(m_iNextPixel, m_pAAPix, m_pAACol);
//        pDevice->ClipPlotPixels(_clip, m_iNextPixel, m_pAAPix, m_pAACol);
	}
}

//____ _onAlphaTest() ___________________________________________________________

bool WgSimpleKnob::_onAlphaTest( const WgCoord& ofs )
{
	return true;
}


//____ _onEnable() _____________________________________________________________

void WgSimpleKnob::_onEnable()
{
	_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgSimpleKnob::_onDisable()
{
	_requestRender();
}

void WgSimpleKnob::drawCircle(const int centerX, const int centerY, const float radX, const float radY)
{
	const float rx2 = radX*radX;
	const float ry2 = radY*radY;
	int quarter;

	quarter = (int) round(rx2 / sqrt(rx2 + ry2));
	for (int x=0; x<=quarter; x++) {
		float y = radY * (float) sqrt(1.0-x*x/rx2);
		float e = y - floor(y);
		plot4(centerX, centerY, x, (int) -floor(y), (float) e);
		plot4(centerX, centerY, x, (int) -floor(y)+1, (float) 1.0-e);
	}

	quarter = (int) round(ry2 / sqrt(rx2 + ry2));
	for (int y=0; y<=quarter; y++) {
		float x = radX * (float) sqrt(1.0-y*y/ry2);
		float e = x - floor(x);
		plot4(centerX, centerY, (int) -floor(x),   y, (float) e);
		plot4(centerX, centerY, (int) -floor(x)+1, y, (float) 1.0-e);
	}
}

// Xiaolin Wu's line algorithm
void WgSimpleKnob::drawLine(float x0, float y0, float x1, float y1)
{
	float dx;
	float dy;
	float intery, gradient;
	int xpxl1, xpxl2;
	int ypxl1, ypxl2;
	float xgap;
	float xend;
	float yend;

	bool steep = fabsf(y1 - y0) > fabs(x1 - x0);

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
	xgap = rfpart(x0 + 0.5f);
	xpxl1 = (int) xend;
	ypxl1 = ipart(yend);

	if (steep) {
		plot(ypxl1    , xpxl1, rfpart(yend) * xgap);
		plot(ypxl1 + 1, xpxl1,  fpart(yend) * xgap);
	} else {
		plot(xpxl1, ypxl1,     rfpart(yend) * xgap);
		plot(xpxl1, ypxl1 + 1,  fpart(yend) * xgap);
	}
	intery = yend + gradient;

	// Second end point
	xend = round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = fpart(x1 + 0.5f);
	xpxl2 = (int) xend;
	ypxl2 = ipart(yend);

	if (steep) {
		plot(ypxl2    , xpxl2, rfpart(yend) * xgap);
		plot(ypxl2 + 1, xpxl2,  fpart(yend) * xgap);
	} else {
		plot(xpxl2, ypxl2,     rfpart(yend) * xgap);
		plot(xpxl2, ypxl2 + 1,  fpart(yend) * xgap);
	}

	for (int x=xpxl1+1; x<xpxl2; x++) {
		if (steep) {
			plot(ipart(intery)    , x, rfpart(intery));
			plot(ipart(intery) + 1, x,  fpart(intery));
		} else {
			plot(x, ipart (intery),  rfpart(intery));
			plot(x, ipart (intery)+1, fpart(intery));
		}
		intery = intery + gradient;
	}

	return;
}


void WgSimpleKnob::plot(const int x, const int y, const float alpha)
{
	m_pAAPix[m_iNextPixel] = WgCoord(x, y)*64;
	m_pAACol[m_iNextPixel] = m_lineColor;
	m_pAACol[m_iNextPixel].a = (uint16_t)(m_lineColor.a * alpha);

	++m_iNextPixel;

}

void WgSimpleKnob::plot4(const int centerX, const int centerY, const int deltaX, const int deltaY, const float alpha)
{
	plot(centerX+deltaX, centerY+deltaY, alpha);
	plot(centerX-deltaX, centerY+deltaY, alpha);
	plot(centerX+deltaX, centerY-deltaY, alpha);
	plot(centerX-deltaX, centerY-deltaY, alpha);
}

