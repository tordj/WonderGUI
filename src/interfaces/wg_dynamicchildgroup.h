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

#ifndef	WG_DYNAMICCHILDGROUP_DOT_H
#define	WG_DYNAMICCHILDGROUP_DOT_H
#pragma once

#include <wg_childgroup.h>

namespace wg 
{

//	template<class SlotType, class HolderType> class ChildGroup;
//	typedef	StrongInterfacePtr<ChildGroup<class SlotType,class HolderType>,Interface_p>		ChildGroup_p;
//	typedef	WeakInterfacePtr<ChildGroup<class SlotType, class HolderType>,Interface_wp>		ChildGroup_wp;

	//____ DynamicChildGroupHolder _________________________________________________________

	class DynamicChildGroupHolder		/** @private */
	{
		virtual void	_didAddSlots( Slot * pSlot, int nb ) = 0;
		virtual void	_willRemoveSlots( Slot * pSlot, int nb ) = 0;
	};

	//____ DynamicChildGroup _________________________________________________________

	template<class SlotType, class HolderType> class DynamicChildGroup : public ChildGroup<SlotType,HolderType>
	{		
	public:

		using		iterator = SlotIterator<SlotType>;
		using		ChildGroup<SlotType,HolderType>::m_pSlotArray;
		using		ChildGroup<SlotType,HolderType>::m_pHolder;
		
		//.____ Creation __________________________________________

		DynamicChildGroup( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : ChildGroup<SlotType,HolderType>(pSlotArray,pHolder) {}

		//.____ Operators __________________________________________

		inline iterator operator<<(Widget * pWidget) { return add(pWidget); }	

		//.____ Content _______________________________________________________

		iterator add( Widget * pWidget )
		{
			SlotType * pSlot = m_pSlotArray->add();
			pSlot->replaceWidget(m_pHolder,pWidget);
			m_pHolder->_didAddSlots(pSlot, 1);
			return iterator(pSlot);
		}

		iterator add( const Widget_p pWidgets[], int amount )
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->add(amount);

			for( int i = 0; i < amount ; i++ )
				pSlot[i].replaceWidget(m_pHolder,pWidgets[i]);
			m_pHolder->_didAddSlots(pSlot, amount);
			return iterator(pSlot);		
		}
		
		iterator insert( int index, Widget * pWidget )
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->insert(index);
			pSlot->replaceWidget(m_pHolder,pWidget);
			m_pHolder->_didAddSlots(pSlot, 1);
			return iterator(pSlot);		
		}

		iterator insert(iterator pos, Widget * pWidget)
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->insert(pos._slot());
			pSlot->replaceWidget(m_pHolder, pWidget);
			m_pHolder->_didAddSlots(pSlot, 1);
			return iterator(pSlot);
		}

		iterator insert( int index, const Widget_p pWidgets[], int amount )
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->insert(index, amount);

			for( int i = 0; i < amount ; i++ )
				pSlot[i].replaceWidget(m_pHolder,pWidgets[i]);
			m_pHolder->_didAddSlots(pSlot, amount);
			return iterator(pSlot);		
		}

		iterator insert(iterator pos, const Widget_p pWidgets[], int amount )
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->insert(pos._slot(), amount);

			for( int i = 0 ; i < amount ; i++ )
				pSlot[i].replaceWidget(m_pHolder, pWidgets[i]);
			m_pHolder->_didAddSlots(pSlot, amount);
			return iterator(pSlot);
		}

		iterator remove( int index )
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->slot(index);
			m_pHolder->_willRemoveSlots(pSlot, 1);
			return iterator(m_pSlotArray->remove(index));
		}

		iterator remove(iterator pos)
		{
			//TODO: Add assert

			m_pHolder->_willRemoveSlots(pos._slot(), 1);
			return iterator(m_pSlotArray->remove(pos._slot()));
		}

		iterator remove( int index, int amount )
		{
			//TODO: Add assert

			SlotType * pSlot = m_pSlotArray->slot(index);
			m_pHolder->_willRemoveSlots(pSlot, amount);
			return iterator(m_pSlotArray->remove(index, amount));
		}

		iterator remove(iterator beg, iterator end)
		{
			//TODO: Add assert

			m_pHolder->_willRemoveSlots(beg._slot(), end._slot() - beg._slot());
			return iterator(m_pSlotArray->remove(beg._slot(), end._slot() ));
		}

		void clear() 
		{ 
			if( m_pSlotArray->isEmpty() ) 
				return; 
			m_pHolder->_willRemoveSlots(m_pSlotArray->begin(), m_pSlotArray->size()); 
			m_pSlotArray->clear(); 
		}
	};
	

} // namespace wg
#endif //WG_CHILDGROUP_DOT_H
