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


//____ CastToRoot() ___________________________________________________________

WgRoot * WgGizmoContainer::CastToRoot()
{
	return 0;
}

//____ FocusRequested() _______________________________________________________

bool WgGizmoContainer::FocusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting )
{
	WgGizmoHook * p = CastToGizmo()->Hook();
	if( p )
		return p->Parent()->FocusRequested( p, pGizmoRequesting );
	else
		return false;
}

//____ FocusReleased() ________________________________________________________

bool WgGizmoContainer::FocusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing )
{
	WgGizmoHook * p = CastToGizmo()->Hook();
	if( p )
		return p->Parent()->FocusReleased( p, pGizmoReleasing );
	else
		return false;
}


