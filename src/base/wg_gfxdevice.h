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

#ifndef	WG_GFXDEVICE_DOT_H
#define WG_GFXDEVICE_DOT_H
#pragma once

#include <climits>

#include <wg_pointers.h>
#include <wg_types.h>
#include <wg_color.h>


#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>

namespace wg
{
	class	Rect;
	class	Border;
	class	Surface;
	class	LegacyTextItem;
	class	CaretInstance;
	class 	Pen;

	class GfxDevice;
	typedef	StrongPtr<GfxDevice>	GfxDevice_p;
	typedef	WeakPtr<GfxDevice>	GfxDevice_wp;


	//____ WaveLine ___________________________________________________________

	struct WaveLine
	{
		int		length;
		float	thickness;
		Color	color;
		int *	pWave;      // Pixel offset in 24.8 format.
		int		hold;      // Value for extending the line if it is too short (or completely missing).
	};
	
	class GfxDevice : public Object
	{
	public:
		friend class GfxStreamPlayer;

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static GfxDevice_p		cast( Object * pObject );
		virtual const char *	surfaceClassName( void ) const = 0;

		//.____ Misc _______________________________________________________

		virtual SurfaceFactory_p	surfaceFactory() = 0;
		
		//.____ Geometry _________________________________________________

		virtual bool		setCanvas(Surface * pCanvas) = 0;
		Surface_p	canvas() const { return m_pCanvas; }

		inline Size	canvasSize() const { return m_canvasSize; }

		//.____ State _________________________________________________

		virtual void		setTintColor( Color color );
		virtual bool		setBlendMode( BlendMode blendMode );
	
		inline const Color&	tintColor() const { return m_tintColor; }
		inline BlendMode 	blendMode() const { return m_blendMode; }
		
		//.____ Rendering ________________________________________________
	
		virtual bool	beginRender();
		virtual bool	endRender();
		
		// Low level draw methods.
	
		virtual void	fill( const Rect& rect, const Color& col ) = 0;
	
        virtual void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors);

		virtual void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f );
		virtual void	drawLine( Coord begin, Direction dir, int length, Color col, float thickness = 1.f);

		virtual void	blit( Surface * pSrc );
		virtual void	blit( Surface * pSrc, Coord dest );
		virtual void	blit( Surface * pSrc, const Rect& src, Coord dest ) = 0;
	
		virtual void	stretchBlit(Surface * pSrc);
		virtual void	stretchBlit( Surface * pSrc, const Rect& dest );
		virtual void	stretchBlit( Surface * pSrc, const Rect& src, const Rect& dest );
		virtual void	stretchBlit(Surface * pSrc, const RectF& src, const Rect& dest) = 0;

		virtual void	tileBlit(Surface * pSrc);
		virtual void	tileBlit( Surface * pSrc, const Rect& dest );
		virtual void	tileBlit( Surface * pSrc, const Rect& src, const Rect& dest );

		virtual void	drawHorrWave(Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill);

		// Versions with clipping

		virtual void    clipPlotPixels(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors) = 0;

		virtual void	clipDrawLine(const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f) = 0;
		virtual void	clipDrawLine(const Rect& clip, Coord begin, Direction dir, int length, Color col, float thickness = 1.f);

		virtual void	clipFill( const Rect& clip, const Rect& rect, const Color& col );
	
		virtual void	clipBlit( const Rect& clip, Surface * pSrc );
		virtual void	clipBlit( const Rect& clip, Surface * pSrc, Coord dest  );
		virtual void	clipBlit( const Rect& clip, Surface * pSrc, const Rect& src, Coord dest  );
	
		virtual void	clipStretchBlit(const Rect& clip, Surface * pSrc);
		virtual void	clipStretchBlit( const Rect& clip, Surface * pSrc, const Rect& dest );
		virtual void	clipStretchBlit( const Rect& clip, Surface * pSrc, const Rect& src, const Rect& dest );
		virtual void	clipStretchBlit( const Rect& clip, Surface * pSrc, const RectF& src, const Rect& dest );
	
		virtual void	clipTileBlit(const Rect& clip, Surface * pSrc);
		virtual void	clipTileBlit( const Rect& clip, Surface * pSrc, const Rect& dest );
		virtual void	clipTileBlit( const Rect& clip, Surface * pSrc, const Rect& src, const Rect& dest );

		virtual void	clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill) = 0;

		// Special draw methods

		virtual void	clipBlitFromCanvas(const Rect& clip, Surface* pSrc, const Rect& src, Coord dest);	// Blit from surface that has been used as canvas. Will flip Y on OpenGL.

	
		// Mid-level draw methods
	
		virtual void	clipBlitHorrBar(	const Rect& _clip, Surface * _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											Coord dest, int _len );
	
		virtual void	clipBlitVertBar(	const Rect& _clip, Surface * _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											Coord dest, int _len );
	
	
		virtual void	blitHorrBar(		Surface * _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											Coord dest, int _len );
	
		virtual void	blitVertBar(		Surface * _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											Coord dest, int _len );
		
		virtual void	fillSubPixel( const RectF& rect, const Color& col ) = 0;

//		virtual void	stretchBlitSubPixel( Surface * pSrc, float sx, float sy, float sw, float sh,
//									   		 float dx, float dy, float dw, float dh ) = 0;


		
	protected:
		GfxDevice( Size canvasSize );
		virtual ~GfxDevice();

		virtual void	_drawStraightLine(Coord start, Orientation orientation, int _length, const Color& _col ) = 0;


		// Static, shared data

		static	int		s_gfxDeviceCount;				// Number of existing gfxDevices. Ref count for shared data.

		void	_genCurveTab();
		void	_traceLine(int * pDest, int nPoints, const WaveLine * pWave, int offset);

		const static int c_nCurveTabEntries = 1024;
		static int *	s_pCurveTab;

		//

		Surface_p	m_pCanvas;

		Color		m_tintColor;		// Current Tint color.
		BlendMode	m_blendMode;		// Current BlendMode.
		uint32_t	m_renderFlags;		// Current flags.
	
		Size		m_canvasSize;
	
		Rect		m_dummyClip;
	};	

} // namespace wg
#endif	// WG_GFXDEVICE_DOT_H

