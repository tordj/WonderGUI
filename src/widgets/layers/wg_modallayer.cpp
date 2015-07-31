/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_modallayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char WgModalLayer::CLASSNAME[] = {"ModalLayer"};
	const char WgModalHook::CLASSNAME[] = {"ModalHook"};
	
	
	//TODO: Improve WgModalHook geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.
	
	
	//____ WgModalHook::isInstanceOf() __________________________________________
	
	bool WgModalHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgLayerHook::isInstanceOf(pClassName);
	}
	
	//____ WgModalHook::className() _____________________________________________
	
	const char * WgModalHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ WgModalHook::cast() __________________________________________________
	
	WgModalHook_p WgModalHook::cast( const WgHook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return WgModalHook_p( static_cast<WgModalHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//_____________________________________________________________________________
	void WgModalHook::top()
	{
		_moveLast();
		_requestRender();
		m_pParent->_updateKeyboardFocus();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setGeo( const WgRect& geometry, WgOrigo origo )
	{
		m_placementGeo 	= geometry;
		m_origo 		= origo;
	
		if( m_placementGeo.w <= 0 )
			m_placementGeo.w = 1;
	
		if( m_placementGeo.h <= 0 )
			m_placementGeo.h = 1;
	
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setGeo( const WgCoord& ofs, WgOrigo origo )
	{
		m_placementGeo.setPos(ofs);
		m_origo	= origo;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setOfs( const WgCoord& ofs )
	{
		m_placementGeo.setPos(ofs);
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setOfsX( int x )
	{
		m_placementGeo.x = x;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setOfsY( int y )
	{
		m_placementGeo.y = y;
		return _refreshRealGeo();
	}
	
	
	//_____________________________________________________________________________
	bool WgModalHook::setSize( WgSize sz )
	{
		if( sz.w < 0 || sz.h < 0 )
			return false;
	
		m_placementGeo.setSize( sz );
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setWidth( int width )
	{
		if( width < 0 )
			return false;
	
		m_placementGeo.w = width;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::setHeight( int height )
	{
		if( height < 0 )
			return false;
	
		m_placementGeo.h = height;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::move( const WgCoord& ofs )
	{
		m_placementGeo += ofs;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::moveX( int x )
	{
		m_placementGeo.x += x;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::moveY( int y )
	{
		m_placementGeo.y += y;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	WgModalLayer_p WgModalHook::parent() const
	{
		return m_pParent;
	}
	
	//_____________________________________________________________________________
	WgModalHook::WgModalHook( WgModalLayer * pParent )
	{
		m_pParent = pParent;
	}
	
	//_____________________________________________________________________________
	bool WgModalHook::_refreshRealGeo()	// Return false if we couldn't get exactly the requested (floating) geometry.
	{
		WgSize sz = m_placementGeo.size();
	
		if( sz.w == 0 && sz.h == 0 )
			sz = m_pWidget->preferredSize();
		else if( sz.w == 0 )
			sz.w = m_pWidget->matchingWidth(sz.h);
		else if( sz.h == 0 )
			sz.h = m_pWidget->matchingHeight(sz.w);
	
		if( sz.w <= 0 )
			sz.w = 1;
		if( sz.h <= 0 )
			sz.h = 1;
	
		WgCoord ofs = WgUtil::origoToOfs( m_origo, m_pParent->size() ) - WgUtil::origoToOfs( m_origo, sz );
		ofs += m_placementGeo.pos();
	
		WgRect newGeo( ofs, sz );
	
		if( newGeo != m_geo )
		{
			_requestRender();
			m_geo = WgRect( ofs, sz );
			_requestRender();
		}
	
		return true;
	}
	
	//_____________________________________________________________________________
	void WgModalHook::_requestResize()
	{
		_refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	WgLayerHook * WgModalHook::_prevLayerHook() const
	{
		WgModalHook * p = _prev();
	
		// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.
	
		if( p )
			return p;
		else
			return 0;
	}
	
	//_____________________________________________________________________________
	WgLayerHook * WgModalHook::_nextLayerHook() const
	{
		WgModalHook * p = _next();
	
		// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.
	
		if( p )
			return p;
		else
			return 0;
	}
	
	//_____________________________________________________________________________
	WgContainer * WgModalHook::_parent() const
	{
		return m_pParent;
	}
	
	//____ Constructor ____________________________________________________________
	
	WgModalLayer::WgModalLayer()
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	WgModalLayer::~WgModalLayer()
	{
		// Children are deleted automaticallly when their hooks are deteled.
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgModalLayer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgLayer::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgModalLayer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgModalLayer_p WgModalLayer::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgModalLayer_p( static_cast<WgModalLayer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ addModalWidget() ________________________________________________________
	
	WgModalHook_p WgModalLayer::addModalWidget( const WgWidget_p& pWidget, const WgRect& geometry, WgOrigo origo )
	{
		// Create Hook and fill in members.
	
		WgModalHook * pHook = new WgModalHook( this );
		pHook->_setWidget(pWidget.rawPtr());
		pHook->m_origo = origo;
		pHook->m_placementGeo = geometry;
		m_modalHooks.pushBack(pHook);
	
		// Refresh geometry and request render.
	
		pHook->_refreshRealGeo();
		_updateKeyboardFocus();
		return pHook;
	}
	
	//____ removeModalWidgets() ________________________________________________
	
	bool WgModalLayer::removeModalWidgets()
	{
		m_modalHooks.clear();
		_requestRender();
		_updateKeyboardFocus();
		return true;
	}
	
	//____ removeWidget() _________________________________________________________
	
	bool WgModalLayer::removeWidget( const WgWidget_p& pWidget )
	{
		if( !pWidget || pWidget->parent() != this )
			return false;
	
		if( pWidget == m_baseHook._widget() )
			return removeBaseWidget();
		else
		{
			WgModalHook * pHook = (WgModalHook *) pWidget->_hook();
			pHook->_requestRender();
			delete pHook;
			_updateKeyboardFocus();
			return true;
		}
	
	}
	
	//____ clear() ________________________________________________________________
	
	bool WgModalLayer::clear()
	{
		removeBaseWidget();
		removeModalWidgets();
		return true;
	}
	
	//____ firstModalHook() ______________________________________________________
	
	WgModalHook_p WgModalLayer::firstModalHook()
	{
		return m_modalHooks.first();
	}
	
	//____ lastModalHook() _______________________________________________________
	
	WgModalHook_p WgModalLayer::lastModalHook()
	{
		return m_modalHooks.last();
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int WgModalLayer::matchingHeight( int width ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingHeight( width );
		else
			return WgWidget::matchingHeight(width);
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int WgModalLayer::matchingWidth( int height ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingWidth( height );
		else
			return WgWidget::matchingWidth(height);
	}
	
	//____ preferredSize() _____________________________________________________________
	
	WgSize WgModalLayer::preferredSize() const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->preferredSize();
		else
			return WgSize(1,1);
	}
	
	//____ _findWidget() ____________________________________________________________
	
	WgWidget *  WgModalLayer::_findWidget( const WgCoord& ofs, WgSearchMode mode )
	{
		// In search mode ACTION_TARGET we always return either the topmost non-hidden modal Widget (or its children),
		// or us.
	
		if( mode == WG_SEARCH_ACTION_TARGET )
		{
			WgModalHook * pHook = m_modalHooks.last();
	
			if( pHook )
			{
				if( pHook->_widget()->IsContainer() )
				{
					WgWidget * pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - pHook->pos(), mode );
					if( pResult )
						return pResult;
				}
				else
				{
					if( pHook->_widget()->markTest(ofs - pHook->pos()) )
						return pHook->_widget();
					else
						return this;
				}
			}
			else if( m_baseHook._widget() )
			{
				if( m_baseHook._widget()->IsContainer() )
				{
					WgWidget * pResult = static_cast<WgContainer*>(m_baseHook._widget())->_findWidget( ofs - m_baseHook.pos(), mode );
					if( pResult )
						return pResult;
				}
				else
					return m_baseHook._widget();
			}
			else
				return 0;
		}
	
		// For the rest of the modes we can rely on the default method.
	
		return WgContainer::_findWidget( ofs, mode );
	}
	
	//____ _updateKeyboardFocus() _______________________________________________________
	
	void WgModalLayer::_updateKeyboardFocus()
	{
		// Get message handler, verify that we have a root
	
		if( !hook() )
			return;
	
		WgMsgRouter * pHandler = hook()->msgRouter().rawPtr();
		if( !pHandler )
			return;
	
		// Retrieve focused Widget and verify it being a descendant to us.
	
		WgWidget * pFocused = pHandler->keyboardFocus().rawPtr();
	
		WgWidget * p = pFocused;
		while( p && p->parent() && p->parent() != this )
			p = p->_parent();
	
		if( p && p->_parent() != this )
			return;								// Focus belongs to a Widget that is not a descendant to us,
												// so we can't save and shouldn't steal focus.
	
		// Save old focus so we can return it properly in the future.
		if( p )
		{
			if( p == m_baseHook._widget() )
				m_pBaseKeyFocus = pFocused;
			else
			{
				WgModalHook * pHook = static_cast<WgModalHook*>(p->_hook());
				pHook->m_pKeyFocus = pFocused;
			}
		}
	
		// Find which child-branch to focus and switch to our previously saved focus
	
		WgModalHook * pHook = m_modalHooks.last();
	
		WgWidget * 	pSavedFocus = 0;
		WgHook *	pBranch	= 0;
	
		if( pHook )
		{
			pSavedFocus = pHook->m_pKeyFocus.rawPtr();
			pHook->m_pKeyFocus = 0;								// Needs to be cleared for the future.
			pBranch = pHook;
		}
		else if( m_baseHook._widget() )
		{
			pSavedFocus = m_pBaseKeyFocus.rawPtr();
			m_pBaseKeyFocus = 0;								// Needs to be cleared for the future.
			pBranch = &m_baseHook;
		}
	
		// Verify that saved focus still is within branch and is not hidden
	
		if( pSavedFocus )
		{
			WgHook * p = pSavedFocus->_hook();
			while( p && p != pBranch )
			{
				WgContainer * pHolder = p->_parent();
				if( pHolder )
					p = pHolder->_hook();
				else
					p = 0;
			}
	
			if( p != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}
	
		// Switch to previously saved focus, or null if not applicable
	
		pHandler->setKeyboardFocus( pSavedFocus );
	}
	
	//____ _onNewSize() ___________________________________________________________
	
	void WgModalLayer::_onNewSize( const WgSize& sz )
	{
		m_size = sz;
	
		// Update size of base widget
	
		if( m_baseHook._widget() )
			m_baseHook._widget()->_onNewSize(sz);
	
		// Refresh modal widgets geometry, their positions might have changed.
	
		WgModalHook * pHook = m_modalHooks.first();
	
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->_next();
		}
	}
	
	//____ _onCloneContent() ______________________________________________________
	
	void WgModalLayer::_onCloneContent( const WgWidget * _pOrg )
	{
	}
	
	//____ _onMsg() ______________________________________________________________
	
	void WgModalLayer::_onMsg( const WgMsg_p& _pMsg )
	{
		WgLayer::_onMsg(_pMsg);
	
		if( !m_modalHooks.isEmpty() && _findWidget( _pMsg->pointerPos(), WG_SEARCH_ACTION_TARGET ) == this )
		{
			switch( _pMsg->type() )
			{
				case WG_MSG_MOUSE_PRESS:
				{
					WgMouseButtonMsg_p pMsg = WgMouseButtonMsg::cast(_pMsg);
					WgBase::msgRouter()->post( new WgModalBlockedPressMsg( pMsg->button(), this) );
				}
				break;
	
				case WG_MSG_MOUSE_RELEASE:
				{
					WgMouseButtonMsg_p pMsg = WgMouseButtonMsg::cast(_pMsg);
					WgBase::msgRouter()->post( new WgModalBlockedPressMsg( pMsg->button(), this) );
				}
				break;
	
				case WG_MSG_MOUSE_MOVE:
				{
					WgBase::msgRouter()->post( new WgModalMoveOutsideMsg(this) );
				}
				break;
			}
		}	
	}
	

} // namespace wg
