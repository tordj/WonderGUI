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
	
	const char ModalLayer::CLASSNAME[] = {"ModalLayer"};
	const char ModalHook::CLASSNAME[] = {"ModalHook"};
	
	
	//TODO: Improve ModalHook geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.
	
	
	//____ ModalHook::isInstanceOf() __________________________________________
	
	bool ModalHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return LayerHook::isInstanceOf(pClassName);
	}
	
	//____ ModalHook::className() _____________________________________________
	
	const char * ModalHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ ModalHook::cast() __________________________________________________
	
	ModalHook_p ModalHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return ModalHook_p( static_cast<ModalHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//_____________________________________________________________________________
	void ModalHook::top()
	{
		_moveLast();
		_requestRender();
		m_pParent->_updateKeyboardFocus();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setGeo( const Rect& geometry, Origo origo )
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
	bool ModalHook::setGeo( const Coord& ofs, Origo origo )
	{
		m_placementGeo.setPos(ofs);
		m_origo	= origo;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setOfs( const Coord& ofs )
	{
		m_placementGeo.setPos(ofs);
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setOfsX( int x )
	{
		m_placementGeo.x = x;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setOfsY( int y )
	{
		m_placementGeo.y = y;
		return _refreshRealGeo();
	}
	
	
	//_____________________________________________________________________________
	bool ModalHook::setSize( Size sz )
	{
		if( sz.w < 0 || sz.h < 0 )
			return false;
	
		m_placementGeo.setSize( sz );
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setWidth( int width )
	{
		if( width < 0 )
			return false;
	
		m_placementGeo.w = width;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setHeight( int height )
	{
		if( height < 0 )
			return false;
	
		m_placementGeo.h = height;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::move( const Coord& ofs )
	{
		m_placementGeo += ofs;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::moveX( int x )
	{
		m_placementGeo.x += x;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::moveY( int y )
	{
		m_placementGeo.y += y;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	ModalLayer_p ModalHook::parent() const
	{
		return m_pParent;
	}
	
	//_____________________________________________________________________________
	ModalHook::ModalHook( ModalLayer * pParent )
	{
		m_pParent = pParent;
	}
	
	//_____________________________________________________________________________
	bool ModalHook::_refreshRealGeo()	// Return false if we couldn't get exactly the requested (floating) geometry.
	{
		Size sz = m_placementGeo.size();
	
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
	
		Coord ofs = Util::origoToOfs( m_origo, m_pParent->size() ) - Util::origoToOfs( m_origo, sz );
		ofs += m_placementGeo.pos();
	
		Rect newGeo( ofs, sz );
	
		if( newGeo != m_geo )
		{
			_requestRender();
			m_geo = Rect( ofs, sz );
			_requestRender();
		}
	
		return true;
	}
	
	//_____________________________________________________________________________
	void ModalHook::_requestResize()
	{
		_refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	LayerHook * ModalHook::_prevLayerHook() const
	{
		ModalHook * p = _prev();
	
		// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.
	
		if( p )
			return p;
		else
			return 0;
	}
	
	//_____________________________________________________________________________
	LayerHook * ModalHook::_nextLayerHook() const
	{
		ModalHook * p = _next();
	
		// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.
	
		if( p )
			return p;
		else
			return 0;
	}
	
	//_____________________________________________________________________________
	Container * ModalHook::_parent() const
	{
		return m_pParent;
	}
	
	//____ Constructor ____________________________________________________________
	
	ModalLayer::ModalLayer()
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	ModalLayer::~ModalLayer()
	{
		// Children are deleted automaticallly when their hooks are deteled.
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ModalLayer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Layer::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ModalLayer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ModalLayer_p ModalLayer::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ModalLayer_p( static_cast<ModalLayer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ addModalWidget() ________________________________________________________
	
	ModalHook_p ModalLayer::addModalWidget( const Widget_p& pWidget, const Rect& geometry, Origo origo )
	{
		// Create MyHook and fill in members.
	
		ModalHook * pHook = new ModalHook( this );
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
	
	bool ModalLayer::removeModalWidgets()
	{
		m_modalHooks.clear();
		_requestRender();
		_updateKeyboardFocus();
		return true;
	}
	
	//____ removeWidget() _________________________________________________________
	
	bool ModalLayer::removeWidget( const Widget_p& pWidget )
	{
		if( !pWidget || pWidget->parent() != this )
			return false;
	
		if( pWidget == m_baseHook._widget() )
			return removeBaseWidget();
		else
		{
			ModalHook * pHook = (ModalHook *) pWidget->_hook();
			pHook->_requestRender();
			delete pHook;
			_updateKeyboardFocus();
			return true;
		}
	
	}
	
	//____ clear() ________________________________________________________________
	
	bool ModalLayer::clear()
	{
		removeBaseWidget();
		removeModalWidgets();
		return true;
	}
	
	//____ firstModalHook() ______________________________________________________
	
	ModalHook_p ModalLayer::firstModalHook()
	{
		return m_modalHooks.first();
	}
	
	//____ lastModalHook() _______________________________________________________
	
	ModalHook_p ModalLayer::lastModalHook()
	{
		return m_modalHooks.last();
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int ModalLayer::matchingHeight( int width ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int ModalLayer::matchingWidth( int height ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size ModalLayer::preferredSize() const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->preferredSize();
		else
			return Size(1,1);
	}
	
	//____ _findWidget() ____________________________________________________________
	
	Widget *  ModalLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// In search mode ACTION_TARGET we always return either the topmost non-hidden modal Widget (or its children),
		// or us.
	
		if( mode == SearchMode::ActionTarget )
		{
			ModalHook * pHook = m_modalHooks.last();
	
			if( pHook )
			{
				if( pHook->_widget()->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(pHook->_widget())->_findWidget( ofs - pHook->pos(), mode );
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
				if( m_baseHook._widget()->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(m_baseHook._widget())->_findWidget( ofs - m_baseHook.pos(), mode );
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
	
		return Container::_findWidget( ofs, mode );
	}
	
	//____ _updateKeyboardFocus() _______________________________________________________
	
	void ModalLayer::_updateKeyboardFocus()
	{
		// Get message handler, verify that we have a root
	
		if( !hook() || !hook()->_root() )
			return;
		
		// Retrieve focused Widget and verify it being a descendant to us.
	
		Widget * pFocused = Base::inputHandler()->focusedWidget().rawPtr();
	
		Widget * p = pFocused;
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
				ModalHook * pHook = static_cast<ModalHook*>(p->_hook());
				pHook->m_pKeyFocus = pFocused;
			}
		}
	
		// Find which child-branch to focus and switch to our previously saved focus
	
		ModalHook * pHook = m_modalHooks.last();
	
		Widget * 	pSavedFocus = 0;
		Hook *	pBranch	= 0;
	
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
			Hook * p = pSavedFocus->_hook();
			while( p && p != pBranch )
			{
				Container * pHolder = p->_parent();
				if( pHolder )
					p = pHolder->_hook();
				else
					p = 0;
			}
	
			if( p != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}
	
		// Switch to previously saved focus, or null if not applicable
	
		_hook()->parent()->_focusRequested(_hook(), pSavedFocus);
	}
	
	//____ _setSize() ___________________________________________________________
	
	void ModalLayer::_setSize( const Size& sz )
	{
		Layer::_setSize(sz);
			
		// Update size of base widget
	
		if( m_baseHook._widget() )
			m_baseHook._widget()->_setSize(sz);
	
		// Refresh modal widgets geometry, their positions might have changed.
	
		ModalHook * pHook = m_modalHooks.first();
	
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->_next();
		}
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void ModalLayer::_cloneContent( const Widget * _pOrg )
	{
	}
	
	//____ _receive() ______________________________________________________________
	
	void ModalLayer::_receive( const Msg_p& _pMsg )
	{
		Layer::_receive(_pMsg);
	
		if( _pMsg->isInstanceOf( InputMsg::CLASSNAME ) )
		{
			InputMsg_p pMsg = InputMsg::cast(_pMsg);
			
			if( !m_modalHooks.isEmpty() && _findWidget( pMsg->pointerPos(), SearchMode::ActionTarget ) == this )
			{
				switch( pMsg->type() )
				{
					case MsgType::MousePress:
					{
						MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;
		
					case MsgType::MouseRelease:
					{
						MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;
		
					case MsgType::MouseMove:
					{
						Base::msgRouter()->post( new ModalMoveOutsideMsg(this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;
				}
			}
		}
	}
	

} // namespace wg
