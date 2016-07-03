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

#include <wg_text.h>

namespace wg 
{
	
	const char Text::CLASSNAME[] = {"Text"};
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Text::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Interface::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Text::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Text_p Text::cast( const Interface_p& pInterface )
	{
		if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
			return Text_p( pInterface.getRealObjectPtr(), static_cast<Text*>( pInterface.rawPtr()) );
	
		return 0;
	}
	
	//____ _object() _______________________________________________________________
	
	Object * Text::_object() const
	{
		return m_pItem->_object();
	}

} // namespace wg
