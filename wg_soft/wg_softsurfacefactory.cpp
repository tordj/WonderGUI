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

#include <wg_softsurfacefactory.h>
#include <wg_util.h>



const char WgSoftSurfaceFactory::CLASSNAME[] = {"SoftSurfaceFactory"};

//____ IsInstanceOf() _________________________________________________________

bool WgSoftSurfaceFactory::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSurfaceFactory::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgSoftSurfaceFactory::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgSoftSurfaceFactoryPtr WgSoftSurfaceFactory::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgSoftSurfaceFactoryPtr( static_cast<WgSoftSurfaceFactory*>(pObject.GetRealPtr()) );

	return 0;
}

//____ WgSoftSurfaceFactory::CreateSurface() ___________________________________

WgSurfacePtr WgSoftSurfaceFactory::CreateSurface( const WgSize& size, WgPixelType type ) const
{
	return new WgSoftSurface( size, type );
}
