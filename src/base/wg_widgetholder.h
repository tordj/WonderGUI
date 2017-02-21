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

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

namespace wg 
{
	class Container;
	class RootPanel;
	class Widget;

	struct ChildSlot
	{
		Widget *	pWidget;
	};


	class WidgetHolder
	{
	public:
		virtual Coord		_childPos( void * pChildRef ) const = 0;				///< Get the local position of the widget.
		virtual Coord		_childGlobalPos( void * pChildRef ) const = 0;

		virtual Size		_childSize( void * pChildRef ) const = 0;

		virtual bool		_isChildVisible( void * pChildRef ) const = 0;
		virtual Rect		_childWindowSection( void * pChildRef ) const = 0;		// Returns the window section within the childs canvas.

	
		virtual Container * _childParent() = 0;
		virtual RootPanel *	_root() = 0;


		virtual void		_childRequestRender( void * pChildRef ) = 0;
		virtual void		_childRequestRender( void * pChildRef, const Rect& rect ) = 0;
		virtual void		_childRequestResize( void * pChildRef ) = 0;
	
		virtual bool		_childRequestFocus( void * pChildRef, Widget * pWidget ) = 0;					// Request focus on behalf of me, child or grandchild.
		virtual bool		_childReleaseFocus( void * pChildRef, Widget * pWidget ) = 0;
	
		virtual void		_childRequestInView( void * pChildRef ) = 0;
		virtual void		_childRequestInView( void * pChildRef, const Rect& mustHaveArea, const Rect& niceToHaveArea ) = 0;
	
		
		virtual Widget *	_prevChild( void * pChildRef ) const = 0;
		virtual Widget *	_nextChild( void * pChildRef ) const = 0;

		virtual void		_replaceChild( ChildSlot * pSlot, Widget * pNewWidget ) {};		// Used by WidgetSlot interface.

	};
	

	
}


#endif //WG_WIDGETHOLDER_DOT_H