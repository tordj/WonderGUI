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



	//____ IChildren _________________________________________________________________

	class IChildren;
	typedef	StrongInterfacePtr<IChildren>	IChildren_p;
	typedef	WeakInterfacePtr<IChildren>		IChildren_wp;

	class IChildren : public Interface
	{
	public:
		using		iterator = ChildIterator;

		//.____ Operators __________________________________________

		virtual Widget& operator[](int index) const = 0;


		//.____ Content _______________________________________________________

		virtual int		size() const = 0;
		virtual int		isEmpty() const = 0;

		virtual Widget_p at(int index) const = 0;
		virtual int index(Widget * pChild) const = 0;

		//.____ Misc _______________________________________________________

		inline IChildren_p ptr() { return IChildren_p(this); }

		inline iterator	begin() const { return _begin(); }
		inline iterator	end() const { return _end(); }

	protected:

		// These are needed since we can't make begin/end virtual (shadowing related issues)

		virtual iterator	_begin() const = 0;
		virtual iterator	_end() const = 0;
	};



	//____ IChildrenSubclass _________________________________________________________

	template<class SlotType, class HolderType> class IChildrenSubclass : public IChildren
	{
	public:

		using		iterator = ChildIteratorSubclass<SlotType>;

		/** @private */

		IChildrenSubclass( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : m_pSlotArray(pSlotArray), m_pHolder(pHolder) {}


		//.____ Operators __________________________________________

		inline Widget& operator[](int index) const override { return * m_pSlotArray->slot(index)->pWidget; }


		//.____ Content _______________________________________________________

		inline int		size() const override { return m_pSlotArray->size(); }
		inline int 		isEmpty() const override { return m_pSlotArray->isEmpty(); }

		inline Widget_p at(int index) const override
		{
			if (index < 0 || index >= m_pSlotArray->size())
				return nullptr;

			return Widget_p(m_pSlotArray->slot(index)->pWidget);
		}

		inline int index(Widget * pChild) const override
		{
			if( pChild->_holder() && pChild->_holder()->_childParent() == m_pHolder->_object() )
				return m_pSlotArray->index(static_cast<SlotType*>(pChild->_slot()));

			return -1;
		}


		//.____ Misc _______________________________________________________

		inline StrongInterfacePtr<IChildrenSubclass<SlotType, HolderType>>	ptr() { return StrongInterfacePtr<IChildrenSubclass<SlotType, HolderType>>(this); }

		inline iterator	begin() const { return iterator(m_pSlotArray->begin(),m_pHolder); }
		inline iterator	end() const { return iterator(m_pSlotArray->end(),m_pHolder); }

	protected:
		Object *	_object() const override {	return m_pHolder->_object(); }

		SlotArray<SlotType> *	m_pSlotArray;
		HolderType *			m_pHolder;

		ChildIterator	_begin() const override { return ChildIterator(m_pSlotArray->begin(),m_pHolder); }
		ChildIterator	_end() const override { return ChildIterator(m_pSlotArray->end(),m_pHolder); }

		void _releaseGuardPointer(Widget * pToRelease, SlotType ** pPointerToGuard);	// Release widget from its parent, update pointer slot one of our slots if it is affected.

	};


} // namespace wg
#endif //WG_ICHILDREN_DOT_H
