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


#ifndef WG_SLOT_DOT_H
#define WG_SLOT_DOT_H
#pragma once

#include <wg_widget.h>

namespace wg
{
	//____ StaticSlot __________________________________________________________

	class StaticSlot
	{
		friend class Widget;
		friend class SlotIterator;
		friend class SlotHolder;

	public:

		//.____ Identification ________________________________________________

		const TypeInfo& typeInfo(void) const { return m_pHolder->_slotTypeInfo(this); }

		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline Widget_p widget() const { return Widget_p(m_pWidget); }
		inline Widget*	rawWidgetPtr() const { return m_pWidget; }
		inline bool		isEmpty() const { return !m_pWidget; }

		//.____ Geometry ______________________________________________________

		inline Coord	pos() const { return m_pHolder->_childPos(this); }
		inline Size		size() const { return m_pWidget->m_size; }
		inline Rect		geo() const { return Rect(m_pHolder->_childPos(this), m_pWidget->m_size); }

		//.____ Operators __________________________________________

		inline operator Widget_p() const { return widget(); }

		inline bool operator==(Widget * other) const { return other == _widget(); }
		inline bool operator!=(Widget * other) const { return other != _widget(); }

		inline Widget* operator->() const { return _widget(); }

		//.____ Internal ______________________________________________________

		inline Widget * _widget() const { return m_pWidget; }


	protected:
		const static bool safe_to_relocate = true;

		StaticSlot(SlotHolder * pHolder) : m_pHolder(pHolder) {}

		StaticSlot(StaticSlot&& o)
		{
			m_pWidget = o.m_pWidget;
			m_pHolder = o.m_pHolder;
			if (m_pWidget)
			{
				m_pWidget->_setSlot(this);
				o.m_pWidget = nullptr;
			}
		}

		// IMPORTANT! SlotVector assumes that Slot destructor doesn't need to be called
		// if content has been moved to another slot!

		~StaticSlot()
		{
			if (m_pWidget != nullptr)
			{
				m_pWidget->_setSlot(nullptr);
				m_pWidget->_decRefCount();
			}
		}

		StaticSlot& operator=(StaticSlot&& o)
		{
			if (m_pWidget)
			{
				m_pWidget->_setSlot(nullptr);
				m_pWidget->_decRefCount();
			}

			m_pWidget = o.m_pWidget;
			//			m_pHolder = o.m_pHolder;

			if (m_pWidget)
			{
				m_pWidget->_setSlot(this);
				o.m_pWidget = nullptr;
			}

			return *this;
		}


		inline void _relink() { m_pWidget->_setSlot( this ); }

		inline void _setWidget( Widget * pWidget )
		{
			if( m_pWidget )
			{
				if (m_pWidget == pWidget)
					return;

				m_pWidget->_setSlot( nullptr );
				m_pWidget->_decRefCount();
			}

			m_pWidget = pWidget;

			if( pWidget )
			{
				pWidget->_incRefCount();
				pWidget->releaseFromParent();
				pWidget->_setSlot( this );
			}
		}

		inline SlotHolder * _holder() { return m_pHolder; }
		inline const SlotHolder * _holder() const { return m_pHolder; }

		inline void		_setSize( Size size ) const { m_pWidget->_resize(size); }

		Widget *		m_pWidget = nullptr;
		SlotHolder *	m_pHolder;
	};

	//____ DynamicSlot __________________________________________________________

	class DynamicSlot : public StaticSlot
	{
		friend class Widget;
		friend class SlotIterator;
		friend class SlotHolder;

	public:

		//.____ Identification ________________________________________________

		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline void		setWidget(const Widget_p& pWidget) { if (pWidget) pWidget->releaseFromParent(); m_pHolder->_replaceChild(this, pWidget); }

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		//		inline ISlot operator=(ISlot& iSlot) { Widget_p pWidget = iSlot.m_pSlot->_widget(); if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }


	protected:
		const static bool safe_to_relocate = true;

		DynamicSlot(SlotHolder * pHolder) : StaticSlot(pHolder) {}

		DynamicSlot(DynamicSlot&& o) : StaticSlot(o.m_pHolder)
		{
			m_pWidget = o.m_pWidget;
			if (m_pWidget)
			{
				m_pWidget->_setSlot(this);
				o.m_pWidget = nullptr;
			}
		}

		DynamicSlot& operator=(DynamicSlot&& o)
		{
			if (m_pWidget)
			{
				m_pWidget->_setSlot(nullptr);
				m_pWidget->_decRefCount();
			}

			m_pWidget = o.m_pWidget;
			//			m_pHolder = o.m_pHolder;

			if (m_pWidget)
			{
				m_pWidget->_setSlot(this);
				o.m_pWidget = nullptr;
			}

			return *this;
		}
	};


}

#endif //WG_SLOT_DOT_H
