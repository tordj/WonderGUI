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

//____ IsInstanceOf() _________________________________________________________

bool WgFinalizer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgFinalizer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgFinalizerPtr WgFinalizer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFinalizerPtr( static_cast<WgFinalizer*>(pObject.RawPtr()) );

	return 0;
}
