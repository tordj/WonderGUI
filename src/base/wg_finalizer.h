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
