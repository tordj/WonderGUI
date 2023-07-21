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
#ifndef WG2_KNOB_DOT_H
#define WG2_KNOB_DOT_H


#ifndef WG2_WIDGET_DOT_H
#	include <wg2_widget.h>
#endif

#include <wg2_color.h>

#include <math.h>

//____ WgSimpleKnob ____________________________________________________________

#define WG2_KNOB_PIXEL_BUFFER_SIZE 1000

class WgSimpleKnob : public WgWidget
{
public:
	WgSimpleKnob();
	virtual ~WgSimpleKnob();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgSimpleKnob(); };

	void    SetValue( float value );
	float   GetValue() { return m_fValue; }
	void    SetColor( WgColor color ) { if( m_lineColor != wg::HiColor(color) ) {m_lineColor = color; _requestRender();} };

	WgSize	PreferredPixelSize() const;
	void    SetPreferredPixelSize(WgSize size);


protected:
	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window);
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onEnable();
	void	_onDisable();


private:
	// Anti-alias
	void drawCircle(const int centerX, const int centerY, const float radX, const float radY);
	void drawLine(const float x0, const float y0, const float x1, const float y1);
	void plot(const int x, const int y, const float alpha);
	void plot4(const int centerX, const int centerY, const int deltaX, const int deltaY, const float alpha);
	static inline int ipart(double x) { return (int)floor(x); }
	static inline float fpart(float x) { return fabsf(x) - ipart(x); }
	static inline float rfpart(float x) { return 1.0f - fpart(x); }

	// Anti-alias
	int m_iNextPixel;
	WgCoord m_pAAPix[WG2_KNOB_PIXEL_BUFFER_SIZE];
	wg::HiColor m_pAACol[WG2_KNOB_PIXEL_BUFFER_SIZE];
	wg::HiColor	m_lineColor;

	WgSize m_preferredSize;


	float m_fValue;
};


#endif //WG2_KNOB_DOT_H
