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

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif


class WgSurface;
typedef	WgSmartPtr<WgSurface,WgObjectPtr>		WgSurfacePtr;
typedef	WgWeakPtr<WgSurface,WgObjectWeakPtr>	WgSurfaceWeakPtr;

//____ WgSurface ______________________________________________________________

class WgSurface : public WgObject
{

public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgSurfacePtr	Cast( const WgObjectPtr& pObject );

	// Methods for reading dimensions and abilities.

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
	virtual bool		CopyFrom( const WgSurfacePtr& pSrcSurf, const WgRect& srcRect, WgCoord dst );
	virtual bool		CopyFrom( const WgSurfacePtr& pSrcSurf, WgCoord dst );

protected:
	WgSurface();
	virtual ~WgSurface();

	WgRect				_lockAndAdjustRegion( WgAccessMode modeNeeded, const WgRect& region );

	WgPixelFormat		m_pixelFormat;
	int					m_pitch;

	WgAccessMode		m_accessMode;
	Uint8 *				m_pPixels;			// Pointer at pixels when surface locked.
	WgRect				m_lockRegion;		// Region of surface that is locked. Width/Height should be set to 0 when not locked.
};

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
