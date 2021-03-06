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

#ifndef	WG_CSLOTARRAY_DOT_H
#define	WG_CSLOTARRAY_DOT_H
#pragma once

#include <wg_cstaticslotcollection.h>
#include <wg_base.h>

namespace wg
{

	//____ CSlotArray _________________________________________________________

	template<class SlotType, int SIZE> class CSlotArray : public CStaticSlotCollection
	{
	public:

		using		iterator = SlotArrayIterator<SlotType>;

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override { return TYPEINFO; }
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline int		size() const override { return SIZE; }
		inline bool		isEmpty() const override { return false; }		// Technically never empty since that would mean has no slots.

		inline SlotType& at(int index) const
		{
//			if (index < 0 || index >= m_pSlotArray->size())
//				return nullptr;

			return m_slots[index];
		}

		int		index(const Widget * pWidget) const override
		{
			auto pSlot = static_cast<SlotType*>(pWidget->_slot());

			if (pSlot >= m_slots && pSlot < &m_slots[SIZE])
				return int(pSlot - m_slots);

			return -1;
		}

		//.____ Misc _______________________________________________________

		inline iterator	begin() const { return iterator(&m_slots[0]); }
		inline iterator	end() const { return iterator(&m_slots[SIZE]); }

		//.____ Operators __________________________________________

		inline SlotType& operator[](int index) { return m_slots[index]; }
		inline const SlotType& operator[](int index) const { return m_slots[index]; }

	protected:
		CSlotArray(SlotHolder * pHolder) : m_pHolder(pHolder) 
		{
			for (auto& slot : m_slots)
				slot.m_pHolder = pHolder;
		}

		SlotIterator	_begin_iterator() override { return iterator(&m_slots[0]); }
		SlotIterator	_end_iterator() override { return iterator(&m_slots[SIZE]); }

		StaticSlot&		_at(int index) override 
		{ 
			if (index < 0 || index >= SIZE)
				Base::handleError(ErrorSeverity::Serious, ErrorCode::OutOfRange, "Slot index out of range", _object(), TYPEINFO, __func__, __FILE__, __LINE__);

			return m_slots[index];
		}

		Object *		_object() override { return m_pHolder->_object(); }
		const Object *	_object() const override { return m_pHolder->_object(); }


	//////
		inline SlotHolder *			_holder() { return m_pHolder; }
		inline const SlotHolder *	_holder() const { return m_pHolder; }

		bool		_contains(const SlotType * pSlot) const { return (pSlot >= m_slots && pSlot < &m_slots[SIZE]); }

		SlotType*	_slot(int index) const { return &m_slots[index]; }

		int			_index(const SlotType* pSlot) const { return int(pSlot - m_slots); }

		SlotType*	_first() const { return m_slots; }
		SlotType*	_last() const { return &m_slots[SIZE-1]; }

		SlotType*	_begin() const { return &m_slots[0]; }
		SlotType*	_end() const { return &m_slots[SIZE]; }

	private:

		SlotType	 m_slots[SIZE];
		SlotHolder * m_pHolder;
	};

	template<class SlotType, int SIZE> const TypeInfo CSlotArray<SlotType,SIZE>::TYPEINFO = { "CSlotArray<Unknow>", &CStaticSlotCollection::TYPEINFO };


} // namespace wg
#endif //WG_CSLOTARRAY_DOT_H
