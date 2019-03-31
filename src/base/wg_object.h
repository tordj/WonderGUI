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
#ifndef WG_OBJECT_DOT_H
#define WG_OBJECT_DOT_H
#pragma once

#include <wg_strongptr.h>

namespace wg 
{
	
	class Object;
	
	typedef StrongPtr<Object>	Object_p;
	
	typedef	void(*Finalizer_p)(Object*);

	class WeakPtrHub		/** @private */
	{
	public:
		int				refCnt;
		Object *		pObj;
		Finalizer_p		pFinalizer;

		static WeakPtrHub *	getHub(Object * pObj);
		static void			releaseHub(WeakPtrHub * pHub);		

		static void			objectWillDestroy(WeakPtrHub * pHub);
		static void			setFinalizer(Object * pObj, Finalizer_p pFinalizer);
		static Finalizer_p	getFinalizer(Object * pObj);
	};
	
	
	/**
	 * @brief Base class for all reference counted objects in WonderGUI.
	 *
	 * Base class for all reference counted objects in WonderGUI.
	 *
	 * Object provides the datastructures needed for smart pointers, weak pointers and
	 * destruction notifiers as well as methods for identifying object types and 
	 * dynamic cast of smart pointers.
	 * 
	 * Objects that are based on Object are implicitly destroyed when their last
	 * reference disappears and should never be explicitly destroyed.
	 *
	 */
	
	class Object
	{
		template<class T> friend class StrongPtr;
		template<class T> friend class WeakPtr;
	
		template<class T> friend class StrongInterfacePtr;
		friend class Interface_wp;

		template<class T> friend class StrongComponentPtr;
		friend class Component_wp;

		friend class WeakPtrHub;
	public:

		//.____ Identification _________________________________________________

		virtual bool		isInstanceOf( const char * pClassName ) const;
		static Object_p		cast( Object * pObject );				// Provided just for completeness sake.
		virtual const char *className( void ) const;
		static const char	CLASSNAME[];
	
		//.____ Misc __________________________________________________________

		void			setFinalizer(Finalizer_p pFinalizer);
		Finalizer_p		finalizer() const;

	protected:
		Object() : m_pWeakPtrHub(0), m_refCount(0) {}
		virtual ~Object() {};
	
		inline void _incRefCount() { m_refCount++; }
		inline void _decRefCount() { m_refCount--; if( m_refCount == 0 ) _destroy(); }

		inline void _incRefCount(int amount) { m_refCount += amount; }
		inline void _decRefCount(int amount) { m_refCount -= amount; if( m_refCount == 0 ) _destroy(); }
	
		WeakPtrHub *	m_pWeakPtrHub;
	
	private:
		virtual void 	_destroy();			// Pointers should call destroy instead of destructor.
		int				m_refCount;
	};
	
	

} // namespace wg
#endif //WG_OBJECT_DOT_H
