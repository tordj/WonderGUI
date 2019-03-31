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

#ifndef	WG_ICHILDREN_DOT_H
#define	WG_ICHILDREN_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_container.h>
#include <wg_slotarray.h>
#include <wg_childiterator.h>


namespace wg 
{

//	template<class SlotType, class HolderType> class Children;
//	typedef	StrongInterfacePtr<Children<class SlotType,class HolderType>,Interface_p>		Children_p;
//	typedef	WeakInterfacePtr<Children<class SlotType, class HolderType>,Interface_wp>		Children_wp;


	//____ ChildrenHolder ___________________________________________________

	class ChildrenHolder /** @private */
	{
	public:
		virtual Object * _object() = 0;
	};


	//____ IChildren _________________________________________________________

	template<class SlotType, class HolderType> class IChildren : public Interface
	{		
	public:

		using		iterator = ChildIterator<SlotType>;

		/** @private */

		IChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : m_pSlotArray(pSlotArray), m_pHolder(pHolder) {}


		//.____ Operators __________________________________________

		inline Widget& operator[](int index) const { return * m_pSlotArray->slot(index)->pWidget; }


		//.____ Content _______________________________________________________

		inline int		size() const { return m_pSlotArray->size(); }
		inline int 		isEmpty() const { return m_pSlotArray->isEmpty(); }
		inline int		capacity() const { return m_pSlotArray->capacity(); }
		inline void		setCapacity(int capacity) { m_pSlotArray->setCapacity(capacity);  }

		inline Widget_p at(int index) const
		{
			if (index < 0 || index >= m_pSlotArray->size())
				return nullptr;

			return Widget_p(m_pSlotArray->slot(index)->pWidget);
		}

		inline int index(Widget * pChild) const
		{
			if( pChild->_holder() && pChild->_holder()->_childParent() == m_pHolder->_object() )
				return m_pSlotArray->index(static_cast<SlotType*>(pChild->_slot()));

			return -1;
		}


		//.____ Misc _______________________________________________________

		inline StrongInterfacePtr<IChildren<SlotType, HolderType>>	ptr() { return StrongInterfacePtr<IChildren<SlotType, HolderType>>(this); }

		iterator	begin() const { return iterator(m_pSlotArray->begin()); }
		iterator	end() const { return iterator(m_pSlotArray->end()); }

	protected:
		Object *	_object() const {	return m_pHolder->_object(); }

		SlotArray<SlotType> *	m_pSlotArray;
		HolderType *			m_pHolder;

	};
	

} // namespace wg
#endif //WG_ICHILDREN_DOT_H
