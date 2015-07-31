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

namespace wg 
{
	
	
	
	const char SoftSurfaceFactory::CLASSNAME[] = {"SoftSurfaceFactory"};
	
	//____ isInstanceOf() _________________________________________________________
	
	bool SoftSurfaceFactory::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return SurfaceFactory::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * SoftSurfaceFactory::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	SoftSurfaceFactory_p SoftSurfaceFactory::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SoftSurfaceFactory_p( static_cast<SoftSurfaceFactory*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ SoftSurfaceFactory::createSurface() ___________________________________
	
	Surface_p SoftSurfaceFactory::createSurface( const Size& size, WgPixelType type ) const
	{
		return new SoftSurface( size, type );
	}

} // namespace wg
