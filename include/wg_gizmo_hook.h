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

#ifndef WG_GIZMO_HOOK_DOT_H
#define WG_GIZMO_HOOK_DOT_H

#ifndef	WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_EMITTER_DOT_H
#	include <wg_emitter.h>
#endif



class WgGizmoCollection;
class WgWidget;
class WgGfxDevice;

class WgGizmoHook
{
	friend class WgGizmo;
	friend class WgGizmoCollection;

public:
	virtual WgCord	Pos() const = 0;
	virtual WgSize	Size() const = 0;
	virtual WgRect	Geo() const = 0;
	virtual WgCord	ScreenPos() const = 0;
	virtual WgRect	ScreenGeo() const = 0;

	virtual WgGizmoHook * PrevHook() const = 0;
	virtual WgGizmoHook * NextHook() const = 0;

	inline	WgGizmo *		Gizmo() const { return m_pGizmo; }
	inline	WgGizmoCollection* Collection() const { return m_pCollection; }

	// Needs to be here for now since Emitters are inherrited by Widgets. Shouldn't be hooks business in the future...

	virtual WgEmitter* 	GetEmitter() = 0;
	virtual WgWidget*	GetRoot() = 0;			// Should in the future not return a widget, but a gizmo.

protected:
	WgGizmoHook( WgGizmo * pGizmo, WgGizmoCollection * pCollection ) : m_pGizmo(pGizmo), m_pCollection(pCollection) {}

	virtual void	RequestRender() = 0;
	virtual void	RequestRender( const WgRect& rect ) = 0;
	virtual void	RequestResize() = 0;

	virtual bool	RequestFocus() = 0;
	virtual bool	ReleaseFocus() = 0;

	inline void		DoRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer );

	WgGizmo *			m_pGizmo;
	WgGizmoCollection*	m_pCollection;
};

#endif //WG_GIZMO_HOOK_DOT_H
