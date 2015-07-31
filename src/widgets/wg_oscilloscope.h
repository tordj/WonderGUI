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

namespace wg 
{
	
	class WgOscilloscope;
	typedef	WgStrongPtr<WgOscilloscope,WgWidget_p>		WgOscilloscope_p;
	typedef	WgWeakPtr<WgOscilloscope,WgWidget_wp>	WgOscilloscope_wp;
	
	//____ WgOscilloscope ____________________________________________________________
	
	class WgOscilloscope : public WgWidget
	{
	public:
		static WgOscilloscope_p	create() { return WgOscilloscope_p(new WgOscilloscope()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgOscilloscope_p	cast( const WgObject_p& pObject );
	
		void	setGridColor( WgColor color );
		void	setVGridLines( int nLines, float pLines[] );
		void	setHGridLines( int nLines, float pLines[] );
	
		void	setLineColor( WgColor color );
		void	setLineThickness( float thickness );
	
		void	setLinePoints( int nPoints, float pPointValues[] );
	
		void	clearMarkers();
		void	addMarker( int xOfs, float yOfs );
		void	setMarkerSkin( const WgSkin_p& pSkin );
	
		WgSize	preferredSize() const;
	
	protected:
		WgOscilloscope();
		virtual ~WgOscilloscope();
		virtual WgWidget* _newOfMyType() const { return new WgOscilloscope(); };
	
		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	
	
	private:
	
		struct Marker
		{
			int		x;		// Offset in pixels from left border of oscillator
			float	y;		// Offset in y from middle of oscillator. Range: -1.0 to 1.0;
		};
	
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
		
		WgSkin_p		m_pMarkerSkin;
	};
	
	

} // namespace wg
#endif //WG_OSCILLOSCOPE_DOT_H
