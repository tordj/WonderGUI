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

#ifndef	WG_FINALIZER_DOT_H
#define	WG_FINALIZER_DOT_H
#pragma once

#include <functional>
#include <wg_pointers.h>
#include <wg_object.h>

namespace wg 
{
	
	class Finalizer;
	typedef	StrongPtr<Finalizer>		Finalizer_p;
	typedef	WeakPtr<Finalizer>	Finalizer_wp;

	/**
	 * @brief Run custom code when an object is destroyed
	 * 
	 * The Finalizer is an object which can attach itself to one or more objects and call
	 * a function when any of them are destroyed.
	 * 
	 * The Finalizer function is called right before the objects destructor is called, so the
	 * object is still fully intact and functional.
	 * 
	 * A small, extra object is created for each object that the Finalizer is attached to,
	 * resulting in an extra heap allocation and minor memory overhead.
	 *
	 * An object can only be attached to one Finalizer. An attempt to attach a Finalizer to an
	 * object that already is attached to another Finalizer will fail. The object needs 
	 * to be detached from the other Finalizer first.
	 * 
	 * A Finalizer is an Object itself, so you can actually attach a Finalizer to another Finalizer.
	 * This can be useful if you want a function to be called when the last of several objects is
	 * destroyed. Simply attach all objects to the same Finalizer which you then attach to a second Finalizer.
	 * As long as you don't have any StrongPointers keeping the first Finalizer alive, it will be destroyed
	 * when the last attached Object is destroyed, causing the second Finalizer to call its function.
	 * 
	*/
	
	class Finalizer : public Object
	{
		friend class WeakPtrHub;
	public:
		//.____ Creation __________________________________________

		static Finalizer_p	create( std::function<void(Object*)> func ) { return new Finalizer(func); };
	
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Finalizer_p	cast( Object * pObject );
	
		//.____ Control _______________________________________________________

		bool	attach(Object* pObject);
		bool	detach(Object* pObject);


	protected:
		Finalizer(std::function<void(Object*)> func);
		virtual ~Finalizer();
	
		void	_objectWillBeDestroyed(Object * pObject);

		std::function<void(Object*)>	m_function;
	};
	
	

} // namespace wg
#endif //WG_FINALIZER_DOT_H
