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

#ifndef	WG_ITEM_DOT_H
#define	WG_ITEM_DOT_H
#pragma once

#include <wg_itemholder.h>


#include <wg_geo.h>


namespace wg 
{
		
	//____ Item _________________________________________________________________

	class Item		/** @private */
	{
	public:
		Item( ItemHolder * pHolder ) : m_pHolder(pHolder) {}

		inline Coord	_pos() const  { return m_pHolder->_itemPos(this); }
		inline Size		_size() const { return m_pHolder->_itemSize(this); }
		inline Rect		_geo() const { return m_pHolder->_itemGeo(this); }
		inline Coord	_globalPos() const { return m_pHolder->_itemGlobalPos(this); }
		inline Rect		_globalGeo() const { return m_pHolder->_itemGlobalGeo(this); }

	
		inline void		_requestRender() const { m_pHolder->_itemRenderRequested(this); }
		inline void		_requestRender( const Rect& rect ) const { m_pHolder->_itemRenderRequested(this, rect); }
		inline void		_requestResize() const { m_pHolder->_itemResizeRequested(this); }
		inline void		_requestVisibility() const { m_pHolder->_itemVisibilityRequested( this ); }
		inline void		_requestVisibility( const Rect& preferred, const Rect& prio ) const { m_pHolder->_itemVisibilityRequested( this, preferred, prio ); }
		inline void		_requestFocus() const { m_pHolder->_itemFocusRequested(this); }

		inline void		_notify( ItemNotif notification, void * pData ) { m_pHolder->_itemNotified( this, notification, pData ); }

		inline Object *	_object() { return m_pHolder->_itemObject(); } 
		inline const Object *	_object() const { return m_pHolder->_itemObject(); } 

	protected:
	
		ItemHolder * m_pHolder;
	};
	
	
	

} // namespace wg
#endif //WG_ITEM_DOT_H
