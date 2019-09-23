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

#include <memory.h>
#include <wg_surface.h>
#include <wg_versionbridge.h>

using namespace std;

static const char    c_surfaceType[] = {"Undefined"};

//____ WgSurface() ____________________________________________________________

WgSurface::WgSurface()
{
	m_pRealSurface = nullptr;

	m_pMetaData		= 0;
	m_nMetaBytes	= 0;
    m_scaleFactor   = WG_SCALE_BASE;
    m_iResource      = -1;                           // handy for debugging
}

WgSurface::WgSurface( wg::Surface * pRealSurface )
{
    m_pRealSurface = pRealSurface;
    
    m_pMetaData        = 0;
    m_nMetaBytes    = 0;
    m_scaleFactor   = WG_SCALE_BASE;
    m_iResource      = -1;                           // handy for debugging
}


//____ ~WgSurface() ____________________________________________________________

WgSurface::~WgSurface()
{
	if( m_pMetaData )
		delete [] (uint8_t*)m_pMetaData;
}

//____ PixelSize() ______________________________________________________________

WgSize WgSurface::PixelSize() const
{
	return m_pRealSurface->size();
}

//____ PointSize() ______________________________________________________________

WgSize WgSurface::PointSize() const
{
	wg::Size sz = m_pRealSurface->size();

    return sz * WG_SCALE_BASE / m_scaleFactor;
}


//____ setScaleMode() _________________________________________________________

void WgSurface::setScaleMode(WgScaleMode mode)
{
	wg::ScaleMode m = mode == WgScaleMode::Interpolate ? wg::ScaleMode::Interpolate : wg::ScaleMode::Nearest;
	m_pRealSurface->setScaleMode(m);
}

//____ scaleMode() ____________________________________________________________

WgScaleMode WgSurface::scaleMode() const 
{ 
	wg::ScaleMode m = m_pRealSurface->scaleMode();
	return m == wg::ScaleMode::Interpolate ? WgScaleMode::Interpolate : WgScaleMode::Nearest;
}

//____ GetPixel() _____________________________________________________________

Uint32 WgSurface::GetPixel(WgCoord coord) const 
{ 
	return m_pRealSurface->pixel(coord);
}

//____ GetOpacity() ___________________________________________________________

Uint8 WgSurface::GetOpacity(WgCoord coord) const 
{ 
	return m_pRealSurface->alpha(coord);
};

//____ Lock() _________________________________________________________________

void * WgSurface::Lock(WgAccessMode mode) 
{ 
	return m_pRealSurface->lock(mode); 
}

//____ LockRegion() ___________________________________________________________

void * WgSurface::LockRegion(WgAccessMode mode, const WgRect& region) 
{ 
	return m_pRealSurface->lockRegion(mode, region);
}



//____ GetLockStatus() ________________________________________________________

WgAccessMode WgSurface::GetLockStatus() const 
{ 
    return m_pRealSurface->lockStatus();
}

//____ GetLockRegion() ________________________________________________________

WgRect WgSurface::GetLockRegion() const 
{ 
	return m_pRealSurface->regionLocked();
}


//____ Col2Pixel() ____________________________________________________________

Uint32 WgSurface::Col2Pixel( const WgColor& col ) const
{
	return m_pRealSurface->colorToPixel(col);
}

//____ Pixel2Col() ____________________________________________________________

WgColor WgSurface::Pixel2Col( Uint32 pixel ) const
{
	return m_pRealSurface->pixelToColor(pixel);
}

//____ PixelFormat() __________________________________________________________

const WgPixelFormat *  WgSurface::PixelFormat()
{
	m_format = * m_pRealSurface->pixelDescription();
	return &m_format;
}

//____ Fill() _________________________________________________________________

bool WgSurface::Fill( WgColor col )
{
	return m_pRealSurface->fill(col);
}

bool WgSurface::Fill( WgColor col, const WgRect& _rect )
{
	return m_pRealSurface->fill(col, _rect);
}

//_____ SetMetaData() _________________________________________________________

void WgSurface::SetMetaData( void * pData, int bytes )
{
	if( m_pMetaData )
		delete [] (uint8_t*)m_pMetaData;

	if( pData == 0 )
	{
		m_pMetaData = 0;
		m_nMetaBytes = 0;
	}
	else
	{
		m_pMetaData = new uint8_t[bytes];
		memcpy(m_pMetaData, pData, bytes);
		m_nMetaBytes = bytes;
	}
}

//_____ ClearMetaData() _________________________________________________________

void WgSurface::ClearMetaData()
{
	if( m_pMetaData )
		delete [] (uint8_t*)m_pMetaData;

	m_pMetaData = 0;
	m_nMetaBytes = 0;
}


//_____ CopyFrom() _____________________________________________________________

bool WgSurface::CopyFrom( WgSurface * pSrcSurface, WgCoord dst )
{
	return m_pRealSurface->copyFrom(pSrcSurface->m_pRealSurface, dst);
}

bool WgSurface::CopyFrom( WgSurface * pSrcSurface, const WgRect& _srcRect, WgCoord _dst )
{
	return m_pRealSurface->copyFrom(pSrcSurface->m_pRealSurface, _srcRect, _dst);
}


//____ PutPixels() _____________________________________________________________

#define PCLIP(x,y) (((x)>(y))?(y):(x))

void WgSurface::PutPixels(const vector<int> &x, const vector<int> &y, const vector<Uint32> &col, int length, bool replace)
{
	WgColor color1;
	WgColor color2;
	int ind;
	
	int pitch = m_pRealSurface->pitch();
	uint8_t * pPixels = m_pRealSurface->pixels();

	switch(m_pRealSurface->pixelDescription()->format)
	{
	case wg::PixelFormat::BGR_8:
			break;
	case wg::PixelFormat::BGRA_8:
			for(int n=0; n<length; n++)
			{
				ind = y[n]*pitch + x[n]*4;
				if(!replace) {
					// Get old (1) and new (2) pixel
					color1.argb = *((Uint32*)&pPixels[ind]);
					color2.argb = col[n];
					// Blend
					color1.r = (Uint8)(PCLIP((int)color1.r + (int)color2.r,0xFF));
					color1.g = (Uint8)(PCLIP((int)color1.g + (int)color2.g,0xFF));
					color1.b = (Uint8)(PCLIP((int)color1.b + (int)color2.b,0xFF));
					color1.a = (Uint8)(PCLIP((int)color1.a + (int)color2.a,0xFF));
					// Store
					*((Uint32*)&pPixels[ind]) = color1.argb;
				} else {
					// Overwrite old pixel with new pixel
					*((Uint32*)&pPixels[ind]) = col[n];
				}
			}
			break;
		default:
			break;
	}
}

//____ Type() __________________________________________________________________

const char * WgSurface::Type() const
{
    return c_surfaceType;
}

