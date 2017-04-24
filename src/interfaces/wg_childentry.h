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

#ifndef	WG_CHILDENTRY_DOT_H
#define	WG_CHILDENTRY_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_slot.h>


namespace wg 
{

//	template<class SlotType, class HolderType> class ChildEntry;
//	typedef	StrongInterfacePtr<ChildEntry<class SlotType, class HolderType>,Interface_p>	ChildEntry_p;
//	typedef	WeakInterfacePtr<ChildEntry<class SlotType, class HolderType>,Interface_wp>	ChildEntry_wp;

	//____ ChildEntryHolder ____________________________________________________

	class ChildEntryHolder		/** @private */
	{
	public:
		virtual void	_setWidget( Slot * pSlot, Widget * pNewWidget ) = 0;
	};

	
	//____ ChildEntry __________________________________________________________
	
	template<class SlotType, class HolderType> class ChildEntry : public Interface
	{
		
	public:
		ChildEntry( SlotType * pSlot, HolderType * pHolder ) : m_pSlot(pSlot), m_pHolder(pHolder) {}

		inline ChildEntry<SlotType,HolderType> operator=(const Widget_p& pWidget ) { m_pHolder->_setWidget( m_pSlot, pWidget.rawPtr()); return *this; }
		inline operator Widget_p() const { return Widget_p(m_pSlot->pWidget); }

		inline bool operator==(const Widget_p& other) const { return other.rawPtr() == m_pSlot->pWidget; }
		inline bool operator!=(const Widget_p& other) const { return other.rawPtr() != m_pSlot->pWidget; }
	
		inline operator bool() const { return m_pSlot->pWidget != nullptr; }

//		inline Widget& operator*() const{ return * m_pSlotCan->pWidget; };
		inline Widget* operator->() const { return m_pSlot->pWidget; }


		inline Widget_p get() const { return Widget_p(m_pSlot->pWidget); }
		inline void clear() { m_pHolder->_setWidget( m_pSlot, nullptr); }

	protected:
		Object * _object() const {	return m_pHolder; }

		SlotType *	m_pSlot;
		HolderType * m_pHolder;
	};
	
	

} // namespace wg
#endif //WG_CHILDENTRY_DOT_H
