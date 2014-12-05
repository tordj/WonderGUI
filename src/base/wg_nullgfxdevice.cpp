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

#include <wg_nullgfxdevice.h>

const char WgNullGfxDevice::CLASSNAME[] = {"NullGfxDevice"};


//____ Create() _______________________________________________________________

WgNullGfxDevicePtr WgNullGfxDevice::Create( WgSize size )
{
	return WgNullGfxDevicePtr(new WgNullGfxDevice(size));
}

WgNullGfxDevice::WgNullGfxDevice( WgSize size ) : WgGfxDevice(size)
{
}

WgNullGfxDevice::~WgNullGfxDevice()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgNullGfxDevice::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgGfxDevice::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgNullGfxDevice::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgNullGfxDevicePtr WgNullGfxDevice::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgNullGfxDevicePtr( static_cast<WgNullGfxDevice*>(pObject.RawPtr()) );

	return 0;
}


void WgNullGfxDevice::Fill( const WgRect& rect, const WgColor& col )
{
}

void WgNullGfxDevice::Blit( const WgSurfacePtr& src, const WgRect& srcrect, int dx, int dy  )
{
}

void WgNullGfxDevice::TileBlit( const WgSurfacePtr& src, const WgRect& srcrect, const WgRect& dest )
{
}

void WgNullGfxDevice::FillSubPixel( const WgRectF& rect, const WgColor& col )
{
}

void WgNullGfxDevice::StretchBlitSubPixel( 	const WgSurfacePtr& pSrc, float sx, float sy, float sw, float sh,
						   		 			float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
{
}

void WgNullGfxDevice::ClipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col )
{
}

void WgNullGfxDevice::ClipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col )
{
}

void WgNullGfxDevice::ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness )
{
}


void WgNullGfxDevice::DrawArcNE( const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::DrawElipse( const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::DrawFilledElipse( const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color )
{
}
