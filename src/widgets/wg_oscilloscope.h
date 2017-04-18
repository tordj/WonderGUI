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
#pragma once

#include <math.h>

#include <wg_widget.h>
#include <wg_skin.h>




namespace wg
{
	class Oscilloscope;
	typedef	StrongPtr<Oscilloscope,Widget_p>	Oscilloscope_p;
	typedef	WeakPtr<Oscilloscope,Widget_wp>		Oscilloscope_wp;	
	
	//____ Oscilloscope ____________________________________________________________

	class Oscilloscope : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static Oscilloscope_p	create() { return Oscilloscope_p(new Oscilloscope()); }

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static Oscilloscope_p	cast( const Object_p& pObject );

		//.____ Appearance __________________________________________

		void	setGridColor( Color color );
		Color   gridColor() const { return m_gridColor; }
		
		void	setVGridLines( int nLines, float pLines[] );
		void	setHGridLines( int nLines, float pLines[] );

		void	setLineColor( Color color );
		Color	lineColor() const { return m_lineColor; }

		void	setLineThickness( float thickness );
		float	lineThickness() const { return m_lineThickness; }

		void	setMarkerSkin( const Skin_p& pSkin );
		Skin_p	markerSkin() const { return m_pMarkerSkin; }

		//.____ Behavior ____________________________________________

		void	setRenderSegments( int nSegments );
		int		renderSegments() const { return m_nRenderSegments; }

		//.____ Control ________________________________________________

		void	setLinePoints( int nPoints, float pPointValues[] );

		void	clearMarkers();
		void	addMarker( int xOfs, float yOfs );

		//.____ Geometry ____________________________________________

		Size	preferredSize() const;
		


	protected:
		Oscilloscope();
		virtual ~Oscilloscope();
		virtual Widget* _newOfMyType() const { return new Oscilloscope(); };

		void	_cloneContent( const Widget * _pOrg );
		void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_setSize( const Size& size );

		static const int PixelBufferSize = 16000;


	private:
		void	_updateRenderSegments( int nSegments, Rect * pSegments );
		void	_resampleLinePoints( Size sz );
		void 	_antiAlias(const int nPoints, const float *pYval, Coord canvasOfs );


		static inline int ipart(double x) { return (int)floor(x); }
		static inline float fpart(float x) { return fabsf(x) - ipart(x); }
		static inline float rfpart(float x) { return 1.0f - fpart(x); }
		void plot(const int x, const int y, const float alpha);

		struct Marker
		{
			int		x;		// Offset in pixels from left border of oscillator
			float	y;		// Offset in y from middle of oscillator. Range: -1.0 to 1.0;
		};

		Color			m_gridColor;
		Color			m_lineColor;
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
		
		Skin_p			m_pMarkerSkin;

		// Anti-alias
		int m_iNextPixel;
		Coord m_pAAPix[PixelBufferSize];
		Color m_pAACol[PixelBufferSize];
	};

} // namespace wg
#endif //WG_OSCILLOSCOPE_DOT_H
