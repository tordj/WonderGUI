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
class WgRoot;
class WgRectLink;
class WgRectChain;

class WgGizmoHook
{
	friend class WgGizmo;
	friend class WgGizmoContainer;
	friend class WgOrderedLayout;

public:
	virtual WgCord	Pos() const = 0;
	virtual WgSize	Size() const = 0;
	virtual WgRect	Geo() const = 0;
	virtual WgCord	ScreenPos() const = 0;
	virtual WgRect	ScreenGeo() const = 0;

	inline WgGizmoHook *	PrevHook() const { return _prevHook(); }
	inline WgGizmoHook *	NextHook() const { return _nextHook(); }

	inline	WgGizmo *			Gizmo() const { return m_pGizmo; }
	inline  WgGizmoContainer * 	Parent() const { return _parent(); }
	virtual WgRoot *			Root();

	virtual WgWidget*	GetRoot() = 0;			// Remove once Widgets are gone...

protected:

	WgGizmoHook() : m_pGizmo(0) {}

	void			_attachGizmo( WgGizmo * pGizmo );				// Make sure Gizmo links us. Call when hook has been relocated.
	void			_relinkGizmo();
	WgGizmo*		_releaseGizmo();								//

	// To be called by Gizmo

	virtual void	RequestRender() = 0;
	virtual void	RequestRender( const WgRect& rect ) = 0;
	virtual void	RequestResize() = 0;

	virtual bool	RequestFocus();
	virtual bool	ReleaseFocus();

	// To be called by Parent

	void			DoRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			DoSetNewSize( const WgSize& size );

	void			_doCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip );
	void			_doMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip );

	void			_doCastDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut );
	void			_doRenderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer );
	void			_doClearDirtyRects();

	//

	virtual WgGizmoHook *	_prevHook() const = 0;
	virtual WgGizmoHook *	_nextHook() const = 0;
	virtual WgGizmoContainer * _parent() const = 0;


	WgGizmo *		m_pGizmo;
};

#endif //WG_GIZMO_HOOK_DOT_H
