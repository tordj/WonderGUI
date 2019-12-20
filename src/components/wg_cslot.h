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

#ifndef	WG_CSLOT_DOT_H
#define	WG_CSLOT_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_pointers.h>
#include <wg_slot.h>


namespace wg
{

	//____ CStaticSlot __________________________________________________________

	class CStaticSlot : public Component
	{
	public:

		//.____ Operators __________________________________________

		//		inline ISlot operator=(ISlot& iSlot) { Widget_p pWidget = iSlot.m_pSlot->_widget(); if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }

		inline operator Widget_p() const { return widget(); }

		inline bool operator==(Widget * other) const { return other == _slot()->_widget(); }
		inline bool operator!=(Widget * other) const { return other != _slot()->_widget(); }

		inline Widget* operator->() const { return _slot()->_widget(); }

		inline operator StaticSlot&() { return *_slot(); }

		//.____ Content _______________________________________________________

		inline bool		isEmpty() const { return _slot()->isEmpty(); }
		inline Widget_p widget() const { return Widget_p(_slot()->_widget()); }
		inline Widget*	rawWidgetPtr() const { return _slot()->rawWidgetPtr(); }

		inline Coord	pos() const { return _slot()->pos(); }
		inline Size		size() const { return _slot()->size(); }
		inline Rect		geo() const { return _slot()->geo(); }


		//.____ Misc __________________________________________________________

		inline StrongComponentPtr<CStaticSlot>	ptr() { return StrongComponentPtr<CStaticSlot>(this); }

	private:
		virtual StaticSlot *			_slot() = 0;
		virtual const StaticSlot *	_slot() const = 0;
	};

	typedef	StrongComponentPtr<CStaticSlot>		CStaticSlot_p;
	typedef	WeakComponentPtr<CStaticSlot>		CStaticSlot_wp;


	//____ CDynamicSlot __________________________________________________________

	class CDynamicSlot : public CStaticSlot
	{
	public:

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		inline operator DynamicSlot&() { return * static_cast<DynamicSlot*>(_slot()); }

		//.____ Content _______________________________________________________

		inline void		setWidget(Widget * pWidget) { static_cast<DynamicSlot*>(_slot())->setWidget(pWidget); }
		inline void		clear() { static_cast<DynamicSlot*>(_slot())->setWidget(nullptr); }


		//.____ Misc __________________________________________________________

		inline StrongComponentPtr<CDynamicSlot>	ptr() { return StrongComponentPtr<CDynamicSlot>(this); }

	private:
		virtual StaticSlot *			_slot() = 0;
		virtual const StaticSlot *	_slot() const = 0;
	};

	typedef	StrongComponentPtr<CDynamicSlot>	CDynamicSlot_p;
	typedef	WeakComponentPtr<CDynamicSlot>		CDynamicSlot_wp;



	//____ CStaticSlotImpl<> __________________________________________________________

	template<class SlotType> class CStaticSlotImpl : public CStaticSlot, public SlotType
	{

	public:

		class Holder : public SlotType::Holder	/** @private */
		{
		};

		/** @private */

		CStaticSlotImpl(Holder * pHolder) : SlotType(pHolder) {}

		//.____ Operators __________________________________________

		inline operator Widget_p() const { return SlotType::widget(); }

		inline bool operator==(Widget * other) const { return other == SlotType::_widget(); }
		inline bool operator!=(Widget * other) const { return other != SlotType::_widget(); }

		inline Widget* operator->() const { return SlotType::_widget(); }

		//.____ Content _______________________________________________________

		inline bool		isEmpty() const { return SlotType::isEmpty(); }
		inline Widget_p widget() const { return Widget_p(SlotType::_widget()); }
		inline Widget*	rawWidgetPtr() const { return SlotType::rawWidgetPtr(); }

		inline Coord	pos() const { return SlotType::pos(); }
		inline Size		size() const { return SlotType::size(); }
		inline Rect		geo() const { return SlotType::geo(); }

	protected:
		Object * _object() override { return SlotType::_holder()->_object(); }
		const Object * _object() const override { return SlotType::_holder()->_object(); }

		const StaticSlot * _slot() const override { return this; }
		StaticSlot * _slot() override { return this; }


	};

	//____ CDynamicSlotImpl<> __________________________________________________________

	template<class SlotType> class CDynamicSlotImpl : public CDynamicSlot, public SlotType
	{

	public:

		class Holder : public SlotType::Holder	/** @private */
		{
		};

		/** @private */

		CDynamicSlotImpl(Holder * pHolder) : SlotType(pHolder) {}


		//.____ Operators __________________________________________

		using SlotType::operator=;

		inline operator Widget_p() const { return SlotType::widget(); }

		inline bool operator==(Widget * other) const { return other == SlotType::_widget(); }
		inline bool operator!=(Widget * other) const { return other != SlotType::_widget(); }

		inline Widget* operator->() const { return SlotType::_widget(); }

		//.____ Content _______________________________________________________

		inline void		setWidget(Widget * pWidget) { SlotType::setWidget(pWidget); }

		inline bool		isEmpty() const { return SlotType::isEmpty(); }
		inline Widget_p widget() const { return Widget_p(SlotType::_widget()); }
		inline Widget*	rawWidgetPtr() const { return SlotType::rawWidgetPtr(); }

		inline Coord	pos() const { return SlotType::pos(); }
		inline Size		size() const { return SlotType::size(); }
		inline Rect		geo() const { return SlotType::geo(); }

	protected:
		Object * _object() override { return SlotType::_holder()->_object(); }
		const Object * _object() const override { return SlotType::_holder()->_object(); }

		const StaticSlot * _slot() const override { return this; }
		StaticSlot * _slot() override { return this; }


	};


} // namespace wg
#endif //WG_CSLOT_DOT_H
