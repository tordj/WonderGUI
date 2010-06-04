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

	virtual	WgRect		Dimensions() const;
	virtual	Uint32		GetWidth() const = 0;
	virtual	Uint32		GetHeight() const = 0;
	virtual bool		IsOpaque() const = 0;

	// Slow, simple methods for reading and parsing individual pixels.

	virtual Uint32		GetPixel( Uint32 x, Uint32 y ) const = 0;
	virtual Uint8		GetOpacity( Uint32 x, Uint32 y ) const = 0;
	virtual	Uint32		Col2Pixel( const WgColor& col ) const;
	virtual	WgColor		Pixel2Col( Uint32 pixel ) const;

	// Enums and methods for locking/unlocking of surface.

	enum LockStatus
	{
		UNLOCKED,
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE
	};

	enum PixelType
	{
		UNSPECIFIED,				///< Pixelformat is unkown or can't be expressed in a PixelFormat struct.
		SPECIFIED,					///< Pixelformat has no PixelType enum, but is fully specified through the PixelFormat struct.
		RGB_8,						///< One byte of red, green and blue respectively in exactly that order. 
		RGBA_8						///< One byte of red, green, blue and alpha respectively in exactly that order.
	};


	struct PixelFormat
	{
		Uint8	type;				///< Enum specifying the format if it exacty matches a predefined format, otherwise set to UNSPECIFIED.
		Uint8	bits;				///< Number of bits for the pixel, includes any non-used padding bits.

		Uint32	R_mask;				///< bitmask for getting the red bits out of the pixel
		Uint32	G_mask;				///< bitmask for getting the green bits out of the pixel
		Uint32	B_mask;				///< bitmask for getting the blue bits out of the pixel
		Uint32	A_mask;				///< bitmask for getting the alpha bits out of the pixel
		
		Sint32	R_shift;			///< amount to shift the red bits to get an 8-bit representation of red. This can be negative.
		Sint32	G_shift;			///< amount to shift the green bits to get an 8-bit representation of red. This can be negative.
		Sint32	B_shift;			///< amount to shift the blue bits to get an 8-bit representation of red. This can be negative.
		Sint32	A_shift;			///< amount to shift the alpha bits to get an 8-bit representation of red. This can be negative.

		Uint8	R_bits;				///< number of bits for red in the pixel
		Uint8	G_bits;				///< number of bits for green in the pixel
		Uint8	B_bits;				///< number of bits for blue in the pixel
		Uint8	A_bits;				///< number of bits for alpha in the pixel
	};


	virtual void *		Lock( LockStatus mode ) = 0;
	virtual void *		LockRegion( LockStatus mode, const WgRect& region ) = 0;
	virtual void		Unlock() = 0;
	inline 	bool		IsLocked() const { return (m_lockStatus != UNLOCKED); }
	inline	LockStatus	GetLockStatus() const { return m_lockStatus; }
	inline  WgRect		GetLockRegion() const { return m_lockRegion; }
	inline Uint32		GetPitch() const;						// of locked surface
	inline const PixelFormat *GetPixelFormat() const;					// of locked surface	


	// Methods for modifying surface content

	virtual bool		Fill( WgColor col );


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
	PixelFormat				m_pixelFormat;
	LockStatus				m_lockStatus;
	Uint32					m_pitch;
	Uint8 *					m_pPixels;			// Pointer at pixels when surface locked.
	WgRect					m_lockRegion;		// Region of surface that is locked. Width/Height should be set to 0 when not locked.


	static WgMemPool *		g_pBlockSetMemPool;
};

//____ WgSurfaceFactory _______________________________________________________

class WgSurfaceFactory
{
public:
	virtual WgSurface * CreateSurface( const WgSize& size, WgSurface::PixelType type = WgSurface::RGBA_8 ) = 0;
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


//____ WgSurface::GetPitch() _______________________________________________

Uint32 WgSurface::GetPitch() const
{
	if( m_lockStatus == UNLOCKED )
		return 0;

	return m_pitch;
}


//____ WgSurface::GetPixelFormat() _______________________________________________

const WgSurface::PixelFormat *  WgSurface::GetPixelFormat() const
{
	if( m_lockStatus == UNLOCKED )
		return 0;

	return &m_pixelFormat;
}



//==============================================================================
#endif // WG_SURFACE_DOT_H
