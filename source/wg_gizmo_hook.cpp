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
	m_pGizmo->OnRender( pDevice, _canvas, _window, _clip, _layer );
}

//____ DoSetNewSize() _________________________________________________________

void WgGizmoHook::DoSetNewSize( const WgSize& size )
{
	m_pGizmo->OnNewSize( size );
}

//____ DoSetGizmo() ___________________________________________________________

void WgGizmoHook::DoSetGizmo()
{
	m_pGizmo->SetHook(this);
}

//____ RelinkGizmo() __________________________________________________________

void WgGizmoHook::RelinkGizmo()
{
	m_pGizmo->m_pHook = this;
}

//____ ReleaseGizmo() _________________________________________________________

WgGizmo* WgGizmoHook::ReleaseGizmo()
{
	WgGizmo * p = m_pGizmo;
	m_pGizmo = 0;
	return p;
}

//____ RequestFocus() _________________________________________________________

bool WgGizmoHook::RequestFocus()
{
	return Parent()->FocusRequested(this, m_pGizmo);
}

//____ ReleaseFocus() _________________________________________________________

bool WgGizmoHook::ReleaseFocus()
{
	return Parent()->FocusReleased(this, m_pGizmo);
}

//____ Root() _________________________________________________________________

WgRoot * WgGizmoHook::Root()
{
	WgGizmoContainer * pParent = Parent();
	
	if( pParent->IsGizmo() )
	{
		WgGizmoHook * pHook = pParent->CastToGizmo()->Hook();
		if( pHook )
			return pHook->Root();
	}
	else if( pParent->IsRoot() )
		return pParent->CastToRoot();

	return 0;
}
