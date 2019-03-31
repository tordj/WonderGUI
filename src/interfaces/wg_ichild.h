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

#ifndef	WG_ICHILD_DOT_H
#define	WG_ICHILD_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_slot.h>


namespace wg 
{

//	template<class SlotType, class HolderType> class Child;
//	typedef	StrongInterfacePtr<Child<class SlotType, class HolderType>,Interface_p>	Child_p;
//	typedef	WeakInterfacePtr<Child<class SlotType, class HolderType>,Interface_wp>	Child_wp;

	//____ ChildHolder ____________________________________________________

	class ChildHolder		/** @private */
	{
	public:
		virtual void		_setWidget( Slot * pSlot, Widget * pNewWidget ) = 0;
		virtual Object *	_object() = 0;
	};

	
	//____ IChild __________________________________________________________
	
	template<class SlotType, class HolderType> class IChild : public Interface
	{
		
	public:

		/** @private */

		IChild( SlotType * pSlot, HolderType * pHolder ) : m_pSlot(pSlot), m_pHolder(pHolder) {}

		//.____ Operators __________________________________________

		inline IChild<SlotType,HolderType> operator=(Widget * pWidget ) { m_pHolder->_setWidget( m_pSlot, pWidget); return *this; }
		inline operator Widget_p() const { return Widget_p(m_pSlot->pWidget); }

		inline bool operator==(Widget * other) const { return other == m_pSlot->pWidget; }
		inline bool operator!=(Widget * other) const { return other != m_pSlot->pWidget; }
	
		inline operator bool() const { return m_pSlot->pWidget != nullptr; }

		inline Widget* operator->() const { return m_pSlot->pWidget; }

		//.____ Content _______________________________________________________

		inline Widget_p get() const { return Widget_p(m_pSlot->pWidget); }
		inline void clear() { m_pHolder->_setWidget( m_pSlot, nullptr); }

		//.____ Misc __________________________________________________________

		inline StrongInterfacePtr<IChild<SlotType, HolderType>>	ptr() { return StrongInterfacePtr<IChild<SlotType, HolderType>>(this); }


	protected:
		Object * _object() const {	return m_pHolder->_object(); }

		SlotType *	m_pSlot;
		HolderType * m_pHolder;
	};
	
	

} // namespace wg
#endif //WG_ICHILD_DOT_H
