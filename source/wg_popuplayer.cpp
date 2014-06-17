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

#include <wg_popuplayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_eventhandler.h>
#include <wg_panel.h>

const char WgPopupLayer::CLASSNAME[] = {"PopupLayer"};
const char WgPopupHook::CLASSNAME[] = {"PopupHook"};



//_____________________________________________________________________________
WgPopupLayerPtr WgPopupHook::Parent() const
{
	return m_pParent;
}

//_____________________________________________________________________________
WgPopupHook::WgPopupHook( WgPopupLayer * pParent, WgWidget * pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize )
{
	m_pParent 		= pParent;
	m_pOpener		= pOpener;
	m_launcherGeo	= launcherGeo;
	m_attachPoint 	= attachPoint;
	m_maxSize 		= maxSize;
}

//____ WgPopupHook::IsInstanceOf() __________________________________________

bool WgPopupHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayerHook::IsInstanceOf(pClassName);
}

//____ WgPopupHook::ClassName() _____________________________________________

const char * WgPopupHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgPopupHook::Cast() __________________________________________________

WgPopupHookPtr WgPopupHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgPopupHookPtr( static_cast<WgPopupHook*>(pHook.GetRealPtr()) );

	return 0;
}

//_____________________________________________________________________________
void WgPopupHook::_requestRender()
{
	m_pParent->_onRequestRender( m_geo, this );
}

//_____________________________________________________________________________
void WgPopupHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect + m_geo.Pos(), this );
}

//_____________________________________________________________________________
void WgPopupHook::_requestResize()
{
	_updateGeo();					// Just update this menus geo, don't recursively update children, their position should remain where it is.
}

//_____________________________________________________________________________
WgLayerHook * WgPopupHook::_prevLayerHook() const
{
	WgPopupHook * p = _prev();

	// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.

	if( p )
		return p;
	else
		return 0;
}

//_____________________________________________________________________________
WgLayerHook * WgPopupHook::_nextLayerHook() const
{
	WgPopupHook * p = _next();

	// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.

	if( p )
		return p;
	else
		return 0;
}

//_____________________________________________________________________________
WgContainer * WgPopupHook::_parent() const
{
	return m_pParent;
}

//____ _updateGeo() __________________________________________________________

bool WgPopupHook::_updateGeo()
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

WgPopupLayer::WgPopupLayer()
{
}

//____ Destructor _____________________________________________________________

WgPopupLayer::~WgPopupLayer()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgPopupLayer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgPopupLayer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgPopupLayerPtr WgPopupLayer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgPopupLayerPtr( static_cast<WgPopupLayer*>(pObject.GetRealPtr()) );

	return 0;
}


//____ OpenPopup() _______________________________________________________________

WgPopupHookPtr WgPopupLayer::OpenPopup( const WgWidgetPtr& pPopup, const WgWidgetPtr& pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize )
{
	// Create Hook and fill in members.

	WgPopupHook * pHook = new WgPopupHook( this, pOpener.GetRealPtr(), launcherGeo, attachPoint, maxSize );
	pHook->_setWidget(pPopup.GetRealPtr());
	m_popupHooks.PushBack(pHook);
	pHook->_updateGeo();
	_stealKeyboardFocus();
	return pHook;
}


//____ CloseAllPopups() ________________________________________________

bool WgPopupLayer::CloseAllPopups()
{
	WgPopupHook * pHook = m_popupHooks.First();
	if( pHook )
		ClosePopup( pHook->_widget() );

	return true;
}


//____ ClosePopup() _________________________________________________________

bool WgPopupLayer::ClosePopup( const WgWidgetPtr& pWidget )
{
	if( !pWidget || pWidget->Parent() != this || pWidget == m_baseHook.Widget() )
		return false;

	WgEventHandler * pEH = _eventHandler();

	WgPopupHook * pHook = (WgPopupHook *) pWidget->_hook();

	while( pHook )
	{
		WgPopupHook * p = pHook;
		pHook = pHook->_next();

		if( pEH )
			pEH->QueueEvent( new WgPopupClosedEvent( p->_widget(), p->m_pOpener ) );

		p->_requestRender();
		delete p;
	}
	_restoreKeyboardFocus();
	return true;
}

//____ FirstPopupHook() ______________________________________________________

WgPopupHookPtr WgPopupLayer::FirstPopupHook()
{
	return m_popupHooks.First();
}

//____ LastPopupHook() _______________________________________________________

WgPopupHookPtr WgPopupLayer::LastPopupHook()
{
	return m_popupHooks.Last();
}

//____ _findWidget() ____________________________________________________________

WgWidget *  WgPopupLayer::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	// MenuPanel has its own _findWidget() method since we need special treatment of
	// searchmode ACTION_TARGET when a menu is open.

	if( mode == WG_SEARCH_ACTION_TARGET && !m_popupHooks.IsEmpty() )
	{
		// In search mode ACTION_TARGET we limit our target to us, our menu-branches and the menu-opener if a menu is open.

		WgPopupHook * pHook = m_popupHooks.Last();
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
			pHook = pHook->_prev();
		}

		if( pResult == 0 )
		{
			// Check the first opener
			
			WgPopupHook * pHook = m_popupHooks.First();
			if( pHook && pHook->m_pOpener )
			{
				WgWidget * pOpener = pHook->m_pOpener.GetRealPtr();

				WgCoord absPos 		= ofs + GlobalPos();
				WgRect	openerGeo 	= pOpener->GlobalGeo();

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

void WgPopupLayer::_onRequestRender( const WgRect& rect, const WgPopupHook * pHook )
{
	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgPopupHook * pCover;

	if( pHook )
		pCover = pHook->_next();
	else
		pCover = m_popupHooks.First();

	while( pCover )
	{
		if( pCover->m_geo.IntersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		_requestRender( * pRect );
}

//____ _onNewSize() ___________________________________________________________

void WgPopupLayer::_onNewSize( const WgSize& sz )
{
	m_size = sz;

	// Update size of base widget

	if( m_baseHook._widget() )
		m_baseHook._widget()->_onNewSize(sz);
}

//____ _onCloneContent() ______________________________________________________

void WgPopupLayer::_onCloneContent( const WgWidget * _pOrg )
{
}

//____ _onEvent() ______________________________________________________________

void WgPopupLayer::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgLayer::_onEvent(_pEvent,pHandler);

	WgWidget * pOpener = 0;

	// Try to find an opener

	WgWidget * pOrigin = _pEvent->Widget();
	if( pOrigin && pOrigin != this )
	{
		WgPopupHook * pHook = m_popupHooks.First();
		while( pHook && pHook->_widget() != pOrigin )
			pHook = pHook->_next();
			
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

			if( !m_popupHooks.IsEmpty() )							// Process only if we have at least one open menu.
			{
				WgCoord ofs = _pEvent->PointerPos();
				WgWidget * p = _findWidget( ofs, WG_SEARCH_ACTION_TARGET );
				if( p != this )
				{
					while( p->Parent() != this )
						p = p->Parent();
						
					if( p != m_popupHooks.	
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
				CloseAllPopups();
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
				if( !m_popupHooks.IsEmpty() )
				{
					ClosePopup( m_popupHooks.Last()->_widget() );
					pHandler->SwallowEvent( _pEvent );
					return;
				}
			}

		}
		break;
	}
}

//____ _stealKeyboardFocus() _________________________________________________

void WgPopupLayer::_stealKeyboardFocus()
{
	// Get event handler, verify that we have a root

	if( !Hook() )
		return;

	WgEventHandlerPtr pHandler = Hook()->EventHandler();
	if( !pHandler )
		return;

	// Save old keyboard focus, which we assume belonged to previous menu in hierarchy.

	if( m_popupHooks.Size() < 2 )
		m_pKeyFocus = pHandler->KeyboardFocus().GetRealPtr();
	else
		m_popupHooks.Last()->Prev()->m_pKeyFocus = pHandler->KeyboardFocus().GetRealPtr();

	// Steal keyboard focus to top menu

	WgWidget * pWidget = m_popupHooks.Last()->_widget();

	if( pWidget->IsInstanceOf( WgPanel::CLASSNAME ) && static_cast<WgPanel*>(pWidget)->IsFocusGroup() )
		pHandler->SetFocusGroup(static_cast<WgPanel*>(pWidget));
	else
		pHandler->SetKeyboardFocus(pWidget);
}

//____ _restoreKeyboardFocus() _________________________________________________

void WgPopupLayer::_restoreKeyboardFocus()
{
	// Get event handler, verify that we have a root

	if( !Hook() )
		return;

	WgEventHandlerPtr pHandler = Hook()->EventHandler();
	if( !pHandler )
		return;

	//

	if( m_popupHooks.IsEmpty() )
		pHandler->SetKeyboardFocus( m_pKeyFocus.GetRealPtr() );
	else
		pHandler->SetKeyboardFocus( m_popupHooks.Last()->m_pKeyFocus.GetRealPtr() );
}

