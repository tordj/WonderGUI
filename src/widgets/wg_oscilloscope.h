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
	
	class Oscilloscope;
	typedef	StrongPtr<Oscilloscope,Widget_p>		Oscilloscope_p;
	typedef	WeakPtr<Oscilloscope,Widget_wp>	Oscilloscope_wp;
	
	//____ Oscilloscope ____________________________________________________________
	
	class Oscilloscope : public Widget
	{
	public:
		static Oscilloscope_p	create() { return Oscilloscope_p(new Oscilloscope()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Oscilloscope_p	cast( const Object_p& pObject );
	
		void	setGridColor( Color color );
		void	setVGridLines( int nLines, float pLines[] );
		void	setHGridLines( int nLines, float pLines[] );
	
		void	setLineColor( Color color );
		void	setLineThickness( float thickness );
	
		void	setLinePoints( int nPoints, float pPointValues[] );
	
		void	clearMarkers();
		void	addMarker( int xOfs, float yOfs );
		void	setMarkerSkin( const Skin_p& pSkin );
	
		Size	preferredSize() const;
	
	protected:
		Oscilloscope();
		virtual ~Oscilloscope();
		virtual Widget* _newOfMyType() const { return new Oscilloscope(); };
	
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
	
	
	private:
	
		struct Marker
		{
			int		x;		// Offset in pixels from left border of oscillator
			float	y;		// Offset in y from middle of oscillator. Range: -1.0 to 1.0;
		};
	
		Color			m_gridColor;
		Color			m_lineColor;
		float			m_lineThickness;
	
		int				m_nVGridLines;
		float *			m_pVGridLines;
		int				m_nHGridLines;
		float *			m_pHGridLines;
	
		int				m_nLinePoints;
		float *			m_pLinePoints;
	
		int				m_nMarkers;
		Marker *		m_pMarkers;
		
		Skin_p		m_pMarkerSkin;
	};
	
	

} // namespace wg
#endif //WG_OSCILLOSCOPE_DOT_H
