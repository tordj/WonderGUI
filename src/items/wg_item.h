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

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif


#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif


namespace wg 
{
		
	//____ Item _________________________________________________________________
	
	class Item
	{
	public:
		Item( Widget * pWidget ) : m_pWidget(pWidget) {}

		inline Coord	_pos() const  { return m_pWidget->_itemPos(this); }
		inline Size		_size() const { return m_pWidget->_itemSize(this); }
		inline Rect		_geo() const { return m_pWidget->_itemGeo(this); }
		inline Coord	_globalPos() const { return m_pWidget->_itemGlobalPos(this); }
		inline Rect		_globalGeo() const { return m_pWidget->_itemGlobalGeo(this); }

	
		inline void		_requestRender() const { m_pWidget->_itemRenderRequested(this); }
		inline void		_requestRender( const Rect& rect ) const { m_pWidget->_itemRenderRequested(this, rect); }
		inline void		_requestResize() const { m_pWidget->_itemResizeRequested(this); }
		inline void		_requestVisibility() const { m_pWidget->_itemVisibilityRequested( this ); }
		inline void		_requestVisibility( const Rect& preferred, const Rect& prio ) const { m_pWidget->_itemVisibilityRequested( this, preferred, prio ); }
		
		inline void		_notify( ItemNotif notification, void * pData ) { m_pWidget->_onItemNotify( this, notification, pData ); }

		inline Object *	_object() { return m_pWidget; } 
		
	protected:
	
		Widget * m_pWidget;
	};
	
	
	

} // namespace wg
#endif //WG_ITEM_DOT_H
