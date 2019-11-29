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

	//____ BasicSlot __________________________________________________________

	class BasicSlot		/** @private */
	{
		friend class Widget;
		friend class ISlot;
		friend class SlotIterator;
		template<class S> friend class ISlotArray;
		template<class S> friend class SlotArray;
		template<class S> friend class ISelectableSlotArray;

	public:
		const static bool safe_to_relocate = true;

		BasicSlot(WidgetHolder * pHolder) : m_pHolder(pHolder) {}

		BasicSlot(BasicSlot&& o)
		{
			m_pWidget = o.m_pWidget;
			m_pHolder = o.m_pHolder;
			if (m_pWidget)
			{
				m_pWidget->_setSlot(this);
				o.m_pWidget = nullptr;
			}
		}

		// IMPORTANT! SlotArray assumes that Slot destructor doesn't need to be called
		// if content has been moved to another slot!

		~BasicSlot()
		{
			if( m_pWidget != nullptr )
			{
				m_pWidget->_setSlot( nullptr );
				m_pWidget->_decRefCount();
			}
		}



		//.____ Operators __________________________________________

		BasicSlot& operator=(BasicSlot&& o)
		{
			if (m_pWidget)
			{
				m_pWidget->_setSlot(nullptr);
				m_pWidget->_decRefCount();
			}

			m_pWidget = o.m_pWidget;
			m_pHolder = o.m_pHolder;

			if (m_pWidget)
			{
				m_pWidget->_setSlot(this);
				o.m_pWidget = nullptr;
			}

			return *this;
		}

//		inline ISlot operator=(ISlot& iSlot) { Widget_p pWidget = iSlot.m_pSlot->_widget(); if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }

//		inline ISlot operator=(Widget * pWidget) { if (pWidget) pWidget->releaseFromParent();  m_pHolder->_setWidget(m_pSlot, pWidget); return *this; }
		inline operator Widget_p() const { return widget(); }

		inline bool operator==(Widget * other) const { return other == _widget(); }
		inline bool operator!=(Widget * other) const { return other != _widget(); }

		inline operator bool() const { return _widget() != nullptr; }

		inline Widget* operator->() const { return _widget(); }


		//.____ Content _______________________________________________________

		Widget_p widget() const { return Widget_p(m_pWidget); }


	protected:

		inline void relink() { m_pWidget->_setSlot( this ); }

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

		inline Widget * _widget() const { return m_pWidget; }
		inline WidgetHolder * _holder() const { return m_pHolder; }

		inline SizeI	_size() const { return m_pWidget->m_size; }
		inline void		_setSize( SizeI size ) const { m_pWidget->_resize(size); }

		inline int		_matchingHeight(int width) const { return m_pWidget->_matchingHeight(width); }
		inline int		_matchingWidth(int height) const { return m_pWidget->_matchingWidth(height); }

		inline SizeI	_preferredSize() const { return m_pWidget->_preferredSize(); }
		inline SizeI	_minSize() const { return m_pWidget->_minSize(); }
		inline SizeI	_maxSize() const { return m_pWidget->_maxSize(); }

		inline bool		_markTest(const CoordI& ofs) const { return m_pWidget->_markTest(ofs); };


		Widget *		m_pWidget = nullptr;
		WidgetHolder *	m_pHolder;
	};

}

#endif //WG_SLOT_DOT_H
