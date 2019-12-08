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

#ifndef	WG_CSLOTCOLLECTION_DOT_H
#define	WG_CSLOTCOLLECTION_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_pointers.h>
#include <wg_container.h>
#include <wg_slot.h>

namespace wg
{

	//____ SlotIterator _______________________________________________________

	class SlotIterator
	{
	public:

		// Iterator traits, previously from std::iterator.
		using value_type = BasicSlot;
		using difference_type = std::ptrdiff_t;
		using pointer = Widget * ;
		using reference = BasicSlot & ;
		using iterator_category = std::bidirectional_iterator_tag;

		//.____ Creation ___________________________________________________

		SlotIterator() = default;
		explicit SlotIterator(BasicSlot* pSlot, void * pHolder, void * pExtra)
		{
			this->pSlot = pSlot;
			this->pHolder = pHolder;
			this->pExtra = pExtra;
		}

		//.____ Operators ______________________________________________________

		inline reference operator*() const
		{
			return *pSlot;
		}

		inline pointer operator->() const
		{
			return pSlot->_widget();
		}


		inline SlotIterator& operator++()
		{
			_inc();
			return *this;
		}

		inline SlotIterator operator++(int)
		{
			SlotIterator it = *this;
			_inc();
			return it;
		}

		inline SlotIterator& operator--()
		{
			_dec();
			return *this;
		}


		inline SlotIterator operator--(int)
		{
			SlotIterator it = *this;
			_dec();
			return it;
		}
		
		inline SlotIterator operator+(int amount) const
		{
			SlotIterator it = *this;
			it._inc(amount);
			return it; 
		}
		
		inline SlotIterator operator-(int amount) const
		{
			SlotIterator it = *this;
			it._dec(amount);
			return it; 
		}
		
		inline bool operator==(const SlotIterator& rhs) const
		{
			return pSlot == rhs.pSlot;
		}

		inline bool operator!=(const SlotIterator& rhs) const
		{
			return pSlot != rhs.pSlot;
		}

		inline bool operator< (const SlotIterator& rhs) const { return pSlot < rhs.pSlot; }
		inline bool operator> (const SlotIterator& rhs) const { return pSlot > rhs.pSlot; }
		inline bool operator<=(const SlotIterator& rhs) const { return pSlot <= rhs.pSlot; }
		inline bool operator>=(const SlotIterator& rhs) const { return pSlot >= rhs.pSlot; }

		//.____ Internal _______________________________________________________________

		inline BasicSlot * _slot() const { return pSlot; }

	protected:

		virtual void	_inc() {};
		virtual void	_dec() {};

		virtual void	_inc(int amount) {};
		virtual void	_dec(int amount) {};

		BasicSlot *		pSlot = nullptr;
		void *			pHolder = nullptr;
		void *			pExtra = nullptr;
	};



	//____ CSlotCollection _________________________________________________________________

	class CSlotCollection;
	typedef	StrongComponentPtr<CSlotCollection>	CSlotCollection_p;
	typedef	WeakComponentPtr<CSlotCollection>	CSlotCollection_wp;

	class CSlotCollection : public Component
	{
	public:
		using		iterator = SlotIterator;

		//.____ Content _______________________________________________________

		virtual int		size() const = 0;
		virtual bool	isEmpty() const = 0;

		inline BasicSlot& at(int index) const { return _at(index); }
		virtual int index(Widget * pChild) const = 0;

		//.____ Misc _______________________________________________________

		inline CSlotCollection_p ptr() { return CSlotCollection_p(this); }

		inline iterator	begin() const { return _begin_iterator(); }
		inline iterator	end() const { return _end_iterator(); }

	protected:

		// These are needed since we can't make begin/end virtual (shadowing related issues)

		virtual iterator	_begin_iterator() const = 0;
		virtual iterator	_end_iterator() const = 0;
		virtual BasicSlot&	_at(int index) const = 0;
	};

} // namespace wg
#endif //WG_CSLOTCOLLECTION_DOT_H