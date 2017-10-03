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

#include <wg_columnheaderitem.h>
#include <wg_msg.h>
#include <wg_inputhandler.h>

namespace wg 
{
	//____ Constructor ___________________________________________________________

	ColumnHeaderItem::ColumnHeaderItem(ItemHolder * pHolder) : Item(pHolder), icon(this), arrow(this), label(this)
	{
		m_bPressed = false;
		m_sortOrder = SortOrder::None;
	}
	
	//____ setSortOrder() ________________________________________________________

	void ColumnHeaderItem::setSortOrder( SortOrder order )
	{
		m_sortOrder = order;
		_requestRender();		//TODO: Only re-render arrow rectangle.
	}

	//____ setSkin() _____________________________________________________________
	
	void ColumnHeaderItem::setSkin( Skin * pSkin )
	{
		if( pSkin != m_pSkin )
		{
			m_pSkin = pSkin;
			_requestResize();
			_requestRender();		// Resize is not guaranteed to result in a re-render.
		}
	}

	//____ setState() ____________________________________________________________

	void ColumnHeaderItem::setState( State state )
	{
		m_state = state;

		//TODO: Set state for icon and arrow when they allow for it.

		label.setState(state);

		_requestRender();			//TODO: Only request render if state change requires it.
	}

	//____ setSize() ____________________________________________________________

	void ColumnHeaderItem::setSize( Size size )
	{
		if( m_size != size )
		{
			m_size = size;
			_requestRender();
		}
	}

	//____ preferredSize() _______________________________________________________

	Size ColumnHeaderItem::preferredSize() const
	{
		Size iconSize = icon.preferredSize();
		Size arrowSize = arrow.preferredSize();
		Size textSize = label.preferredSize();

		Size size;

		//TODO: Assumes icon/arrow origos to not be NORTH, SOUTH or CENTER.
		//TODO: Assumes text not wrapping.

		size.h = wg::max(iconSize.h, arrowSize.h, textSize.h );
		size.w = textSize.w;
		if( icon.overlap() )
			size.w = wg::max(size.w,iconSize.w);
		else
			size.w += iconSize.w;

		if( arrow.overlap() )
			size.w = wg::max(size.w,arrowSize.w);
		else
			size.w += arrowSize.w;

		//

		if( m_pSkin )
			size = m_pSkin->sizeForContent(size);

		return size;
	}


	//____ matchingWidth() ____________________________________________________

	int ColumnHeaderItem::matchingWidth( int height ) const
	{
		return preferredSize().w;
	}

	//____ matchingHeight() ___________________________________________________

	int ColumnHeaderItem::matchingHeight( int width ) const
	{
		return preferredSize().h; //TODO: Assumes text not wrapping.
	}

	//____ receive() _____________________________________________________________

	bool  ColumnHeaderItem::receive( Msg * _pMsg )
	{
		switch( _pMsg->type() )
		{
			case MsgType::MouseMove:
			{
				MouseMoveMsg_p pMsg = MouseMoveMsg::cast(_pMsg);
				Coord ofs = pMsg->pointerPos();
				Rect geo = _globalGeo();
				bool bHovered = geo.contains(ofs) && (!Base::inputHandler()->isAnyButtonPressed() || 
					(Base::inputHandler()->isButtonPressed(MouseButton::Left) && m_bPressed));
				if( bHovered != m_state.isHovered() )
				{
					m_state.setHovered(bHovered);
					_requestRender();
				}
				break;
			}

			case MsgType::MouseLeave:
			{
				MouseLeaveMsg_p pMsg = MouseLeaveMsg::cast(_pMsg);
				if( m_state.isHovered() )
				{
					m_state.setPressed(false);
					m_state.setHovered(false);
					_requestRender();
				}
				break;
			}

			case MsgType::MousePress:
			{
				MousePressMsg_p pMsg = MousePressMsg::cast(_pMsg);
				Coord ofs = pMsg->pointerPos();
				Rect geo = _globalGeo();
				if(pMsg->button() == MouseButton::Left && geo.contains(ofs))
				{
					m_bPressed = true;
					m_state.setPressed(true);
					_requestRender();
					pMsg->swallow();
					return true;
				}
				break;
			}

			case MsgType::MouseDrag:
			{
				MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);
				if( m_bPressed )
				{
					Coord ofs = pMsg->pointerPos();
					bool bHovered = _globalGeo().contains(ofs);
					if( bHovered != m_state.isHovered() )
					{
						m_state.setHovered(bHovered);
						m_state.setPressed(bHovered);
						_requestRender();
					}
					pMsg->swallow();
					return true;
				}
				break;
			}

			case MsgType::MouseRelease:
			{
				MouseReleaseMsg_p pMsg = MouseReleaseMsg::cast(_pMsg);
				if(pMsg->button() == MouseButton::Left && m_bPressed )
				{
					m_bPressed = false;
					m_state.setPressed(false);
					_requestRender();

					Coord ofs = pMsg->pointerPos();
					if( _globalGeo().contains(ofs) )
					{
						if( m_sortOrder == SortOrder::Ascending )
							m_sortOrder = SortOrder::Descending;
						else
							m_sortOrder = SortOrder::Ascending;
						_notify( ItemNotif::SortOrderChanged, nullptr );
					}
					pMsg->swallow();
					return true;
				}
				break;
			}
		}
		return false;
	}

	//____ render() ____________________________________________________________

	void ColumnHeaderItem::render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		Rect canvas( _canvas );
		Rect clip( _canvas, _clip );
	
		if( m_pSkin )
		{
			m_pSkin->render( pDevice, canvas, m_state, _clip );
			canvas = m_pSkin->contentRect( canvas, m_state );
		}
	
		Rect sortRect = arrow.getIconRect( canvas );
		Rect labelRect = arrow.getTextRect( canvas, sortRect );
		Rect iconRect = icon.getIconRect( labelRect );
		labelRect = icon.getTextRect( labelRect, iconRect );
	
		if( m_sortOrder != SortOrder::None && !arrow.isEmpty() )
		{
			State iconState = m_state;
			iconState.setSelected( m_sortOrder == SortOrder::Descending );
			arrow.skin()->render( pDevice, sortRect, iconState, _clip );
		}
	
		if( !icon.isEmpty() )
			icon.skin()->render( pDevice, iconRect, m_state, _clip );
	
		if( !label.isEmpty() )
			label.render( pDevice, labelRect, _clip );	
	}



	Object * ColumnHeaderItem::_itemObject()
	{
		return _object();
	}

	const Object * ColumnHeaderItem::_itemObject() const
	{
		return _object();
	}

	Coord ColumnHeaderItem::_itemPos( const Item * pItem ) const
	{
		Coord	p = _pos();
		if( m_pSkin )
			p += m_pSkin->contentOfs(m_state);
		return p;
	}

	Size ColumnHeaderItem::_itemSize( const Item * pItem ) const
	{
		Size	s  = m_size;
		if( m_pSkin )
			s -= m_pSkin->contentPadding();
		return s;
	}

	Rect ColumnHeaderItem::_itemGeo( const Item * pItem ) const
	{
		if( m_pSkin )
			return m_pSkin->contentRect( m_size, m_state );
		else
			return Rect( 0,0,m_size );			
	}

	Coord ColumnHeaderItem::_itemGlobalPos( const Item * pItem ) const
	{
		Coord	p = _globalPos();
		if( m_pSkin )
			p += m_pSkin->contentOfs(m_state);
		return p;
	}

	Rect ColumnHeaderItem::_itemGlobalGeo( const Item * pItem ) const
	{
		Rect	geo = _globalGeo();
		if( m_pSkin )
			geo = m_pSkin->contentRect(geo, m_state);
		return geo;
	}

	void ColumnHeaderItem::_itemRenderRequested( const Item * pItem )
	{
		return _requestRender();		//TODO: Only request render on what is needed
	}

	void ColumnHeaderItem::_itemRenderRequested( const Item * pItem, const Rect& rect )
	{
		return _requestRender(rect);	//TODO: Only request render on what is needed
	}

	void ColumnHeaderItem::_itemResizeRequested( const Item * pItem )
	{
		return _requestResize();
	}

	void ColumnHeaderItem::_itemFocusRequested( const Item * pItem )
	{
		// Do nothing, our subitems are not expected to request or get focus.
	}

	void ColumnHeaderItem::_itemVisibilityRequested( const Item * pItem )
	{
		// Do nothing, our subitems are not expected to request or get visibility.
	}
	void ColumnHeaderItem::_itemVisibilityRequested( const Item * pItem, const Rect& preferred, const Rect& prio )
	{
		// Do nothing, our subitems are not expected to request or get visibility.
	}

	void ColumnHeaderItem::_itemNotified( Item * pItem, ItemNotif notification, void * pData )
	{
		// Do nothing, our subitems are not expected to send notifications.
	}



} // namespace wg
