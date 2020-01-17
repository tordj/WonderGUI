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

#ifndef	WG_SLOTITERATOR_DOT_H
#define	WG_SLOTITERATOR_DOT_H
#pragma once


namespace wg
{
	class Widget;
	class StaticSlot;



	//____ SlotIterator _______________________________________________________

	class SlotIterator
	{
	public:

		// Iterator traits, previously from std::iterator.
		using value_type = StaticSlot;
		using difference_type = std::ptrdiff_t;
		using pointer = Widget * ;
		using reference = StaticSlot & ;
		using iterator_category = std::bidirectional_iterator_tag;

		//.____ Creation ___________________________________________________

		SlotIterator() = default;
		explicit SlotIterator(StaticSlot* pSlot, void * pHolder, void * pExtra)
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

		inline StaticSlot * _slot() const { return pSlot; }

	protected:

		virtual void	_inc() {};
		virtual void	_dec() {};

		virtual void	_inc(int amount) {};
		virtual void	_dec(int amount) {};

		StaticSlot *	pSlot = nullptr;
		void *			pHolder = nullptr;
		void *			pExtra = nullptr;
	};


	//____ SlotArrayIterator _______________________________________________________

	template<class SlotType> class SlotArrayIterator : public SlotIterator
	{
	public:

		using reference = SlotType & ;

		//.____ Operators ____________________________________________________________

		inline reference operator*() const
		{
			return *static_cast<SlotType*>(pSlot);
		}

		inline SlotArrayIterator<SlotType>& operator++()
		{
			pSlot = _slot() + 1;
			return *this;
		};

		inline SlotArrayIterator<SlotType> operator++(int)
		{
			SlotArrayIterator<SlotType> tmp = *this;
			pSlot = _slot() + 1;
			return tmp;
		}

		inline SlotArrayIterator<SlotType>& operator--()
		{
			pSlot = _slot() - 1;
			return *this;
		}

		inline SlotArrayIterator<SlotType> operator--(int)
		{
			SlotArrayIterator<SlotType> tmp = *this;
			pSlot = _slot() - 1;
			return tmp;
		}

		inline SlotArrayIterator<SlotType> operator+(int amount) const
		{
			SlotArrayIterator<SlotType> it = *this;
			it._inc(amount);
			return it;
		}

		inline SlotArrayIterator<SlotType> operator-(int amount) const
		{
			SlotArrayIterator<SlotType> it = *this;
			it._dec(amount);
			return it;
		}



		//.____ Internal _______________________________________________________________

		SlotArrayIterator(SlotType * pSlot)
		{
			this->pSlot = pSlot;
			this->pHolder = nullptr;
			this->pExtra = nullptr;
		}

		inline SlotType* _slot() const { return static_cast<SlotType*>(pSlot); }

	protected:

		void	_inc() override { pSlot = (static_cast<SlotType*>(pSlot) + 1); }
		void	_dec() override { pSlot = (static_cast<SlotType*>(pSlot) - 1); }

		void	_inc(int amount) override { pSlot = (static_cast<SlotType*>(pSlot) + amount); }
		void	_dec(int amount) override { pSlot = (static_cast<SlotType*>(pSlot) - amount); }
	};



} // namespace wg
#endif //WG_CSTATICSLOTVECTOR_DOT_H
