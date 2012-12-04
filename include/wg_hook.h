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



class WgGizmoContainer;
class WgGfxDevice;
class WgGizmo;
class WgRootPanel;
class WgRectLink;
class WgEventHandler;

class WgHook
{
	friend class WgGizmo;
	friend class WgPanel;
	friend class WgSortablePanel;

public:
	virtual WgCoord	Pos() const = 0;
	virtual WgSize	Size() const = 0;
	virtual WgRect	Geo() const = 0;
	virtual WgCoord	ScreenPos() const = 0;
	virtual WgRect	ScreenGeo() const = 0;

	WgHook *		Prev() const { return _prevHook(); }
	WgHook *		Next() const { return _nextHook(); }

	virtual bool	SetVisible( bool bVisible );
	bool			IsVisible() { return m_bVisible; }

	WgGizmo *			Gizmo() const { return m_pGizmo; }
	WgGizmoContainer * 	Parent() const { return _parent(); }
	virtual WgRootPanel *	Root() const;
	WgEventHandler *	EventHandler() const;

	virtual const char *Type( void ) const = 0;
	
protected:

	WgHook() : m_pGizmo(0), m_bVisible(true) {}
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
	virtual WgGizmoContainer * _parent() const = 0;


	WgGizmo *		m_pGizmo;
	bool			m_bVisible;

};

template<typename T> T* WgCast(WgHook * pHook)
{
	if(pHook)
	{
		if(T::ClassType() == pHook->Type())
			return static_cast<T*>(pHook);
	}
	return 0;
}

#endif //WG_HOOK_DOT_H
