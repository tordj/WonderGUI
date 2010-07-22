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

#ifndef	WG_GFX_DOT_H
#define WG_GFX_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef	WG_ORIGO_DOT_H
#	include <wg_origo.h>
#endif

#ifndef	WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef	WG_TEXTTOOL_DOT_H
#	include <wg_texttool.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

class WgColor;
class WgSurface;
class WgText;
class WgFont;
class WgCursorInstance;

//____ WgGfx __________________________________________________________________

class WgGfx
{
	friend class WgSurface;

public:

	// Device handling methods

	static void		SetDevice(WgGfxDevice * pDevice);
	static inline WgGfxDevice * GetDevice() { return m_pDevice; }

	// Device Direct Methods

	static inline bool 	BeginRender();
	static inline bool 	EndRender();

	static inline void	setTintColor( WgColor color );
	static inline bool	setBlendMode( WgBlendMode mode );
	static inline Uint32 setRenderFlags( Uint32 flags );

	static inline const WgColor&	tintColor();
	static inline WgBlendMode 		blendMode();

	static inline void	fillRect( const WgRect& rect, const WgColor& col );
	static inline void	clipFillRect( const WgRect& clip, const WgRect& rect, const WgColor& col );


	// High-level blit methods

	static inline void	clipBlitBlock(	const WgRect& clip, const WgBlock& block, const WgRect& dest );

	static inline void	blitBlock(		const WgBlock& block, const WgRect& dest );


	// Mid-level blit methods

	static inline void	clipBlitHorrBar(	const WgRect& _clip, const WgSurface * _pSurf, const WgRect& _src,
											const WgBorders& _borders, bool _bTile,
											Sint32 _dx, Sint32 _dy, Sint32 _len );

	static inline void	clipBlitVertBar(	const WgRect& _clip, const WgSurface * _pSurf, const WgRect& _src,
											const WgBorders& _borders, bool _bTile,
											Sint32 _dx, Sint32 _dy, Sint32 _len );


	static inline void	blitHorrBar(		const WgSurface * _pSurf, const WgRect& _src,
											const WgBorders& _borders, bool _bTile,
											Sint32 _dx, Sint32 _dy, Sint32 _len );

	static inline void	blitVertBar(		const WgSurface * _pSurf, const WgRect& _src,
											const WgBorders& _borders, bool _bTile,
											Sint32 _dx, Sint32 _dy, Sint32 _len );


	// Low-level blit methods, redirected to device.

	static inline void		blit( const WgSurface* src );
	static inline void		blit( const WgSurface* src, Sint32 dx, Sint32 dy );
	static inline void		blit( const WgSurface* src, const WgRect& srcrect, Sint32 dx, Sint32 dy  );

	static inline void		clipBlit( const WgRect& clip, const WgSurface* src );
	static inline void		clipBlit( const WgRect& clip, const WgSurface* src, Sint32 dx, Sint32 dy );
	static inline void		clipBlit( const WgRect& clip, const WgSurface* src, const WgRect& srcrect, Sint32 dx, Sint32 dy  );

	static inline void		stretchBlit( const WgSurface* _pSrc );
	static inline void		stretchBlit( const WgSurface * pSrc, const WgRect& dest );
	static inline void		stretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest );

	static inline void		clipStretchBlit( const WgRect& clip, const WgSurface* _pSrc );
	static inline void		clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest );
	static inline void		clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest );
	static inline void		clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias);
	static inline void		clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias);

	static inline void		tileBlit( const WgSurface* src );
	static inline void		tileBlit( const WgSurface* src, const WgRect& dest );
	static inline void		tileBlit( const WgSurface* src, const WgRect& srcrect, const WgRect& dest );

	static inline void		clipTileBlit( const WgRect& clip, const WgSurface* src );

	static inline void		clipTileBlit( const WgRect& clip, const WgSurface* src,
										  const WgRect& dest );

	static inline void		clipTileBlit( const WgRect& clip, const WgSurface* src,
										  const WgRect& srcrect, const WgRect& dest );


	// Deprecated High-level blit methods

	static void		clipBlitRow( const WgRect& clip, const WgSurface * src, const WgRect& srcrect,
								 Uint32 tileOfs, Uint32 tileLen, Sint32 dx, Sint32 dy, Uint32 len, bool bStretch = false );

	// High-level print methods

	static inline void		printText( const WgRect& clip, const WgText * pText, const WgRect& dest );
	static inline void		printTextWithCursor( const WgRect& clip, const WgText * pText, const WgCursorInstance& cursor, const WgRect& dest );

	// Low-level print methods

	static inline void		printLine( WgPen * pPen, const WgTextPropPtr& pTextProp, WgMode mode,
									   const WgChar * pString, int maxChars = 0x1FFFFFFF, bool bLineEnding = true );

private:
	static WgGfxDevice *	m_pDevice;
	static WgGfxDevice *	m_pNullDevice;
};

//____ Inline methods mapping directly to device _______________________________

inline bool WgGfx::BeginRender()
{
	return m_pDevice->BeginRender();
}

inline bool WgGfx::EndRender()
{
	return m_pDevice->EndRender();
}

inline void	WgGfx::setTintColor( WgColor color )
{
	m_pDevice->SetTintColor( color );
}

inline bool	WgGfx::setBlendMode( WgBlendMode mode )
{
	return m_pDevice->SetBlendMode( mode );
}

inline Uint32 WgGfx::setRenderFlags( Uint32 flags )
{
	return m_pDevice->SetRenderFlags( flags );
}

inline const WgColor& WgGfx::tintColor()
{
	return m_pDevice->GetTintColor();
}

inline WgBlendMode WgGfx::blendMode()
{
	return m_pDevice->GetBlendMode();
}

inline void WgGfx::fillRect( const WgRect& _rect, const WgColor& _col )
{
	m_pDevice->Fill( _rect, _col );
}

inline void WgGfx::clipFillRect( const WgRect& _clip, const WgRect& _rect, const WgColor& _col )
{
	m_pDevice->ClipFill( _clip, _rect , _col );
}

inline void WgGfx::blit( const WgSurface* src )
{
	m_pDevice->Blit( src );
}

inline void	WgGfx::blit( const WgSurface* src, Sint32 dx, Sint32 dy )
{
	m_pDevice->Blit( src, dx, dy );
}

inline void	WgGfx::blit( const WgSurface* src, const WgRect& srcrect, Sint32 dx, Sint32 dy  )
{
	m_pDevice->Blit( src, srcrect, dx, dy );
}

inline void WgGfx::clipBlit( const WgRect& clip, const WgSurface* src )
{
	m_pDevice->ClipBlit( clip, src );
}

inline void	WgGfx::clipBlit( const WgRect& clip, const WgSurface* src, Sint32 dx, Sint32 dy )
{
	m_pDevice->ClipBlit( clip, src, dx, dy );
}

inline void	WgGfx::clipBlit( const WgRect& clip, const WgSurface* src, const WgRect& srcrect, Sint32 dx, Sint32 dy  )
{
	m_pDevice->ClipBlit( clip, src, srcrect, dx, dy );
}

inline void	WgGfx::stretchBlit( const WgSurface* pSrc )
{
	m_pDevice->StretchBlit( pSrc );
}

inline void	WgGfx::stretchBlit( const WgSurface * pSrc, const WgRect& dest )
{
	m_pDevice->StretchBlit( pSrc, dest );
}

inline void	WgGfx::stretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest )
{
	m_pDevice->StretchBlit( pSrc, src, dest );
}

inline void	WgGfx::clipStretchBlit( const WgRect& clip, const WgSurface* pSrc )
{
	m_pDevice->ClipStretchBlit( clip, pSrc );
}

inline void WgGfx::clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest )
{
	m_pDevice->ClipStretchBlit( clip, pSrc, dest );
}

inline void WgGfx::clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest )
{
	m_pDevice->ClipStretchBlit( clip, pSrc, src, dest );
}

inline void WgGfx::clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias)
{
	m_pDevice->ClipStretchBlit( clip, pSrc, float(src.x), float(src.y), float(src.w), float(src.h), dx, dy, dw, dh, bTriLinear, mipBias );
}

inline void WgGfx::clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias)
{
	m_pDevice->ClipStretchBlit( clip, pSrc, sx, sy, sw, sh, dx, dy, dw, dh, bTriLinear, mipBias );
}

inline void WgGfx::tileBlit( const WgSurface* src )
{
	m_pDevice->TileBlit( src );
}

inline void WgGfx::tileBlit( const WgSurface* src, const WgRect& dest )
{
	m_pDevice->TileBlit( src, dest );
}

inline void WgGfx::tileBlit( const WgSurface* src, const WgRect& srcrect, const WgRect& dest )
{
	m_pDevice->TileBlit( src, srcrect, dest );
}

inline void	WgGfx::clipTileBlit( const WgRect& clip, const WgSurface* src )
{
	m_pDevice->ClipTileBlit( clip, src );
}

inline void	WgGfx::clipTileBlit( const WgRect& clip, const WgSurface* src,
								 const WgRect& dest )
{
	m_pDevice->ClipTileBlit( clip, src, dest );
}

inline void	WgGfx::clipTileBlit( const WgRect& clip, const WgSurface* src,
								 const WgRect& srcrect, const WgRect& dest )
{
	m_pDevice->ClipTileBlit( clip, src, srcrect, dest );
}

inline void	WgGfx::clipBlitHorrBar(	const WgRect& _clip, const WgSurface * _pSurf, const WgRect& _src,
									const WgBorders& _borders, bool _bTile,
									Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	m_pDevice->ClipBlitHorrBar( _clip, _pSurf, _src, _borders, _bTile, _dx, _dy, _len );
}

inline void	WgGfx::clipBlitVertBar(	const WgRect& _clip, const WgSurface * _pSurf, const WgRect& _src,
									const WgBorders& _borders, bool _bTile,
									Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	m_pDevice->ClipBlitVertBar( _clip, _pSurf, _src, _borders, _bTile, _dx, _dy, _len );
}


inline void	WgGfx::blitHorrBar(		const WgSurface * _pSurf, const WgRect& _src,
									const WgBorders& _borders, bool _bTile,
									Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	m_pDevice->BlitHorrBar( _pSurf, _src, _borders, _bTile, _dx, _dy, _len );
}

inline void	WgGfx::blitVertBar(		const WgSurface * _pSurf, const WgRect& _src,
									const WgBorders& _borders, bool _bTile,
									Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	m_pDevice->BlitVertBar( _pSurf, _src, _borders, _bTile, _dx, _dy, _len );
}

inline void	WgGfx::clipBlitBlock(	const WgRect& clip, const WgBlock& block, const WgRect& dest )
{
	m_pDevice->ClipBlitBlock( clip, block, dest );
}

inline void	WgGfx::blitBlock(		const WgBlock& block, const WgRect& dest )
{
	m_pDevice->BlitBlock( block, dest );
}

inline void WgGfx::printText( const WgRect& clip, const WgText * pText, const WgRect& dest )
{
	m_pDevice->PrintText( clip, pText, dest );
}

inline void WgGfx::printTextWithCursor( const WgRect& clip, const WgText * pText,
											const WgCursorInstance& cursor, const WgRect& dest )
{
	m_pDevice->PrintTextWithCursor( clip, pText, cursor, dest );
}

inline void WgGfx::printLine( WgPen * pPen, const WgTextPropPtr& pTextProp, WgMode mode, const WgChar * pString, int maxChars, bool bLineEnding )
{
	m_pDevice->PrintLine( pPen, pTextProp, mode, pString, maxChars, bLineEnding );
}





#endif // WG_GFX_DOT_H
