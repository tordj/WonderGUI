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

#include <wg_vboxlayout.h>


WgVBoxHook::WgVBoxHook( WgGizmo * pGizmo, WgVBoxLayout * pParent )
{
}


WgGizmoContainer* WgVBoxHook::_parent() const
{
}


WgVBoxLayout::WgVBoxLayout()
{
}

WgVBoxLayout::~WgVBoxLayout()
{
}

const char * WgVBoxLayout::Type() const
{
}

const char * WgVBoxLayout::GetMyType()
{
}



int WgVBoxLayout::HeightForWidth( int width ) const
{
}

int WgVBoxLayout::WidthForHeight( int height ) const
{
}

WgSize WgVBoxLayout::MinSize() const
{
}

WgSize WgVBoxLayout::BestSize() const
{
}

WgSize WgVBoxLayout::MaxSize() const
{
}


void WgVBoxLayout::_onNewSize( const WgSize& size )
{
}

void WgVBoxLayout::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}


void WgVBoxLayout::_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
}

void WgVBoxLayout::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
}

void WgVBoxLayout::_clearDirtyRects()
{
}


WgRect WgVBoxLayout::_hookGeo( const WgOrderedHook * pHook )
{
}

void  WgVBoxLayout::_advanceGeoToHook( WgRect& prevHookGeo, const WgOrderedHook * pHook )
{
}

void WgVBoxLayout::_onResizeRequested( WgOrderedHook * pHook )
{
}

void  WgVBoxLayout::_onRenderRequested( WgOrderedHook * pHook )
{
}

void  WgVBoxLayout::_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect )
{
}

void  WgVBoxLayout::_onGizmoAppeared( WgOrderedHook * pInserted )
{
}

void WgVBoxLayout::_onGizmoDisappeared( WgOrderedHook * pToBeRemoved )
{
}

void  WgVBoxLayout::_onGizmosReordered()
{
}

void  WgVBoxLayout::_refreshAllGizmos()
{
}

//____ _newHook() _____________________________________________________________

WgOrderedHook *  WgVBoxLayout::_newHook(WgGizmo * pGizmo)
{
	return new WgVBoxHook( pGizmo, this );
}
