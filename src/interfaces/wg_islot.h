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
	class ISlot;
	typedef	StrongInterfacePtr<ISlot>	ISlot_p;
	typedef	WeakInterfacePtr<ISlot>	ISlot_wp;

	//____ SlotHolder ____________________________________________________

	class SlotHolder		/** @private */
	{
	public:
		virtual void		_setWidget( BasicSlot * pSlot, Widget * pNewWidget ) = 0;
		virtual Object *	_object() = 0;
	};


	//____ ISlot __________________________________________________________

	class ISlot : public Interface
	{

	public:

		/** @private */

		ISlot( BasicSlot * pSlot, SlotHolder * pHolder ) : m_pSlot(pSlot), m_pHolder(pHolder) {}

		//.____ Operators __________________________________________

		inline ISlot operator=(ISlot& iSlot) { Widget_p pWidget = iSlot.m_pSlot->_widget(); if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }

		inline ISlot operator=(Widget * pWidget) { if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }
		inline operator Widget_p() const { return m_pSlot->widget(); }

		inline bool operator==(Widget * other) const { return other == m_pSlot->_widget(); }
		inline bool operator!=(Widget * other) const { return other != m_pSlot->_widget(); }

		inline operator bool() const { return m_pSlot->_widget() != nullptr; }

		inline Widget* operator->() const { return m_pSlot->_widget(); }

		//.____ Content _______________________________________________________

		inline Widget_p get() const { return m_pSlot->widget(); }
		inline void clear() { m_pHolder->_setWidget( m_pSlot, nullptr); }

		//.____ Misc __________________________________________________________

		inline StrongInterfacePtr<ISlot>	ptr() { return StrongInterfacePtr<ISlot>(this); }

	protected:
		Object * _object() const override {	return m_pHolder->_object(); }

		BasicSlot *			m_pSlot;
		SlotHolder *	m_pHolder;
	};



} // namespace wg
#endif //WG_ISLOT_DOT_H
