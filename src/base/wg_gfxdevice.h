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

#include <climits>

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_COLOR_DOT_H
#	include <wg_color.h>
#endif


#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif

namespace wg 
{
	class	Rect;
	class	Border;
	class	Surface;
	class	LegacyTextItem;
	class	CaretInstance;
	class 	Pen;
	
	class GfxDevice;
	typedef	StrongPtr<GfxDevice,Object_p>	GfxDevice_p;
	typedef	WeakPtr<GfxDevice,Object_wp>	GfxDevice_wp;
	
	class GfxDevice : public Object
	{
	public:
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static GfxDevice_p		cast( const Object_p& pObject );

		virtual const char *	surfaceClassName( void ) const = 0;
		virtual SurfaceFactory_p	surfaceFactory() = 0;
		
		
		virtual void		setTintColor( Color color );
		virtual bool		setBlendMode( BlendMode blendMode );
		virtual uint32_t	setRenderFlags( uint32_t flags );
		virtual bool		setSaveDirtyRects( bool bSave );
	
		inline const Color&	getTintColor() const { return m_tintColor; }
		inline BlendMode 	getBlendMode() const { return m_blendMode; }
		inline uint32_t		getRenderFlags() const { return m_renderFlags; }
		inline bool			getSaveDirtyRects() const { return m_bSaveDirtyRects; }
	
		// Geometry related methods.
	
		inline Size	canvasSize() const { return m_canvasSize; }
	
		// Begin/end render methods.
	
		virtual bool	beginRender();
		virtual bool	endRender();
	
	
		// Low level draw methods.
	
		virtual void	fill( const Rect& rect, const Color& col ) = 0;
	
		virtual void	clipDrawHorrLine( const Rect& clip, const Coord& start, int length, const Color& col ) = 0;
		virtual void	clipDrawVertLine( const Rect& clip, const Coord& start, int length, const Color& col ) = 0;
		virtual void	clipPlotSoftPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color& col, float thickness ) = 0;

        virtual void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) = 0;
        virtual void    clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors) = 0;

		virtual void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f ) = 0;
		virtual void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f ) = 0;

	
		virtual void	drawArcNE( const Rect& rect, Color color ) = 0;
		virtual void	drawElipse( const Rect& rect, Color color ) = 0;
		virtual void	drawFilledElipse( const Rect& rect, Color color ) = 0;
	
		virtual void	clipDrawArcNE( const Rect& clip, const Rect& rect, Color color ) = 0;
		virtual void	clipDrawElipse( const Rect& clip, const Rect& rect, Color color ) = 0;
		virtual void	clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color ) = 0;
	
	
		virtual void	blit( const Surface_p& pSrc );
		virtual void	blit( const Surface_p& pSrc, int dx, int dy );
		virtual void	blit( const Surface_p& pSrc, const Rect& src, int dx, int dy ) = 0;
	
		virtual void	stretchBlit( const Surface_p& pSrc, bool bTriLinear = false, float mipmapBias = 0.f );
		virtual void	stretchBlit( const Surface_p& pSrc, const Rect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
		virtual void	stretchBlit( const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
	
		virtual void	tileBlit( const Surface_p& pSrc );
		virtual void	tileBlit( const Surface_p& pSrc, const Rect& dest );
		virtual void	tileBlit( const Surface_p& pSrc, const Rect& src, const Rect& dest );
	
	
		virtual void	clipFill( const Rect& clip, const Rect& rect, const Color& col );
	
		virtual void	clipBlit( const Rect& clip, const Surface_p& src );
		virtual void	clipBlit( const Rect& clip, const Surface_p& src, int dx, int dy  );
		virtual void	clipBlit( const Rect& clip, const Surface_p& src,
								  const Rect& srcrect, int dx, int dy  );
	
		virtual void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, bool bTriLinear = false, float mipBias = 0.f );
		virtual void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& dest, bool bTriLinear = false, float mipBias = 0.f );
		virtual void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear = false, float mipBias = 0.f );
		virtual void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f);
	
		virtual void	clipTileBlit( const Rect& clip, const Surface_p& src );
		virtual void	clipTileBlit( const Rect& clip, const Surface_p& src,
									  const Rect& dest );
		virtual void	clipTileBlit( const Rect& clip, const Surface_p& src,
									  const Rect& srcrect, const Rect& dest );
	
	
		// Mid-level draw methods
	
		virtual void	clipBlitHorrBar(	const Rect& _clip, const Surface_p& _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											int _dx, int _dy, int _len );
	
		virtual void	clipBlitVertBar(	const Rect& _clip, const Surface_p& _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											int _dx, int _dy, int _len );
	
	
		virtual void	blitHorrBar(		const Surface_p& _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											int _dx, int _dy, int _len );
	
		virtual void	blitVertBar(		const Surface_p& _pSurf, const Rect& _src,
											const Border& _borders, bool _bTile,
											int _dx, int _dy, int _len );
		
		virtual void	fillSubPixel( const RectF& rect, const Color& col ) = 0;
		virtual void	stretchBlitSubPixel( const Surface_p& pSrc, float sx, float sy, float sw, float sh,
									   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f ) = 0;
	
		
	protected:
		GfxDevice( Size canvasSize );
		virtual ~GfxDevice() {};
		
		Color		m_tintColor;		// Current Tint color.
		BlendMode	m_blendMode;		// Current BlendMode.
		uint32_t	m_renderFlags;		// Current flags.
	
		Size		m_canvasSize;
		bool		m_bSaveDirtyRects;
	
	};
	

} // namespace wg
#endif	// WG_GFXDEVICE_DOT_H

