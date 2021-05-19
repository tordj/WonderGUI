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

#include <wg_ccolumnheader.h>
#include <wg_msg.h>
#include <wg_inputhandler.h>
#include <wg_internal.h>

namespace wg
{
	const TypeInfo CColumnHeader::TYPEINFO = { "CColumnHeader", &GeoComponent::TYPEINFO };


	//____ constructor ___________________________________________________________

	CColumnHeader::CColumnHeader(GeoComponent::Holder * pHolder) : GeoComponent(pHolder), icon(this), arrow(this), text(this)
	{
		m_bPressed = false;
		m_sortOrder = SortOrder::None;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CColumnHeader::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSkin() _____________________________________________________________

	void CColumnHeader::setSkin( Skin * pSkin )
	{
		if( pSkin != m_pSkin )
		{
			m_pSkin = pSkin;
			_requestResize();
			_requestRender();		// Resize is not guaranteed to result in a re-render.
		}
	}

	//____ _setSortOrder() ________________________________________________________

	void CColumnHeader::_setSortOrder(SortOrder order)
	{
		m_sortOrder = order;
		_requestRender();		//TODO: Only re-render arrow rectangle.
	}

	//____ _setState() ____________________________________________________________

	void CColumnHeader::_setState( State state )
	{
		m_state = state;

		//TODO: Set state for icon and arrow when they allow for it.

		OO(text)._setState(state);

		_requestRender();			//TODO: Only request render if state change requires it.
	}

	//____ _setSize() ____________________________________________________________

	void CColumnHeader::_setSize( SizeSPX size, int scale )
	{
		if( m_size != size || m_scale != scale )
		{
			m_size = size;
			m_scale = scale;
			_requestRender();
		}
	}

	//____ _preferredSize() _______________________________________________________

	SizeSPX CColumnHeader::_preferredSize(int scale) const
	{
		SizeSPX iconSize = _icon()._preferredSize(scale);
		SizeSPX arrowSize = _arrow()._preferredSize(scale);
		SizeSPX textSize = OO(text)._preferredSize(scale);

		SizeSPX size;

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
			size = m_pSkin->_sizeForContent(size,scale);

		return size;
	}


	//____ _matchingWidth() ____________________________________________________

	spx CColumnHeader::_matchingWidth( spx height, int scale ) const
	{
		return _preferredSize(scale).w;
	}

	//____ _matchingHeight() ___________________________________________________

	spx CColumnHeader::_matchingHeight( spx width, int scale ) const
	{
		return _preferredSize(scale).h; //TODO: Assumes text not wrapping.
	}

	//____ _receive() _____________________________________________________________

	bool  CColumnHeader::_receive( Msg * _pMsg )
	{
		switch( _pMsg->type() )
		{
			case MsgType::MouseMove:
			{
				MouseMoveMsg_p pMsg = static_cast<MouseMoveMsg*>(_pMsg);
				CoordSPX ofs = pMsg->_pointerPos();
				RectSPX geo = _globalGeo();
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
				MouseLeaveMsg_p pMsg = static_cast<MouseLeaveMsg*>(_pMsg);
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
				MousePressMsg_p pMsg = static_cast<MousePressMsg*>(_pMsg);
				CoordSPX ofs = pMsg->_pointerPos();
				RectSPX geo = _globalGeo();
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
				MouseDragMsg_p pMsg = static_cast<MouseDragMsg*>(_pMsg);
				if( m_bPressed )
				{
					CoordSPX ofs = pMsg->_pointerPos();
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
				MouseReleaseMsg_p pMsg = static_cast<MouseReleaseMsg*>(_pMsg);
				if(pMsg->button() == MouseButton::Left && m_bPressed )
				{
					m_bPressed = false;
					m_state.setPressed(false);
					_requestRender();

					CoordSPX ofs = pMsg->_pointerPos();
					if( _globalGeo().contains(ofs) )
					{
						if( m_sortOrder == SortOrder::Ascending )
							m_sortOrder = SortOrder::Descending;
						else
							m_sortOrder = SortOrder::Ascending;
						_notify( ComponentNotif::SortOrderChanged, 0, nullptr );
					}
					pMsg->swallow();
					return true;
				}
				break;
			}
			default:
				break;
		}
		return false;
	}

	//____ _render() ____________________________________________________________

	void CColumnHeader::_render( GfxDevice * pDevice, const RectSPX& _canvas )
	{
		RectSPX canvas( _canvas );

		if( m_pSkin )
		{
			m_pSkin->_render( pDevice, canvas, m_scale, m_state );
			canvas = m_pSkin->_contentRect( canvas, m_scale, m_state );
		}

		RectSPX sortRect = _arrow()._getIconRect( canvas, m_scale );
		RectSPX labelRect = _arrow()._getTextRect( canvas, sortRect, m_scale );
		RectSPX iconRect = _icon()._getIconRect( labelRect, m_scale );
		labelRect = _icon()._getTextRect( labelRect, iconRect, m_scale );

		if( m_sortOrder != SortOrder::None && !arrow.isEmpty() )
		{
			State iconState = m_state;
			iconState.setSelected( m_sortOrder == SortOrder::Descending );
			arrow.skin()->_render( pDevice, sortRect, m_scale, iconState );
		}

		if( !icon.isEmpty() )
			icon.skin()->_render( pDevice, iconRect, m_scale, m_state );

		if( !text.isEmpty() )
			OO(text)._render( pDevice, labelRect );
	}


	CoordSPX CColumnHeader::_componentPos( const GeoComponent * pComponent ) const
	{
		CoordSPX	p = _pos();
		if( m_pSkin )
			p += m_pSkin->_contentOfs(m_scale, m_state);
		return p;
	}

	SizeSPX CColumnHeader::_componentSize( const GeoComponent * pComponent ) const
	{
		SizeSPX	s  = m_size;
		if( m_pSkin )
			s -= m_pSkin->_contentPaddingSize(m_scale);
		return s;
	}

	RectSPX CColumnHeader::_componentGeo( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_pSkin->_contentRect( m_size, m_scale, m_state );
		else
			return RectSPX( m_size );
	}

	CoordSPX CColumnHeader::_globalComponentPos( const GeoComponent * pComponent ) const
	{
		CoordSPX	p = _globalPos();
		if( m_pSkin )
			p += m_pSkin->_contentOfs(m_scale, m_state);
		return p;
	}

	RectSPX CColumnHeader::_globalComponentGeo( const GeoComponent * pComponent ) const
	{
		RectSPX	geo = _globalGeo();
		if( m_pSkin )
			geo = m_pSkin->_contentRect(geo, m_scale, m_state);
		return geo;
	}

	void CColumnHeader::_componentRequestRender( const GeoComponent * pComponent )
	{
		return _requestRender();		//TODO: Only request render on what is needed
	}

	void CColumnHeader::_componentRequestRender( const GeoComponent * pComponent, const RectSPX& rect )
	{
		return _requestRender(rect);	//TODO: Only request render on what is needed
	}

	void CColumnHeader::_componentRequestResize( const GeoComponent * pComponent )
	{
		return _requestResize();
	}

	void CColumnHeader::_componentRequestFocus( const GeoComponent * pComponent )
	{
		// Do nothing, our sub components are not expected to request or get focus.
	}

	void CColumnHeader::_componentRequestInView( const GeoComponent * pComponent )
	{
		// Do nothing, our sub components are not expected to request or get visibility.
	}
	void CColumnHeader::_componentRequestInView( const GeoComponent * pComponent, const RectSPX& mustHave, const RectSPX& niceToHave )
	{
		// Do nothing, our sub components are not expected to request or get visibility.
	}

	void CColumnHeader::_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData )
	{
		// Do nothing, our sub components are not expected to send notifications.
	}



} // namespace wg
