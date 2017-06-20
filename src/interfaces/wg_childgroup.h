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
#include <wg_slotiterator.h>


namespace wg 
{

//	template<class SlotType, class HolderType> class ChildGroup;
//	typedef	StrongInterfacePtr<ChildGroup<class SlotType,class HolderType>,Interface_p>		ChildGroup_p;
//	typedef	WeakInterfacePtr<ChildGroup<class SlotType, class HolderType>,Interface_wp>		ChildGroup_wp;


	//____ ChildGroup _________________________________________________________

	template<class SlotType, class HolderType> class ChildGroup : public Interface
	{		
	public:

		using		iterator = SlotIterator<SlotType>;

		/** @private */

		ChildGroup( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : m_pSlotArray(pSlotArray), m_pHolder(pHolder) {}


		//.____ Operators __________________________________________

		inline Widget& operator[](int index) const { return * m_pSlotArray->slot(index)->pWidget; }


		//.____ Content _______________________________________________________

		inline int		size() const { return m_pSlotArray->size(); }
		inline int 		isEmpty() const { return m_pSlotArray->isEmpty(); }
		inline int		capacity() const { return m_pSlotArray->capacity(); }
		inline int		setCapacity(int capacity) { return m_pSlotArray->setCapacity(capacity);  }

		inline Widget_p at(int index) const
		{
			if (index < 0 || index >= m_pSlotArray->size())
				return nullptr;

			return Widget_p(m_pSlotArray->slot(index)->pWidget);
		}

		//.____ Misc _______________________________________________________

		inline StrongInterfacePtr<ChildGroup<SlotType, HolderType>>	ptr() { return StrongInterfacePtr<ChildGroup<SlotType, HolderType>>(this); }

		iterator	begin() const { return iterator(m_pSlotArray->begin()); }
		iterator	end() const { return iterator(m_pSlotArray->end()); }

	protected:
		Object *	_object() const {	return m_pHolder; }

		SlotArray<SlotType> *	m_pSlotArray;
		HolderType *			m_pHolder;

	};
	

} // namespace wg
#endif //WG_CHILDGROUP_DOT_H
