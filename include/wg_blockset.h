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

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

class WgColor;
class WgRect;
class WgBlock;
class WgSurface;

enum WgBlockFlags
{
	WG_OPAQUE_CENTER	= 0x001,
	WG_OPAQUE			= 0x003,

	WG_TILE_CENTER		= 0x004,
	WG_SCALE_CENTER		= 0x008,
	WG_FIXED_CENTER		= 0x010,

	WG_TILE_LEFT		= 0x020,
	WG_TILE_RIGHT		= 0x040,
	WG_TILE_TOP			= 0x080,
	WG_TILE_BOTTOM		= 0x100,



	WG_TILE_BORDERS		= WG_TILE_LEFT | WG_TILE_RIGHT | WG_TILE_TOP | WG_TILE_BOTTOM,
	WG_TILE_ALL			= WG_TILE_BORDERS | WG_TILE_CENTER,

	// On a skip-request the block is assumed to be totally transparent and
	// will not be rendered at all (even if it actually contains graphics).

	WG_SKIP_NORMAL		= 0x200,
	WG_SKIP_MARKED		= 0x400,
	WG_SKIP_SELECTED	= 0x800,
	WG_SKIP_DISABLED	= 0x1000,
	WG_SKIP_SPECIAL		= 0x2000,

	// Flags that can not be set by user

	WG_HAS_BORDERS		= 0x4000,
};


//____ WgHorrTileX ____________________________________________________________

class	WgHorrTile2 : public WgRect
{
public:
	WgHorrTile2() {};
	WgHorrTile2( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile2( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgHorrTile3 : public WgRect
{
public:
	WgHorrTile3() {};
	WgHorrTile3( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile3( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgHorrTile4 : public WgRect
{
public:
	WgHorrTile4() {};
	WgHorrTile4( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile4( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgHorrTile5 : public WgRect
{
public:
	WgHorrTile5() {};
	WgHorrTile5( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgHorrTile5( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

//____ WgVertTileX ____________________________________________________________

class	WgVertTile2 : public WgRect
{
public:
	WgVertTile2() {};
	WgVertTile2( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile2( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgVertTile3 : public WgRect
{
public:
	WgVertTile3() {};
	WgVertTile3( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile3( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgVertTile4 : public WgRect
{
public:
	WgVertTile4() {};
	WgVertTile4( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile4( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};

class	WgVertTile5 : public WgRect
{
public:
	WgVertTile5() {};
	WgVertTile5( int _x, int _y, int _w, int _h, Uint32 _skip = 0 ) { x = _x; y = _y; w = _w; h = _h; skip = _skip; }
	WgVertTile5( const WgRect& _r, Uint32 _skip = 0 ) { x = _r.x; y = _r.y; w = _r.w; h = _r.h; skip = _skip; }

	Uint32	skip;
};


//____ WgBlock ________________________________________________________________

class WgBlock
{
public:
	WgBlock( const WgSurface * pSurf, const WgRect& rect, const WgBorders& gfxBorders, const WgBorders& contentBorders, WgCoord8 contentDisplacement, Uint32 flags );
	WgBlock() : m_pSurf(0), m_flags(0) { }

	inline const WgRect&		Rect() const { return m_rect; }
	inline const WgSurface *	Surface() const { return m_pSurf; }
	inline const WgBorders&		GfxBorders() const { return m_gfxBorders; }
	inline const WgBorders&		ContentBorders() const { return m_contentBorders; }
	inline Uint32				Flags() const { return m_flags; }
	inline int					Width() const { return m_rect.w; }
	inline int					Height() const { return m_rect.h; }
	inline WgSize				Size() const { return WgSize(m_rect.w, m_rect.h); }

	inline int					MinWidth() const { return m_gfxBorders.Width(); }
	inline int					MinHeight() const { return m_gfxBorders.Height(); }
	inline WgSize				MinSize() const { return m_gfxBorders.Size(); }

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
	inline bool					IsFixedSize() const { return ((m_flags & WG_FIXED_CENTER) != 0); }

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

class WgColorSet;
class WgBlockSet;
class WgMemPool;

typedef	WgSmartPtr<WgColorSet>			WgColorSetPtr;
typedef	WgSmartPtrPooled<WgBlockSet>	WgBlockSetPtr;


class WgBlockSet : private WgLink, public WgRefCountedPooled
{
	friend class WgSurface;

protected:
	WgBlockSet(	WgMemPool * pPool, const WgSurface * pSurf, const WgRect& normal, const WgRect& marked, 
				const WgRect& selected, const WgRect& disabled, const WgRect& special, 
				const WgBorders& gfxBorders, const WgBorders& contentBorders, const WgColorSetPtr& pTextColors, Uint32 flags );
	WgBlockSet(	WgMemPool * pPool, const WgSurface * pSurf, const WgBorders& gfxBorders, const WgBorders& contentBorders, const WgColorSetPtr& pTextColors, Uint32 flags );

	struct Alt_Data
	{
		const WgSurface *	pSurf;
		WgBorders			gfxBorders;
		WgBorders			contentBorders;

		WgCoord8			contentDisplacement[WG_NB_MODES];		// Displacement for WG_MODE_NORMAL is ignored.
		Uint16				x[WG_NB_MODES];
		Uint16				y[WG_NB_MODES];
		Uint16				w;
		Uint16				h;
	};

	class LinkedAlt : public WgLink
	{
	public:
		LINK_METHODS( LinkedAlt );

		WgSize		activationSize;	// If (dest_width <= size.w) or (dest_height <= size.h) this or a smaller Alt will be used. Setting either size.w or size.h
									// to 0 ignores that dimension in the comparison. Setting both to 0 makes it accessible only by other ways.
		Alt_Data	data;
	};

public:
	bool				AddAlternative( WgSize activationSize, const WgSurface * pSurf, const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgRect& disabled, const WgRect& special,
										const WgBorders& gfxBorders, const WgBorders& contentBorders );

	inline WgBlock		GetBlock( WgMode mode, WgSize destSize ) const { return _getBlock( mode, _getAlt(destSize) ); }
	inline WgBlock		GetBlock( WgMode mode, int alt = 0 ) const { return _getBlock( mode, _getAlt(alt) ); }
	inline bool			HasBlock( WgMode mode, int alt = 0 ) const;

	int					NbAlternatives() const;
	WgSize				ActivationSize( int alt ) const;

	bool				SetSize( WgSize size, int alt = 0 );
	bool				SetPos( WgMode mode, WgCoord pos, int alt = 0 );

	inline WgColorSetPtr	TextColors() const { return m_pTextColors; }
	WgColor				TextColor( WgMode mode ) const;
	WgRect				Rect( WgMode mode, int alt = 0 ) const;
	WgSize				Size( int alt = 0 ) const;
	int					Width( int alt = 0 ) const;
	int					Height( int alt = 0 ) const;

	int					MinWidth( int alt = 0 ) const;
	int					MinHeight( int alt = 0 ) const;
	WgSize				MinSize( int alt = 0 ) const;

	const WgSurface *	Surface( int alt = 0 ) const;
	WgBorders			GfxBorders( int alt = 0 ) const;
	void				SetGfxBorders( const WgBorders& borders, int alt = 0 );
	WgBorders			ContentBorders( int alt = 0 ) const;
	void				SetContentBorders( const WgBorders& borders, int alt = 0 );
	WgCoord				ContentDisplacement( WgMode mode ) const;
	bool				SetContentDisplacement( WgMode mode, WgCoord ofs );
	inline Uint32		Flags() const { return m_flags; }

	inline bool			IsOpaque() const { return ((m_flags & WG_OPAQUE) != 0); }
	inline bool			HasOpaqueCenter() const { return ((m_flags & WG_OPAQUE_CENTER) != 0); }
	inline bool			HasBorders() const { return ((m_flags & WG_HAS_BORDERS) != 0); }

	inline bool			HasTiles() const { return ((m_flags & WG_TILE_ALL) != 0); }
	inline bool			HasTiledCenter() const { return ((m_flags & WG_TILE_CENTER) != 0); }
	inline bool			HasTiledTopBorder() const { return ((m_flags & WG_TILE_TOP) != 0); }
	inline bool			HasTiledBottomBorder() const { return ((m_flags & WG_TILE_BOTTOM) != 0); }
	inline bool			HasTiledLeftBorder() const { return ((m_flags & WG_TILE_LEFT) != 0); }
	inline bool			HasTiledRightBorder() const { return ((m_flags & WG_TILE_RIGHT) != 0); }

	bool				SetTile(Uint32 place, bool bTile);

	bool				IsScaled() const { return ((m_flags & WG_SCALE_CENTER) != 0); }
	bool				SetScale(bool bScale);

	bool				IsFixedSize() const { return ((m_flags & WG_FIXED_CENTER) != 0); }
	bool				SetFixedSize(bool bFixedSize);						//TODO: Make center-mode enum of fixed-size/tile/stretch/scale.

	bool				SameBlock( WgMode one, WgMode two, int alt = 0 );
	inline bool			IsModeSkipable( WgMode m ) const;


private:
	static Uint32	_verifyFlags(Uint32 flags);

	Alt_Data *		_getAlt( int n );
	Alt_Data *		_getAlt( WgSize destSize );
	const Alt_Data*	_getAlt( int n ) const;
	const Alt_Data*	_getAlt( WgSize destSize ) const;

	inline WgBlock	_getBlock( WgMode mode, const Alt_Data * pAlt ) const;

	WgColorSetPtr				m_pTextColors;		// Default colors for text placed on this block.
	Uint32						m_flags;
	Alt_Data					m_base;				// Original blocks (Alt=0)

	WgChain<LinkedAlt>			m_altChain;			// Blocks for Alts 1+.

};


inline WgBlock WgBlockSet::_getBlock(WgMode m, const Alt_Data * p) const
{
	if( !p )
		return WgBlock();

	const Uint32 SKIP_MASK = WG_SKIP_NORMAL | WG_SKIP_MARKED | WG_SKIP_SELECTED | WG_SKIP_DISABLED | WG_SKIP_SPECIAL;
	Uint32 flags = m_flags & ~SKIP_MASK;
	flags |= IsModeSkipable(m) ? WG_SKIP_NORMAL : 0;	// reuse bit
	return WgBlock( p->pSurf, WgRect(p->x[m], p->y[m], p->w, p->h), p->gfxBorders, p->contentBorders, p->contentDisplacement[m], flags );
}

inline bool WgBlockSet::HasBlock(WgMode m, int alt) const
{
	const Alt_Data * p = _getAlt(alt);
	if( !p )
		return false;

	if( p->x[m] != p->x[WG_MODE_NORMAL] || p->y[m] != p->y[WG_MODE_NORMAL] )
		return true;

	return false;
}

inline bool WgBlockSet::IsModeSkipable(WgMode m) const
{
	// NOTE: This code is depending on WG_SKIP_* and WgMode being in sync...

	if( (WG_SKIP_NORMAL << m ) & m_flags )
		return true;

	return false;
}



#endif //WG_BLOCKSET_DOT_H
