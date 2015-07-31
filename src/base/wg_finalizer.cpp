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
#include <wg_finalizer.h>

namespace wg 
{
	
	const char WgFinalizer::CLASSNAME[] = {"Finalizer"};
	
	
	//____ Constructor ____________________________________________________________
	
	WgFinalizer::WgFinalizer( void(*pCallback)(void*), void * pObject )
	{
		m_pCallback = pCallback;
		m_pObject = pObject;
	}
	
	
	//____ Destructor _____________________________________________________________
	
	WgFinalizer::~WgFinalizer()
	{
		if( m_pCallback )
			m_pCallback( m_pObject );
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgFinalizer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgObject::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgFinalizer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgFinalizer_p WgFinalizer::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgFinalizer_p( static_cast<WgFinalizer*>(pObject.rawPtr()) );
	
		return 0;
	}

} // namespace wg
