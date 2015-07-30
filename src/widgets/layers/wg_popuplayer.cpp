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
#include <wg_msgrouter.h>
#include <wg_panel.h>
#include <wg_base.h>

const char WgPopupLayer::CLASSNAME[] = {"PopupLayer"};
const char WgPopupHook::CLASSNAME[] = {"PopupHook"};



//_____________________________________________________________________________
WgPopupLayer_p WgPopupHook::parent() const
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

//____ WgPopupHook::isInstanceOf() __________________________________________

bool WgPopupHook::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayerHook::isInstanceOf(pClassName);
}

//____ WgPopupHook::className() _____________________________________________

const char * WgPopupHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ WgPopupHook::cast() __________________________________________________

WgPopupHook_p WgPopupHook::cast( const WgHook_p& pHook )
{
	if( pHook && pHook->isInstanceOf(CLASSNAME) )
		return WgPopupHook_p( static_cast<WgPopupHook*>(pHook.rawPtr()) );

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
	m_pParent->_onRequestRender( rect + m_geo.pos(), this );
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

	WgRect 	parentSize = m_pParent->size();

	//

	WgRect geo(0,0,WgSize::min(_widget()->preferredSize(),WgSize::min(m_maxSize,parentSize)));

	switch( m_attachPoint )
	{
		case WG_NORTHEAST:					// Right side of launcherGeo, going down.
		{
			geo.x = m_launcherGeo.right();
			geo.y = m_launcherGeo.top();
			break;
		}

		case WG_SOUTHEAST:					// Right side of launcherGeo, going up.
		{
			geo.x = m_launcherGeo.right();
			geo.y = m_launcherGeo.bottom() - geo.h;
			break;
		}

		case WG_NORTHWEST:					// Left-aligned above launcher.
		{
			geo.x = m_launcherGeo.left();
			geo.y = m_launcherGeo.top() - geo.h;
			break;
		}

		case WG_SOUTHWEST:					// Left-aligned below launcher.
		{
			geo.x = m_launcherGeo.left();
			geo.y = m_launcherGeo.bottom();
			break;
		}

		case WG_EAST:						// Centered left of launcherGeo.
		{
			geo.x = m_launcherGeo.left() - geo.w;
			geo.y = m_launcherGeo.top() + m_launcherGeo.h/2 - geo.h/2;
			break;
		}

		case WG_NORTH:						// Centered above launcherGeo.
		{
			geo.x = m_launcherGeo.left() + m_launcherGeo.w/2 + geo.w/2;
			geo.y = m_launcherGeo.top() - geo.h;
			break;
		}

		case WG_WEST:						// Centered right of launcherGeo.
		{
			geo.x = m_launcherGeo.right();
			geo.y = m_launcherGeo.top() + m_launcherGeo.h/2 - geo.h/2;
			break;
		}

		case WG_SOUTH:						// Centered below launcherGeo.
		{
			geo.x = m_launcherGeo.left() + m_launcherGeo.w/2 + geo.w/2;
			geo.y = m_launcherGeo.bottom();
			break;
		}

	}

	// Adjust geometry to fit inside parent.

	if( geo.right() > parentSize.w )
	{
		if( geo.left() == m_launcherGeo.right() )
		{
			if( m_launcherGeo.left() > parentSize.w - m_launcherGeo.right() )
			{
				geo.x = m_launcherGeo.left() - geo.w;
				if( geo.x < 0 )
				{
					geo.x = 0;
					geo.w = m_launcherGeo.left();
				}
			}
			else
				geo.w = parentSize.w - geo.x;
		}
		else
			geo.x = parentSize.w - geo.w;
	}

	if( geo.left() < 0 )
	{
		if( geo.right() == m_launcherGeo.left() )
		{
			if( m_launcherGeo.left() < parentSize.w - m_launcherGeo.right() )
			{
				geo.x = m_launcherGeo.right();
				if( geo.right() > parentSize.w )
					geo.w = parentSize.w - geo.x;
			}
			else
			{
				geo.x = 0;
				geo.w = m_launcherGeo.left();
			}

		}
		else
			geo.x = 0;
	}

	if( geo.bottom() > parentSize.h )
	{
		if( geo.top() == m_launcherGeo.bottom() )
		{
			if( m_launcherGeo.top() > parentSize.h - m_launcherGeo.bottom() )
			{
				geo.y = m_launcherGeo.top() - geo.h;
				if( geo.y < 0 )
				{
					geo.y = 0;
					geo.h = m_launcherGeo.top();
				}
			}
			else
				geo.h = parentSize.h - geo.y;
		}
		else
			geo.y = parentSize.h - geo.h;
	}

	if( geo.top() < 0 )
	{
		if( geo.bottom() == m_launcherGeo.top() )
		{
			if( m_launcherGeo.top() < parentSize.h - m_launcherGeo.bottom() )
			{
				geo.y = m_launcherGeo.bottom();
				if( geo.bottom() > parentSize.h )
					geo.h = parentSize.h - geo.y;
			}
			else
			{
				geo.y = 0;
				geo.h = m_launcherGeo.bottom();
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

//____ isInstanceOf() _________________________________________________________

bool WgPopupLayer::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgLayer::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgPopupLayer::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgPopupLayer_p WgPopupLayer::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgPopupLayer_p( static_cast<WgPopupLayer*>(pObject.rawPtr()) );

	return 0;
}


//____ openPopup() _______________________________________________________________

WgPopupHook_p WgPopupLayer::openPopup( const WgWidget_p& pPopup, const WgWidget_p& pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize )
{
	// Create Hook and fill in members.

	WgPopupHook * pHook = new WgPopupHook( this, pOpener.rawPtr(), launcherGeo, attachPoint, maxSize );
	pHook->_setWidget(pPopup.rawPtr());
	m_popupHooks.pushBack(pHook);
	pHook->_updateGeo();
	_stealKeyboardFocus();
	return pHook;
}


//____ closeAllPopups() ________________________________________________

bool WgPopupLayer::closeAllPopups()
{
	WgPopupHook * pHook = m_popupHooks.first();
	if( pHook )
		closePopup( pHook->_widget() );

	return true;
}


//____ closePopup() _________________________________________________________

bool WgPopupLayer::closePopup( const WgWidget_p& pWidget )
{
	if( !pWidget || pWidget->parent() != this || pWidget == m_baseHook.widget() )
		return false;

	WgMsgRouter * pEH = WgBase::msgRouter().rawPtr();

	WgPopupHook * pHook = (WgPopupHook *) pWidget->_hook();

	while( pHook )
	{
		WgPopupHook * p = pHook;
		pHook = pHook->_next();

		if( pEH )
			pEH->post( new WgPopupClosedMsg( p->_widget(), p->m_pOpener ) );

		p->_requestRender();
		delete p;
	}
	_restoreKeyboardFocus();
	return true;
}

//____ firstPopupHook() ______________________________________________________

WgPopupHook_p WgPopupLayer::firstPopupHook()
{
	return m_popupHooks.first();
}

//____ lastPopupHook() _______________________________________________________

WgPopupHook_p WgPopupLayer::lastPopupHook()
{
	return m_popupHooks.last();
}

//____ _findWidget() ____________________________________________________________

WgWidget *  WgPopupLayer::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	// MenuPanel has its own _findWidget() method since we need special treatment of
	// searchmode ACTION_TARGET when a menu is open.

	if( mode == WG_SEARCH_ACTION_TARGET && !m_popupHooks.isEmpty() )
	{
		// In search mode ACTION_TARGET we limit our target to us, our menu-branches and the menu-opener if a menu is open.

		WgPopupHook * pHook = m_popupHooks.last();
		WgWidget * pResult = 0;

		while( pHook && !pResult )
		{
			if( pHook->m_geo.contains( ofs ) )
			{
				if( pHook->_widget()->IsContainer() )
					pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - pHook->m_geo.pos(), mode );
				else if( pHook->_widget()->markTest( ofs - pHook->m_geo.pos() ) )
					pResult = pHook->_widget();
			}
			pHook = pHook->_prev();
		}

		if( pResult == 0 )
		{
			// Check the first opener
			
			WgPopupHook * pHook = m_popupHooks.first();
			if( pHook && pHook->m_pOpener )
			{
				WgWidget * pOpener = pHook->m_pOpener.rawPtr();

				WgCoord absPos 		= ofs + globalPos();
				WgRect	openerGeo 	= pOpener->globalGeo();

				if( openerGeo.contains(absPos) && pOpener->markTest( absPos - openerGeo.pos() ) )
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
	patches.add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgPopupHook * pCover;

	if( pHook )
		pCover = pHook->_next();
	else
		pCover = m_popupHooks.first();

	while( pCover )
	{
		if( pCover->m_geo.intersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
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

//____ _onMsg() ______________________________________________________________

void WgPopupLayer::_onMsg( const WgMsg_p& _pMsg )
{
	WgLayer::_onMsg(_pMsg);

	WgWidget * pOpener = 0;

	// Try to find an opener

	WgObject * pSource = _pMsg->sourceRawPtr();
	if( pSource && pSource != this )
	{
		WgPopupHook * pHook = m_popupHooks.first();
		while( pHook && pHook->_widget() != pSource )
			pHook = pHook->_next();
			
		if( pHook && pHook->m_pOpener )
			pOpener = pHook->m_pOpener.rawPtr();
	}
	
	// First we try to repost message to opener (if any)

	if( pOpener )
	{
		_pMsg->setRepost( _pMsg->source().rawPtr(), pOpener );
		return;
	}	

	// Secondly we take care of message ourselves if it is addressed to one of our menus or us.

	switch( _pMsg->type() )
	{
/*
		case WG_MSG_MOUSE_POSITION:

			if( !m_popupHooks.isEmpty() )							// Process only if we have at least one open menu.
			{
				WgCoord ofs = _pMsg->pointerPos();
				WgWidget * p = _findWidget( ofs, WG_SEARCH_ACTION_TARGET );
				if( p != this )
				{
					while( p->parent() != this )
						p = p->parent();
						
					if( p != m_popupHooks.	
				}	
			}
		break;
*/		
		case WG_MSG_MOUSE_RELEASE:
		case WG_MSG_MOUSE_PRESS:
		{
			WgMouseButtonMsg_p pMsg = WgMouseButtonMsg::cast(_pMsg);

			WgCoord ofs = pMsg->pointerPos() - globalPos();
			WgWidget * p = _findWidget( ofs, WG_SEARCH_ACTION_TARGET );
			if( p == this )
			{
				closeAllPopups();
				_pMsg->swallow();
				return;
			}
		}
		break;

		case WG_MSG_KEY_PRESS:
		case WG_MSG_KEY_REPEAT:
		{
			WgKeyMsg_p pMsg = WgKeyMsg::cast(_pMsg);

			if( pMsg->translatedKeyCode() == WG_KEY_ESCAPE )
			{
				if( !m_popupHooks.isEmpty() )
				{
					closePopup( m_popupHooks.last()->_widget() );
					_pMsg->swallow();
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

	if( !hook() )
		return;

	WgMsgRouter_p pHandler = hook()->msgRouter();
	if( !pHandler )
		return;

	// Save old keyboard focus, which we assume belonged to previous menu in hierarchy.

	if( m_popupHooks.size() < 2 )
		m_pKeyFocus = pHandler->keyboardFocus().rawPtr();
	else
		m_popupHooks.last()->prev()->m_pKeyFocus = pHandler->keyboardFocus().rawPtr();

	// Steal keyboard focus to top menu

	WgWidget * pWidget = m_popupHooks.last()->_widget();

	if( pWidget->isInstanceOf( WgPanel::CLASSNAME ) && static_cast<WgPanel*>(pWidget)->isFocusGroup() )
		pHandler->setFocusGroup(static_cast<WgPanel*>(pWidget));
	else
		pHandler->setKeyboardFocus(pWidget);
}

//____ _restoreKeyboardFocus() _________________________________________________

void WgPopupLayer::_restoreKeyboardFocus()
{
	// Get event handler, verify that we have a root

	if( !hook() )
		return;

	WgMsgRouter_p pHandler = hook()->msgRouter();
	if( !pHandler )
		return;

	//

	if( m_popupHooks.isEmpty() )
		pHandler->setKeyboardFocus( m_pKeyFocus.rawPtr() );
	else
		pHandler->setKeyboardFocus( m_popupHooks.last()->m_pKeyFocus.rawPtr() );
}

