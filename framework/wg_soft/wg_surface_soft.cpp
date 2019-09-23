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

#include <assert.h>
#include <memory.h>
#include <wg_surface_soft.h>
#include <wg_util.h>

#include <wg3_softsurface.h>
#include <wg_versionbridge.h>


static const char	c_surfaceType[] = {"Software"};

//____ MaxSize() _______________________________________________________________

WgSize WgSurfaceSoft::MaxSize()
{
	return wg::SoftSurface::maxSize();
}


//____ Constructor ________________________________________________________________

WgSurfaceSoft::WgSurfaceSoft( WgSize size, WgPixelType type, int flags )
{
	m_pRealSurface = wg::SoftSurface::create(size, type, flags);
}

WgSurfaceSoft::WgSurfaceSoft( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, int flags )
{
	auto pBlob = wg::Blob::create(pPixels, nullptr);
	m_pRealSurface = wg::SoftSurface::create(size, type, pBlob, pitch, flags);
}

WgSurfaceSoft::WgSurfaceSoft( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, const WgPixelFormat& pixelFormat, int flags )
{
	m_pRealSurface = wg::SoftSurface::create(size, type, pPixels, pitch, &pixelFormat, flags);
}


WgSurfaceSoft::WgSurfaceSoft( const WgSurfaceSoft * pOther, int flags )
{
	m_pRealSurface = wg::SoftSurface::create(pOther->m_pRealSurface, flags);
}

//____ Destructor ______________________________________________________________

WgSurfaceSoft::~WgSurfaceSoft()
{
}

//____ Type() __________________________________________________________________

const char * WgSurfaceSoft::Type() const
{
	return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgSurfaceSoft::GetClass()
{
	return c_surfaceType;
}
