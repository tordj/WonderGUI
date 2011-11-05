/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_root.h>
#include <new>

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

#include <wg_eventhandler.h>

//____ Constructor ____________________________________________________________

WgRoot::WgRoot()
{
	m_bHasGeo = false;
	m_geo = WgRect(0,0,0,0);
	m_pGfxDevice = 0;
	m_pEventHandler = new WgEventHandler(this);
	m_hook.m_pRoot = this;

}


WgRoot::WgRoot( WgGfxDevice * pGfxDevice )
{
	m_bHasGeo = false;
	m_geo = WgRect(0,0,0,0);
	m_pGfxDevice = pGfxDevice;
	m_pEventHandler = new WgEventHandler(this);
	m_hook.m_pRoot = this;
}

//____ Destructor _____________________________________________________________

WgRoot::~WgRoot()
{
	delete m_pEventHandler;
}

//____ SetGfxDevice() _________________________________________________________

bool WgRoot::SetGfxDevice( WgGfxDevice * pDevice )
{
	m_pGfxDevice = pDevice;

	if( m_pGfxDevice && !m_bHasGeo && m_hook.Gizmo() )
		m_hook.Gizmo()->_onNewSize( m_pGfxDevice->CanvasSize() );

	return true;
}

//_____ SetGeo() _____________________________________________________________

bool WgRoot::SetGeo( const WgRect& geo )
{
	if( geo.x == 0 && geo.y == 0 && geo.w == 0 && geo.h == 0 )
		m_bHasGeo = false;
	else
		m_bHasGeo = true;

	m_geo = geo;
	return true;
}

//____ Geo() __________________________________________________________________

WgRect WgRoot::Geo() const
{
	if( m_bHasGeo )
		return m_geo;
	else if( m_pGfxDevice )
		return  WgRect( WgCoord(0,0), m_pGfxDevice->CanvasSize() );
	else
		return WgRect(0,0,0,0);
}


//____ SetChild() _____________________________________________________________

bool WgRoot::SetChild( WgGizmoContainer * _pGizmo )
{
	if( !_pGizmo )
		return false;

	WgGizmo * pGizmo = _pGizmo->CastToGizmo();
	if( !pGizmo )
		return false;

	m_hook._attachGizmo(pGizmo);
	m_hook.Gizmo()->_onNewSize(m_geo.Size());

	m_hook.Gizmo()->_onCollectRects( m_dirtyRects, Geo(), Geo() );

	return true;
}

//____ ReleaseChild() _________________________________________________________

WgGizmo * WgRoot::ReleaseChild()
{
	return m_hook._releaseGizmo();
}

WgGizmo * WgRoot::ReleaseChild( WgGizmo * pGizmo )
{
	if( pGizmo == m_hook.Gizmo() )
		return ReleaseChild();

	return false;
}


//____ DeleteChild() __________________________________________________________

bool WgRoot::DeleteChild( WgGizmo * pGizmo )
{
	if( pGizmo == m_hook.Gizmo() )
		return SetChild(0);

	return false;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgRoot::DeleteAllChildren()
{
	DeleteChild();
	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgRoot::ReleaseAllChildren()
{
	return ReleaseChild()==0?false:true;
}


//____ Render() _______________________________________________________________

bool WgRoot::Render()
{
	return Render( Geo() );
}

bool WgRoot::Render( const WgRect& clip )
{
	if( !BeginRender( clip ) )
		return false;

	if( !RenderSection() )
	{
		EndRender();
		return false;
	}

	if( !EndRender() )
		return false;

	return true;
}

//____ BeginRender() __________________________________________________________

bool WgRoot::BeginRender( const WgRect& clip )
{
	if( !m_pGfxDevice || !m_hook.Gizmo() )
		return false;						// No GFX-device or no widgets to render.

	WgRect canvas = Geo();
	WgRect clip2( clip, canvas );
	if( clip2.w == 0 || clip2.h == 0 )
		return false;						// Invalid rect area.

	WgRectLink * pRect = m_dirtyRects.Pop();
	WgRectChain	outDummy;

	while( pRect )
	{
		m_hook.Gizmo()->CastToContainer()->_castDirtyRect( canvas, clip, pRect, &outDummy );
		pRect = m_dirtyRects.Pop();
	}

	return m_pGfxDevice->BeginRender();
}


//____ RenderSection() __________________________________________________________

bool WgRoot::RenderSection( int layer )
{
	if( !m_pGfxDevice || !m_hook.Gizmo() )
		return false;						// No GFX-device or no widgets to render.

	WgRect canvas = Geo();

	m_hook.Gizmo()->CastToContainer()->_renderDirtyRects( m_pGfxDevice, canvas, canvas, layer );

	return true;
}

//____ EndRender() ____________________________________________________________

bool WgRoot::EndRender( void )
{
	if( !m_pGfxDevice || !m_hook.Gizmo() )
		return false;						// No GFX-device or no widgets to render.

	m_hook.Gizmo()->CastToContainer()->_clearDirtyRects();

	return m_pGfxDevice->EndRender();
}

//____ AddDirtyRect() _________________________________________________________

void WgRoot::AddDirtyRect( WgRect rect )
{
	m_dirtyRects.Add( rect );
}

//____ ExportDirtyRects() _____________________________________________________

int WgRoot::ExportDirtyRects( WgRect * pDest, int maxRects ) const
{
	WgRectLink * pRect = m_dirtyRects.pRectList;
	int	nExported = 0;

	while( pRect && nExported < maxRects )
	{
		pDest[nExported++] = * pRect;
		pRect = pRect->pNext;
	}

	return nExported;
}

//____ FindGizmo() _____________________________________________________________

WgGizmo * WgRoot::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	if( !Geo().Contains(ofs) || !m_hook.Gizmo() )
		return 0;

	if( m_hook.Gizmo() && m_hook.Gizmo()->IsContainer() )
		return m_hook.Gizmo()->CastToContainer()->FindGizmo( ofs, mode );

	return m_hook.Gizmo();
}


//____ _focusRequested() _______________________________________________________

bool WgRoot::_focusRequested( WgHook * pBranch, WgGizmo * pGizmoRequesting )
{
	if( m_pEventHandler )
		return m_pEventHandler->SetKeyboardFocus(pGizmoRequesting);
	else
		return false;
}

//____ _focusReleased() ________________________________________________________

bool WgRoot::_focusReleased( WgHook * pBranch, WgGizmo * pGizmoReleasing )
{
	if( m_pEventHandler )
		return m_pEventHandler->SetKeyboardFocus(0);
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////

WgRoot::Hook::~Hook()
{
}

WgCoord WgRoot::Hook::Pos() const
{
	return m_pRoot->Geo();
}

WgSize WgRoot::Hook::Size() const
{
	return m_pRoot->Geo();
}

WgRect WgRoot::Hook::Geo() const
{
	return m_pRoot->Geo();
}

WgCoord WgRoot::Hook::ScreenPos() const
{
	return m_pRoot->Geo();
}

WgRect WgRoot::Hook::ScreenGeo() const
{
	return m_pRoot->Geo();
}

WgWidget* WgRoot::Hook::GetRoot()
{
	return 0;
}

WgRoot* WgRoot::Hook::Root() const
{
	return m_pRoot;
}

void WgRoot::Hook::RequestRender()
{
	m_pRoot->AddDirtyRect( Geo() );
}

void WgRoot::Hook::RequestRender( const WgRect& rect )
{
	m_pRoot->AddDirtyRect( WgRect( Pos() + rect.Pos(), rect.Size() ) );
}

void WgRoot::Hook::RequestResize()
{
	// Do nothing, root ignores size requests.
}

WgHook * WgRoot::Hook::_prevHook() const
{
	return 0;
}

WgHook * WgRoot::Hook::_nextHook() const
{
	return 0;
}

WgGizmoParent * WgRoot::Hook::_parent() const
{
	return m_pRoot;
}

