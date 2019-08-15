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
	public:
		Slot() : pWidget(nullptr) {}

		const static bool safe_to_relocate = true;

		Slot(Slot&& o)
		{
			pWidget = o.pWidget;
			if (pWidget)
			{
				pWidget->_setHolder(pWidget->_holder(), this);
				o.pWidget = nullptr;
			}
		}

		// IMPORTANT! SlotArray assumes that Slot destructor doesn't need to be called
		// if content has been moved to another slot!

		~Slot()
		{
			if( pWidget != nullptr )
			{
				pWidget->_setHolder( nullptr, nullptr );
				pWidget->_decRefCount();
			}
		}

		Slot& operator=(Slot&& o)
		{
			if (pWidget)
			{
				pWidget->_setHolder(nullptr, nullptr);
				pWidget->_decRefCount();
			}

			pWidget =o.pWidget;

			if (pWidget)
			{
				pWidget->_setHolder(pWidget->_holder(), this);
				o.pWidget = nullptr;
			}

			return *this;
		}

		inline void relink() { pWidget->_setHolder( pWidget->_holder(), this ); }

		inline void replaceWidget( WidgetHolder * pHolder, Widget * pNewWidget )
		{
			if( pWidget )
			{
				if (pWidget == pNewWidget)
					return;

				pWidget->_setHolder( nullptr, nullptr );
				pWidget->_decRefCount();
			}

			pWidget = pNewWidget;

			if( pNewWidget )
			{
				pNewWidget->_incRefCount();
				pNewWidget->_setHolder( pHolder, this );
			}
		}

		inline SizeI	size() const { return pWidget->m_size; }
		inline void		setSize( SizeI size ) const { pWidget->_setSize(size); }
		
		inline int		matchingHeight(int width) const { return pWidget->_matchingHeight(width); }
		inline int		matchingWidth(int height) const { return pWidget->_matchingWidth(height); }

		inline SizeI	preferredSize() const { return pWidget->_preferredSize(); }
		inline SizeI	minSize() const { return pWidget->_minSize(); }
		inline SizeI	maxSize() const { return pWidget->_maxSize(); }

		inline bool		markTest(const CoordI& ofs) const { return pWidget->_markTest(ofs); };


		Widget *	pWidget;
	};

}

#endif //WG_SLOT_DOT_H
