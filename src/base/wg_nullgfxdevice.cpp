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


//____ create() _______________________________________________________________

WgNullGfxDevice_p WgNullGfxDevice::create( WgSize size )
{
	return WgNullGfxDevice_p(new WgNullGfxDevice(size));
}

WgNullGfxDevice::WgNullGfxDevice( WgSize size ) : WgGfxDevice(size)
{
}

WgNullGfxDevice::~WgNullGfxDevice()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgNullGfxDevice::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgGfxDevice::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgNullGfxDevice::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgNullGfxDevice_p WgNullGfxDevice::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgNullGfxDevice_p( static_cast<WgNullGfxDevice*>(pObject.rawPtr()) );

	return 0;
}


void WgNullGfxDevice::fill( const WgRect& rect, const WgColor& col )
{
}

void WgNullGfxDevice::blit( const WgSurface_p& src, const WgRect& srcrect, int dx, int dy  )
{
}

void WgNullGfxDevice::tileBlit( const WgSurface_p& src, const WgRect& srcrect, const WgRect& dest )
{
}

void WgNullGfxDevice::fillSubPixel( const WgRectF& rect, const WgColor& col )
{
}

void WgNullGfxDevice::stretchBlitSubPixel( 	const WgSurface_p& pSrc, float sx, float sy, float sw, float sh,
						   		 			float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
{
}

void WgNullGfxDevice::clipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col )
{
}

void WgNullGfxDevice::clipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col )
{
}

void WgNullGfxDevice::clipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness )
{
}


void WgNullGfxDevice::drawArcNE( const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::drawElipse( const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::drawFilledElipse( const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::clipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::clipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color )
{
}

void WgNullGfxDevice::clipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color )
{
}
