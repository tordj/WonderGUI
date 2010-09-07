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

#include	<wg_gfx.h>
#include	<wg_gfxdevice.h>
#include	<wg_gfxdevice_null.h>
#include	<wg_color.h>
#include	<wg_surface.h>
#include	<wg_font.h>
#include	<wg_text.h>
#include	<wg_gfxanim.h>
#include	<wg_texttool.h>
#include	<wg_types.h>


WgGfxDevice *			WgGfx::m_pNullDevice = 0;					
WgGfxDevice *			WgGfx::m_pDevice = new WgGfxDeviceNull();


//____ SetDevice() _____________________________________________________________

void WgGfx::SetDevice( WgGfxDevice * pDevice )
{
	// HACK: Save what must be the null-device if we don't have anyone yet.
	
	if( m_pNullDevice == 0 )
		m_pNullDevice = m_pDevice;

	
	if( pDevice == 0 )
	{
		m_pDevice = m_pNullDevice;
	}
	else
		m_pDevice = pDevice;
}


//____ clipBlitRow() __________________________________________________________

void WgGfx::clipBlitRow( const WgRect& clip, const WgSurface * pSrc, const WgRect& _srcrect, 
						 Uint32 tileOfs, Uint32 tileLen, Sint32 dx, Sint32 dy, Uint32 len, bool bStretch )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	// Blit left edge

	WgRect	r = _srcrect;
	r.w = tileOfs;
	clipBlit( clip, pSrc, r, dx, dy );

	len -= _srcrect.w - tileLen;			// Remove left and right edges from len.
	dx += tileOfs;

	// Blit tiling part

	r.x += tileOfs;
	r.w = tileLen;

	if( bStretch )
	{
		clipStretchBlit( clip, pSrc, r, WgRect( dx, dy, len, r.h ) );
		dx += len;
	}
	else
	{
		while( len > tileLen )
		{
			clipBlit( clip, pSrc, r, dx, dy );
			len -= tileLen;
			dx += tileLen;
		}
		if( len != 0 )
		{
			r.w = len;
			clipBlit( clip, pSrc, r, dx, dy );
			dx += len;
		}
	}

	// Blit right edge

	r.x += tileLen;
	r.w = _srcrect.x + _srcrect.w - r.x;
	clipBlit( clip, pSrc, r, dx, dy );
}


