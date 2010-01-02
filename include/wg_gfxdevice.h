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

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

/*
#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif
*/

class	WgBlock;
class	WgRect;
class	WgBorders;
class	WgSurface;
class	WgText;
class	WgCursorInstance;

class WgGfxDevice
{
public:

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

	virtual ~WgGfxDevice() {};

	virtual void	SetTintColor( WgColor color );
	virtual bool	SetBlendMode( WgBlendMode blendMode );
	virtual Uint32	SetRenderFlags( Uint32 flags );
	virtual bool	SetSaveDirtyRects( bool bSave );

	inline WgColor&	 	GetTintColor()	{ return m_tintColor; }
	inline WgBlendMode 	GetBlendMode()	{ return m_blendMode; }
	inline Uint32		GetRenderFlags()	{ return m_renderFlags; }
	inline bool			GetSaveDirtyRects() { return m_bSaveDirtyRects; }

	// Begin/end render methods.

	virtual bool	BeginRender();
	virtual bool	EndRender();


	// Low level draw methods.

	virtual void	Fill( const WgRect& rect, const WgColor& col ) = 0;

	virtual void	Blit( const WgSurface* pSrc );
	virtual void	Blit( const WgSurface* pSrc, Sint32 dx, Sint32 dy );
	virtual void	Blit( const WgSurface* pSrc, const WgRect& src, Sint32 dx, Sint32 dy ) = 0;

	virtual void	StretchBlit( const WgSurface * pSrc );
	virtual void	StretchBlit( const WgSurface * pSrc, const WgRect& dest );
	virtual void	StretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest );

	virtual void	TileBlit( const WgSurface* pSrc );
	virtual void	TileBlit( const WgSurface* pSrc, const WgRect& dest );
	virtual void	TileBlit( const WgSurface* pSrc, const WgRect& src, const WgRect& dest );


	virtual void	ClipFill( const WgRect& clip, const WgRect& rect, const WgColor& col );

	virtual void	ClipBlit( const WgRect& clip, const WgSurface* src );
	virtual void	ClipBlit( const WgRect& clip, const WgSurface* src, Sint32 dx, Sint32 dy  );
	virtual void	ClipBlit( const WgRect& clip, const WgSurface* src,
							  const WgRect& srcrect, Sint32 dx, Sint32 dy  );

	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc );
	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc,
									 const WgRect& dest );
	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc,
									 const WgRect& src, const WgRect& dest );

	virtual void	ClipTileBlit( const WgRect& clip, const WgSurface* src );
	virtual void	ClipTileBlit( const WgRect& clip, const WgSurface* src,
								  const WgRect& dest );
	virtual void	ClipTileBlit( const WgRect& clip, const WgSurface* src,
								  const WgRect& srcrect, const WgRect& dest );


	// High-level draw methods

	virtual void	ClipBlitBlock(	const WgRect& clip, const WgBlock& block, const WgRect& dest );

	virtual void	BlitBlock(		const WgBlock& block, const WgRect& dest );


	// Mid-level draw methods

	virtual void	ClipBlitHorrBar(	const WgRect& _clip, const WgSurface * _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										Sint32 _dx, Sint32 _dy, Sint32 _len );

	virtual void	ClipBlitVertBar(	const WgRect& _clip, const WgSurface * _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										Sint32 _dx, Sint32 _dy, Sint32 _len );


	virtual void	BlitHorrBar(		const WgSurface * _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										Sint32 _dx, Sint32 _dy, Sint32 _len );

	virtual void	BlitVertBar(		const WgSurface * _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										Sint32 _dx, Sint32 _dy, Sint32 _len );

	// High-level print methods

	virtual void		PrintText( const WgText * pText, const WgRect& dest );
	virtual void		ClipPrintText( const WgRect& clip, const WgText * pText, const WgRect& dest );

	virtual void		PrintTextWithCursor( const WgText * pText, const WgCursorInstance& cursor, const WgRect& dest );
	virtual void		ClipPrintTextWithCursor( const WgRect& clip, const WgText * pText, const WgCursorInstance& cursor, const WgRect& dest );

	// Low-level print methods

	virtual void		PrintLine( const WgTextPropPtr& pTextProp, WgMode mode,
									int tabOrigo, int x, int y, const WgChar * pString, Uint32 maxChars = 0xFFFF );
	virtual void		ClipPrintLine( const WgRect& clip, const WgTextPropPtr& pTextProp, WgMode mode,
										int tabOrigo, int x, int y, const WgChar * pString, Uint32 maxChars = 0xFFFF );


protected:
	WgGfxDevice();

//	virtual void	FillSubPixel( float dx, float dy, float dw, float dh, const WgColor& col ) = 0;
//
//	virtual void	BlitSubPixel( const WgSurface * pSrc, const WgRect& srcrect,
//								  float dx, float dy ) = 0;
	virtual void	StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
								   		 float dx, float dy, float dw, float dh ) = 0;
	virtual void 	DrawUnderline( 	const WgRect& clip, const WgTextPropPtr& pTextProp,
									WgMode mode, int _x, int _y, const WgChar * pLine );



	WgColor		m_tintColor;		// Current Tint color.
	WgBlendMode	m_blendMode;		// Current BlendMode.
	Uint32		m_renderFlags;		// Current flags.

	Uint32		m_canvasHeight;
	Uint32		m_canvasWidth;
	bool		m_bSaveDirtyRects;

};

#endif	// WG_GFXDEVICE_DOT_H

