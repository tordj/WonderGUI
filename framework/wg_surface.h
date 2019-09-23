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

#include <vector>

#include <wg3_surface.h>

//____ WgSurface ______________________________________________________________

class WgSurface
{
	friend class WgGfxDevice;
	friend class WgGfxDeviceSoft;
	friend class WgGlGfxDevice;
    

public:
    WgSurface( wg::Surface * pRealSurface );
    
	virtual ~WgSurface();

	// Methods for reading dimensions and abilities.

	virtual const char *Type() const;
	virtual	WgSize		PixelSize() const;
    virtual WgSize      PointSize() const;
    
    // Deprecated methods, not retina aware. Use PointSize().w, PointSize().h instead.
    // virtual	int			Width() const;
	// virtual	int			Height() const;

	inline bool			IsOpaque() const { return m_pRealSurface->isOpaque(); }

    void                SetScaleFactor( int factor ) { m_scaleFactor = factor; }
    int                 ScaleFactor() const { return m_scaleFactor; }
    
	virtual void		setScaleMode( WgScaleMode mode );
	WgScaleMode			scaleMode() const;


	// Slow, simple methods for reading and parsing individual pixels.


	virtual Uint32		GetPixel(WgCoord coord) const;
	inline Uint32		GetPixel( int x, int y ) const;
	virtual Uint8		GetOpacity( WgCoord coord ) const;
	inline Uint8		GetOpacity( int x, int y ) const;
	virtual	Uint32		Col2Pixel( const WgColor& col ) const;
	virtual	WgColor		Pixel2Col( Uint32 pixel ) const;

	// Enums and methods for locking/unlocking of surface.

	virtual void *			Lock(WgAccessMode mode);
	virtual void *			LockRegion(WgAccessMode mode, const WgRect& region);
	virtual void			Unlock() { return m_pRealSurface->unlock(); };
	inline 	bool			IsLocked() const { return (m_pRealSurface->isLocked()); }
	WgAccessMode 			GetLockStatus() const;
	WgRect					GetLockRegion() const;
	inline  int				Pitch() const;									// of locked surface
	const WgPixelFormat *	PixelFormat();						// of locked surface
	inline void *			Pixels() const { return m_pRealSurface->pixels(); }			// of locked surface


	// Methods for modifying surface content

	virtual bool		Fill( WgColor col );
	virtual bool		Fill( WgColor col, const WgRect& rect );
	virtual bool		CopyFrom( WgSurface * pSrcSurf, const WgRect& srcRect, WgCoord dst );
	virtual bool		CopyFrom( WgSurface * pSrcSurf, WgCoord dst );

	// Merthods for handling meta data
	
	void *				MetaData() const { return m_pMetaData; }
	int					MetaDataSize() const { return m_nMetaBytes; }
	void				SetMetaData( void * pData, int byts );
	void				ClearMetaData();

    wg::Surface *       RealSurface() const { return m_pRealSurface; }
    
	
	// Softube specific...
	
	void    PutPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<Uint32> &col, int length, bool replace);
    
    void    SetResourceID(int iResource) { m_iResource = iResource; }
	
protected:
	WgSurface();
    
	WgPixelFormat		m_format;
	void *				m_pMetaData;
	int					m_nMetaBytes;    
    int                 m_scaleFactor;      // Scale of surface content. Default is WG_SCALE_BASE.
    int                 m_iResource;         // handy to have when debugging
	wg::Surface_p		m_pRealSurface;
};




//____ WgSurface::Pitch() _______________________________________________

int WgSurface::Pitch() const
{
	return m_pRealSurface->pitch();
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
