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

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

namespace wg 
{

	class Slot
	{
	public:
		Slot() : pWidget(nullptr) {}
		~Slot() { if( pWidget != nullptr ) { pWidget->_setHolder( nullptr, nullptr ); pWidget->_decRefCount(); } }

		inline void relink() { pWidget->_setHolder( pWidget->_holder(), this ); }

		inline void replaceWidget( WidgetHolder * pHolder, Widget * pNewWidget )
		{
			if( pWidget )
			{
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
			
			


		Widget *	pWidget;
	};

}

#endif //WG_SLOT_DOT_H