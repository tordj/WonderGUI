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

#ifndef WG_GIZMO_COLLECTION_DOT_H
#define WG_GIZMO_COLLECTION_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_GIZMO_HOOK_DOT_H
#	include <wg_gizmo_hook.h>
#endif


class WgGizmo;
class WgEmitter;


class WgGizmoCollection
{
	friend class WgGizmo;

public:
	WgGizmoCollection() {};
	virtual ~WgGizmoCollection() {};

	inline WgGizmo *	FirstGizmo() const { WgGizmoHook * p = FirstHook(); if( p ) return p->Gizmo(); else return 0; }
	inline WgGizmo *	LastGizmo() const { WgGizmoHook * p = LastHook(); if( p ) return p->Gizmo(); else return 0; }

	inline WgGizmoHook *	FirstHook() const { return _firstHook(); }
	inline WgGizmoHook *	LastHook() const { return _lastHook(); }


//	dynamic_cast<WgGizmoStackHook*>(w->GetHook())

//  WgGizmoStackHook * h = m->GetHookCasted(w);

//	b->AddGizmo( a->ReleaseGizmo( 0 ), .... );


/* TO BE PART OF ALL GIZMO MANAGERS ONCE WE ARE RID OF WIDGETS.

//	virtual WgGizmoHook *	AddGizmo( WgGizmo * pGizmo ) = 0;
//	virtual WgGizmoHook *	InsertGizmo( WgGizmo * pGizmo, Uint32 pos ) = 0;

//	virtual WgGizmoHook *	InsertGizmoBefore( WgGizmo * pGizmo, WgGizmo * pBefore ) = 0;
//	virtual WgGizmoHook *	InsertGizmoAfter( WgGizmo * pGizmo, WgGizmo * pAfter ) = 0;




	virtual WgGizmo *		ReleaseGizmo( WgGizmo * pGizmo ) = 0;
	virtual int				ReleaseGizmos( WgGizmo * pFirst, WgGizmo * pLast ) = 0;
	virtual int				ReleaseAllGizmos() = 0;

	virtual bool			DeleteGizmo( WgGizmo * pGizmo ) = 0;
	virtual int				DeleteGizmos( WgGizmo * pFirst, WgGizmo * pLast ) = 0;
	virtual int				DeleteAllGizmos() = 0;
*/

protected:
	virtual WgGizmoHook*	_firstHook() const = 0;
	virtual WgGizmoHook*	_lastHook() const = 0;


};




#endif //WG_GIZMO_COLLECTION_DOT_H

