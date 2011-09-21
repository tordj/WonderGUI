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

#ifndef	WG_GIZMO_PARENT_DOT_H
#define	WG_GIZMO_PARENT_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_gizmo_types.h>
#endif

#ifndef WG_GIZMO_COLLECTION_DOT_H
#	include <wg_gizmo_collection.h>
#endif

class WgGizmo;
class WgRoot;
class WgGizmoHook;


class WgGizmoParent : public WgGizmoCollection
{
	public:

		virtual WgGizmo *	FindGizmo( const WgCoord& ofs, WgSearchMode mode ) = 0;

		virtual bool		DeleteGizmo( WgGizmo * pGizmo ) = 0;
		virtual WgGizmo *	ReleaseGizmo( WgGizmo * pGizmo ) = 0;

		virtual bool		DeleteAllGizmos() = 0;
		virtual bool		ReleaseAllGizmos() = 0;

		virtual WgGizmo *	CastToGizmo() = 0;
		virtual WgRoot *	CastToRoot() = 0;

		virtual bool		IsGizmo() const = 0;
		virtual bool		IsRoot() const = 0;

	protected:

		virtual bool _focusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting ) = 0;
		virtual bool _focusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing ) = 0;
};


#endif //WG_GIZMO_PARENT_DOT_H
