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

#include <wg_gizmo_menulayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"MenuLayer"};

//_____________________________________________________________________________
WgCoord WgMenuHook::ScreenPos() const
{
	return m_pParent->ScreenPos() + m_geo.Pos();
}

//_____________________________________________________________________________
WgRect WgMenuHook::ScreenGeo() const
{
	return m_geo + m_pParent->ScreenPos();
}

//_____________________________________________________________________________
WgGizmoContainer* WgMenuHook::Parent() const
{
	return m_pParent;
}

//_____________________________________________________________________________
WgWidget* WgMenuHook::GetRoot()
{
	return 0;
}

//_____________________________________________________________________________
WgMenuHook::WgMenuHook( WgGizmoMenuLayer * pParent )
{
	m_pParent = pParent;
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
	_refreshRealGeo();
}

//_____________________________________________________________________________
WgHook * WgMenuHook::_prevHook() const
{
	WgMenuHook * p = _prev();

	if( p )
		return p;
	else if( m_pParent->m_baseHook.Gizmo() )
		return &m_pParent->m_baseHook;
	else
		return 0;
}

//_____________________________________________________________________________
WgHook * WgMenuHook::_nextHook() const
{
	WgMenuHook * p = _prev();

	// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.

	if( p )
		return p;
	else
		return 0;
}

//_____________________________________________________________________________
WgGizmoContainer * WgMenuHook::_parent() const
{
	return m_pParent;
}

//_____________________________________________________________________________
void WgGizmoMenuLayer::BaseHook::_requestRender()
{
	if( !m_bHidden )
		m_pParent->_onRequestRender( WgRect( 0,0, m_pParent->m_size ), 0 );
}

//_____________________________________________________________________________
void WgGizmoMenuLayer::BaseHook::_requestRender( const WgRect& rect )
{
	if( !m_bHidden )
		m_pParent->_onRequestRender( rect, 0 );
}

//_____________________________________________________________________________
void WgGizmoMenuLayer::BaseHook::_requestResize()
{
	m_pParent->_requestResize();					// Just forward to our parent
}

//____ Constructor ____________________________________________________________

WgGizmoMenuLayer::WgGizmoMenuLayer() : m_baseHook(this)
{
}

//____ Destructor _____________________________________________________________

WgGizmoMenuLayer::~WgGizmoMenuLayer()
{
	// Children are deleted automaticallly when their hooks are deteled.
}

//____ Type() _________________________________________________________________

const char *WgGizmoMenuLayer::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoMenuLayer::GetMyType()
{
	return c_gizmoType;
}

//____ SetBase() _________________________________________________________

WgHook * WgGizmoMenuLayer::SetBase( WgGizmo * pGizmo )
{
	// Replace Gizmo

	WgGizmo * pOldGizmo = m_baseHook._releaseGizmo();
	if( pOldGizmo )
		delete pOldGizmo;
	m_baseHook._attachGizmo(pGizmo);
	if( !m_baseHook.m_bHidden )
		_onRequestRender( WgRect(0,0,m_size), 0 );

	// Notify that we might want a new size now...

	_requestResize();
	return &m_baseHook;
}

//____ Base() ____________________________________________________________

WgGizmo * WgGizmoMenuLayer::Base()
{
	return m_baseHook.Gizmo();
}

//____ DeleteBase() ______________________________________________________

bool WgGizmoMenuLayer::DeleteBase()
{
	WgGizmo * pGizmo = m_baseHook._releaseGizmo();
	if( pGizmo )
	{
		if( !m_baseHook.m_bHidden )
			_onRequestRender( WgRect(0,0,m_size), 0 );
		_requestResize();
		delete pGizmo;
		return true;
	}

	return false;
}

//____ ReleaseBase() _____________________________________________________

WgGizmo * WgGizmoMenuLayer::ReleaseBase()
{
	WgGizmo * pGizmo = m_baseHook._releaseGizmo();
	if( pGizmo )
	{
		if( !m_baseHook.m_bHidden )
			_onRequestRender( WgRect(0,0,m_size), 0 );
		_requestResize();
	}

	return pGizmo;
}

//____ AddMenu() _______________________________________________________________

WgMenuHook * WgGizmoMenuLayer::AddMenu( WgGizmo * pMenu, const WgRect& launcherGeo, WgOrientation attachPoint, WgSize maxSize )
{
	// Create Hook and fill in members.

	WgMenuHook * pHook = new WgMenuHook( this );
	pHook->_attachGizmo(pMenu);
	pHook->m_bIsMenu = true;
	pHook->m_origo = WG_NORTHWEST;

	WgSize menuSize = pMenu->DefaultSize();

	if( maxSize.w < menuSize.w )
		menuSize.w = maxSize.w;

	if( maxSize.h < menuSize.h )
		menuSize.h = maxSize.h;

	pHook->m_placementGeo = _decideMenuGeo( menuSize, launcherGeo, attachPoint );
	m_menuHooks.PushBack(pHook);

	// Refresh geometry and request render.

	pHook->_refreshRealGeo();
	_updateKeyboardFocus();
	return pHook;

}



//____ ReleaseAllModal() ________________________________________________

bool WgGizmoMenuLayer::ReleaseAllModal()
{
	WgMenuHook * pHook = m_menuHooks.First();
	while( pHook )
	{
		pHook->_releaseGizmo();
		pHook = pHook->_next();
	}

	m_menuHooks.Clear();
	_requestRender();
	_updateKeyboardFocus();
	return true;
}


//____ ReleaseChild() _________________________________________________________

WgGizmo * WgGizmoMenuLayer::ReleaseChild( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->ParentX() != this )
		return 0;

	if( pGizmo == m_baseHook.Gizmo() )
		return ReleaseBase();
	else
	{
		WgMenuHook * pHook = (WgMenuHook *) pGizmo->Hook();
		pHook->_requestRender();
		pHook->_releaseGizmo();
		delete pHook;
		_updateKeyboardFocus();
		return pGizmo;
	}

}

//____ ReleaseAllChildren() _____________________________________________________

bool WgGizmoMenuLayer::ReleaseAllChildren()
{
	ReleaseBase();
	ReleaseAllModal();
	return true;
}

//____ FirstMenu() ______________________________________________________

WgMenuHook * WgGizmoMenuLayer::FirstMenu()
{
	return m_menuHooks.First();
}

//____ LastMenu() _______________________________________________________

WgMenuHook * WgGizmoMenuLayer::LastMenu()
{
	return m_menuHooks.Last();
}

//____ HeightForWidth() _______________________________________________________

int WgGizmoMenuLayer::HeightForWidth( int width ) const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->HeightForWidth( width );
	else
		return WgGizmo::HeightForWidth(width);
}

//____ WidthForHeight() _______________________________________________________

int WgGizmoMenuLayer::WidthForHeight( int height ) const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->WidthForHeight( height );
	else
		return WgGizmo::WidthForHeight(height);
}

//____ DefaultSize() _____________________________________________________________

WgSize WgGizmoMenuLayer::DefaultSize() const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->DefaultSize();
	else
		return WgSize(1,1);
}

//____ FindGizmo() ____________________________________________________________

WgGizmo *  WgGizmoMenuLayer::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	// In search mode ACTION_TARGET we always return the topmost non-hidden modal Gizmo (or its children)
	// no matter its geometry.

	if( mode == WG_SEARCH_ACTION_TARGET )
	{
		WgMenuHook * pHook = m_menuHooks.Last();

		while( pHook && pHook->Hidden() )
			pHook = pHook->Prev();

		if( pHook )
		{
			if( pHook->Gizmo()->IsContainer() )
			{
				WgGizmo * pResult = pHook->Gizmo()->CastToContainer()->FindGizmo( ofs - pHook->Pos(), mode );
				if( pResult )
					return pResult;
			}
			else
				return pHook->Gizmo();
		}
		else if( m_baseHook.Gizmo() && !m_baseHook.Hidden() )
		{
			if( m_baseHook.Gizmo()->IsContainer() )
			{
				WgGizmo * pResult = m_baseHook.Gizmo()->CastToContainer()->FindGizmo( ofs - m_baseHook.Pos(), mode );
				if( pResult )
					return pResult;
			}
			else
				return m_baseHook.Gizmo();
		}
		else
			return 0;
	}

	// For the rest of the modes we can rely on the default method.

	return WgGizmoContainer::FindGizmo( ofs, mode );
}

//____ _decideMenuGeo() __________________________________________________________

WgRect WgGizmoMenuLayer::_decideMenuGeo( WgSize menuSize, const WgRect& launcherGeo, WgOrientation attachPoint )
{
	WgRect parentSize = Size();

	WgRect geo(0,0,menuSize);

	if( geo.w > parentSize.w)
		geo.w = parentSize.w;

	if(geo.h > parentSize.h)
		geo.h = parentSize.h;

	switch( attachPoint )
	{
		case WG_NORTHEAST:					// Right side of launcherGeo, going down.
		{
			geo.x = launcherGeo.Right();
			geo.y = launcherGeo.Top();
			break;
		}

		case WG_SOUTHEAST:					// Right side of launcherGeo, going up.
		{
			geo.x = launcherGeo.Right();
			geo.y = launcherGeo.Bottom() - geo.h;
			break;
		}

		case WG_NORTHWEST:					// Left-aligned above launcher.
		{
			geo.x = launcherGeo.Left();
			geo.y = launcherGeo.Top() - geo.h;
			break;
		}

		case WG_SOUTHWEST:					// Left-aligned below launcher.
		{
			geo.x = launcherGeo.Left();
			geo.y = launcherGeo.Bottom();
			break;
		}

		case WG_EAST:						// Centered left of launcherGeo.
		{
			geo.x = launcherGeo.Left() - geo.w;
			geo.y = launcherGeo.Top() + launcherGeo.h/2 - geo.h/2;
			break;
		}

		case WG_NORTH:						// Centered above launcherGeo.
		{
			geo.x = launcherGeo.Left() + launcherGeo.w/2 + geo.w/2;
			geo.y = launcherGeo.Top() - geo.h;
			break;
		}

		case WG_WEST:						// Centered right of launcherGeo.
		{
			geo.x = launcherGeo.Right();
			geo.y = launcherGeo.Top() + launcherGeo.h/2 - geo.h/2;
			break;
		}

		case WG_SOUTH:						// Centered below launcherGeo.
		{
			geo.x = launcherGeo.Left() + launcherGeo.w/2 + geo.w/2;
			geo.y = launcherGeo.Bottom();
			break;
		}

	}

	// Adjust geometry to fit inside parent.

	if( geo.Right() > parentSize.w )
	{
		if( geo.Left() == launcherGeo.Right() )
		{
			if( launcherGeo.Left() > parentSize.w - launcherGeo.Right() )
			{
				geo.x = launcherGeo.Left() - geo.w;
				if( geo.x < 0 )
				{
					geo.x = 0;
					geo.w = launcherGeo.Left();
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
		if( geo.Right() == launcherGeo.Left() )
		{
			if( launcherGeo.Left() < parentSize.w - launcherGeo.Right() )
			{
				geo.x = launcherGeo.Right();
				if( geo.Right() > parentSize.w )
					geo.w = parentSize.w - geo.x;
			}
			else
			{
				geo.x = 0;
				geo.w = launcherGeo.Left();
			}

		}
		else
			geo.x = 0;
	}

	if( geo.Bottom() > parentSize.h )
	{
		if( geo.Top() == launcherGeo.Bottom() )
		{
			if( launcherGeo.Top() > parentSize.h - launcherGeo.Bottom() )
			{
				geo.y = launcherGeo.Top() - geo.h;
				if( geo.y < 0 )
				{
					geo.y = 0;
					geo.h = launcherGeo.Top();
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
		if( geo.Bottom() == launcherGeo.Top() )
		{
			if( launcherGeo.Top() < parentSize.h - launcherGeo.Bottom() )
			{
				geo.y = launcherGeo.Bottom();
				if( geo.Bottom() > parentSize.h )
					geo.h = parentSize.h - geo.y;
			}
			else
			{
				geo.y = 0;
				geo.h = launcherGeo.Bottom();
			}
		}
		else
			geo.y = 0;
	}

	//

	return geo;
}

//____ _updateKeyboardFocus() _______________________________________________________

void WgGizmoMenuLayer::_updateKeyboardFocus()
{
	// Get event handler, verify that we have a root

	if( !Hook() )
		return;

	WgEventHandler * pHandler = Hook()->EventHandler();
	if( !pHandler )
		return;

	// Retrieve focused Gizmo and verify it being a descendant to us.

	WgGizmo * pFocused = pHandler->KeyboardFocus();

	WgGizmo * p = pFocused;
	while( p && p->ParentX() && p->ParentX() != this )
		p = p->ParentX()->CastToGizmo();

	if( p && p->ParentX() != this )
		return;								// Focus belongs to a Gizmo that is not a descendant to us,
											// so we can't save and shouldn't steal focus.

	// Save old focus so we can return it properly in the future.
	if( p )
	{
		if( p == m_baseHook.Gizmo() )
			m_baseHook.m_pKeyFocus = pFocused;
		else
		{
			WgMenuHook * pHook = static_cast<WgMenuHook*>(p->Hook());
			pHook->m_pKeyFocus = pFocused;
		}
	}

	// Find which child-branch to focus and switch to our previously saved focus

	WgMenuHook * pHook = m_menuHooks.Last();

	while( pHook && pHook->Hidden() )
		pHook = pHook->Prev();

	WgGizmo * 	pSavedFocus = 0;
	WgHook *	pBranch	= 0;

	if( pHook )
	{
		pSavedFocus = pHook->m_pKeyFocus.GetRealPtr();
		pHook->m_pKeyFocus = 0;								// Needs to be cleared for the future.
		pBranch = pHook;
	}
	else if( m_baseHook.Gizmo() && !m_baseHook.Hidden() )
	{
		pSavedFocus = m_baseHook.m_pKeyFocus.GetRealPtr();
		m_baseHook.m_pKeyFocus = 0;							// Needs to be cleared for the future.
		pBranch = &m_baseHook;
	}

	// Verify that saved focus still is within branch and is not hidden

	if( pSavedFocus )
	{
		WgHook * p = pSavedFocus->Hook();
		while( p && p != pBranch )
		{
			if( p->Hidden() )
				p = 0;						// Branch is hidden so we can not focus saved Gizmo.
			else
			{
				WgGizmoParent * pParent = p->Parent();
				if( pParent && pParent->CastToGizmo() )
					p = pParent->CastToGizmo()->Hook();
				else
					p = 0;
			}
		}

		if( p != pBranch )
			pSavedFocus = 0;				// Previously focused Gizmo is no longer a child of focused branch.
	}

	// Switch to previously saved focus, or null if not applicable

	pHandler->SetKeyboardFocus( pSavedFocus );


}

//____ _onRequestRender() _____________________________________________________

void WgGizmoMenuLayer::_onRequestRender( const WgRect& rect, const WgMenuHook * pHook )
{
	if( pHook && pHook->m_bHidden )
		return;

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
		if( !pCover->m_bHidden && pCover->m_geo.IntersectsWith( rect ) )
			pCover->Gizmo()->_onMaskPatches( patches, pCover->m_geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->Next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		_requestRender( * pRect );
}

//____ _onNewSize() ___________________________________________________________

void WgGizmoMenuLayer::_onNewSize( const WgSize& sz )
{
	m_size = sz;

	// Update size of base gizmo

	if( m_baseHook.Gizmo() )
		m_baseHook.Gizmo()->_onNewSize(sz);

	// Refresh modal gizmos geometry, their positions might have changed.

	WgMenuHook * pHook = m_menuHooks.First();

	while( pHook )
	{
		pHook->_refreshRealGeo();
		pHook = pHook->_next();
	}
}

//____ _onCloneContent() ______________________________________________________

void WgGizmoMenuLayer::_onCloneContent( const WgGizmo * _pOrg )
{
}

//____ _onEvent() ______________________________________________________________

void WgGizmoMenuLayer::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
{
	switch( _pEvent->Type() )
	{
		case WG_EVENT_MOUSEBUTTON_RELEASE:
		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			WgEvent::MouseButtonEvent * pEvent = static_cast<const WgEvent::MouseButtonEvent*>(_pEvent);



		}
		break;

		case WG_EVENT_MOUSE_ENTER:
		case WG_EVENT_MOUSE_LEAVE:
		{
			WgEvent::MouseEvent * pEvent = static_cast<const WgEvent::MouseEvent*>(_pEvent);
		}
		break;

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_REPEAT:
		{
			WgEvent::KeyEvent * pEvent = static_cast<const WgEvent::KeyEvent*>(_pEvent);

			int key = pEvent->TranslatedKeyCode();
			switch( key )
			{
				case WG_KEY_ESCAPE:
					if( _getModalMenu() )
						_getModalMenu()->ReleaseChild( _getModalMenu()->Gizmo() );
				break;
				case WG_KEY_LEFT:
				break;
				case WG_KEY_RIGHT;
				break;

				default:
			}

		}
		break;
}

//____ _firstHook() ___________________________________________________________

WgHook* WgGizmoMenuLayer::_firstHook() const
{
	if( m_baseHook.Gizmo() )
		return const_cast<BaseHook*>(&m_baseHook);
	else
		return m_menuHooks.First();
}

//____ _lastHook() ____________________________________________________________

WgHook* WgGizmoMenuLayer::_lastHook() const
{
	return m_menuHooks.Last();
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgGizmoMenuLayer::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_baseHook.Gizmo() )
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

WgHook * WgGizmoMenuLayer::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->Next();
	if( p )
		geo = ((WgMenuHook*)p)->m_geo;

	return p;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgGizmoMenuLayer::_lastHookWithGeo( WgRect& geo ) const
{
	WgMenuHook * p = m_menuHooks.Last();
	if( p )
	{
		geo = p->m_geo;
		return p;
	}
	else if( m_baseHook.Gizmo() )
	{
		geo = WgRect(0,0,m_size);
		return const_cast<BaseHook*>(&m_baseHook);
	}
	else
		return 0;
}

//____ _prevHookWithGeo() _______________________________________________________

WgHook * WgGizmoMenuLayer::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->Prev();
	if( p )
		geo = p->Geo();

	return p;
}


