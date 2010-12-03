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



class WgGizmoContainer;
class WgWidget;
class WgGfxDevice;
class WgGizmo;

class WgGizmoHook
{
	friend class WgGizmo;
	friend class WgGizmoContainer;

public:
	virtual WgCord	Pos() const = 0;
	virtual WgSize	Size() const = 0;
	virtual WgRect	Geo() const = 0;
	virtual WgCord	ScreenPos() const = 0;
	virtual WgRect	ScreenGeo() const = 0;

	virtual WgGizmoHook *	PrevHook() const = 0;
	virtual WgGizmoHook *	NextHook() const = 0;

	inline	WgGizmo *		Gizmo() const { return m_pGizmo; }
	virtual	WgGizmoContainer* Parent() const = 0;

	virtual WgWidget*	GetRoot() = 0;			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	WgGizmoHook( WgGizmo * pGizmo ) : m_pGizmo(pGizmo) {}

	void			RelinkGizmo();				// Make sure Gizmo links us. Call when hook has been relocated.

	virtual void	RequestRender() = 0;
	virtual void	RequestRender( const WgRect& rect ) = 0;
	virtual void	RequestResize() = 0;

	virtual bool	RequestFocus() = 0;
	virtual bool	ReleaseFocus() = 0;

	void			DoRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			DoSetNewSize( const WgSize& size );
	void			DoSetGizmo();

	WgGizmo *		m_pGizmo;
};

#endif //WG_GIZMO_HOOK_DOT_H
