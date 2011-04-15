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

//____ Constructor ____________________________________________________________

WgRoot::WgRoot() : m_pGfxDevice(0), m_pInputDevice(0), m_geo(0,0,0,0), m_bHasGeo(false)
{
}


WgRoot::WgRoot( WgGfxDevice * pGfxDevice, WgInputDevice * pInputDevice )
{
	m_bHasGeo = false;
	m_geo = WgRect(0,0,0,0);
	m_pGfxDevice = pGfxDevice;
	m_pInputDevice = pInputDevice;
}

//____ Destructor _____________________________________________________________

WgRoot::~WgRoot()
{
}

//____ SetGfxDevice() _________________________________________________________

bool WgRoot::SetGfxDevice( WgGfxDevice * pDevice )
{
	m_pGfxDevice = pDevice;

	if( m_pGfxDevice && !m_bHasGeo && m_hook.Gizmo() )
		m_hook.DoSetNewSize( m_pGfxDevice->CanvasSize() );

	return true;
}

//____ SetInputDevice() _______________________________________________________

bool WgRoot::SetInputDevice( WgInputDevice * pDevice )
{
	//TODO: Handle mouse and keyboard focus changes...

	m_pInputDevice = pDevice;
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
		return  WgRect( WgCord(0,0), m_pGfxDevice->CanvasSize() );
	else
		return WgRect(0,0,0,0);
}


//____ SetGizmo() _____________________________________________________________

bool WgRoot::SetGizmo( WgGizmo * pGizmo )
{
	// Until WgGizmoContainer properly can inherit from WgGizmo we need to check that
	// we get a container. In the future we should take a WgGizmoContainer as parameter instead.

	if( pGizmo && !pGizmo->IsContainer() )
		return false;

	if( m_hook.Gizmo() )
		m_hook.~Hook();

	new (&m_hook) Hook(pGizmo, this);

	m_hook.DoCollectRects( m_dirtyRects, Geo(), Geo() );

	return true;
}

//____ ReleaseGizmo() _________________________________________________________

WgGizmo * WgRoot::ReleaseGizmo()
{
	return m_hook.ReleaseGizmo();
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
		m_hook._doCastDirtyRect( canvas, clip, pRect, &outDummy );
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

	m_hook._doRenderDirtyRects( m_pGfxDevice, canvas, canvas, layer );

	return true;
}

//____ EndRender() ____________________________________________________________

bool WgRoot::EndRender( void )
{
	if( !m_pGfxDevice || !m_hook.Gizmo() )
		return false;						// No GFX-device or no widgets to render.

	m_hook._doClearDirtyRects();

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

WgGizmo * WgRoot::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{
	if( !Geo().contains(ofs) || !m_hook.Gizmo() )
		return 0;

	if( m_hook.Gizmo() && m_hook.Gizmo()->IsContainer() )
		return m_hook.Gizmo()->CastToContainer()->FindGizmo( ofs, mode );

	return m_hook.Gizmo();
}


//____ _focusRequested() _______________________________________________________

bool WgRoot::_focusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting )
{
	//TODO: Implement
	return false;
}

//____ _focusReleased() ________________________________________________________

bool WgRoot::_focusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing )
{
	//TODO: Implement
	return false;
}

///////////////////////////////////////////////////////////////////////////////

WgRoot::Hook::Hook( WgGizmo * pGizmo, WgRoot * pRoot) : WgGizmoHook(pGizmo)
{
	m_pRoot = pRoot;
	DoSetGizmo();
}

WgRoot::Hook::~Hook()
{
}

WgCord WgRoot::Hook::Pos() const
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

WgCord WgRoot::Hook::ScreenPos() const
{
	return m_pRoot->Geo();
}

WgRect WgRoot::Hook::ScreenGeo() const
{
	return m_pRoot->Geo();
}

WgGizmoContainer* WgRoot::Hook::Parent() const
{
	return 0;
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
	m_pRoot->AddDirtyRect( WgRect( Pos() + rect.pos(), rect.size() ) );
}

void WgRoot::Hook::RequestResize()
{
	// Do nothing, root ignores size requests.
}

WgGizmoHook * WgRoot::Hook::_prevHook() const
{
	return 0;
}

WgGizmoHook * WgRoot::Hook::_nextHook() const
{
	return 0;
}


