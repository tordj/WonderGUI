

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

#include <wg_edittextfield.h>

const char WgEditTextField::CLASSNAME[] = {"EditTextField"};

//____ IsInstanceOf() _________________________________________________________

bool WgEditTextField::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgModifTextField::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgEditTextField::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgEditTextFieldPtr WgEditTextField::Cast( const WgComponentPtr& pComponent )
{
	if( pComponent && pComponent->IsInstanceOf(CLASSNAME) )
		return WgEditTextFieldPtr( static_cast<WgEditTextField*>(pComponent.GetRealPtr()) );

	return 0;
}
