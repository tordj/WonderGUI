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

namespace wg 
{
	
	const char NullGfxDevice::CLASSNAME[] = {"NullGfxDevice"};
	
	
	//____ create() _______________________________________________________________
	
	NullGfxDevice_p NullGfxDevice::create( Size size )
	{
		return NullGfxDevice_p(new NullGfxDevice(size));
	}
	
	NullGfxDevice::NullGfxDevice( Size size ) : GfxDevice(size)
	{
	}
	
	NullGfxDevice::~NullGfxDevice()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool NullGfxDevice::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return GfxDevice::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * NullGfxDevice::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	NullGfxDevice_p NullGfxDevice::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return NullGfxDevice_p( static_cast<NullGfxDevice*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	void NullGfxDevice::fill( const Rect& rect, const Color& col )
	{
	}
	
	void NullGfxDevice::blit( const Surface_p& src, const Rect& srcrect, int dx, int dy  )
	{
	}
	
	void NullGfxDevice::tileBlit( const Surface_p& src, const Rect& srcrect, const Rect& dest )
	{
	}
	
	void NullGfxDevice::fillSubPixel( const RectF& rect, const Color& col )
	{
	}
	
	void NullGfxDevice::stretchBlitSubPixel( 	const Surface_p& pSrc, float sx, float sy, float sw, float sh,
							   		 			float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
	{
	}
	
	void NullGfxDevice::clipDrawHorrLine( const Rect& clip, const Coord& start, int length, const Color& col )
	{
	}
	
	void NullGfxDevice::clipDrawVertLine( const Rect& clip, const Coord& start, int length, const Color& col )
	{
	}
	
	void NullGfxDevice::clipPlotSoftPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color& col, float thickness )
	{
	}

    void NullGfxDevice::clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors)
	{
	}
	
	void NullGfxDevice::drawLine( Coord begin, Coord end, Color color, float thickness )
	{
	}
	
	void NullGfxDevice::drawArcNE( const Rect& rect, Color color )
	{
	}
	
	void NullGfxDevice::drawElipse( const Rect& rect, Color color )
	{
	}
	
	void NullGfxDevice::drawFilledElipse( const Rect& rect, Color color )
	{
	}
	
	void NullGfxDevice::clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness )
	{
	}
	
	void NullGfxDevice::clipDrawArcNE( const Rect& clip, const Rect& rect, Color color )
	{
	}
	
	void NullGfxDevice::clipDrawElipse( const Rect& clip, const Rect& rect, Color color )
	{
	}
	
	void NullGfxDevice::clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color )
	{
	}

} // namespace wg
