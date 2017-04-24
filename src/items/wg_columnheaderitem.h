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
#ifndef WG_COLUMNHEADERITEM_DOT_H
#define WG_COLUMNHEADERITEM_DOT_H
#pragma once

#include <wg_iconitem.h>
#include <wg_textitem.h>

namespace wg 
{
	//____ ColumnHeaderItem ___________________________________________________

	class ColumnHeaderItem : public Item, protected ItemHolder   /** @private */
	{
	public:
		ColumnHeaderItem(ItemHolder * pHolder);
	
		//____ Subitems __________________________________
	
		IconItem		icon;
		IconItem		arrow;
		TextItem		label;
	
		//____ Methods _____________________________________
	
		void			setSortOrder( SortOrder order );
		inline SortOrder sortOrder() const { return m_sortOrder; }

		void			setSkin( const Skin_p& pSkin );
		inline Skin_p	skin() const { return m_pSkin; }
	
		void			setState( State state );
		inline State	state() const { return m_state; }

		void			setSize( Size size );	
		inline Size		size() const { return m_size; }

		Size			preferredSize() const;	
		int				matchingWidth( int height ) const;
		int				matchingHeight( int width ) const;

		bool			receive( const Msg_p& pMsg );

		void			render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip );


	protected:
		
		//TODO: Implement proper solution!!!
		// Subitems are currently bypassing ColumnHeaderItem and making direct calls to widget
		// How do we solve this?

		Object * _itemObject();
		const Object * _itemObject() const;

		Coord	_itemPos( const Item * pItem ) const;
		Size	_itemSize( const Item * pItem ) const;
		Rect	_itemGeo( const Item * pItem ) const;
		Coord	_itemGlobalPos( const Item * pItem ) const;
		Rect	_itemGlobalGeo( const Item * pItem ) const;

		void	_itemRenderRequested( const Item * pItem );
		void	_itemRenderRequested( const Item * pItem, const Rect& rect );
		void	_itemResizeRequested( const Item * pItem );

		void	_itemFocusRequested( const Item * pItem );
		void	_itemVisibilityRequested( const Item * pItem );
		void	_itemVisibilityRequested( const Item * pItem, const Rect& preferred, const Rect& prio );

		void	_itemNotified( Item * pItem, ItemNotif notification, void * pData );


	
		Skin_p			m_pSkin;

		State			m_state;
		Size			m_size;

		bool			m_bPressed;
		SortOrder		m_sortOrder;
	};
	
	

} // namespace wg
#endif //WG_COLUMNHEADERITEM_DOT_H
