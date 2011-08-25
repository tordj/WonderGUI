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

static const char	c_gizmoType[] = {"Modal"};

// Improve WgModalHook geometry handling, should be able to run on BestSize by default, answering to resize-requests.


//_____________________________________________________________________________
void WgModalHook::Top()
{
	_moveLast();
	RequestRender();
}

//_____________________________________________________________________________
bool WgModalHook::SetGeo( const WgRect& geometry, WgLocation origo )
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
bool WgModalHook::SetGeo( const WgCord& ofs, WgLocation origo )
{
	m_placementGeo.setPos(ofs);
	m_origo	= origo;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetOfs( const WgCord& ofs )
{
	m_placementGeo.setPos(ofs);
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

	m_placementGeo.setSize( sz );
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
bool WgModalHook::Move( const WgCord& ofs )
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
WgCord WgModalHook::ScreenPos() const
{
	return m_pParent->ScreenPos() + m_realGeo.pos();
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
	WgSize sz = m_placementGeo.size();

	if( sz.w == 0 && sz.h == 0 )
		sz = m_pGizmo->BestSize();
	else if( sz.w == 0 )
		sz.w = m_pGizmo->WidthForHeight(sz.h);
	else if( sz.h == 0 )
		sz.h = m_pGizmo->HeightForWidth(sz.w);

	if( sz.w <= 0 )
		sz.w = 1;
	if( sz.h <= 0 )
		sz.h = 1;

	WgCord ofs = WgUtil::LocationToOfs( m_origo, m_pParent->Size() ) - WgUtil::LocationToOfs( m_origo, sz );
	ofs += m_placementGeo.pos();

	WgRect newGeo( ofs, sz );

	if( newGeo != m_realGeo )
	{
		RequestRender();
		m_realGeo = WgRect( ofs, sz );
		RequestRender();
	}

	return true;
}

//_____________________________________________________________________________
void WgModalHook::RequestRender()
{
	m_pParent->_onRequestRender( m_realGeo, this );
}

//_____________________________________________________________________________
void WgModalHook::RequestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect + m_realGeo.pos(), this );
}

//_____________________________________________________________________________
void WgModalHook::RequestResize()
{
	_refreshRealGeo();
}

//_____________________________________________________________________________
void WgModalHook::_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut )
{
	WgRectChain	siblingDirt;

	// Recurse through the siblings ontop of us if there are any, filling dirt

	if( NextHook() )
	{
		NextHook()->_castDirtRecursively( parentGeo, clip, pDirtIn, &siblingDirt );
	}
	else
		siblingDirt.PushExistingRect( pDirtIn );

	// Get our screen geometry and clippedArea.

	WgRect screenGeo = m_realGeo + parentGeo.pos();
	WgRect clippedArea( screenGeo, clip );

	// If we are outside visible bounds, we just transfer the dirt and return.

	if( clippedArea.w == 0 || clippedArea.h == 0 )
	{
		siblingDirt.Transfer( pDirtOut) ;
		return;
	}

	// Loop through dirty rects

	WgRectLink * pRect = siblingDirt.Pop();;
	while( pRect )
	{
		WgRect dirtyArea( clippedArea, *pRect );
		if( dirtyArea.w != 0 && dirtyArea.h != 0 )
		{

			if( m_pGizmo->IsContainer() )
			{
				// This is a container, call CastDirt recursively,

				m_pGizmo->CastToContainer()->_castDirtyRect( screenGeo, clippedArea, pRect, pDirtOut );
			}
			else
			{
				// Add dirtyArea to our list of what to render

				m_dirt.Add( dirtyArea );

				// Mask ourselves from the rectangle and move remains to dirtOut

				WgRectChain temp;
				temp.PushExistingRect( pRect );
				m_pGizmo->_onMaskRects( temp, screenGeo, clippedArea );
				temp.Transfer( pDirtOut);
			}
		}
		else
		{
			pDirtOut->PushExistingRect( pRect );
		}

		pRect = siblingDirt.Pop();
	}
}

//_____________________________________________________________________________
WgGizmoHook * WgModalHook::_prevHook() const
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
WgGizmoHook * WgModalHook::_nextHook() const
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
void WgGizmoModal::BaseHook::RequestRender()
{
	m_pParent->_onRequestRender( WgRect( 0,0, m_pParent->m_size ), 0 );
}

//_____________________________________________________________________________
void WgGizmoModal::BaseHook::RequestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect, 0 );
}

//_____________________________________________________________________________
void WgGizmoModal::BaseHook::RequestResize()
{
	m_pParent->RequestResize();					// Just forward to our parent
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

//____ SetBaseGizmo() _________________________________________________________

WgGizmoHook * WgGizmoModal::SetBaseGizmo( WgGizmo * pGizmo )
{
	// Replace Gizmo

	WgGizmo * pOldGizmo = m_baseHook._releaseGizmo();
	if( pOldGizmo )
		delete pOldGizmo;
	m_baseHook._attachGizmo(pGizmo);
	_onRequestRender( WgRect(0,0,m_size), 0 );

	// Notify that we might want a new size now...

	RequestResize();
	return &m_baseHook;
}

//____ BaseGizmo() ____________________________________________________________

WgGizmo * WgGizmoModal::BaseGizmo()
{
	return m_baseHook.Gizmo();
}

//____ DeleteBaseGizmo() ______________________________________________________

bool WgGizmoModal::DeleteBaseGizmo()
{
	WgGizmo * pGizmo = m_baseHook._releaseGizmo();
	if( pGizmo )
	{
		delete pGizmo;
		_onRequestRender( WgRect(0,0,m_size), 0 );
		RequestResize();
		return true;
	}

	return false;
}

//____ ReleaseBaseGizmo() _____________________________________________________

WgGizmo * WgGizmoModal::ReleaseBaseGizmo()
{
	WgGizmo * pGizmo = m_baseHook._releaseGizmo();
	if( pGizmo )
	{
		_onRequestRender( WgRect(0,0,m_size), 0 );
		RequestResize();
	}

	return pGizmo;
}

//____ AddModalGizmo() ________________________________________________________

WgModalHook * WgGizmoModal::AddModalGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation origo )
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

//____ DeleteAllModalGizmos() _________________________________________________

bool WgGizmoModal::DeleteAllModalGizmos()
{
	m_modalHooks.Clear();
	RequestRender();
	return true;
}

//____ ReleaseAllModalGizmos() ________________________________________________

bool WgGizmoModal::ReleaseAllModalGizmos()
{
	WgModalHook * pHook = m_modalHooks.First();
	while( pHook )
	{
		pHook->_releaseGizmo();
		pHook = pHook->_next();
	}

	m_modalHooks.Clear();
	RequestRender();
	return true;
}

//____ DeleteGizmo() __________________________________________________________

bool WgGizmoModal::DeleteGizmo( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->ParentX() != this )
		return false;

	if( pGizmo == m_baseHook.Gizmo() )
		return DeleteBaseGizmo();
	else
	{
		WgModalHook * pHook = (WgModalHook *) pGizmo->Hook();
		pHook->RequestRender();
		delete pHook;
		return true;
	}
}

//____ ReleaseGizmo() _________________________________________________________

WgGizmo * WgGizmoModal::ReleaseGizmo( WgGizmo * pGizmo )
{
	if( !pGizmo || pGizmo->ParentX() != this )
		return 0;

	if( pGizmo == m_baseHook.Gizmo() )
		return ReleaseBaseGizmo();
	else
	{
		WgModalHook * pHook = (WgModalHook *) pGizmo->Hook();
		pHook->RequestRender();
		pHook->_releaseGizmo();
		delete pHook;
		return pGizmo;
	}

}

//____ DeleteAllGizmos() ______________________________________________________

bool WgGizmoModal::DeleteAllGizmos()
{
	DeleteBaseGizmo();
	DeleteAllModalGizmos();
	return true;
}

//____ ReleaseAllGizmos() _____________________________________________________

bool WgGizmoModal::ReleaseAllGizmos()
{
	ReleaseBaseGizmo();
	ReleaseAllGizmos();
	return true;
}

//____ FirstMocalGizmo() ______________________________________________________

WgModalHook * WgGizmoModal::FirstModalGizmo()
{
	return m_modalHooks.First();
}

//____ LastModalGizmo() _______________________________________________________

WgModalHook * WgGizmoModal::LastModalGizmo()
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

//____ MinSize() ______________________________________________________________

WgSize WgGizmoModal::MinSize() const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->MinSize();
	else
		return WgGizmo::MinSize();
}

//____ BestSize() _____________________________________________________________

WgSize WgGizmoModal::BestSize() const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->BestSize();
	else
		return WgGizmo::BestSize();
}

//____ MaxSize() ______________________________________________________________

WgSize WgGizmoModal::MaxSize() const
{
	if( m_baseHook.Gizmo() )
		return m_baseHook.Gizmo()->MaxSize();
	else
		return WgGizmo::MaxSize();
}

//____ FindGizmo() ____________________________________________________________

WgGizmo *  WgGizmoModal::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{


	WgModalHook * pHook = m_modalHooks.Last();

	// In search mode ACTION_TARGET we always return the topmost Gizmo or its children.

	if( mode == WG_SEARCH_ACTION_TARGET && pHook )
	{
		if( pHook->Gizmo()->IsContainer() )
		{
			WgGizmo * pResult = pHook->Gizmo()->CastToContainer()->FindGizmo( ofs - pHook->m_realGeo.pos(), mode );
			if( pResult )
				return pResult;
		}

		return pHook->Gizmo();
	}


	// Test against all our modal gizmos

	while( pHook )
	{
		if( pHook->m_realGeo.contains(ofs) )
		{
			if( pHook->Gizmo()->IsContainer() )
			{
				WgGizmo * pResult = pHook->Gizmo()->CastToContainer()->FindGizmo( ofs - pHook->m_realGeo.pos(), mode );
				if( pResult )
					return pResult;
			}
			else if( mode == WG_SEARCH_GEOMETRY )
				return pHook->Gizmo();
			else if( pHook->Gizmo()->MarkTest( ofs - pHook->m_realGeo.pos() ) )
				return pHook->Gizmo();
		}
		pHook = pHook->_prev();
	}

	// Test against our base Gizmo.

	if( m_baseHook.Gizmo() )
	{
		if( m_baseHook.Gizmo()->IsContainer() )
		{
			WgGizmo * pResult = m_baseHook.Gizmo()->CastToContainer()->FindGizmo( ofs, mode );
			if( pResult )
				return pResult;
		}
		else if( mode == WG_SEARCH_GEOMETRY )
			return m_baseHook.Gizmo();
		else if( m_baseHook.Gizmo()->MarkTest(ofs) )
				return m_baseHook.Gizmo();
	}

	// Return us if search mode is GEOMETRY

	if( mode == WG_SEARCH_GEOMETRY )
		return this;

	return 0;
}

//____ _onCollectRects() ______________________________________________________

void WgGizmoModal::_onCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	if( m_baseHook.Gizmo() )
		m_baseHook.Gizmo()->_onCollectRects( rects, geo, clip );

	WgModalHook * pHook = m_modalHooks.First();
	while( pHook )
	{
		pHook->Gizmo()->_onCollectRects( rects, pHook->m_realGeo + geo.pos(), clip );
		pHook = pHook->NextHook();
	}
}

//____ _onMaskRects() _________________________________________________________

void WgGizmoModal::_onMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	if( m_baseHook.Gizmo() )
		m_baseHook.Gizmo()->_onMaskRects( rects, geo, clip );

	WgModalHook * pHook = m_modalHooks.First();
	while( pHook )
	{
		pHook->Gizmo()->_onMaskRects( rects, pHook->m_realGeo + geo.pos(), clip );
		pHook = pHook->NextHook();
	}
}

//____ _onRequestRender() _____________________________________________________

void WgGizmoModal::_onRequestRender( const WgRect& rect, const WgModalHook * pHook )
{
	// Clip our geometry and put it in a dirtyrect-list

	WgRectChain rects;
	rects.Add( WgRect( rect, WgRect(0,0,m_size)) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgModalHook * pCover;

	if( pHook )
		pCover = pHook->NextHook();
	else
		pCover = m_modalHooks.First();

	while( pCover )
	{
		if( pCover->m_realGeo.intersectsWith( rect ) )
			pCover->Gizmo()->_onMaskRects( rects, pCover->m_realGeo, WgRect(0,0,65536,65536 ) );

		pCover = pCover->NextHook();
	}

	// Make request render calls

	WgRectLink * pRect = rects.pRectList;
	while( pRect )
	{
		RequestRender( * pRect );
		pRect = pRect->pNext;
	}
}

//____ _castDirtyRect() _______________________________________________________

void WgGizmoModal::_castDirtyRect( const WgRect& geo, const WgRect& _clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
	// Preparations

	WgRectChain dirt;
	WgRect 		clip(_clip,geo);

	WgRect		c = _clip;
	WgRect		g = geo;

	if( clip.w == 0 || clip.h == 0 )
		return;

	// Handle modals recursively

	WgModalHook * pHook = m_modalHooks.First();
	if( pHook )
	{
		pHook->_castDirtRecursively( geo, clip, pDirtIn, &dirt );
	}
	else
		dirt.PushExistingRect(pDirtIn);

	// Handle base gizmo.

	WgGizmo * pGizmo = m_baseHook.Gizmo();
	if( pGizmo )
	{
		if( pGizmo->IsContainer() )
		{
			WgRectLink * pRect = dirt.Pop();;
			while( pRect )
			{
				pGizmo->CastToContainer()->_castDirtyRect( geo, clip, pRect, pDirtOut );
				pRect = dirt.Pop();
			}
		}
		else
		{
			// Add dirtyAreas to our list of what to render

			WgRectLink * pRect = dirt.Pop();;
			while( pRect )
			{
				m_baseHook.m_dirt.Add( WgRect(clip,*pRect) );
				pRect = dirt.Pop();
			}

			// Mask ourselves from the rectangle and move remains to dirtOut
			pGizmo->_onMaskRects( dirt, geo, clip );
		}
	}

	dirt.Transfer( pDirtOut);
}


//____ _renderDirtyRects() ____________________________________________________

void WgGizmoModal::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
	// Render dirty rects belonging to base gizmo.

	if( m_baseHook.Gizmo() )
	{
		if( m_baseHook.Gizmo()->IsContainer() )
		{
			m_baseHook.Gizmo()->CastToContainer()->_renderDirtyRects( pDevice, _canvas, _canvas, _layer );
		}
		else
		{
			WgRectLink * pDirt = m_baseHook.m_dirt.pRectList;
			while( pDirt )
			{
				m_baseHook.Gizmo()->_onRender( pDevice, _canvas, _canvas, *pDirt, _layer );
				pDirt = pDirt->pNext;
			}
		}
	}

	// Render dirty rects belonging to modal gizmos.

	WgModalHook * pHook = m_modalHooks.First();

	while( pHook )
	{
		WgRect geo = pHook->m_realGeo + _canvas.pos();
		if( pHook->Gizmo()->IsContainer() )
		{
			pHook->Gizmo()->CastToContainer()->_renderDirtyRects( pDevice, geo, geo, _layer );
		}
		else
		{
			WgRectLink * pDirt = pHook->m_dirt.pRectList;
			while( pDirt )
			{
				pHook->Gizmo()->_onRender( pDevice, geo, geo, *pDirt, _layer );
				pDirt = pDirt->pNext;
			}
		}

		pHook = pHook->_next();
	}

}

//____ _clearDirtyRects() _____________________________________________________

void WgGizmoModal::_clearDirtyRects()
{
	// Clear dirty rects for modals

	WgModalHook *pHook	= m_modalHooks.First();

	while( pHook )
	{
		pHook->m_dirt.Clear();
		if( pHook->Gizmo()->IsContainer() )
			pHook->Gizmo()->CastToContainer()->_clearDirtyRects();

		pHook = pHook->NextHook();
	}

	// Clear dirty rects for base

	m_baseHook.m_dirt.Clear();
	if( m_baseHook.Gizmo() && m_baseHook.Gizmo()->IsContainer() )
		m_baseHook.Gizmo()->CastToContainer()->_clearDirtyRects();

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

//____ _onRender() ____________________________________________________________

void WgGizmoModal::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

//____ _onAction() ____________________________________________________________

void WgGizmoModal::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

//____ _onAlphaTest() _________________________________________________________

bool WgGizmoModal::_onAlphaTest( const WgCord& ofs )
{
	return false;
}

//____ _firstHook() ___________________________________________________________

WgGizmoHook* WgGizmoModal::_firstHook() const
{
	if( m_baseHook.Gizmo() )
		return const_cast<BaseHook*>(&m_baseHook);
	else
		return m_modalHooks.First();
}

//____ _lastHook() ____________________________________________________________

WgGizmoHook* WgGizmoModal::_lastHook() const
{
	return m_modalHooks.Last();
}




