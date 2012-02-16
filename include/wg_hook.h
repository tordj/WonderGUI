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

#ifndef WG_HOOK_DOT_H
#define WG_HOOK_DOT_H

#ifndef	WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_EMITTER_DOT_H
#	include <wg_emitter.h>
#endif



class WgGizmoParent;
class WgWidget;
class WgGfxDevice;
class WgGizmo;
class WgRoot;
class WgRectLink;

class WgHook
{
	friend class WgGizmo;
	friend class WgGizmoContainer;
	friend class WgOrderedLayout;

public:
	virtual WgCoord	Pos() const = 0;
	virtual WgSize	Size() const = 0;
	virtual WgRect	Geo() const = 0;
	virtual WgCoord	ScreenPos() const = 0;
	virtual WgRect	ScreenGeo() const = 0;

	inline WgHook *	Prev() const { return _prevHook(); }
	inline WgHook *	Next() const { return _nextHook(); }

	inline bool		Hide() { return SetHidden(true); }
	inline bool		Unhide() { return SetHidden(false); }
	virtual bool	SetHidden( bool bHide );
	bool			Hidden() { return m_bHidden; }

	inline	WgGizmo *			Gizmo() const { return m_pGizmo; }
	inline  WgGizmoParent * 	Parent() const { return _parent(); }
	virtual WgRoot *			Root();

	virtual WgWidget*	GetRoot() = 0;			// Remove once Widgets are gone...

protected:

	WgHook() : m_pGizmo(0), m_bHidden(false) {}
	virtual ~WgHook();

	virtual void	_attachGizmo( WgGizmo * pGizmo );				// Make sure Gizmo links us. Call when hook has been relocated.
	void			_relinkGizmo();
	WgGizmo*		_releaseGizmo();								//

	// To be called by Gizmo

	virtual void	_requestRender() = 0;
	virtual void	_requestRender( const WgRect& rect ) = 0;
	virtual void	_requestResize() = 0;

	virtual bool	_requestFocus();
	virtual bool	_releaseFocus();

	//

	virtual WgHook *	_prevHook() const = 0;
	virtual WgHook *	_nextHook() const = 0;
	virtual WgGizmoParent * _parent() const = 0;


	WgGizmo *		m_pGizmo;
	bool			m_bHidden;

};

#endif //WG_HOOK_DOT_H
