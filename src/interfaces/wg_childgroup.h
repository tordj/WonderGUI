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

#ifndef	WG_CHILDGROUP_DOT_H
#define	WG_CHILDGROUP_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_container.h>
#include <wg_slotarray.h>


namespace wg 
{

//	template<class SlotType, class HolderType> class ChildGroup;
//	typedef	StrongInterfacePtr<ChildGroup<class SlotType,class HolderType>,Interface_p>		ChildGroup_p;
//	typedef	WeakInterfacePtr<ChildGroup<class SlotType, class HolderType>,Interface_wp>		ChildGroup_wp;

	//____ ChildGroupHolder _________________________________________________________

	class ChildGroupHolder
	{
		virtual void	_didAddSlots( Slot * pSlot, int nb ) = 0;
		virtual void	_willRemoveSlots( Slot * pSlot, int nb ) = 0;
	};

	//____ ChildGroup _________________________________________________________

	template<class SlotType, class HolderType> class ChildGroup : public Interface
	{
		
	public:
		ChildGroup( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : m_pSlotArray(pSlotArray), m_pHolder(pHolder) {}

		inline StrongInterfacePtr<ChildGroup<SlotType,HolderType>,Interface_p>	ptr() { return StrongInterfacePtr<ChildGroup<SlotType,HolderType>,Interface_p>(_object(),this); }


		inline Widget& operator[](int index) const { return * m_pSlotArray->slot(index)->pWidget; }

		inline int		size() const { return m_pSlotArray->size(); } 

		inline Widget_p get( int index) const 
		{ 
			if( index < 0 || index >= m_pSlotArray->size() )
				return nullptr;

			return Widget_p(m_pSlotArray->slot(index)->pWidget); 
		}

		bool add( const Widget_p& pWidget )
		{
			SlotType * pSlot = m_pSlotArray->add();
			pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());
			m_pHolder->_didAddSlots(pSlot, 1);
			return true;
		}
		
		bool insert( int index, const Widget_p& pWidget )
		{
			if( index < 0 || index >= m_pSlotArray->size() )
				return false;

			SlotType * pSlot = m_pSlotArray->insert(index);
			pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());
			m_pHolder->_didAddSlots(pSlot, 1);
			return true;		
		}
		
		bool remove( int index )
		{
			if( index < 0 || index >= m_pSlotArray->size() )
				return false;

			SlotType * pSlot = m_pSlotArray->slot(index);
			m_pHolder->_willRemoveSlots(pSlot, 1);
			m_pSlotArray->remove(index);
			return true;		
		}


		void clear() 
		{ 
			if( m_pSlotArray->isEmpty() ) 
				return; 
			m_pHolder->_willRemoveSlots(m_pSlotArray->begin(), m_pSlotArray->size()); 
			m_pSlotArray->clear(); 
		}
	
	protected:
		Object *	_object() const {	return m_pHolder; }

		SlotArray<SlotType> *	m_pSlotArray;
		HolderType *			m_pHolder;

	};
	
	

} // namespace wg
#endif //WG_CHILDGROUP_DOT_H
