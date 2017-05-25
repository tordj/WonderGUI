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
	
	const char Finalizer::CLASSNAME[] = {"Finalizer"};
	
	
	//____ Constructor ____________________________________________________________
	
	Finalizer::Finalizer( std::function<void(Object*)> func )
	{
		m_function = func;
	}
	
	
	//____ Destructor _____________________________________________________________
	
	Finalizer::~Finalizer()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Finalizer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Finalizer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Finalizer_p Finalizer::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Finalizer_p( static_cast<Finalizer*>(pObject) );
	
		return 0;
	}

	//____ attach() ______________________________________________________________

	bool Finalizer::attach(Object* pObject)
	{
		Finalizer * pFinalizer = WeakPtrHub::getFinalizer(pObject);
		if (pFinalizer == nullptr)
		{
			WeakPtrHub::setFinalizer(pObject, this);
			return true;
		}
		return pFinalizer == this;
	}

	//____ detach() ______________________________________________________________
 
	bool Finalizer::detach(Object* pObject)
	{
		Finalizer * pFinalizer = WeakPtrHub::getFinalizer(pObject);
		if (pFinalizer == this)
		{
			WeakPtrHub::setFinalizer(pObject, nullptr);
			return true;
		}
		return false;
	}


	//____ _objectWillBeDestroyed() ___________________________________________

	void Finalizer::_objectWillBeDestroyed(Object * pObject)
	{
		m_function(pObject);
	}


} // namespace wg
