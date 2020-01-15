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

#ifndef WG_SURFACE_DOT_H
#	include <wg3_surface.h>
#endif

#include <wg_color.h>

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


//____ WgBlock ________________________________________________________________

class WgBlock
{
public:
    WgBlock( wg::Surface * pSurf, const WgRect& rect, const WgBorders& sourceFrame, const WgBorders& canvasFrame, const WgBorders& padding, WgCoord contentShift, int scale, Uint32 flags );
	WgBlock() : m_pSurf(0), m_flags(0) { }

	inline const WgRect&		Rect() const { return m_rect; }
    inline wg::Surface *	    Surface() const { return m_pSurf; }
	inline const WgBorders&		SourceFrame() const { return m_sourceFrame; }
	inline const WgBorders&		CanvasFrame() const { return m_canvasFrame; }
	inline const WgRect			ContentRect( const WgRect& blockGeo ) const { return (blockGeo + m_contentShift) - m_padding; }
	inline Uint32				Flags() const { return m_flags; }
	inline int					Width() const { return m_rect.w; }
	inline int					Height() const { return m_rect.h; }
	inline WgSize				Size() const { return WgSize(m_rect.w, m_rect.h); }
	inline int 					OutputScale() const { return m_scale; }
	inline int 					SurfaceScale() const { return (int) m_pSurf->scale()*4096; }

	inline int					MinWidth() const { return m_canvasFrame.width(); }
	inline int					MinHeight() const { return m_canvasFrame.height(); }
	inline WgSize				MinSize() const { return m_canvasFrame.size(); }

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

	bool 						operator==( const WgBlock& b) const;
	bool 						operator!=( const WgBlock& b) const;


private:

    wg::Surface *       m_pSurf;
	WgRect				m_rect;
	WgBorders 			m_sourceFrame;
	WgBorders			m_canvasFrame;
	WgBorders			m_padding;
	WgCoord				m_contentShift;
	Uint32				m_flags;
	int                 m_scale = WG_SCALE_BASE;
};

//____ WgBlockset _____________________________________________________________

class WgColorset;
class WgBlockset;
class WgMemPool;

typedef	WgSmartPtr<WgColorset>			WgColorsetPtr;
typedef	WgSmartPtrPooled<WgBlockset>	WgBlocksetPtr;


class WgBlockset : private WgLink, public WgRefCountedPooled
{
protected:
    WgBlockset(	WgMemPool * pPool, wg::Surface * pSurf, Uint32 flags );

	struct Alt_Data
	{
        wg::Surface_p	pSurf;
	};

	class LinkedAlt : public WgLink
	{
	public:
		LINK_METHODS( LinkedAlt );

		Alt_Data	data;
	};

public:
    static WgBlocksetPtr CreateFromSurface( wg::Surface * pSurf, int flags = 0 );
    static WgBlocksetPtr CreateFromRect( wg::Surface * pSurf, const WgRect& normal, int flags = 0 );
    static WgBlocksetPtr CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, int flags = 0 );
    static WgBlocksetPtr CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, int flags = 0 );
    static WgBlocksetPtr CreateFromRects( wg::Surface * pSurf, const WgRect& normal, const WgCoord& marked, const WgCoord& selected, const WgCoord& disabled, const WgCoord& special, int flags = 0 );
    static WgBlocksetPtr CreateFromRow( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int spacing=0, int flags = 0 );
    static WgBlocksetPtr CreateFromColumn( wg::Surface * pSurf, const WgRect& rect, int nBlocks, int spacing=0, int flags = 0 );


    bool				AddAlternative( wg::Surface * pSurf);

//	bool				SetSize( WgSize size, int alt = 0 );
//	bool				SetPos( WgMode mode, WgCoord pos, int alt = 0 );

	void				SetTextColors( const WgColorsetPtr& colors );
	void				SetFrame( const WgBorders& frame );
	void				SetPadding( const WgBorders& padding );
	bool				SetContentShift( WgMode mode, WgCoord ofs );
	bool				SetTile(Uint32 place, bool bTile);
	bool				SetScale(bool bScale);
	bool				SetFixedSize(bool bFixedSize);						//TODO: Make center-mode enum of fixed-size/tile/stretch/scale.

	inline WgBlock		GetBlock( WgMode mode, int scale ) const { return _getBlock( mode, _getAltForScale(scale), scale ); }


	inline WgColorsetPtr	TextColors() const { return m_pTextColors; }
	WgColor				TextColor( WgMode mode ) const;
	WgRect				Rect( WgMode mode, int scale ) const;
	WgSize				Size( int scale ) const;
	int					Width( int scale ) const;
	int					Height( int scale ) const;

	int					MinWidth( int scale ) const;
	int					MinHeight( int scale ) const;
	WgSize				MinSize( int scale ) const;
		
    wg::Surface_p     	Surface( int scale ) const;
	WgBorders			Frame( int scale ) const;
	WgBorders			Padding( int scale ) const;
	WgCoord				ContentShift( WgMode mode, int scale ) const;
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


	bool				IsScaled() const { return ((m_flags & WG_SCALE_CENTER) != 0); }

	bool				IsFixedSize() const { return ((m_flags & WG_FIXED_CENTER) != 0); }

	bool				SameBlock( WgMode one, WgMode two );
	inline bool			IsModeSkipable( WgMode m ) const;


private:
	static Uint32	_verifyFlags(Uint32 flags);
    static WgBlockset * _alloc( wg::Surface * pSurf, int flags );

	Alt_Data *		_getAlt( int n );
	const Alt_Data*	_getAlt( int n ) const;
	const Alt_Data*	_getAltForScale( int scale ) const;

	WgBlock	_getBlock( WgMode mode, const Alt_Data * pAlt, int scale ) const;

	WgColorsetPtr			m_pTextColors;		// Default colors for text placed on this block.
	Uint32					m_flags;
    
    WgBorders               m_frame;
    WgBorders               m_padding;
    
    WgCoord8                m_contentShift[WG_NB_MODES];		// Only shift for WG_MODE_MARKED and WG_MODE_SELECTED are used.
    
	WgCoord					m_pos[WG_NB_MODES];
	WgSize					m_size;
    
	Alt_Data				m_base;				// Original blocks (Alt=0)

	WgChain<LinkedAlt>		m_altChain;			// Blocks for Alts 1+.


	static WgMemPool *	g_pMemPool;
};




inline bool WgBlockset::IsModeSkipable(WgMode m) const
{
	// NOTE: This code is depending on WG_SKIP_* and WgMode being in sync...

	if( (WG_SKIP_NORMAL << m ) & m_flags )
		return true;

	return false;
}



#endif //WG_BLOCKSET_DOT_H
