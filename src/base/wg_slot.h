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

	class Slot		/** @private */
	{
	friend class ISlot;
	friend class SlotIterator;
	template<class S> friend class ISlotArray;
	template<class S> friend class SlotArray;
	template<class S> friend class ISelectableSlotArray;

	public:

		Widget_p widget() const { return Widget_p(m_pWidget); }


		Slot() {}

		const static bool safe_to_relocate = true;

		Slot(Slot&& o)
		{
			m_pWidget = o.m_pWidget;
			m_pHolder = o.m_pHolder;
			if (m_pWidget)
			{
				m_pWidget->_setHolder(m_pWidget->_holder(), this);
				o.m_pWidget = nullptr;
			}
		}

		// IMPORTANT! SlotArray assumes that Slot destructor doesn't need to be called
		// if content has been moved to another slot!

		~Slot()
		{
			if( m_pWidget != nullptr )
			{
				m_pWidget->_setHolder( nullptr, nullptr );
				m_pWidget->_decRefCount();
			}
		}


		Slot& operator=(Slot&& o)
		{
			if (m_pWidget)
			{
				m_pWidget->_setHolder(nullptr, nullptr);
				m_pWidget->_decRefCount();
			}

			m_pWidget =o.m_pWidget;
			m_pHolder = o.m_pHolder;

			if (m_pWidget)
			{
				m_pWidget->_setHolder(m_pWidget->_holder(), this);
				o.m_pWidget = nullptr;
			}

			return *this;
		}

	protected:

		inline void relink() { m_pWidget->_setHolder( m_pHolder, this ); }

		inline void replaceWidget( WidgetHolder * pHolder, Widget * pWidget )
		{
			if( m_pWidget )
			{
				if (m_pWidget == pWidget)
					return;

				m_pWidget->_setHolder( nullptr, nullptr );
				m_pWidget->_decRefCount();
			}

			m_pWidget = pWidget;
			m_pHolder = pHolder;

			if( pWidget )
			{
				pWidget->_incRefCount();
				pWidget->_setHolder( pHolder, this );
			}
		}

		inline Widget * _widget() const { return m_pWidget; }

		inline SizeI	size() const { return m_pWidget->m_size; }
		inline void		setSize( SizeI size ) const { m_pWidget->_resize(size); }

		inline int		matchingHeight(int width) const { return m_pWidget->_matchingHeight(width); }
		inline int		matchingWidth(int height) const { return m_pWidget->_matchingWidth(height); }

		inline SizeI	preferredSize() const { return m_pWidget->_preferredSize(); }
		inline SizeI	minSize() const { return m_pWidget->_minSize(); }
		inline SizeI	maxSize() const { return m_pWidget->_maxSize(); }

		inline bool		markTest(const CoordI& ofs) const { return m_pWidget->_markTest(ofs); };


		Widget *		m_pWidget = nullptr;
		WidgetHolder *	m_pHolder = nullptr;
	};

}

#endif //WG_SLOT_DOT_H
