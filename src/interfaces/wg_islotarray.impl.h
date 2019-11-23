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

#ifndef	WG_ISLOTARRAY_IMPL_DOT_H
#define	WG_ISLOTARRAY_IMPL_DOT_H
#pragma once

#include <wg_islotarray.h>

//#define INSTANTIATE_CHILDREN(SlotType)		template class ISlotArray< SlotType >;

namespace wg
{
	class Widget;


	//____ add() _________________________________________________________________

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::add(Widget * pWidget)
	{
		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		SlotType * pSlot = m_pSlotArray->add();
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::add(const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		for (int i = 0; i < amount; i++)
			pWidgets[i]->releaseFromParent();

		SlotType * pSlot = m_pSlotArray->add(amount);
		auto pWidgetHolder = m_pHolder->_widgetHolder();

		for (int i = 0; i < amount; i++)
			pSlot[i].replaceWidget(pWidgetHolder, pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	//____ insert() ______________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::insert(int index, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->insert(index);

		this->_releaseGuardPointer(pWidget, &pSlot);
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::insert(const SlotIterator& pos, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(pos._slot());
		pSlot = m_pSlotArray->insert(pSlot);

		this->_releaseGuardPointer(pWidget, &pSlot);
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	void ISlotArray<SlotType>::insert(int index, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->insert(index, amount);
		auto pWidgetHolder = m_pHolder->_widgetHolder();

		for (int i = 0; i < amount; i++)
		{
			this->_releaseGuardPointer(pWidgets[i], &pSlot);
			pSlot[i].replaceWidget(pWidgetHolder, pWidgets[i]);
		}
		m_pHolder->_didAddSlots(pSlot, amount);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::insert(const SlotIterator& pos, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(pos._slot());
		pSlot = m_pSlotArray->insert(pSlot, amount);
		auto pWidgetHolder = m_pHolder->_widgetHolder();

		for (int i = 0; i < amount; i++)
		{
			this->_releaseGuardPointer(pWidgets[i], &pSlot);
			pSlot[i].replaceWidget(pWidgetHolder, pWidgets[i]);
		}
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	//____ remove() ______________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::remove(int index)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->slot(index);
		m_pHolder->_willRemoveSlots(pSlot, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::remove(const SlotIterator& pos)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(pos._slot());
		m_pHolder->_willRemoveSlots(pSlot, 1);
		return iterator(m_pSlotArray->remove(pSlot));
	}

	template < class SlotType>
	void ISlotArray<SlotType>::remove(int index, int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = m_pSlotArray->slot(index);
		m_pHolder->_willRemoveSlots(pSlot, amount);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::remove(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Add assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());

		m_pHolder->_willRemoveSlots(pBeg, pEnd-pBeg);
		return iterator(m_pSlotArray->remove(pBeg, pEnd));
	}

	//____ clear() _______________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::clear()
	{
		if (m_pSlotArray->isEmpty())
			return;
		m_pHolder->_willRemoveSlots(m_pSlotArray->begin(), m_pSlotArray->size());
		m_pSlotArray->clear();
	}

	//____ moveToBack() __________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::moveToBack(int index)
	{
		//TODO: Assert

		SlotType * pFrom = m_pSlotArray->slot(index);
		SlotType * pTo = m_pSlotArray->end();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::moveToBack(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = m_pSlotArray->end();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(m_pSlotArray->last());
	}

	//____ moveToFront() _________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::moveToFront(int index)
	{
		//TODO: Assert

		SlotType * pFrom = m_pSlotArray->slot(index);
		SlotType * pTo = m_pSlotArray->begin();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::moveToFront(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = m_pSlotArray->begin();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(m_pSlotArray->begin());
	}

	//____ moveBefore() __________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::moveBefore(int index, int sibling)
	{
		//TODO: Assert

		SlotType * pFrom = m_pSlotArray->slot(index);
		SlotType * pTo = m_pSlotArray->slot(sibling);

		if (pFrom < pTo)
			pTo--;

		if (pFrom != pTo)
		{
			m_pSlotArray->move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}
	}

	template < class SlotType>
	SlotArrayIterator<SlotType> ISlotArray<SlotType>::moveBefore(const SlotIterator& it, const SlotIterator& sibling)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = static_cast<SlotType*>(sibling._slot());

		if (pFrom < pTo)
			pTo--;

		if (pFrom != pTo)
		{
			m_pSlotArray->move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}

		return iterator(pTo);
	}

	//____ hide() ________________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::hide(int index)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(m_pSlotArray->slot(index), 1);
	};

	template < class SlotType>
	void ISlotArray<SlotType>::hide(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		m_pHolder->_hideSlots(pSlot, 1);
	};


	template < class SlotType>
	void ISlotArray<SlotType>::hide(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_hideSlots(m_pSlotArray->slot(index), amount);
	};

	template < class SlotType>
	void ISlotArray<SlotType>::hide(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());
		m_pHolder->_hideSlots(pBeg, pEnd-pBeg);
	};

	//____ hideAll() _____________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::hideAll()
	{
		m_pHolder->_hideSlots(m_pSlotArray->slot(0), m_pSlotArray->size());
	};

	//____ unhide() ______________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::unhide(int index)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(m_pSlotArray->slot(index), 1);
	};

	template < class SlotType>
	void ISlotArray<SlotType>::unhide(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		m_pHolder->_unhideSlots(pSlot, 1);
	};

	template < class SlotType>
	void ISlotArray<SlotType>::unhide(int index, int amount)
	{
		//TODO: Assert

		m_pHolder->_unhideSlots(m_pSlotArray->slot(index), amount);
	};

	template < class SlotType>
	void ISlotArray<SlotType>::unhide(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());
		m_pHolder->_unhideSlots(pBeg, pEnd-pBeg);
	};

	//____ unhideAll() ________________________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::unhideAll()
	{
		m_pHolder->_unhideSlots(m_pSlotArray->slot(0), m_pSlotArray->size());
	};

	//____ _begin() ___________________________________________________________

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_begin() const 
	{ 
		return iterator(m_pSlotArray->begin()); 
	}

	//____ _end() _____________________________________________________________

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_end() const 
	{ 
		return iterator(m_pSlotArray->end()); 
	}

	//____ _object() __________________________________________________________

	template < class SlotType>
	Object * ISlotArray<SlotType>::_object() const
	{
		return m_pHolder->_object();
	}


	//____ _releaseGuardPointer() _____________________________________________

	template < class SlotType>
	void ISlotArray<SlotType>::_releaseGuardPointer(Widget * pToRelease, SlotType ** pPointerToGuard)
	{
		Container * pParent = pToRelease->_parent();

		if (pParent)
		{
			Slot * pReleaseFromSlot = pToRelease->_slot();

			if (m_pSlotArray->contains(pReleaseFromSlot))
			{
				// We are releasing a widget from our own slot array, so we need to make sure pointer still is correct afterwards.

				int ofs = (int)((*pPointerToGuard) - m_pSlotArray->first());
				if (*pPointerToGuard > pReleaseFromSlot)
					ofs--;

				pToRelease->releaseFromParent();
				*pPointerToGuard = m_pSlotArray->first() + ofs;
			}
			else
				pToRelease->releaseFromParent();
		}
	}




	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_add(Widget * pWidget)
	{
		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		SlotType * pSlot = m_pSlotArray->add();
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_add(const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		for (int i = 0; i < amount; i++)
			pWidgets[i]->releaseFromParent();

		SlotType * pSlot = m_pSlotArray->add(amount);
		auto pWidgetHolder = m_pHolder->_widgetHolder();

		for (int i = 0; i < amount; i++)
			pSlot[i].replaceWidget(pWidgetHolder, pWidgets[i]);
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_insert(const SlotIterator& it, Widget * pWidget)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		pSlot = m_pSlotArray->insert(pSlot);

		this->_releaseGuardPointer(pWidget, &pSlot);
		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_insert(const SlotIterator& it, const Widget_p pWidgets[], int amount)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		pSlot = m_pSlotArray->insert(pSlot, amount);
		auto pWidgetHolder = m_pHolder->_widgetHolder();

		for (int i = 0; i < amount; i++)
		{
			this->_releaseGuardPointer(pWidgets[i], &pSlot);
			pSlot[i].replaceWidget(pWidgetHolder, pWidgets[i]);
		}
		m_pHolder->_didAddSlots(pSlot, amount);
		return iterator(pSlot);
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_remove(const SlotIterator& it)
	{
		//TODO: Add assert

		SlotType * pSlot = static_cast<SlotType*>(it._slot());
		m_pHolder->_willRemoveSlots(pSlot, 1);
		return iterator(m_pSlotArray->remove(pSlot));
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_remove(const SlotIterator& beg, const SlotIterator& end)
	{
		//TODO: Add assert

		SlotType * pBeg = static_cast<SlotType*>(beg._slot());
		SlotType * pEnd = static_cast<SlotType*>(end._slot());
		m_pHolder->_willRemoveSlots(pBeg, pEnd-pBeg);
		return iterator(m_pSlotArray->remove(pBeg, pEnd));
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_moveToFront(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = m_pSlotArray->begin();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(m_pSlotArray->begin());
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_moveToBack(const SlotIterator& it)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = m_pSlotArray->end();

		m_pSlotArray->move(pFrom, pTo);
		m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		return iterator(m_pSlotArray->last());
	}

	template < class SlotType>
	SlotIterator ISlotArray<SlotType>::_moveBefore(const SlotIterator& it, const SlotIterator& sibling)
	{
		//TODO: Assert

		SlotType * pFrom = static_cast<SlotType*>(it._slot());
		SlotType * pTo = static_cast<SlotType*>(sibling._slot());

		if (pFrom < pTo)
			pTo--;

		if (pFrom != pTo)
		{
			m_pSlotArray->move(pFrom, pTo);
			m_pHolder->_didMoveSlots(pFrom, pTo, 1);
		}

		return iterator(pTo);
	}



} // namespace wg

#endif //WG_ISLOTARRAY_IMPL_DOT_H

