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
#include <wg_pointers.h>
#include <wg_base.h>

namespace wg
{

	const char Object::CLASSNAME[] = {"Object"};


	WeakPtrHub * WeakPtrHub::getHub(Object * pObj)
	{
		if( !pObj)
			return nullptr;

		WeakPtrHub * pHub;
		if( !pObj->m_pWeakPtrHub )
		{
			pHub = Base::_allocWeakPtrHub();
			pHub->refCnt = 1;
			pHub->pObj = pObj;
			pObj->m_pWeakPtrHub = pHub;
		}
		else
		{
			pHub = pObj->m_pWeakPtrHub;
			pHub->refCnt++;
		}
		return pHub;
	}

	void WeakPtrHub::releaseHub(WeakPtrHub * pHub)
	{
		if( pHub )
		{
			pHub->refCnt--;

			if( pHub->refCnt == 0 && !pHub->pFinalizer )
			{
				if( pHub->pObj )
					pHub->pObj->m_pWeakPtrHub = nullptr;
				Base::_freeWeakPtrHub(pHub);
			}
		}
	}

	void WeakPtrHub::objectWillDestroy(WeakPtrHub * pHub)
	{
		if (pHub->pFinalizer)
		{
			pHub->pFinalizer(pHub->pObj);
			pHub->pObj = nullptr;
			pHub->pFinalizer = nullptr;
			if (pHub->refCnt == 0)
				Base::_freeWeakPtrHub(pHub);
		}
		else
			pHub->pObj = nullptr;
	}

	void WeakPtrHub::setFinalizer(Object * pObj, Finalizer_p pFinalizer)
	{
		WeakPtrHub * pHub;
		if (!pObj->m_pWeakPtrHub)
		{
			pHub = Base::_allocWeakPtrHub();
			pHub->refCnt = 0;
			pHub->pObj = pObj;
			pObj->m_pWeakPtrHub = pHub;
		}
		else
		{
			pHub = pObj->m_pWeakPtrHub;
		}

		pHub->pFinalizer = pFinalizer;
	}

	Finalizer_p WeakPtrHub::getFinalizer(Object * pObj)
	{
		if (pObj->m_pWeakPtrHub)
			return pObj->m_pWeakPtrHub->pFinalizer;

		return nullptr;
	}

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
		if (m_pWeakPtrHub)
			WeakPtrHub::objectWillDestroy(m_pWeakPtrHub);
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

	Object_p Object::cast( Object * pObject )
	{
		return pObject;
	}

	/**
	* @brief Set a callback for when object is destroyed.
	*
	* Set a callback that will be called when the object is destroyed.
	*
	* @param pFinalizer		Pointer to a function of format "void FunctionName(Object*)", where the Object pointer points to the object being destroyed.
	*
	* A WonderGUI Object will automatically be destroyed when there no longer is any reference to it (besides any weak pointer).
	* Setting a Finalizer function allows us to be notified right before the objects destructor is called.
	*
	* Each object can have only one finalizer, so setting a finalizer will remove any previous finalizer for that object.
	*
	* Finalizers comes with a cost. A small, extra memory allocation is performed for each object with a finalizer.
	*
	* @return Nothing, method always succeeds.
	*/

	void Object::setFinalizer(Finalizer_p pFinalizer)
	{
		WeakPtrHub::setFinalizer(this, pFinalizer);
	}

	/**
	* @brief Get the Finalizer for this Object.
	*
	* Get the Finalizer (if any) set for this Object.
	*
	* A WonderGUI Object will automatically be destroyed when there no longer is any reference to it (besides any weak pointer).
	* The finalizer is a function that is called right before the objects destructor, thus notifying us of its immediate demise.
	*
	* @return Pointer to the finalizer if one has been set, otherwise null.
	*/

	Finalizer_p Object::finalizer() const
	{
		if (m_pWeakPtrHub)
			return m_pWeakPtrHub->pFinalizer;

		return nullptr;
	}

} // namespace wg
