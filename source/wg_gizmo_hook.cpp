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

#include <wg_gizmo_hook.h>
#include <wg_gizmo_container.h>
#include <wg_gizmo.h>


//____ DoRender() _____________________________________________________________

void WgGizmoHook::DoRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	m_pGizmo->_onRender( pDevice, _canvas, _window, _clip, _layer );
}

//____ DoSetNewSize() _________________________________________________________

void WgGizmoHook::DoSetNewSize( const WgSize& size )
{
	m_pGizmo->_onNewSize( size );
}

//____ _doCollectRects() _______________________________________________________

void WgGizmoHook::_doCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	m_pGizmo->_onCollectRects( rects, geo, clip );
}

//____ _doMaskRects() __________________________________________________________

void WgGizmoHook::_doMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	m_pGizmo->_onMaskRects( rects, geo, clip );
}

//_____________________________________________________________________________

void WgGizmoHook::_doCastDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
	if( m_pGizmo->IsContainer() )
		m_pGizmo->CastToContainer()->_castDirtyRect( geo, clip, pDirtIn, pDirtOut );
}

void WgGizmoHook::_doRenderDirtyRects( WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& window, Uint8 layer )
{
	if( m_pGizmo->IsContainer() )
		m_pGizmo->CastToContainer()->_renderDirtyRects( pDevice, canvas, window, layer );
}

void WgGizmoHook::_doClearDirtyRects()
{
	if( m_pGizmo->IsContainer() )
		m_pGizmo->CastToContainer()->_clearDirtyRects();
}

//____ _attachGizmo() __________________________________________________________

void WgGizmoHook::_attachGizmo( WgGizmo * pGizmo )
{
	if( m_pGizmo )
		m_pGizmo->m_pHook = 0;

	m_pGizmo = pGizmo;

	if( pGizmo )
		pGizmo->m_pHook = this;
}

//____ _relinkGizmo() __________________________________________________________

void WgGizmoHook::_relinkGizmo()
{
	if( m_pGizmo )
		m_pGizmo->m_pHook = this;
}

//____ _releaseGizmo() _________________________________________________________

WgGizmo* WgGizmoHook::_releaseGizmo()
{
	WgGizmo * p = m_pGizmo;
	m_pGizmo = 0;

	if( p )
		p->m_pHook = 0;

	return p;
}

//____ RequestFocus() _________________________________________________________

bool WgGizmoHook::RequestFocus()
{
	return Parent()->_focusRequested(this, m_pGizmo);
}

//____ ReleaseFocus() _________________________________________________________

bool WgGizmoHook::ReleaseFocus()
{
	return Parent()->_focusReleased(this, m_pGizmo);
}

//____ Root() _________________________________________________________________

WgRoot * WgGizmoHook::Root()
{
	WgGizmoContainer * pParent = Parent();

	if( pParent->_isGizmo() )
	{
		WgGizmoHook * pHook = pParent->_castToGizmo()->Hook();
		if( pHook )
			return pHook->Root();
	}
	else if( pParent->_isRoot() )
		return pParent->_castToRoot();

	return 0;
}
