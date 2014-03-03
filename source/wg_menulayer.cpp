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

#include <wg_menulayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_eventhandler.h>
#include <wg_panel.h>

const char WgMenuLayer::CLASSNAME[] = {"MenuLayer"};

static const char	c_hookType[] = {"MenuHook"};
static const char	c_basehookType[] = {"MenuLayerBasehook"};


//_____________________________________________________________________________
WgMenuLayerPtr WgMenuHook::Parent() const
{
	return m_pParent;
}

//_____________________________________________________________________________
WgMenuHook::WgMenuHook( WgMenuLayer * pParent, WgWidget * pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize )
{
	m_pParent 		= pParent;
	m_pOpener		= pOpener;
	m_launcherGeo	= launcherGeo;
	m_attachPoint 	= attachPoint;
	m_maxSize 		= maxSize;
}

//_____________________________________________________________________________
const char * WgMenuHook::Type( void ) const
{
	return ClassType();
}

//_____________________________________________________________________________
const char * WgMenuHook::ClassType()
{
	return c_hookType;
}


//_____________________________________________________________________________
void WgMenuHook::_requestRender()
{
	m_pParent->_onRequestRender( m_geo, this );
}

//_____________________________________________________________________________
void WgMenuHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect + m_geo.Pos(), this );
}

//_____________________________________________________________________________
void WgMenuHook::_requestResize()
{
	_updateGeo();					// Just update this menus geo, don't recursively update children, their position should remain where it is.
}

//_____________________________________________________________________________
WgHook * WgMenuHook::_prevHook() const
{
	WgMenuHook * p = _prev();

	if( p )
		return p;
	else if( m_pParent->m_baseHook._widget() )
		return &m_pParent->m_baseHook;
	else
		return 0;
}

//_____________________________________________________________________________
WgHook * WgMenuHook::_nextHook() const
{
	WgMenuHook * p = _next();

	// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.

	if( p )
		return p;
	else
		return 0;
}

//_____________________________________________________________________________
WgContainer * WgMenuHook::_parent() const
{
	return m_pParent;
}

//____ _updateGeo() __________________________________________________________

bool WgMenuHook::_updateGeo()
{
	// Get size of parent and correct launcherGeo

	WgRect 	parentSize = m_pParent->Size();

	//

	WgRect geo(0,0,WgSize::Min(_widget()->PreferredSize(),WgSize::Min(m_maxSize,parentSize)));

	switch( m_attachPoint )
	{
		case WG_NORTHEAST:					// Right side of launcherGeo, going down.
		{
			geo.x = m_launcherGeo.Right();
			geo.y = m_launcherGeo.Top();
			break;
		}

		case WG_SOUTHEAST:					// Right side of launcherGeo, going up.
		{
			geo.x = m_launcherGeo.Right();
			geo.y = m_launcherGeo.Bottom() - geo.h;
			break;
		}

		case WG_NORTHWEST:					// Left-aligned above launcher.
		{
			geo.x = m_launcherGeo.Left();
			geo.y = m_launcherGeo.Top() - geo.h;
			break;
		}

		case WG_SOUTHWEST:					// Left-aligned below launcher.
		{
			geo.x = m_launcherGeo.Left();
			geo.y = m_launcherGeo.Bottom();
			break;
		}

		case WG_EAST:						// Centered left of launcherGeo.
		{
			geo.x = m_launcherGeo.Left() - geo.w;
			geo.y = m_launcherGeo.Top() + m_launcherGeo.h/2 - geo.h/2;
			break;
		}

		case WG_NORTH:						// Centered above launcherGeo.
		{
			geo.x = m_launcherGeo.Left() + m_launcherGeo.w/2 + geo.w/2;
			geo.y = m_launcherGeo.Top() - geo.h;
			break;
		}

		case WG_WEST:						// Centered right of launcherGeo.
		{
			geo.x = m_launcherGeo.Right();
			geo.y = m_launcherGeo.Top() + m_launcherGeo.h/2 - geo.h/2;
			break;
		}

		case WG_SOUTH:						// Centered below launcherGeo.
		{
			geo.x = m_launcherGeo.Left() + m_launcherGeo.w/2 + geo.w/2;
			geo.y = m_launcherGeo.Bottom();
			break;
		}

	}

	// Adjust geometry to fit inside parent.

	if( geo.Right() > parentSize.w )
	{
		if( geo.Left() == m_launcherGeo.Right() )
		{
			if( m_launcherGeo.Left() > parentSize.w - m_launcherGeo.Right() )
			{
				geo.x = m_launcherGeo.Left() - geo.w;
				if( geo.x < 0 )
				{
					geo.x = 0;
					geo.w = m_launcherGeo.Left();
				}
			}
			else
				geo.w = parentSize.w - geo.x;
		}
		else
			geo.x = parentSize.w - geo.w;
	}

	if( geo.Left() < 0 )
	{
		if( geo.Right() == m_launcherGeo.Left() )
		{
			if( m_launcherGeo.Left() < parentSize.w - m_launcherGeo.Right() )
			{
				geo.x = m_launcherGeo.Right();
				if( geo.Right() > parentSize.w )
					geo.w = parentSize.w - geo.x;
			}
			else
			{
				geo.x = 0;
				geo.w = m_launcherGeo.Left();
			}

		}
		else
			geo.x = 0;
	}

	if( geo.Bottom() > parentSize.h )
	{
		if( geo.Top() == m_launcherGeo.Bottom() )
		{
			if( m_launcherGeo.Top() > parentSize.h - m_launcherGeo.Bottom() )
			{
				geo.y = m_launcherGeo.Top() - geo.h;
				if( geo.y < 0 )
				{
					geo.y = 0;
					geo.h = m_launcherGeo.Top();
				}
			}
			else
				geo.h = parentSize.h - geo.y;
		}
		else
			geo.y = parentSize.h - geo.h;
	}

	if( geo.Top() < 0 )
	{
		if( geo.Bottom() == m_launcherGeo.Top() )
		{
			if( m_launcherGeo.Top() < parentSize.h - m_launcherGeo.Bottom() )
			{
				geo.y = m_launcherGeo.Bottom();
				if( geo.Bottom() > parentSize.h )
					geo.h = parentSize.h - geo.y;
			}
			else
			{
				geo.y = 0;
				geo.h = m_launcherGeo.Bottom();
			}
		}
		else
			geo.y = 0;
	}

	// Update geometry if it has changed.

	if( geo != m_geo )
	{
		_requestRender();
		m_geo = geo;
		_requestRender();

		return true;
	}
	else
		return false;
}

//____ Constructor ____________________________________________________________

WgMenuLayer::WgMenuLayer()
{
}

//____ Destructor _____________________________________________________________

WgMenuLayer::~WgMenuLayer()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgMenuLayer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgMenuLayer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgMenuLayerPtr WgMenuLayer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMenuLayerPtr( static_cast<WgMenuLayer*>(pObject.GetRealPtr()) );

	return 0;
}


//____ OpenMenu() _______________________________________________________________

WgMenuHook * WgMenuLayer::OpenMenu( const WgWidgetPtr& pMenu, const WgWidgetPtr& pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize )
{
	// Create Hook and fill in members.

	WgMenuHook * pHook = new WgMenuHook( this, pOpener.GetRealPtr(), launcherGeo, attachPoint, maxSize );
	pHook->_setWidget(pMenu.GetRealPtr());
	m_menuHooks.PushBack(pHook);
	pHook->_updateGeo();
	_stealKeyboardFocus();
	return pHook;
}


//____ CloseAllMenus() ________________________________________________

bool WgMenuLayer::CloseAllMenus()
{
	WgMenuHook * pHook = m_menuHooks.First();
	if( pHook )
		CloseMenu( pHook->_widget() );

	return true;
}


//____ CloseMenu() _________________________________________________________

bool WgMenuLayer::CloseMenu( const WgWidgetPtr& pWidget )
{
	if( !pWidget || pWidget->Parent() != this || pWidget == m_baseHook.Widget() )
		return false;

	WgEventHandler * pEH = _eventHandler();

	WgMenuHook * pHook = (WgMenuHook *) pWidget->Hook();

	while( pHook )
	{
		WgMenuHook * p = pHook;
		pHook = pHook->Next();

		if( pEH )
			pEH->QueueEvent( new WgMenuClosedEvent( p->_widget(), p->m_pOpener ) );

		p->_requestRender();
		delete p;
	}
	_restoreKeyboardFocus();
	return true;
}

//____ FirstMenu() ______________________________________________________

WgMenuHook * WgMenuLayer::FirstMenu()
{
	return m_menuHooks.First();
}

//____ LastMenu() _______________________________________________________

WgMenuHook * WgMenuLayer::LastMenu()
{
	return m_menuHooks.Last();
}

//____ _findWidget() ____________________________________________________________

WgWidget *  WgMenuLayer::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	// MenuPanel has its own _findWidget() method since we need special treatment of
	// searchmode ACTION_TARGET when a menu is open.

	if( mode == WG_SEARCH_ACTION_TARGET && !m_menuHooks.IsEmpty() )
	{
		// In search mode ACTION_TARGET we limit our target to us, our menu-branches and the menu-opener if a menu is open.

		WgMenuHook * pHook = m_menuHooks.Last();
		WgWidget * pResult = 0;

		while( pHook && !pResult )
		{
			if( pHook->m_geo.Contains( ofs ) )
			{
				if( pHook->_widget()->IsContainer() )
					pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - pHook->m_geo.Pos(), mode );
				else if( pHook->_widget()->MarkTest( ofs - pHook->m_geo.Pos() ) )
					pResult = pHook->_widget();
			}
			pHook = pHook->Prev();
		}

		if( pResult == 0 )
		{
			// Check the first opener
			
			WgMenuHook * pHook = m_menuHooks.First();
			if( pHook && pHook->m_pOpener )
			{
				WgWidget * pOpener = pHook->m_pOpener.GetRealPtr();

				WgCoord absPos 		= ofs + ScreenPos();
				WgRect	openerGeo 	= pOpener->ScreenGeo();

				if( openerGeo.Contains(absPos) && pOpener->MarkTest( absPos - openerGeo.Pos() ) )
					pResult = pOpener;
			}
			
			// Fall back to us.
			
			if( pResult == 0 )
				pResult = this;
		}
		return pResult;
	}
	else
	{
		// For the rest of the modes we can rely on the default method.

		return WgContainer::_findWidget( ofs, mode );
	}
}

//____ _onRequestRender() _____________________________________________________

void WgMenuLayer::_onRequestRender( const WgRect& rect, const WgMenuHook * pHook )
{
	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgMenuHook * pCover;

	if( pHook )
		pCover = pHook->Next();
	else
		pCover = m_menuHooks.First();

	while( pCover )
	{
		if( pCover->m_geo.IntersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->Next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		_requestRender( * pRect );
}

//____ _onNewSize() ___________________________________________________________

void WgMenuLayer::_onNewSize( const WgSize& sz )
{
	m_size = sz;

	// Update size of base widget

	if( m_baseHook._widget() )
		m_baseHook._widget()->_onNewSize(sz);
}

//____ _onCloneContent() ______________________________________________________

void WgMenuLayer::_onCloneContent( const WgWidget * _pOrg )
{
}

//____ _onEvent() ______________________________________________________________

void WgMenuLayer::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgWidget * pOpener = 0;

	// Try to find an opener

	WgWidget * pOrigin = _pEvent->Widget();
	if( pOrigin && pOrigin != this )
	{
		WgMenuHook * pHook = m_menuHooks.First();
		while( pHook && pHook->_widget() != pOrigin )
			pHook = pHook->Next();
			
		if( pHook && pHook->m_pOpener )
			pOpener = pHook->m_pOpener.GetRealPtr();
	}
	
	// First we try to forward event to opener (if any)

	if( pOpener )
	{
		pHandler->ForwardEvent( _pEvent, pOpener );
		return;
	}	

	// Secondly we take care of event ourselves if it is addressed to one of our menus or us.

	switch( _pEvent->Type() )
	{
/*
		case WG_EVENT_MOUSE_POSITION:

			if( !m_menuHooks.IsEmpty() )							// Process only if we have at least one open menu.
			{
				WgCoord ofs = _pEvent->PointerPos();
				WgWidget * p = _findWidget( ofs, WG_SEARCH_ACTION_TARGET );
				if( p != this )
				{
					while( p->Parent() != this )
						p = p->Parent();
						
					if( p != m_menuHooks.	
				}	
			}
		break;
*/		
		case WG_EVENT_MOUSE_RELEASE:
		case WG_EVENT_MOUSE_PRESS:
		{
			WgMouseButtonEventPtr pEvent = WgMouseButtonEvent::Cast(_pEvent);

			WgCoord ofs = pEvent->PointerPos();
			WgWidget * p = _findWidget( ofs, WG_SEARCH_ACTION_TARGET );
			if( p == this )
			{
				CloseAllMenus();
				pHandler->SwallowEvent( _pEvent );
				return;
			}
		}
		break;

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_REPEAT:
		{
			WgKeyEventPtr pEvent = WgKeyEvent::Cast(_pEvent);

			if( pEvent->TranslatedKeyCode() == WG_KEY_ESCAPE )
			{
				if( !m_menuHooks.IsEmpty() )
				{
					CloseMenu( m_menuHooks.Last()->_widget() );
					pHandler->SwallowEvent( _pEvent );
					return;
				}
			}

		}
		break;
	}
}

//____ _stealKeyboardFocus() _________________________________________________

void WgMenuLayer::_stealKeyboardFocus()
{
	// Get event handler, verify that we have a root

	if( !Hook() )
		return;

	WgEventHandlerPtr pHandler = Hook()->EventHandler();
	if( !pHandler )
		return;

	// Save old keyboard focus, which we assume belonged to previous menu in hierarchy.

	if( m_menuHooks.Size() < 2 )
		m_pKeyFocus = pHandler->KeyboardFocus().GetRealPtr();
	else
		m_menuHooks.Last()->Prev()->m_pKeyFocus = pHandler->KeyboardFocus().GetRealPtr();

	// Steal keyboard focus to top menu

	WgWidget * pWidget = m_menuHooks.Last()->_widget();

	if( pWidget->IsInstanceOf( WgPanel::CLASSNAME ) && static_cast<WgPanel*>(pWidget)->IsFocusGroup() )
		pHandler->SetFocusGroup(static_cast<WgPanel*>(pWidget));
	else
		pHandler->SetKeyboardFocus(pWidget);
}

//____ _restoreKeyboardFocus() _________________________________________________

void WgMenuLayer::_restoreKeyboardFocus()
{
	// Get event handler, verify that we have a root

	if( !Hook() )
		return;

	WgEventHandlerPtr pHandler = Hook()->EventHandler();
	if( !pHandler )
		return;

	//

	if( m_menuHooks.IsEmpty() )
		pHandler->SetKeyboardFocus( m_pKeyFocus.GetRealPtr() );
	else
		pHandler->SetKeyboardFocus( m_menuHooks.Last()->m_pKeyFocus.GetRealPtr() );
}


//____ _firstHook() ___________________________________________________________

WgHook* WgMenuLayer::_firstHook() const
{
	if( m_baseHook._widget() )
		return const_cast<BaseHook*>(&m_baseHook);
	else
		return m_menuHooks.First();
}

//____ _lastHook() ____________________________________________________________

WgHook* WgMenuLayer::_lastHook() const
{
	return m_menuHooks.Last();
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgMenuLayer::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_baseHook._widget() )
	{
		geo = WgRect(0,0,m_size);
		return const_cast<BaseHook*>(&m_baseHook);
	}
	else
	{
		WgMenuHook * p = m_menuHooks.First();
		if( p )
			geo = p->m_geo;

		return p;
	}
}

//____ _nextHookWithGeo() _______________________________________________________

WgHook * WgMenuLayer::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->Next();
	if( p )
		geo = ((WgMenuHook*)p)->m_geo;

	return p;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgMenuLayer::_lastHookWithGeo( WgRect& geo ) const
{
	WgMenuHook * p = m_menuHooks.Last();
	if( p )
	{
		geo = p->m_geo;
		return p;
	}
	else if( m_baseHook._widget() )
	{
		geo = WgRect(0,0,m_size);
		return const_cast<BaseHook*>(&m_baseHook);
	}
	else
		return 0;
}

//____ _prevHookWithGeo() _______________________________________________________

WgHook * WgMenuLayer::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->Prev();
	if( p )
		geo = p->Geo();

	return p;
}

