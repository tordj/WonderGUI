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

#ifndef	WG_SURFACE_SW_DOT_H
#define	WG_SURFACE_SW_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

//____ WgSurfaceSW _____________________________________________________________

class WgSurfaceSW : public WgSurface
{
public:
	WgSurfaceSW( Uint32 iWidth, Uint32 iHeight, Uint32 iBitdepth = 32 );
	~WgSurfaceSW();

	inline	Uint8 *	SW_Surf() const { return m_pData; };

	inline	Uint32	GetPitch() const;				// Bytes per scanline, need to lock first.
	inline	Uint32	GetBitdepth() const;
	inline	void*	Lock( bool bModify = false );
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
	Uint32			m_width;
	Uint32			m_height;
	Uint32			m_pitch;
	Uint32			m_bitdepth;		// Only 24 and 32 are supported.
	Uint8*			m_pData;

};

//____ Lock() __________________________________________________________________

inline void * WgSurfaceSW::Lock( bool bModify )
{
	return m_pData;
}

//____ Unlock() ________________________________________________________________

inline void WgSurfaceSW::Unlock()
{
}

//____ GetPitch() ______________________________________________________________

inline Uint32 WgSurfaceSW::GetPitch() const
{
	return m_pitch;
}

//____ GetBitdepth() ___________________________________________________________

inline Uint32 WgSurfaceSW::GetBitdepth() const
{
	return m_bitdepth;
}


#endif //WG_SURFACE_SW_DOT_H

