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

#ifndef	WG_ISLOT_DOT_H
#define	WG_ISLOT_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_slot.h>


namespace wg
{

	//____ CSlot __________________________________________________________

	class CSlot : public Interface
	{
	public:

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		//		inline ISlot operator=(ISlot& iSlot) { Widget_p pWidget = iSlot.m_pSlot->_widget(); if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }

		inline operator Widget_p() const { return widget(); }

		inline bool operator==(Widget * other) const { return other == _slot()->_widget(); }
		inline bool operator!=(Widget * other) const { return other != _slot()->_widget(); }

		inline Widget* operator->() const { return _slot()->_widget(); }

		inline operator BasicSlot&() { return *_slot(); }

		//.____ Content _______________________________________________________

		inline void		setWidget(Widget * pWidget) { _slot()->setWidget(pWidget); }

		inline bool		isEmpty() const { return _slot()->isEmpty(); }
		inline Widget_p widget() const { return Widget_p(_slot()->_widget()); }
		inline Widget*	rawWidgetPtr() const { return _slot()->rawWidgetPtr(); }

		inline Coord	pos() const { return _slot()->pos(); }
		inline Size		size() const { return _slot()->size(); }
		inline Rect		geo() const { return _slot()->geo(); }


		//.____ Misc __________________________________________________________

		inline StrongInterfacePtr<CSlot>	ptr() { return StrongInterfacePtr<CSlot>(this); }

	private:
		virtual BasicSlot *			_slot() = 0;
		virtual const BasicSlot *	_slot() const = 0;
	};

	typedef	StrongInterfacePtr<CSlot>	CSlot_p;
	typedef	WeakInterfacePtr<CSlot>		CSlot_wp;



	//____ ISlot __________________________________________________________


	template<class SlotType> class ISlot : public CSlot, public SlotType
	{

	public:

		/** @private */

		ISlot( typename SlotType::Holder * pHolder ) : SlotType(pHolder) {}


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
		Object * _object() const override { return SlotType::_holder()->_object(); }

		const BasicSlot * _slot() const override { return this; }
		BasicSlot * _slot() override { return this; }


	};




} // namespace wg
#endif //WG_ISLOT_DOT_H
