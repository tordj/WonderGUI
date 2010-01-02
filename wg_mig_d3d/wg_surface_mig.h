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
	inline	void	Unlock();


	// Methods needed by WgSurface

	Uint32		GetWidth() const;
	Uint32		GetHeight() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( Uint32 x, Uint32 y ) const;
	Uint8		GetOpacity( Uint32 x, Uint32 y ) const;
	Uint32		Col2Pixel( const WgColor& col ) const;
	WgColor		Pixel2Col( Uint32 pixel ) const;




private:
	ETextureDataPtr m_pTexture;
	Uint32			m_pitch;
	Uint8 *			m_pLockedSurf;

};

//____ Lock() _________________________________________________________________

inline void * WgSurfaceMIG::Lock( LockStatus mode )
{
	bool bModify = true;

	if( mode == READ_ONLY )
		bModify = false;

	if(m_pTexture)
		m_pLockedSurf = m_pTexture->Lock( (int*) &m_pitch, 0, bModify );
	return m_pLockedSurf;
}

//____ Unlock() _______________________________________________________________

inline void WgSurfaceMIG::Unlock()
{
	if(m_pTexture)
		m_pTexture->Unlock();
	m_pLockedSurf = 0;
	m_pitch = 0;
}


//____ GetPitch() _____________________________________________________________

inline Uint32 WgSurfaceMIG::GetPitch() const
{
	return m_pitch;
}


#endif //WG_SURFACE_MIG_DOT_H

