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


#ifndef WG_RECTCHAIN_DOT_H
#	include <wg_rectchain.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif


class WgMemPool;


//____ WgSurface ______________________________________________________________

class WgSurface
{
friend class WgGfx;

public:

	virtual ~WgSurface();

	// Methods for keeping track of dirty rectangles

	inline	void		ClearDirtyRects();
	inline	void		AddDirtyRect( const WgRect& rect );

	// Methods for reading dimensions and abilities.

	virtual const char *Type() const = 0;
	virtual	WgSize		Size() const = 0;
	virtual	int			Width() const;
	virtual	int			Height() const;
	virtual bool		IsOpaque() const = 0;

	// Slow, simple methods for reading and parsing individual pixels.


	virtual Uint32		GetPixel( WgCoord coord ) const = 0;
	inline Uint32		GetPixel( int x, int y ) const;
	virtual Uint8		GetOpacity( WgCoord coord ) const = 0;
	inline Uint8		GetOpacity( int x, int y ) const;
	virtual	Uint32		Col2Pixel( const WgColor& col ) const;
	virtual	WgColor		Pixel2Col( Uint32 pixel ) const;

	// Enums and methods for locking/unlocking of surface.

	virtual void *			Lock( WgAccessMode mode ) = 0;
	virtual void *			LockRegion( WgAccessMode mode, const WgRect& region ) = 0;
	virtual void			Unlock() = 0;
	inline 	bool			IsLocked() const { return (m_accessMode != WG_NO_ACCESS); }
	inline	WgAccessMode 	GetLockStatus() const { return m_accessMode; }
	inline  WgRect			GetLockRegion() const { return m_lockRegion; }
	inline  int				Pitch() const;									// of locked surface
	inline const WgPixelFormat *PixelFormat() const;						// of locked surface
	inline void *			Pixels() const { return m_pPixels; }			// of locked surface


	// Methods for modifying surface content

	virtual bool		Fill( WgColor col );
	virtual bool		Fill( WgColor col, const WgRect& rect );
	virtual bool		CopyFrom( WgSurface * pSrcSurf, const WgRect& srcRect, WgCoord dst );
	virtual bool		CopyFrom( WgSurface * pSrcSurf, WgCoord dst );

	// Methods for defining BlockSets

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgRect& disabled,
										const WgRect& special, const WgBorders& gfxBorders,
										const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgRect& disabled,
										const WgBorders& gfxBorders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgRect& disabled,
										const WgBorders& gfxBorders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgRect& normal, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgHorrTile5& horrTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgHorrTile4& horrTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgHorrTile3& horrTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgHorrTile2& horrTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;

	WgBlockSetPtr		defineBlockSet( const WgVertTile5& vertTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgVertTile4& vertTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgVertTile3& vertTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;
	WgBlockSetPtr		defineBlockSet( const WgVertTile2& vertTile, const WgBorders& borders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const;


protected:
	WgSurface();
	WgRect				_lockAndAdjustRegion( WgAccessMode modeNeeded, const WgRect& region );

	WgPixelFormat		m_pixelFormat;
	int					m_pitch;

	WgRectChain 		m_dirtyRects;
	WgAccessMode		m_accessMode;
	Uint8 *				m_pPixels;			// Pointer at pixels when surface locked.
	WgRect				m_lockRegion;		// Region of surface that is locked. Width/Height should be set to 0 when not locked.


	static WgMemPool *	g_pBlockSetMemPool;
};

//____ WgSurfaceFactory _______________________________________________________

class WgSurfaceFactory
{
public:
	virtual WgSurface * CreateSurface( const WgSize& size, WgPixelType type = WG_PIXEL_ARGB_8 ) const = 0;
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


//____ WgSurface::Pitch() _______________________________________________

int WgSurface::Pitch() const
{
	if( m_accessMode == WG_NO_ACCESS )
		return 0;

	return m_pitch;
}


//____ WgSurface::PixelFormat() _______________________________________________

const WgPixelFormat *  WgSurface::PixelFormat() const
{
	if( m_accessMode == WG_NO_ACCESS )
		return 0;

	return &m_pixelFormat;
}


Uint32 WgSurface::GetPixel( int x, int y ) const
{
	return GetPixel( WgCoord(x,y) );
}

Uint8 WgSurface::GetOpacity( int x, int y ) const
{
	return GetOpacity( WgCoord(x,y) );
}


//==============================================================================
#endif // WG_SURFACE_DOT_H
