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

#ifndef	WG_GIZMOCONTAINER_DOT_H
#define	WG_GIZMOCONTAINER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GIZMO_COLLECTION_DOT_H
#	include <wg_gizmo_collection.h>
#endif

class WgGizmo;
class WgRootPanel;
class WgHook;
class WgPanel;
class WgModalPanel;
class WgMenuPanel;

class WgGizmoContainer : public WgGizmoCollection
{
	friend class WgPanel;
	friend class WgMenu;
	friend class WgMenubar;
	friend class WgHook;
	public:

		virtual WgGizmo *	FindGizmo( const WgCoord& ofs, WgSearchMode mode ) = 0;

		virtual bool		DeleteChild( WgGizmo * pGizmo ) = 0;
		virtual WgGizmo *	ReleaseChild( WgGizmo * pGizmo ) = 0;

		virtual bool		DeleteAllChildren() = 0;
		virtual bool		ReleaseAllChildren() = 0;

		virtual WgGizmo *			CastToGizmo() = 0;
		virtual const WgGizmo *		CastToGizmo() const = 0;
		virtual WgPanel *	CastToPanel() = 0;
		virtual const WgPanel *	CastToPanel() const = 0;
		virtual WgRootPanel *			CastToRoot() = 0;
		virtual const WgRootPanel *		CastToRoot() const = 0;

		virtual bool		IsGizmo() const = 0;
		virtual bool		IsPanel() const = 0;
		virtual bool		IsRoot() const = 0;

		virtual bool		IsAncestorTo( const WgGizmo * pGizmo ) const;

	protected:

		virtual bool 		_focusRequested( WgHook * pBranch, WgGizmo * pGizmoRequesting ) = 0;
		virtual bool 		_focusReleased( WgHook * pBranch, WgGizmo * pGizmoReleasing ) = 0;

		virtual WgModalPanel *		_getModalPanel() const = 0;
		virtual WgMenuPanel*	_getMenuPanel() const = 0;

};


#endif //WG_GIZMOCONTAINER_DOT_H
