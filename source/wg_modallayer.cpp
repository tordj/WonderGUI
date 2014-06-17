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
#include <wg_eventhandler.h>

const char WgModalLayer::CLASSNAME[] = {"ModalLayer"};
const char WgModalHook::CLASSNAME[] = {"ModalHook"};


//TODO: Improve WgModalHook geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.


//____ WgModalHook::IsInstanceOf() __________________________________________

bool WgModalHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayerHook::IsInstanceOf(pClassName);
}

//____ WgModalHook::ClassName() _____________________________________________

const char * WgModalHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgModalHook::Cast() __________________________________________________

WgModalHookPtr WgModalHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgModalHookPtr( static_cast<WgModalHook*>(pHook.GetRealPtr()) );

	return 0;
}

//_____________________________________________________________________________
void WgModalHook::Top()
{
	_moveLast();
	_requestRender();
	m_pParent->_updateKeyboardFocus();
}

//_____________________________________________________________________________
bool WgModalHook::SetGeo( const WgRect& geometry, WgOrigo origo )
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
bool WgModalHook::SetGeo( const WgCoord& ofs, WgOrigo origo )
{
	m_placementGeo.SetPos(ofs);
	m_origo	= origo;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetOfs( const WgCoord& ofs )
{
	m_placementGeo.SetPos(ofs);
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetOfsX( int x )
{
	m_placementGeo.x = x;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetOfsY( int y )
{
	m_placementGeo.y = y;
	return _refreshRealGeo();
}


//_____________________________________________________________________________
bool WgModalHook::SetSize( WgSize sz )
{
	if( sz.w < 0 || sz.h < 0 )
		return false;

	m_placementGeo.SetSize( sz );
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetWidth( int width )
{
	if( width < 0 )
		return false;

	m_placementGeo.w = width;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetHeight( int height )
{
	if( height < 0 )
		return false;

	m_placementGeo.h = height;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::Move( const WgCoord& ofs )
{
	m_placementGeo += ofs;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::MoveX( int x )
{
	m_placementGeo.x += x;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::MoveY( int y )
{
	m_placementGeo.y += y;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
WgModalLayerPtr WgModalHook::Parent() const
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
	WgSize sz = m_placementGeo.Size();

	if( sz.w == 0 && sz.h == 0 )
		sz = m_pWidget->PreferredSize();
	else if( sz.w == 0 )
		sz.w = m_pWidget->WidthForHeight(sz.h);
	else if( sz.h == 0 )
		sz.h = m_pWidget->HeightForWidth(sz.w);

	if( sz.w <= 0 )
		sz.w = 1;
	if( sz.h <= 0 )
		sz.h = 1;

	WgCoord ofs = WgUtil::OrigoToOfs( m_origo, m_pParent->Size() ) - WgUtil::OrigoToOfs( m_origo, sz );
	ofs += m_placementGeo.Pos();

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

//____ IsInstanceOf() _________________________________________________________

bool WgModalLayer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgModalLayer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgModalLayerPtr WgModalLayer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgModalLayerPtr( static_cast<WgModalLayer*>(pObject.GetRealPtr()) );

	return 0;
}

//____ AddModalWidget() ________________________________________________________

WgModalHookPtr WgModalLayer::AddModalWidget( const WgWidgetPtr& pWidget, const WgRect& geometry, WgOrigo origo )
{
	// Create Hook and fill in members.

	WgModalHook * pHook = new WgModalHook( this );
	pHook->_setWidget(pWidget.GetRealPtr());
	pHook->m_origo = origo;
	pHook->m_placementGeo = geometry;
	m_modalHooks.PushBack(pHook);

	// Refresh geometry and request render.

	pHook->_refreshRealGeo();
	_updateKeyboardFocus();
	return pHook;
}

//____ RemoveModalWidgets() ________________________________________________

bool WgModalLayer::RemoveModalWidgets()
{
	m_modalHooks.Clear();
	_requestRender();
	_updateKeyboardFocus();
	return true;
}

//____ RemoveWidget() _________________________________________________________

bool WgModalLayer::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || pWidget->Parent() != this )
		return false;

	if( pWidget == m_baseHook._widget() )
		return RemoveBaseWidget();
	else
	{
		WgModalHook * pHook = (WgModalHook *) pWidget->_hook();
		pHook->_requestRender();
		delete pHook;
		_updateKeyboardFocus();
		return true;
	}

}

//____ Clear() ________________________________________________________________

bool WgModalLayer::Clear()
{
	RemoveBaseWidget();
	RemoveModalWidgets();
	return true;
}

//____ FirstModalHook() ______________________________________________________

WgModalHookPtr WgModalLayer::FirstModalHook()
{
	return m_modalHooks.First();
}

//____ LastModalHook() _______________________________________________________

WgModalHookPtr WgModalLayer::LastModalHook()
{
	return m_modalHooks.Last();
}

//____ HeightForWidth() _______________________________________________________

int WgModalLayer::HeightForWidth( int width ) const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->HeightForWidth( width );
	else
		return WgWidget::HeightForWidth(width);
}

//____ WidthForHeight() _______________________________________________________

int WgModalLayer::WidthForHeight( int height ) const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->WidthForHeight( height );
	else
		return WgWidget::WidthForHeight(height);
}

//____ PreferredSize() _____________________________________________________________

WgSize WgModalLayer::PreferredSize() const
{
	if( m_baseHook._widget() )
		return m_baseHook._widget()->PreferredSize();
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
		WgModalHook * pHook = m_modalHooks.Last();

		if( pHook )
		{
			if( pHook->_widget()->IsContainer() )
			{
				WgWidget * pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - pHook->Pos(), mode );
				if( pResult )
					return pResult;
			}
			else
			{
				if( pHook->_widget()->MarkTest(ofs - pHook->Pos()) )
					return pHook->_widget();
				else
					return this;
			}
		}
		else if( m_baseHook._widget() )
		{
			if( m_baseHook._widget()->IsContainer() )
			{
				WgWidget * pResult = static_cast<WgContainer*>(m_baseHook._widget())->_findWidget( ofs - m_baseHook.Pos(), mode );
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
	// Get event handler, verify that we have a root

	if( !Hook() )
		return;

	WgEventHandler * pHandler = Hook()->EventHandler().GetRealPtr();
	if( !pHandler )
		return;

	// Retrieve focused Widget and verify it being a descendant to us.

	WgWidget * pFocused = pHandler->KeyboardFocus().GetRealPtr();

	WgWidget * p = pFocused;
	while( p && p->Parent() && p->Parent() != this )
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

	WgModalHook * pHook = m_modalHooks.Last();

	WgWidget * 	pSavedFocus = 0;
	WgHook *	pBranch	= 0;

	if( pHook )
	{
		pSavedFocus = pHook->m_pKeyFocus.GetRealPtr();
		pHook->m_pKeyFocus = 0;								// Needs to be cleared for the future.
		pBranch = pHook;
	}
	else if( m_baseHook._widget() )
	{
		pSavedFocus = m_pBaseKeyFocus.GetRealPtr();
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

	pHandler->SetKeyboardFocus( pSavedFocus );
}

//____ _onNewSize() ___________________________________________________________

void WgModalLayer::_onNewSize( const WgSize& sz )
{
	m_size = sz;

	// Update size of base widget

	if( m_baseHook._widget() )
		m_baseHook._widget()->_onNewSize(sz);

	// Refresh modal widgets geometry, their positions might have changed.

	WgModalHook * pHook = m_modalHooks.First();

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

//____ _onEvent() ______________________________________________________________

void WgModalLayer::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgLayer::_onEvent(_pEvent,pHandler);

	if( !m_modalHooks.IsEmpty() && _findWidget( _pEvent->PointerPos(), WG_SEARCH_ACTION_TARGET ) == this )
	{
		switch( _pEvent->Type() )
		{
			case WG_EVENT_MOUSE_PRESS:
			{
				WgMouseButtonEventPtr pEvent = WgMouseButtonEvent::Cast(_pEvent);
				pHandler->QueueEvent( new WgModalBlockedPressEvent( pEvent->Button(), this) );
			}
			break;

			case WG_EVENT_MOUSE_RELEASE:
			{
				WgMouseButtonEventPtr pEvent = WgMouseButtonEvent::Cast(_pEvent);
				pHandler->QueueEvent( new WgModalBlockedPressEvent( pEvent->Button(), this) );
			}
			break;

			case WG_EVENT_MOUSE_MOVE:
			{
				pHandler->QueueEvent( new WgModalMoveOutsideEvent(this) );
			}
			break;
		}
	}	
}

