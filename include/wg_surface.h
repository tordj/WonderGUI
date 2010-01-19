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

#ifndef WG_SURFACE_DOT_H
#define	WG_SURFACE_DOT_H
//==============================================================================

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif


#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif


#ifndef WG_DIRTYRECT_DOT_H
#	include <wg_dirtyrect.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif


class WgMemPool;

//____ Class WgSurface _________________________________________________________

class WgSurface
{
friend class WgGfx;

public:

	virtual ~WgSurface();

	// Methods for keeping track of dirty rectangles

	inline	void		ClearDirtyRects();
	inline	void		AddDirtyRect( const WgRect& rect );

	// Methods for reading dimensions and abilities.

	virtual	WgRect		Dimensions() const;
	virtual	Uint32		GetWidth() const = 0;
	virtual	Uint32		GetHeight() const = 0;
	virtual bool		IsOpaque() const = 0;

	// Slow, simple methods for reading and parsing individual pixels.

	virtual Uint32		GetPixel( Uint32 x, Uint32 y ) const = 0;
	virtual Uint8		GetOpacity( Uint32 x, Uint32 y ) const = 0;
	virtual	Uint32		Col2Pixel( const WgColor& col ) const = 0;
	virtual	WgColor		Pixel2Col( Uint32 pixel ) const = 0;

	// Enums and methods for locking/unlocking of surface.

	enum LockStatus
	{
		UNLOCKED,
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE
	};


	virtual void *		Lock( LockStatus mode ) = 0;
	virtual void		Unlock() = 0;
	inline 	bool		IsLocked() { return (m_lockStatus != UNLOCKED); }
	inline	LockStatus	GetLockStatus() { return m_lockStatus; }

    // Deprecated legacy methods

	inline Uint32		width() const { return GetWidth(); }
	inline Uint32		height() const { return GetHeight(); }
	inline bool			hasAlpha() const { return !IsOpaque(); }
	inline Uint32		pixel( Uint32 x, Uint32 y ) const { return GetPixel(x,y); }
	inline Uint8		opacity( Uint32 x, Uint32 y ) const { return GetOpacity(x,y); }
	inline Uint32		col2pixel( const WgColor& col ) const { return Col2Pixel(col); }
	inline WgColor		pixel2col( Uint32 pixel ) const { return Pixel2Col(pixel); }

	// Methods for defining BlockSets

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgRect& disabled,
										const WgRect& special, const WgBorders& gfxBorders, 
										const WgBorders& contentBorders, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgRect& disabled,
										const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& disabled,
										const WgBorders& gfxBorders, const WgBorders& contentBorders, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgHorrTile5& horrTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgHorrTile4& horrTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgHorrTile3& horrTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgHorrTile2& horrTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgVertTile5& vertTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgVertTile4& vertTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgVertTile3& vertTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgVertTile2& vertTile, const WgBorders& borders, const WgBorders& contentBorders, Uint32 flags ) const;


protected:
	WgSurface();

	WgDirtyRectObj 			m_dirtyRects;
	LockStatus				m_lockStatus;

	static WgMemPool *		g_pBlockSetMemPool;
};

//____ WgSurface::ClearDirtyRects() ____________________________________________

inline void WgSurface::ClearDirtyRects()
{
	m_dirtyRects.Clear();
}

//____ WgSurface::AddDirtyRect() _______________________________________________

inline void WgSurface::AddDirtyRect( const WgRect& rect )
{
	m_dirtyRects.Add( rect );
}



//==============================================================================
#endif // WG_SURFACE_DOT_H
