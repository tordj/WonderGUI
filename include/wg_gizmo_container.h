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

#ifndef WG_GIZMO_COLLECTION_DOT_H
#	include <wg_gizmo_collection.h>
#endif

class WgGizmoContainer : public WgGizmoCollection
{
	friend class WgGizmoHook;

	public:
		virtual bool		IsGizmo() const;
		virtual bool		IsRoot() const;

		virtual WgGizmo *	CastToGizmo() = 0;
		virtual WgRoot *	CastToRoot();

		virtual WgGizmo *	FindGizmo( const WgCord& ofs, WgSearchMode mode ) = 0;


	protected:
		virtual bool FocusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting );
		virtual bool FocusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing );
};



#endif //WG_GIZMO_CONTAINER_DOT_H