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

#ifndef WG_BLOCKSET_DOT_H
#define WG_BLOCKSET_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

#ifndef WG_REFCOUNTED_DOT_H
#	include <wg_refcounted.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif


class WgRect;
class WgBlock;
class WgSurface;

enum WgBlockFlags
{
	WG_OPAQUE_CENTER	= 0x01,
	WG_OPAQUE			= 0x03,

	WG_TILE_CENTER		= 0x04,
	WG_SCALE_CENTER		= 0x08,

	WG_TILE_LEFT		= 0x10,
	WG_TILE_RIGHT		= 0x20,
	WG_TILE_TOP			= 0x40,
	WG_TILE_BOTTOM		= 0x80,

	WG_TILE_BORDERS		= WG_TILE_LEFT | WG_TILE_RIGHT | WG_TILE_TOP | WG_TILE_BOTTOM,
	WG_TILE_ALL			= WG_TILE_BORDERS | WG_TILE_CENTER,

	// On a skip-request the block is assumed to be totally transparent and
	// will not be rendered at all (even if it actually contains graphics).

	WG_SKIP_NORMAL		= 0x100,
	WG_SKIP_MARKED		= 0x200,
	WG_SKIP_SELECTED	= 0x400,
	WG_SKIP_DISABLED	= 0x800,
	WG_SKIP_SPECIAL		= 0x1000,

	// Flags that can not be set by user

	WG_HAS_BORDERS		= 0x2000,	
};


//____ WgHorrTileX ____________________________________________________________

class	WgHorrTile2 : public WgRect
{
public:
	WgHorrTile2() {};
	WgHorrTile2( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile2( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgHorrTile3 : public WgRect
{
public:
	WgHorrTile3() {};
	WgHorrTile3( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile3( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgHorrTile4 : public WgRect
{
public:
	WgHorrTile4() {};
	WgHorrTile4( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile4( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgHorrTile5 : public WgRect
{
public:
	WgHorrTile5() {};
	WgHorrTile5( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile5( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

//____ WgVertTileX ____________________________________________________________

class	WgVertTile2 : public WgRect
{
public:
	WgVertTile2() {};
	WgVertTile2( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile2( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgVertTile3 : public WgRect
{
public:
	WgVertTile3() {};
	WgVertTile3( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile3( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgVertTile4 : public WgRect
{
public:
	WgVertTile4() {};
	WgVertTile4( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile4( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgVertTile5 : public WgRect
{
public:
	WgVertTile5() {};
	WgVertTile5( Sint32 _x, Sint32 _y, Sint32 _w, Sint32 _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile5( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};


//____ WgBlock ________________________________________________________________

class WgBlock
{
public:
	WgBlock(	const WgSurface * pSurf, const WgRect& rect, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags );
	WgBlock() : m_pSurf(0), m_flags(0) { }
	
	inline const WgRect&		GetRect() const { return m_rect; }
	inline const WgSurface *	GetSurface() const { return m_pSurf; }
	inline const WgBorders&		GetBorders() const { return m_gfxBorders; }
	inline const WgBorders&		GetContentBorders() const { return m_contentBorders; }
	inline Uint32				GetFlags() const { return m_flags; }
	inline Sint32				GetWidth() const { return m_rect.w; }
	inline Sint32				GetHeight() const { return m_rect.h; }
	inline WgSize				GetSize() const { return WgSize(m_rect.w, m_rect.h); }

	inline Sint32				GetMinWidth() const { return m_gfxBorders.GetWidth(); }
	inline Sint32				GetMinHeight() const { return m_gfxBorders.GetHeight(); }
	inline WgSize				GetMinSize() const { return WgSize(m_gfxBorders.GetWidth(), m_gfxBorders.GetHeight()); }

	inline bool					IsOpaque() const { return ((m_flags & WG_OPAQUE) != 0); }
	inline bool					HasOpaqueCenter() const { return ((m_flags & WG_OPAQUE_CENTER) != 0); }
	inline bool					HasBorders() const { return ((m_flags & WG_HAS_BORDERS) != 0); }

	inline bool					HasTiles() const { return ((m_flags & WG_TILE_ALL) != 0); }
	inline bool					HasTiledCenter() const { return ((m_flags & WG_TILE_CENTER) != 0); }
	inline bool					HasTiledTopBorder() const { return ((m_flags & WG_TILE_TOP) != 0); }
	inline bool					HasTiledBottomBorder() const { return ((m_flags & WG_TILE_BOTTOM) != 0); }
	inline bool					HasTiledLeftBorder() const { return ((m_flags & WG_TILE_LEFT) != 0); }
	inline bool					HasTiledRightBorder() const { return ((m_flags & WG_TILE_RIGHT) != 0); }

	inline bool					IsScaled() const { return ((m_flags & WG_SCALE_CENTER) != 0); }

	inline bool					IsSkipable() const	{ return ((m_flags & WG_SKIP_NORMAL) != 0); }

	inline bool					IsValid() const { return (m_pSurf && m_rect.w != 0 && m_rect.h != 0 )?true:false; }

private:

	const WgSurface *	m_pSurf;
	WgRect				m_rect;
	WgBorders			m_gfxBorders;
	Uint32				m_flags;
	WgBorders			m_contentBorders;
};

//____ WgBlockSet _____________________________________________________________

class WgBlockSet;
class WgMemPool;

typedef	WgSmartPtrPooled<WgBlockSet>	WgBlockSetPtr;


class WgBlockSet : private WgLink, private WgRefCountedPooled
{
	friend class WgSurface;
	friend class WgChain<WgBlockSet>;
	friend class WgSmartPtrPooled<WgBlockSet>;

protected:
	WgBlockSet(	WgMemPool * pPool, const WgSurface * pSurf, const WgRect& normal, const WgRect& marked, 
				const WgRect& selected, const WgRect& disabled, const WgRect& special, 
				const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags );
	WgBlockSet(	WgMemPool * pPool, const WgSurface * pSurf, const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags );


public:
	inline WgBlock				GetBlock( WgMode mode ) const;
	inline bool					HasBlock( WgMode mode ) const;
	inline WgRect				GetRect( WgMode mode ) const;
	inline void					SetPos( WgMode mode, Uint32 x, Uint32 y );

	inline WgRect				GetSizeRect() const;
	inline WgSize				GetSize() const { return WgSize(m_width, m_height); }
	inline void					SetSize( Uint32 w, Uint32 h );

	inline Sint32				GetMinWidth() const { return m_gfxBorders.GetWidth(); }
	inline Sint32				GetMinHeight() const { return m_gfxBorders.GetHeight(); }
	inline WgSize				GetMinSize() const { return WgSize(m_gfxBorders.GetWidth(), m_gfxBorders.GetHeight()); }

	inline const WgSurface *	GetSurface() const { return m_pSurf; }
	inline const WgBorders&		GetGfxBorders() const { return m_gfxBorders; }
	inline void					SetGfxBorders(const WgBorders& borders) { m_gfxBorders = borders; }
	inline const WgBorders&		GetContentBorders() const { return m_contentBorders; }
	inline void					SetContentBorders(const WgBorders& borders) { m_contentBorders = borders; }
	inline Uint32				GetFlags() const { return m_flags; }
	inline Sint32				GetWidth() const { return m_width; }
	inline Sint32				GetHeight() const { return m_height; }

	inline bool					IsOpaque() const { return ((m_flags & WG_OPAQUE) != 0); }
	inline bool					HasOpaqueCenter() const { return ((m_flags & WG_OPAQUE_CENTER) != 0); }
	inline bool					HasBorders() const { return ((m_flags & WG_HAS_BORDERS) != 0); }

	inline bool					HasTiles() const { return ((m_flags & WG_TILE_ALL) != 0); }
	inline bool					HasTiledCenter() const { return ((m_flags & WG_TILE_CENTER) != 0); }
	inline bool					HasTiledTopBorder() const { return ((m_flags & WG_TILE_TOP) != 0); }
	inline bool					HasTiledBottomBorder() const { return ((m_flags & WG_TILE_BOTTOM) != 0); }
	inline bool					HasTiledLeftBorder() const { return ((m_flags & WG_TILE_LEFT) != 0); }
	inline bool					HasTiledRightBorder() const { return ((m_flags & WG_TILE_RIGHT) != 0); }

	bool						SetTile(Uint32 place, bool bTile);

	bool						IsScaled() const { return ((m_flags & WG_SCALE_CENTER) != 0); }
	bool						SetScale(bool bScale);

	inline bool					SameBlock( WgMode one, WgMode two ) { if( m_x[one] == m_x[two] && 
													m_y[one] == m_y[two] ) return true; return false; }
	inline bool					IsModeSkipable( WgMode m ) const;


private:
	static Uint32				VerifyFlags(Uint32 flags);

	const WgSurface *	m_pSurf;
	WgBorders			m_gfxBorders;
	WgBorders			m_contentBorders;
	Uint32				m_flags;

	Uint16				m_width;
	Uint16				m_height;

	Uint16				m_x[WG_NB_MODES];
	Uint16				m_y[WG_NB_MODES];
};


inline WgBlock WgBlockSet::GetBlock(WgMode m) const
{
	const Uint32 SKIP_MASK = WG_SKIP_NORMAL | WG_SKIP_MARKED | WG_SKIP_SELECTED | WG_SKIP_DISABLED | WG_SKIP_SPECIAL;
	Uint32 flags = m_flags & ~SKIP_MASK;
	flags |= IsModeSkipable(m) ? WG_SKIP_NORMAL : 0;	// reuse bit
	return WgBlock( m_pSurf, WgRect(m_x[m], m_y[m], m_width, m_height), m_gfxBorders, m_contentBorders, flags );
}

inline bool WgBlockSet::HasBlock(WgMode m) const
{
	if( m_x[m] != m_x[WG_MODE_NORMAL] || m_y[m] != m_y[WG_MODE_NORMAL] )
		return true;
	
	return false;
}


inline WgRect WgBlockSet::GetRect(WgMode m) const
{
	return WgRect(m_x[m], m_y[m], m_width, m_height );
}

inline void WgBlockSet::SetPos( WgMode mode, Uint32 x, Uint32 y )
{
	m_x[mode] = x;
	m_y[mode] = y;
}

inline void WgBlockSet::SetSize( Uint32 w, Uint32 h )
{
	m_width = w;
	m_height = h;
}

inline WgRect WgBlockSet::GetSizeRect() const
{
	return WgRect(0, 0, m_width, m_height );
}

inline bool WgBlockSet::IsModeSkipable(WgMode m) const
{
	// NOTE: This code is depending on WG_SKIP_* and WgMode being in sync...

	if( (WG_SKIP_NORMAL << m ) & m_flags )
		return true;

	return false;
}



#endif //WG_BLOCKSET_DOT_H
