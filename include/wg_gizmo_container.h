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

class WgDirtyRect;
class WgDirtyRectObj;

class WgGizmoContainer : public WgGizmoParent
{
	friend class WgGizmoHook;

	protected:
		virtual void	OnEnable();
		virtual void	OnDisable();

	private:
		bool		_isGizmo() const;
		bool		_isRoot() const;

//		WgGizmo *	_castToGizmo();	TODO: Implement once we inherit from WgGizmo as we are supposed to.
		WgRoot *	_castToRoot();


		virtual void	_castDirtyRect( const WgRect& geo, const WgRect& clip, WgDirtyRect * pDirtIn, WgDirtyRectObj* pDirtOut ) = 0;
		virtual void	_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer ) = 0;
		virtual void	_clearDirtyRects() = 0;



		bool 		_focusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting );	// Needed until WgGizmoContainer inerits from WgGizmo
		bool 		_focusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing );		// Needed until WgGizmoContainer inerits from WgGizmo
};



#endif //WG_GIZMO_PARENT_DOT_H
