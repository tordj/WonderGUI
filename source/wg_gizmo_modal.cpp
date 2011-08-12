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

static const char	c_gizmoType[] = {"Modal"};

Improve WgModalHook geometry handling, should be able to run on BestSize by default, answering to resize-requests.


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

	if( geometry.w <= 0 )
		geometry.w = 1;

	if( geometry.h <= 0 )
		gemoetry.h = 1;

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
	m_placementGeo.setSize( sz );
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetWidth( int width )
{
	m_placementGeo.w = width;
	return _refreshRealGeo();
}

//_____________________________________________________________________________
bool WgModalHook::SetHeight( int height )
{
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
	WgCord ofs = WgUtil::LocationToOfs( m_origo, m_pParent->Size() );
	ofs += m_placementGeo.pos();

	m_realGeo = WgRect( ofs, m_placementGeo.size() );
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
}

//_____________________________________________________________________________
void WgModalHook::_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut )
{
}

//_____________________________________________________________________________
void WgModalHook::_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer )
{
}

//_____________________________________________________________________________
WgGizmoHook * WgModalHook::_prevHook() const
{
	WgModalHook * p = _prev();

	if( p )
		return p;
	else if( m_baseHook.Gizmo() )
		return &m_baseHook;
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


void WgGizmoModal::BaseHook::RequestRender()
{
	m_pParent->_onRequestRender( WgRect( 0,0, m_pParent->Size() ), 0 );
}

void WgGizmoModal::BaseHook::RequestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect, 0 );
}

void WgGizmoModal::BaseHook::RequestResize()
{
	m_pParent->RequestResize();					// Just forward to our parent
}

void WgGizmoModal::BaseHook::_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut )
{
}

void WgGizmoModal::BaseHook::_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer )
{
}

//____ Constructor ____________________________________________________________

WgGizmoModal::WgGizmoModal() : m_dimColor(WgColor::None()), m_baseHook(this)
{
}

//____ Destructor _____________________________________________________________

WgGizmoModal::~WgGizmoModal()
{
	DeleteAllGizmos();
}

//____ Type() _________________________________________________________________

virtual const char *WgGizmoModal::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

static const char * WgGizmoModal::GetMyType()
{
	return c_gizmoType;
}

WgGizmoHook * WgGizmoModal::SetBaseGizmo( WgGizmo * pGizmo )
{
}

WgGizmo * WgGizmoModal::BaseGizmo()
{
}

bool WgGizmoModal::DeleteBaseGizmo()
{
}

WgGizmo * WgGizmoModal::ReleaseBaseGizmo()
{
}

WgModalHook * WgGizmoModal::AddModalGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation origo )
{
}

WgModalHook * WgGizmoModal::AddModalGizmo( WgGizmo * pGizmo, const WgCord& pos, WgLocation origo )
{
}

bool WgGizmoModal::DeleteAllModalGizmos()
{
}

bool WgGizmoModal::ReleaseAllModalGizmos()
{
}

bool WgGizmoModal::DeleteGizmo( WgGizmo * pGizmo )
{
}

bool WgGizmoModal::ReleaseGizmo( WgGizmo * pGizmo )
{
}

bool WgGizmoModal::DeleteAllGizmos()
{
}

bool WgGizmoModal::ReleaseAllGizmos()
{
}

WgModalHook * WgGizmoModal::FirstModalChild()
{
}

WgModalHook * WgGizmoModal::LastModalChild()
{
}


int WgGizmoModal::HeightForWidth( int width ) const
{
}

int WgGizmoModal::WidthForHeight( int height ) const
{
}

WgSize WgGizmoModal::MinSize() const
{
}

WgSize WgGizmoModal::BestSize() const
{
}

WgSize WgGizmoModal::MaxSize() const
{
}



WgGizmo *  WgGizmoModal::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{
}


void WgGizmoModal::_onCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
}

void WgGizmoModal::_onMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
}

void WgGizmoModal::_onCloneContent( const WgGizmo * _pOrg )
{
}

void WgGizmoModal::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

void WgGizmoModal::_onNewSize( const WgSize& size )
{
}

void WgGizmoModal::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

bool WgGizmoModal::_onAlphaTest( const WgCord& ofs )
{
}

void WgGizmoModal::onRequestRender( const WgRect& rect, const WgFlexHook * pHook )
{
}

void WgGizmoModal::_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
}

void WgGizmoModal::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
}

void WgGizmoModal::_clearDirtyRects()
{
}



