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

#ifndef	WG_GIZMO_CONTAINER_DOT_H
#define	WG_GIZMO_CONTAINER_DOT_H

#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_GIZMO_PARENT_DOT_H
#	include <wg_gizmo_parent.h>
#endif

class WgGizmoContainer : public WgGizmoParent
{
	friend class WgGizmoHook;

	public:
		WgRect		GetRenderBox() const;

	private:
		bool		_isGizmo() const;
		bool		_isRoot() const;

		// Call when a child container has modified its renderBox, caused by its children.

		void		_renderBoxModified( WgGizmoHook * pHook, const WgRect& oldBox, const WgRect& newBox ) = 0;

//		WgGizmo *	_castToGizmo();	TODO: Implement once we inherit from WgGizmo as we are supposed to.
		WgRoot *	_castToRoot();


		bool 		_focusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting );
		bool 		_focusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing );

		WgRect		m_renderBox;		// Bounding-box for all rendering from this branch.
};



#endif //WG_GIZMO_PARENT_DOT_H
