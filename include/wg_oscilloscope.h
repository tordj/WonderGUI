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
#ifndef WG_OSCILLOSCOPE_DOT_H
#define WG_OSCILLOSCOPE_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

//____ WgOscilloscope ____________________________________________________________

class WgOscilloscope : public WgWidget
{
public:
	WgOscilloscope();
	virtual ~WgOscilloscope();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgOscilloscope(); };

	void	SetBackground( const WgSkinPtr& skin );
	void	SetGridColor( WgColor color );
	void	SetVGridLines( int nLines, float pLines[] );
	void	SetHGridLines( int nLines, float pLines[] );

	void	SetLineColor( WgColor color );
	void	SetLineThickness( float thickness );

	void	SetLinePoints( int nPoints, float pPointValues[] );

	void	ClearMarkers();
	void	AddMarker( int xOfs, float yOfs );
	void	SetMarkerGfx( const WgBlocksetPtr& pBlockset );

	WgSize	PreferredSize() const;

protected:

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );


private:

	struct Marker
	{
		int		x;		// Offset in pixels from left border of oscillator
		float	y;		// Offset in y from middle of oscillator. Range: -1.0 to 1.0;
	};

	WgSkinPtr		m_pBG;
	WgColor			m_gridColor;
	WgColor			m_lineColor;
	float			m_lineThickness;

	int				m_nVGridLines;
	float *			m_pVGridLines;
	int				m_nHGridLines;
	float *			m_pHGridLines;

	int				m_nLinePoints;
	float *			m_pLinePoints;

	int				m_nMarkers;
	Marker *		m_pMarkers;
	
	WgBlocksetPtr	m_pMarkerGfx;
};


#endif //WG_OSCILLOSCOPE_DOT_H
