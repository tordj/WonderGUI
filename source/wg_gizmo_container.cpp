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

#include <wg_gizmo_container.h>

//____ () __________________________________________________

WgGizmoContainer::WgGizmoContainer() : m_bFocusGroup(false), m_bRadioGroup(false), m_bTooltipGroup(false) 
{
}


//____ () _________________________________________________

void WgGizmoContainer::_onEnable()
{
	WgGizmo * p = FirstGizmo();
	while( p )
	{
		p->Enable();
		p = p->NextSibling();
	}
}

//____ () _________________________________________________

void WgGizmoContainer::_onDisable()
{
	WgGizmo * p = FirstGizmo();
	while( p )
	{
		p->Disable();
		p = p->NextSibling();
	}
}

//____ IsGizmo() ______________________________________________________________

bool WgGizmoContainer::IsGizmo() const
{
	return true;
}

//____ IsRoot() _______________________________________________________________

bool WgGizmoContainer::IsRoot() const
{
	return false;
}

//____ CastToContainer() ___________________________________________________________

WgGizmoContainer * WgGizmoContainer::CastToContainer()
{
	return this;
}


//____ CastToRoot() ___________________________________________________________

WgRoot * WgGizmoContainer::CastToRoot()
{
	return 0;
}

//____ _focusRequested() _______________________________________________________

bool WgGizmoContainer::_focusRequested( WgHook * pBranch, WgGizmo * pGizmoRequesting )
{
	WgHook * p = CastToGizmo()->Hook();
	if( p )
		return p->Parent()->_focusRequested( p, pGizmoRequesting );
	else
		return false;
}

//____ _focusReleased() ________________________________________________________

bool WgGizmoContainer::_focusReleased( WgHook * pBranch, WgGizmo * pGizmoReleasing )
{
	WgHook * p = CastToGizmo()->Hook();
	if( p )
		return p->Parent()->_focusReleased( p, pGizmoReleasing );
	else
		return false;
}


