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


namespace wg
{
	class Container;
	class RootPanel;
	class Widget;
	class BasicSlot;
	class Object;


	//____ SlotHolder ______________________________________________________________

	class SlotHolder		/** @private */
	{
	public:
		virtual Container * _container() = 0;
		virtual RootPanel *	_root() = 0;
		virtual Object *	_object() = 0;
		virtual const Object *	_object() const = 0;

		virtual CoordI		_childPos( const BasicSlot * pSlot ) const = 0;				///< Get the local position of the widget.
		virtual CoordI		_childGlobalPos( const BasicSlot * pSlot ) const = 0;

		virtual bool		_isChildVisible( const BasicSlot * pSlot ) const = 0;
		virtual RectI		_childWindowSection( const BasicSlot * pSlot ) const = 0;			// Returns the window section within the childs canvas.

		virtual void		_childRequestRender( BasicSlot * pSlot ) = 0;
		virtual void		_childRequestRender( BasicSlot * pSlot, const RectI& rect ) = 0;
		virtual void		_childRequestResize( BasicSlot * pSlot ) = 0;

		virtual bool		_childRequestFocus( BasicSlot * pSlot, Widget * pWidget ) = 0;					// Request focus on behalf of me, child or grandchild.
		virtual bool		_childReleaseFocus( BasicSlot * pSlot, Widget * pWidget ) = 0;

		virtual void		_childRequestInView( BasicSlot * pSlot ) = 0;
		virtual void		_childRequestInView( BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea ) = 0;

		virtual Widget *	_prevChild( const BasicSlot * pSlot ) const = 0;
		virtual Widget *	_nextChild( const BasicSlot * pSlot ) const = 0;

		virtual void		_releaseChild(BasicSlot * pSlot) = 0;
		virtual void		_replaceChild(BasicSlot * pSlot, Widget * pNewChild) = 0;
	};

} // namespace wg
#endif //WG_WIDGETHOLDER_DOT_H
