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

#include <wg_object.h>
#include <wg_mempool.h>
#include <wg_pointers.h>

namespace wg 
{
	
	const char Object::CLASSNAME[] = {"Object"};
	
	
	/**
	 * @brief	Check if the object is an instance or subclass of specified class.
	 *
	 * Check if the object is an instance or subclass of specified class.
	 *
	 * @param pClassName	Pointer to the class name. This needs to be a pointer returned by 
	 * 						a call to Object::className() or the CLASSNAME member of a subclass of Object.
	 * 
	 * This method compares the specified class name to the CLASSNAME member of all classes implemented by the
	 * object. This is needed when checking if the object implements a class other than the leaf class.  
	 * When just checking for a leaf class, a direct comparison between object->className() and Foo::CLASSNAME
	 * is faster.
	 * 
	 * @return True if the object implements the specied class.
	 * 
	 */
	
	bool Object::isInstanceOf( const char * pClassName ) const
	{ 
		return (pClassName==CLASSNAME); 
	}
	
	/**
	 * @brief Get a char pointer which identifies the class of the object.
	 *
	 * Get a char pointer which identifies the class of the object.
	 *
	 * The char pointer points to a char string containing the class name without the Wg prefix.
	 * The pointer itself can be used for object type comparison since it is always the same for 
	 * objects of the same type. There is no need to do a string compare on the string content.
	 * 
	 * To see if an object is an instance of class Foo you can compare the pointer to Foo::CLASSNAME.
	 * To see if an object is an instance of a subclass of Foo, you will need to call object->isInstanceOf( Foo::CLASSNAME ).
	 * 
	 * @return Pointer to a char string containing the class name.
	 */
	
	const char * Object::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	void Object::_destroy()
	{
		delete this;
	}
	
	/**
	 * @brief Dynamic casting of smartpointer to a Object derived class.
	 *
	 * Dynamic casting of smartpointer to a Object derived class.
	 *
	 * @param pObject	Pointer to be cast.
	 * 
	 * This method is needed to cast a smartpointer to one of its subclasses. It can also be used to
	 * cast from a subclass to a baseclass, but is more expensive than a normal static cast.
	 * 
	 * @return If successful, the returned pointer points to the cast object. On failure a null pointer is returned.
	 */
	 
	Object_p Object::cast( const Object_p& pObject )
	{
		return pObject;
	}

} // namespace wg
