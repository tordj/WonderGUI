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

#include <wg_widgetcontainer.h>
#include <wg_widget.h>

bool WgWidgetContainer::IsAncestorTo( const WgWidget * pWidget ) const
{
	while( pWidget )
	{
		WgWidgetContainer * pParent = pWidget->Parent();
		if( !pParent )
			return false;

		if( pParent == this )
			return true;

		pWidget = pParent->CastToWidget();
	}

	return false;
}


