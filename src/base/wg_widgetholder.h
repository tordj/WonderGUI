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


#ifndef WG_WIDGETHOLDER_DOT_H
#define WG_WIDGETHOLDER_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_geo.h>


namespace wg 
{
	class Container;
	class RootPanel;
	class Widget;
	class Slot;


	class WidgetHolder
	{
	public:
		virtual Coord		_childPos( Slot * pSlot ) const = 0;				///< Get the local position of the widget.
		virtual Coord		_childGlobalPos( Slot * pSlot ) const = 0;

		virtual Size		_childSize( Slot * pSlot ) const = 0;

		virtual bool		_isChildVisible( Slot * pSlot ) const = 0;
		virtual Rect		_childWindowSection( Slot * pSlot ) const = 0;		// Returns the window section within the childs canvas.

	
		virtual Container * _childParent() = 0;
		virtual RootPanel *	_root() = 0;


		virtual void		_childRequestRender( Slot * pSlot ) = 0;
		virtual void		_childRequestRender( Slot * pSlot, const Rect& rect ) = 0;
		virtual void		_childRequestResize( Slot * pSlot ) = 0;
	
		virtual bool		_childRequestFocus( Slot * pSlot, Widget * pWidget ) = 0;					// Request focus on behalf of me, child or grandchild.
		virtual bool		_childReleaseFocus( Slot * pSlot, Widget * pWidget ) = 0;
	
		virtual void		_childRequestInView( Slot * pSlot ) = 0;
		virtual void		_childRequestInView( Slot * pSlot, const Rect& mustHaveArea, const Rect& niceToHaveArea ) = 0;
	
		
		virtual Widget *	_prevChild( Slot * pSlot ) const = 0;
		virtual Widget *	_nextChild( Slot * pSlot ) const = 0;
	};
	

	
}


#endif //WG_WIDGETHOLDER_DOT_H
