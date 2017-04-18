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

#ifndef WG_ITEMHOLDER_DOT_H
#define WG_ITEMHOLDER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_geo.h>

namespace wg 
{
	class Item;
	class Object;

	class ItemHolder
	{
	public:
		virtual Object * _itemObject() = 0;
		virtual const Object * _itemObject() const = 0;

		virtual Coord	_itemPos( const Item * pItem ) const = 0;
		virtual Size	_itemSize( const Item * pItem ) const = 0;
		virtual Rect	_itemGeo( const Item * pItem ) const = 0;
		virtual Coord	_itemGlobalPos( const Item * pItem ) const = 0;
		virtual Rect	_itemGlobalGeo( const Item * pItem ) const = 0;

		virtual void	_itemRenderRequested( const Item * pItem ) = 0;
		virtual void	_itemRenderRequested( const Item * pItem, const Rect& rect ) = 0;
		virtual void	_itemResizeRequested( const Item * pItem ) = 0;

 		virtual void	_itemFocusRequested( const Item * pItem ) = 0;
		virtual void	_itemVisibilityRequested( const Item * pItem ) = 0;
		virtual void	_itemVisibilityRequested( const Item * pItem, const Rect& preferred, const Rect& prio ) = 0;



		virtual void	_itemNotified( Item * pItem, ItemNotif notification, void * pData ) = 0;

	};
	
} // namespace wg


#endif //WG_ITEMHOLDER_DOT_H
