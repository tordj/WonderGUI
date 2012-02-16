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

#include <wg_gizmo_modal.h>
#include <wg_util.h>
#include <wg_patches.h>

static const char	c_gizmoType[] = {"Modal"};

// Improve WgModalHook geometry handling, should be able to run on BestSize by default, answering to resize-requests.


//_____________________________________________________________________________
void WgModalHook::Top()
{
	_moveLast();
	_requestRender();
}

//_____________________________________________________________________________
bool WgModalHook::SetGeo( const WgRect& geometry, WgOrientation origo )
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
bool WgModalHook::SetGeo( const WgCoord& ofs, WgOrientation origo )
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
WgCoord WgModalHook::ScreenPos() const
{
	return m_pParent->ScreenPos() + m_realGeo.Pos();
}

//_____________________________________________________________________________
WgRect WgModalHook::ScreenGeo() const
{
	return m_realGeo + m_pParent->ScreenPos();
}

//_____________________________________________________________________________
WgGizmoContainer* WgModalHook::Parent() const
{
	return m_pParent;
}

//_____________________________________________________________________________
WgWidget* WgModalHook::GetRoot()
{
	return 0;
}

//_____________________________________________________________________________
WgModalHook::WgModalHook( WgGizmoModal * pParent )
{
	m_pParent = pParent;
}

//_____________________________________________________________________________
bool WgModalHook::_refreshRealGeo()	// Return false if we couldn't get exactly the requested (floating) geometry.
{
	WgSize sz = m_placementGeo.Size();

	if( sz.w == 0 && sz.h == 0 )
		sz = m_pGizmo->DefaultSize();
	else if( sz.w == 0 )
		sz.w = m_pGizmo->WidthForHeight(sz.h);
	else if( sz.h == 0 )
		sz.h = m_pGizmo->HeightForWidth(sz.w);

	if( sz.w <= 0 )
		sz.w = 1;
	if( sz.h <= 0 )
		sz.h = 1;

	WgCoord ofs = WgUtil::OrientationToOfs( m_origo, m_pParent->Size() ) - WgUtil::OrientationToOfs( m_origo, sz );
	ofs += m_placementGeo.Pos();

	WgRect newGeo( ofs, sz );

	if( newGeo != m_realGeo )
	{
		_requestRender();
		m_realGeo = WgRect( ofs, sz );
		_requestRender();
	}

	return true;
}

//_____________________________________________________________________________
void WgModalHook::_requestRender()
{
	m_pParent->_onRequestRender( m_realGeo, this );
}

//_____________________________________________________________________________
void WgModalHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect + m_realGeo.Pos(), this );
}

//_____________________________________________________________________________
void WgModalHook::_requestResize()
{
	_refreshRealGeo();
}

//_____________________________________________________________________________
WgHook * WgModalHook::_prevHook() const
{
	WgModalHook * p = _prev();

	if( p )
		return p;
	else if( m_pParent->m_baseHook.Gizmo() )
		return &m_pParent->m_baseHook;
	else
		return 0;
}

//_____________________________________________________________________________
WgHook * WgModalHook::_nextHook() const
{
	WgModalHook * p = _prev();

	// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.

	if( p )
		return p;
	else
		return 0;
}

//_____________________________________________________________________________
WgGizmoContainer * WgModalHook::_parent() const
{
	return m_pParent;
}

//_____________________________________________________________________________
void WgGizmoModal::BaseHook::_requestRender()
{
	if( !m_bHidden )
		m_pParent->_onRequestRender( WgRect( 0,0, m_pParent->m_size ), 0 );
}

//_____________________________________________________________________________
void WgGizmoModal::BaseHook::_requestRender( const WgRect& rect )
{
	if( !m_bHidden )
		m_pParent->_onRequestRender( rect, 0 );
}

//_____________________________________________________________________________
void WgGizmoModal::BaseHook::_requestResize()
{
	m_pParent->_requestResize();					// Just forward to our parent
}

//____ Constructor ____________________________________________________________

WgGizmoModal::WgGizmoModal() : m_baseHook(this)
{
}

//____ Destructor _____________________________________________________________

WgGizmoModal::~WgGizmoModal()
{
	// Children are deleted automaticallly when their hooks are deteled.
}

//____ Type() _________________________________________________________________

const char *WgGizmoModal::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoModal::GetMyType()
{
	return c_gizmoType;
}

//____ SetBase() _________________________________________________________

WgHook * WgGizmoModal::SetBase( WgGizmo * pGizmo )
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

WgGizmo * WgGizmoModal::Base()
{
	return m_baseHook.Gizmo();
}

//____ DeleteBase() ______________________________________________________

bool WgGizmoModal::DeleteBase()
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

WgGizmo * WgGizmoModal::ReleaseBase()
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

//____ AddModal() ________________________________________________________

WgModalHook * WgGizmoModal::AddModal( WgGizmo * pGizmo, const WgRect& geometry, WgOrientation origo )
{
	// Create Hook and fill in members.

	WgModalHook * pHook = new WgModalHook( this );
	pHook->_attachGizmo(pGizmo);
	pHook->m_origo = origo;
	pHook->m_placementGeo = geometry;
	m_modalHooks.PushBack(pHook);

	// Refresh geometry and request render.

	pHook->_refreshRealGeo();
	return pHook;
}

//____ DeleteAllModal() _________________________________________________

bool WgGizmoModal::DeleteAllModal()
{
	m_modalHooks.Clear();
	_requestRender();
	return true;
}

//____ ReleaseAllModal() ________________________________________________

bool WgGizmoModal::ReleaseAllModal()
{
	WgModalHook * pHook = m_modalHooks.First();
	while( pHook )
	{
		pHook->_releaseGizmo();
		pHook = pHook->_next();
	}

	m_modalHooks.Clear();
	_requestRender();
	return true;
}

//____ DeleteChild() __________________________________________________________

bool WgGizmoModal::DeleteChild( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->ParentX() != this )
		return false;

	if( pGizmo == m_baseHook.Gizmo() )
		return DeleteBase();
	else
	{
		WgModalHook * pHook = (WgModalHook *) pGizmo->Hook();
		pHook->_requestRender();
		delete pHook;
		return true;
	}
}

//____ ReleaseChild() _________________________________________________________

WgGizmo * WgGizmoModal::ReleaseChild( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->ParentX() != this )
		return 0;

	if( pGizmo == m_baseHook.Gizmo() )
		return ReleaseBase();
	else
	{
		WgModalHook * pHook = (WgModalHook *) pGizmo->Hook();
		pHook->_requestRender();
		pHook->_releaseGizmo();
		delete pHook;
		return pGizmo;
	}

}

//____ DeleteAllChildren() ______________________________________________________

bool WgGizmoModal::DeleteAllChildren()
{
	DeleteBase();
	DeleteAllModal();
	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgGizmoModal::ReleaseAllChildren()
{
	ReleaseBase();
	ReleaseAllModal();
	return true;
}

//____ FirstModal() ______________________________________________________

WgModalHook * WgGizmoModal::FirstModal()
{
	return m_modalHooks.First();
}

//____ LastModal() _______________________________________________________

WgModalHook * WgGizmoModal::LastModal()
{
	return m_modalHooks.Last();
}

//____ HeightForWidth() _______________________________________________________

int WgGizmoModal::HeightForWidth( int width ) const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->HeightForWidth( width );
	else
		return WgGizmo::HeightForWidth(width);
}

//____ WidthForHeight() _______________________________________________________

int WgGizmoModal::WidthForHeight( int height ) const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->WidthForHeight( height );
	else
		return WgGizmo::WidthForHeight(height);
}

//____ DefaultSize() _____________________________________________________________

WgSize WgGizmoModal::DefaultSize() const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->DefaultSize();
	else
		return WgSize(1,1);
}

//____ FindGizmo() ____________________________________________________________

WgGizmo *  WgGizmoModal::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	// In search mode ACTION_TARGET we always return the topmost non-hidden modal Gizmo (or its children)
	// no matter its geometry.

	if( mode == WG_SEARCH_ACTION_TARGET )
	{
		WgModalHook * pHook = m_modalHooks.Last();

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

//____ _onRequestRender() _____________________________________________________

void WgGizmoModal::_onRequestRender( const WgRect& rect, const WgModalHook * pHook )
{
	if( pHook && pHook->m_bHidden )
		return;

	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgModalHook * pCover;

	if( pHook )
		pCover = pHook->Next();
	else
		pCover = m_modalHooks.First();

	while( pCover )
	{
		if( !pCover->m_bHidden && pCover->m_realGeo.IntersectsWith( rect ) )
			pCover->Gizmo()->_onMaskPatches( patches, pCover->m_realGeo, WgRect(0,0,65536,65536 ) );

		pCover = pCover->Next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		_requestRender( * pRect );
}

//____ _onNewSize() ___________________________________________________________

void WgGizmoModal::_onNewSize( const WgSize& sz )
{
	m_size = sz;

	// Update size of base gizmo

	if( m_baseHook.Gizmo() )
		m_baseHook.Gizmo()->_onNewSize(sz);

	// Refresh modal gizmos geometry, their positions might have changed.

	WgModalHook * pHook = m_modalHooks.First();

	while( pHook )
	{
		pHook->_refreshRealGeo();
		pHook = pHook->_next();
	}
}

//____ _onCloneContent() ______________________________________________________

void WgGizmoModal::_onCloneContent( const WgGizmo * _pOrg )
{
}

//____ _onAction() ____________________________________________________________

void WgGizmoModal::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

//____ _firstHook() ___________________________________________________________

WgHook* WgGizmoModal::_firstHook() const
{
	if( m_baseHook.Gizmo() )
		return const_cast<BaseHook*>(&m_baseHook);
	else
		return m_modalHooks.First();
}

//____ _lastHook() ____________________________________________________________

WgHook* WgGizmoModal::_lastHook() const
{
	return m_modalHooks.Last();
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgGizmoModal::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_baseHook.Gizmo() )
	{
		geo = WgRect(0,0,m_size);
		return const_cast<BaseHook*>(&m_baseHook);
	}
	else
	{
		WgModalHook * p = m_modalHooks.First();
		if( p )
			geo = p->m_realGeo;

		return p;
	}
}

//____ _nextHookWithGeo() _______________________________________________________

WgHook * WgGizmoModal::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->Next();
	if( p )
		geo = ((WgModalHook*)p)->m_realGeo;

	return p;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgGizmoModal::_lastHookWithGeo( WgRect& geo ) const
{
	WgModalHook * p = m_modalHooks.Last();
	if( p )
	{
		geo = p->m_realGeo;
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

WgHook * WgGizmoModal::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgHook * p = pHook->Prev();
	if( p )
		geo = p->Geo();

	return p;
}


