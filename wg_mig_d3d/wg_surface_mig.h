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

#ifndef	WG_SURFACE_MIG_DOT_H
#define	WG_SURFACE_MIG_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#include "EObjects.h"

//____ WgSurfaceMIG ___________________________________________________________

class WgSurfaceMIG : public WgSurface
{
public:
	WgSurfaceMIG(ETextureDataPtr pTexture);
	~WgSurfaceMIG();

	inline	ETextureDataPtr	MIG_Surf() const { return m_pTexture; };

	inline	int		Pitch() const;				// Bytes per scanline, need to lock first.
	inline	void*	Lock( WgAccessMode mode );
	inline	void*	LockRegion( WgAccessMode mode, const WgRect& region );
	inline	void	Unlock();


	// Methods needed by WgSurface

	const char *Type() const;
	static const char * GetMyType();

	WgSize		Size() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( WgCoord coord ) const;
	Uint8		GetOpacity( WgCoord coord ) const;




private:
	void		_setPixelFormat( ETextureDataPtr pTexture );

	static const int c_pixelBytes = 4;

	ETextureDataPtr m_pTexture;

};


//____ WgSurfaceFactoryMIG ____________________________________________________

class WgSurfaceFactoryMIG : public WgSurfaceFactory
{
public:
	WgSurface * CreateSurface( const WgSize& size, WgPixelType type = WG_PIXEL_RGBA_8 ) const;
};


//____ Lock() _________________________________________________________________

inline void * WgSurfaceMIG::Lock( WgAccessMode mode )
{
	bool bModify = true;

	if( mode == WG_READ_ONLY )
		bModify = false;

	if(m_pTexture)
		m_pPixels = m_pTexture->Lock( (int*) &m_pitch, 0, bModify );

	if( m_pPixels )
	{
		m_accessMode = mode;
		m_lockRegion = WgRect( 0,0,m_pTexture->GetWidth(),m_pTexture->GetHeight() );
	}
	return m_pPixels;
}

//____ LockRegion() ___________________________________________________________

inline void * WgSurfaceMIG::LockRegion( WgAccessMode mode, const WgRect& region )
{
	if( !m_pTexture )
		return 0;

	int width = m_pTexture->GetWidth();
	int height = m_pTexture->GetHeight();

	if( region.x < 0 || region.y < 0 || region.x + region.w > width || region.y + region.h > height )
		return 0;

	Lock( mode );

	if( m_pPixels )
	{
		m_lockRegion = region;
		return m_pPixels + (width*region.y+region.x)*c_pixelBytes;
	}
	return 0;
}



//____ Unlock() _______________________________________________________________

inline void WgSurfaceMIG::Unlock()
{
	if(m_pTexture)
		m_pTexture->Unlock();
	m_pPixels = 0;
	m_pitch = 0;
	m_accessMode = WG_NO_ACCESS;
	m_lockRegion.w = 0;
	m_lockRegion.h = 0;
}


//____ GetPitch() _____________________________________________________________

inline int WgSurfaceMIG::Pitch() const
{
	return m_pitch;
}


#endif //WG_SURFACE_MIG_DOT_H

