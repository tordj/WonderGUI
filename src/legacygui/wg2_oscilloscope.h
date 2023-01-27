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
#ifndef WG2_OSCILLOSCOPE_DOT_H
#define WG2_OSCILLOSCOPE_DOT_H

#include <math.h>

#ifndef WG2_WIDGET_DOT_H
#	include <wg2_widget.h>
#endif

#include <wg_skin.h>

#ifndef WG2_BLOCKSET_DOT_H
#	include <wg2_blockset.h>
#endif

#include <wg2_geo.h>
#include <wg2_color.h>

//____ WgOscilloscope ____________________________________________________________
#define WG2_OSC_PIXEL_BUFFER_SIZE 16000

class WgOscilloscope : public WgWidget
{
public:
	WgOscilloscope();
	virtual ~WgOscilloscope();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgOscilloscope(); };

	void	SetBackground( wg::Skin * skin );
	void	SetGridColor( WgColor color );
	void	SetVGridLines( int nLines, float pLines[] );
	void	SetHGridLines( int nLines, float pLines[] );

	void	SetLineColor( WgColor color );
	void	SetLineThickness( float thickness );

	void	SetLinePoints( int nPoints, float pPointValues[] );

	void	ClearMarkers();
	void	AddMarker( int xOfs, float yOfs );
	void	SetMarkerGfx( wg::Skin * pSkin );

	WgSize	PreferredPixelSize() const;

	void	SetRenderSegments( int nSegments );


protected:

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window );
	void	_onNewSize( const WgSize& size );


private:
	void	_updateRenderSegments( int nSegments, WgRect * pSegments );
	void	_resampleLinePoints( WgSize sz );
	void 	_antiAlias(const int nPoints, const float *pYval, WgCoord canvasOfs );


	static inline int ipart(double x) { return (int)floor(x); }
	static inline float fpart(float x) { return fabsf(x) - ipart(x); }
	static inline float rfpart(float x) { return 1.0f - fpart(x); }
	void plot(const int x, const int y, const float alpha);

	struct Marker
	{
		int		x;		// Offset in pixels from left border of oscillator
		float	y;		// Offset in y from middle of oscillator. Range: -1.0 to 1.0;
	};

	wg::Skin_p		m_pBG;
	WgColor			m_gridColor;
	wg::HiColor		m_lineColor;
	float			m_lineThickness;
	int				m_nRenderSegments;


	int				m_nVGridLines;
	float *			m_pVGridLines;
	int				m_nHGridLines;
	float *			m_pHGridLines;

	int				m_nLinePoints;
	float *			m_pLinePoints;

	int				m_nDisplayPoints;
	float *			m_pDisplayPoints;


	int				m_nMarkers;
	Marker *		m_pMarkers;

	wg::Skin_p		m_pMarkerSkin;

	// Anti-alias
	int m_iNextPixel;
	WgCoord m_pAAPix[WG2_OSC_PIXEL_BUFFER_SIZE];
	wg::HiColor m_pAACol[WG2_OSC_PIXEL_BUFFER_SIZE];
};


#endif //WG2_OSCILLOSCOPE_DOT_H
