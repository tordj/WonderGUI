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


//____ Destructor _____________________________________________________________

WgGizmoHook::~WgGizmoHook()
{
	if( m_pGizmo )
		delete m_pGizmo;
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
	WgGizmoParent * pParent = Parent();

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
