
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

#include <wg_component.h>
#include <wg_pointers.h>

const char WgComponent::CLASSNAME[] = {"Component"};


//____ IsInstanceOf() _________________________________________________________

bool WgComponent::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return false;
}

//____ ClassName() ____________________________________________________________

const char * WgComponent::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgComponentPtr WgComponent::Cast( const WgComponentPtr& pComponent )
{
	if( pComponent && pComponent->IsInstanceOf(CLASSNAME) )
		return WgComponentPtr( static_cast<WgComponent*>(pComponent.GetRealPtr()) );

	return 0;
}

//____ Object() ________________________________________________________________

WgObjectPtr WgComponent::Object()
{
	return WgObjectPtr(_object());
}

//____ Ptr() ___________________________________________________________________

WgComponentPtr WgComponent::Ptr()
{
	return WgComponentPtr(this);
}
