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

	if( m_hook.Gizmo() )
		m_hook.~Hook();

	new (&m_hook) Hook(pGizmo, this);

	return true;
}

//____ ReleaseGizmo() _________________________________________________________

WgGizmo * WgRoot::ReleaseGizmo()
{
	return m_hook.ReleaseGizmo();
}

//____ Update() _______________________________________________________________

void WgRoot::Update( int msTime )
{
}

//____ Render() _______________________________________________________________

int WgRoot::Render()
{
	return Render( Geo() );
}

int WgRoot::Render( const WgRect& clip )
{
	int retVal = BeginRender( clip );
	RenderLayer(0xFF);
	EndRender();
	return retVal;
}

//____ BeginRender() __________________________________________________________

bool WgRoot::BeginRender()
{
	if( m_pGfxDevice )
	{
		return m_pGfxDevice->BeginRender();
	}
	else
		return false;
}


//____ RenderLayer() __________________________________________________________

void WgRoot::RenderLayer( int layer )
{
}

//____ EndRender() ____________________________________________________________

void WgRoot::EndRender( void )
{
	m_pGfxDevice->EndRender();
	m_dirtyRects.Clear();
}

//____ AddDirtyRect() _________________________________________________________

void WgRoot::AddDirtyRect( WgRect rect )
{
	m_dirtyRects.Add( rect );
}






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

WgGizmoHook * WgRoot::Hook::PrevHook() const
{
	return 0;
}

WgGizmoHook * WgRoot::Hook::NextHook() const
{
	return 0;
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
	// Do nothing.
}

bool WgRoot::Hook::RequestFocus()
{
}

bool WgRoot::Hook::ReleaseFocus()
{
}





