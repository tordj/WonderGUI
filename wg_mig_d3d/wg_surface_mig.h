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

	inline	Uint32	GetPitch() const;				// Bytes per scanline, need to lock first.
	inline	void*	Lock( LockStatus mode );
	inline	void*	LockRegion( LockStatus mode, const WgRect& region );
	inline	void	Unlock();


	// Methods needed by WgSurface

	Uint32		GetWidth() const;
	Uint32		GetHeight() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( Uint32 x, Uint32 y ) const;
	Uint8		GetOpacity( Uint32 x, Uint32 y ) const;




private:
	void		SetPixelFormat( ETextureDataPtr pTexture );

	static const int c_pixelBytes = 4;

	ETextureDataPtr m_pTexture;

};


//____ WgSurfaceFactoryMIG ____________________________________________________

class WgSurfaceFactoryMIG : public WgSurfaceFactory
{
public:
	WgSurface * CreateSurface( const WgSize& size, WgSurface::PixelType type = WgSurface::RGBA_8 );
};


//____ Lock() _________________________________________________________________

inline void * WgSurfaceMIG::Lock( LockStatus mode )
{
	bool bModify = true;

	if( mode == READ_ONLY )
		bModify = false;

	if(m_pTexture)
		m_pPixels = m_pTexture->Lock( (int*) &m_pitch, 0, bModify );

	if( m_pPixels )
	{
		m_lockStatus = mode;
		m_lockRegion = WgRect( 0,0,m_pTexture->GetWidth(),m_pTexture->GetHeight() );
	}
	return m_pPixels;
}

//____ LockRegion() ___________________________________________________________

inline void * WgSurfaceMIG::LockRegion( LockStatus mode, const WgRect& region )
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
	m_lockStatus = UNLOCKED;
	m_lockRegion.w = 0;
	m_lockRegion.h = 0;
}


//____ GetPitch() _____________________________________________________________

inline Uint32 WgSurfaceMIG::GetPitch() const
{
	return m_pitch;
}


#endif //WG_SURFACE_MIG_DOT_H

