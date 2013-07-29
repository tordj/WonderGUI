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

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
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
class	WgBorders;
class	WgSurface;
class	WgText;
class	WgCursorInstance;
class 	WgPen;

class WgGfxDevice;
typedef	WgSmartPtr<WgGfxDevice,WgObjectPtr>	WgGfxDevicePtr;
typedef	WgWeakPtr<WgGfxDevice,WgObjectWeakPtr>	WgGfxDeviceWeakPtr;

class WgGfxDevice : public WgObject
{
public:

	bool					IsInstanceOf( const char * pClassName ) const;
	const char *			ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgGfxDevicePtr	Cast( const WgObjectPtr& pObject );

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


	virtual void	SetTintColor( WgColor color );
	virtual bool	SetBlendMode( WgBlendMode blendMode );
	virtual Uint32	SetRenderFlags( Uint32 flags );
	virtual bool	SetSaveDirtyRects( bool bSave );

	inline const WgColor&	GetTintColor() const { return m_tintColor; }
	inline WgBlendMode 	GetBlendMode() const { return m_blendMode; }
	inline Uint32		GetRenderFlags() const { return m_renderFlags; }
	inline bool			GetSaveDirtyRects() const { return m_bSaveDirtyRects; }

	// Geometry related methods.

	inline WgSize	CanvasSize() const { return m_canvasSize; }

	// Begin/end render methods.

	virtual bool	BeginRender();
	virtual bool	EndRender();


	// Low level draw methods.

	virtual void	Fill( const WgRect& rect, const WgColor& col ) = 0;

	virtual void	ClipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col ) = 0;
	virtual void	ClipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col ) = 0;
	virtual void	ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness ) = 0;

	virtual void	DrawArcNE( const WgRect& rect, WgColor color ) = 0;
	virtual void	DrawElipse( const WgRect& rect, WgColor color ) = 0;
	virtual void	DrawFilledElipse( const WgRect& rect, WgColor color ) = 0;

	virtual void	ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color ) = 0;
	virtual void	ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color ) = 0;
	virtual void	ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color ) = 0;


	virtual void	Blit( const WgSurfacePtr& pSrc );
	virtual void	Blit( const WgSurfacePtr& pSrc, int dx, int dy );
	virtual void	Blit( const WgSurfacePtr& pSrc, const WgRect& src, int dx, int dy ) = 0;

	virtual void	StretchBlit( const WgSurfacePtr& pSrc, bool bTriLinear = false, float mipmapBias = 0.f );
	virtual void	StretchBlit( const WgSurfacePtr& pSrc, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
	virtual void	StretchBlit( const WgSurfacePtr& pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );

	virtual void	TileBlit( const WgSurfacePtr& pSrc );
	virtual void	TileBlit( const WgSurfacePtr& pSrc, const WgRect& dest );
	virtual void	TileBlit( const WgSurfacePtr& pSrc, const WgRect& src, const WgRect& dest );


	virtual void	ClipFill( const WgRect& clip, const WgRect& rect, const WgColor& col );

	virtual void	ClipBlit( const WgRect& clip, const WgSurfacePtr& src );
	virtual void	ClipBlit( const WgRect& clip, const WgSurfacePtr& src, int dx, int dy  );
	virtual void	ClipBlit( const WgRect& clip, const WgSurfacePtr& src,
							  const WgRect& srcrect, int dx, int dy  );

	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, bool bTriLinear = false, float mipBias = 0.f );
	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	virtual void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f);

	virtual void	ClipTileBlit( const WgRect& clip, const WgSurfacePtr& src );
	virtual void	ClipTileBlit( const WgRect& clip, const WgSurfacePtr& src,
								  const WgRect& dest );
	virtual void	ClipTileBlit( const WgRect& clip, const WgSurfacePtr& src,
								  const WgRect& srcrect, const WgRect& dest );


	// Mid-level draw methods

	virtual void	ClipBlitHorrBar(	const WgRect& _clip, const WgSurfacePtr& _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										int _dx, int _dy, int _len );

	virtual void	ClipBlitVertBar(	const WgRect& _clip, const WgSurfacePtr& _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										int _dx, int _dy, int _len );


	virtual void	BlitHorrBar(		const WgSurfacePtr& _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										int _dx, int _dy, int _len );

	virtual void	BlitVertBar(		const WgSurfacePtr& _pSurf, const WgRect& _src,
										const WgBorders& _borders, bool _bTile,
										int _dx, int _dy, int _len );

	// High-level print methods

	virtual bool		PrintText( const WgRect& clip, const WgText * pText, const WgRect& dest );

	// Low-level print methods

	virtual void		PrintLine( WgPen& pen, const WgTextAttr& baseAttr, const WgChar * _pLine, int maxChars = INT_MAX, WgState state = WG_STATE_NORMAL );

	virtual void	FillSubPixel( const WgRectF& rect, const WgColor& col ) = 0;
	virtual void	StretchBlitSubPixel( const WgSurfacePtr& pSrc, float sx, float sy, float sw, float sh,
								   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f ) = 0;

	
protected:
	WgGfxDevice( WgSize canvasSize );
	virtual ~WgGfxDevice() {};

	void	_printTextSpan( WgPen& pen, const WgText * pText, int ofs, int len, bool bLineEnding );
	void	_printEllipsisTextSpan( WgPen& pen, const WgText * pText, int ofs, int len, int endX );

	void	_drawTextBg( const WgRect& clip, const WgText * pText, const WgRect& dest );
	void	_drawTextSectionBg( const WgRect& clip, const WgText * pText, const WgRect& dstRect,
							  int iStartOfs, int iEndOfs, WgColor color );


//
//	virtual void	BlitSubPixel( const WgSurface * pSrc, const WgRect& srcrect,
//								  float dx, float dy ) = 0;

	virtual void	_drawUnderline( const WgRect& clip, const WgText * pText, int _x, int _y, int ofs, int maxChars );


	WgColor		m_tintColor;		// Current Tint color.
	WgBlendMode	m_blendMode;		// Current BlendMode.
	Uint32		m_renderFlags;		// Current flags.

	WgSize		m_canvasSize;
	bool		m_bSaveDirtyRects;

};

#endif	// WG_GFXDEVICE_DOT_H

