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

#include <wg_modspan.h>
#include <wg_spanitem.h>

namespace wg 
{
	
	const char WgModSpan::CLASSNAME[] = {"ModSpan"};
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgModSpan::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return WgSlideSpan::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgModSpan::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	WgModSpan_p WgModSpan::cast( const WgInterface_p& pInterface )
	{
		if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
			return WgModSpan_p( pInterface.getRealObjectPtr(), static_cast<WgModSpan*>( pInterface.rawPtr()) );
	
		return 0;
	}
	

} // namespace wg
