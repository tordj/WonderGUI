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

#include <wg_gizmo_flexgeo.h>

void WgFlexHook::SetAnchored()
{
}

bool WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight )
{
}

void WgFlexHook::SetFloating()
{
}

bool WgFlexHook::SetFloating( const WgRect& geometry )
{
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot )
{
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot, int anchor )
{
}

void WgFlexHook::Top()
{
}

void WgFlexHook::Bottom()
{
}

bool WgFlexHook::Up()
{
}

bool WgFlexHook::Down()
{
}

bool WgFlexHook::MoveOver( WgFlexHook * pSibling )
{
}

bool WgFlexHook::MoveUnder( WgFlexHook * pSibling )
{
}

void WgFlexHook::Hide()
{
}

void WgFlexHook::Show()
{
}

bool WgFlexHook::SetHotspot( WgLocation hotspot )
{
}

bool WgFlexHook::SetSizePolicy( WgSizePolicy policy )
{
}

bool WgFlexHook::SetMinSize( const WgSize& size )
{
}

bool WgFlexHook::SetMaxSize( const WgSize& size )
{
}

bool WgFlexHook::SetGeo( const WgRect& geometry )
{
}

bool WgFlexHook::SetOfs( const WgCord& ofs )
{
}

bool WgFlexHook::SetOfsX( int x )
{
}

bool WgFlexHook::SetOfsY( int y )
{
}

bool WgFlexHook::SetSize( const WgSize& size )
{
}

bool WgFlexHook::SetWidth( int width )
{
}

bool WgFlexHook::SetHeight( int height )
{
}

bool WgFlexHook::Move( const WgCord& ofs )
{
}

bool WgFlexHook::MoveX( int x )
{
}

bool WgFlexHook::MoveY( int y )
{
}

WgCord WgFlexHook::Pos() const
{
}

WgSize WgFlexHook::Size() const
{
}

WgRect WgFlexHook::Geo() const
{
}

WgCord WgFlexHook::ScreenPos() const
{
}

WgRect WgFlexHook::ScreenGeo() const
{
}

WgGizmoHook * WgFlexHook::PrevHook() const
{
}

WgGizmoHook * WgFlexHook::NextHook() const
{
}

WgGizmoContainer* WgFlexHook::Parent() const
{
}

WgWidget* WgFlexHook::GetRoot()
{
}

WgFlexHook::WgFlexHook( WgGizmo * pGizmo, WgGizmoFlexGeo * pParent )
{
}

void WgFlexHook::RefreshRealGeo()
{
}

void WgFlexHook::RequestRender()
{
}

void WgFlexHook::RequestRender( const WgRect& rect )
{
}

void WgFlexHook::RequestResize()
{
}












