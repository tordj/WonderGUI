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

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif


#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif
/*
#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

*/

class	WgRect;
class	WgBorder;
class	WgSurface;
class	WgLegacyTextField;
class	WgCaretInstance;
class 	WgPen;

class WgGfxDevice;
typedef	WgStrongPtr<WgGfxDevice,WgObject_p>	WgGfxDevice_p;
typedef	WgWeakPtr<WgGfxDevice,WgObject_wp>	WgGfxDevice_wp;

class WgGfxDevice : public WgObject
{
public:

	bool					isInstanceOf( const char * pClassName ) const;
	const char *			className( void ) const;
	static const char		CLASSNAME[];
	static WgGfxDevice_p	cast( const WgObject_p& pObject );

	enum WgRenderFlags
	{
		WG_ORIENT_NORMAL				= 0x0,
		WG_ORIENT_ROTATE_CW90			= 0x1,
		WG_ORIENT_ROTATE_180			= 0x2,
		WG_ORIENT_ROTATE_CCW90			= 0x3,
		WG_ORIENT_MIRROR_X				= 0x4,
		WG_ORIENT_MIRROR_X_ROTATE_CW90	= 0x5,
		WG_ORIENT_MIRROR_X_ROTATE_180	= 0x6,
		WG_ORIENT_MIRROR_X_ROTATE_CCW90	= 0x7,
		WG_ORIENT_MASK					= 0x7,
	};


	virtual void	setTintColor( WgColor color );
	virtual bool	setBlendMode( WgBlendMode blendMode );
	virtual Uint32	setRenderFlags( Uint32 flags );
	virtual bool	setSaveDirtyRects( bool bSave );

	inline const WgColor&	getTintColor() const { return m_tintColor; }
	inline WgBlendMode 	getBlendMode() const { return m_blendMode; }
	inline Uint32		getRenderFlags() const { return m_renderFlags; }
	inline bool			getSaveDirtyRects() const { return m_bSaveDirtyRects; }

	// Geometry related methods.

	inline WgSize	canvasSize() const { return m_canvasSize; }

	// Begin/end render methods.

	virtual bool	beginRender();
	virtual bool	endRender();


	// Low level draw methods.

	virtual void	fill( const WgRect& rect, const WgColor& col ) = 0;

	virtual void	clipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col ) = 0;
	virtual void	clipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col ) = 0;
	virtual void	clipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness ) = 0;

	virtual void	drawArcNE( const WgRect& rect, WgColor color ) = 0;
	virtual void	drawElipse( const WgRect& rect, WgColor color ) = 0;
	virtual void	drawFilledElipse( const WgRect& rect, WgColor color ) = 0;

	virtual void	clipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color ) = 0;
	virtual void	clipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color ) = 0;
	virtual void	clipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color ) = 0;


	virtual void	blit( const WgSurface_p& pSrc );
	virtual void	blit( const WgSurface_p& pSrc, int dx, int dy );
	virtual void	blit( const WgSurface_p& pSrc, const WgRect& src, int dx, int dy ) = 0;

	virtual void	stretchBlit( const WgSurface_p& pSrc, bool bTriLinear = false, float mipmapBias = 0.f );
	virtual void	stretchBlit( const WgSurface_p& pSrc, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
	virtual void	stretchBlit( const WgSurface_p& pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );

	virtual void	tileBlit( const WgSurface_p& pSrc );
	virtual void	tileBlit( const WgSurface_p& pSrc, const WgRect& dest );
	virtual void	tileBlit( const WgSurface_p& pSrc, const WgRect& src, const WgRect& dest );


	virtual void	clipFill( const WgRect& clip, const WgRect& rect, const WgColor& col );

	virtual void	clipBlit( const WgRect& clip, const WgSurface_p& src );
	virtual void	clipBlit( const WgRect& clip, const WgSurface_p& src, int dx, int dy  );
	virtual void	clipBlit( const WgRect& clip, const WgSurface_p& src,
							  const WgRect& srcrect, int dx, int dy  );

	virtual void	clipStretchBlit( const WgRect& clip, const WgSurface_p& pSrc, bool bTriLinear = false, float mipBias = 0.f );
	virtual void	clipStretchBlit( const WgRect& clip, const WgSurface_p& pSrc, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	virtual void	clipStretchBlit( const WgRect& clip, const WgSurface_p& pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	virtual void	clipStretchBlit( const WgRect& clip, const WgSurface_p& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f);

	virtual void	clipTileBlit( const WgRect& clip, const WgSurface_p& src );
	virtual void	clipTileBlit( const WgRect& clip, const WgSurface_p& src,
								  const WgRect& dest );
	virtual void	clipTileBlit( const WgRect& clip, const WgSurface_p& src,
								  const WgRect& srcrect, const WgRect& dest );


	// Mid-level draw methods

	virtual void	clipBlitHorrBar(	const WgRect& _clip, const WgSurface_p& _pSurf, const WgRect& _src,
										const WgBorder& _borders, bool _bTile,
										int _dx, int _dy, int _len );

	virtual void	clipBlitVertBar(	const WgRect& _clip, const WgSurface_p& _pSurf, const WgRect& _src,
										const WgBorder& _borders, bool _bTile,
										int _dx, int _dy, int _len );


	virtual void	blitHorrBar(		const WgSurface_p& _pSurf, const WgRect& _src,
										const WgBorder& _borders, bool _bTile,
										int _dx, int _dy, int _len );

	virtual void	blitVertBar(		const WgSurface_p& _pSurf, const WgRect& _src,
										const WgBorder& _borders, bool _bTile,
										int _dx, int _dy, int _len );

	// High-level print methods

	virtual bool		printText( const WgRect& clip, const WgLegacyTextField * pText, const WgRect& dest );

	// Low-level print methods

	virtual void		printLine( WgPen& pen, const WgTextAttr& baseAttr, const WgChar * _pLine, int maxChars = INT_MAX, WgState state = WG_STATE_NORMAL );

	virtual void	fillSubPixel( const WgRectF& rect, const WgColor& col ) = 0;
	virtual void	stretchBlitSubPixel( const WgSurface_p& pSrc, float sx, float sy, float sw, float sh,
								   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f ) = 0;

	
protected:
	WgGfxDevice( WgSize canvasSize );
	virtual ~WgGfxDevice() {};

	void	_printTextSpan( WgPen& pen, const WgLegacyTextField * pText, int ofs, int len, bool bLineEnding );
	void	_printEllipsisTextSpan( WgPen& pen, const WgLegacyTextField * pText, int ofs, int len, int endX );

	void	_drawTextBg( const WgRect& clip, const WgLegacyTextField * pText, const WgRect& dest );
	void	_drawTextSectionBg( const WgRect& clip, const WgLegacyTextField * pText, const WgRect& dstRect,
							  int iStartOfs, int iEndOfs, WgColor color );


//
//	virtual void	blitSubPixel( const WgSurface * pSrc, const WgRect& srcrect,
//								  float dx, float dy ) = 0;

	virtual void	_drawUnderline( const WgRect& clip, const WgLegacyTextField * pText, int _x, int _y, int ofs, int maxChars );


	WgColor		m_tintColor;		// Current Tint color.
	WgBlendMode	m_blendMode;		// Current BlendMode.
	Uint32		m_renderFlags;		// Current flags.

	WgSize		m_canvasSize;
	bool		m_bSaveDirtyRects;

};

#endif	// WG_GFXDEVICE_DOT_H

