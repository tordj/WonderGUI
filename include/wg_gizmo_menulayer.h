/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_GIZMO_MENULAYER_DOT_H
#define WG_GIZMO_MENULAYER_DOT_H

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif


class WgGizmoMenulayer;

class WgMenuHook : public WgHook, protected WgLink
{
	friend class WgGizmoMenulayer;
	friend class WgChain<WgMenuHook>;

public:

	const char *Type( void ) const;
	static const char * ClassType();

	// Standard Hook methods

	WgCoord			Pos() const { return m_geo.Pos(); }
	WgSize			Size() const { 	return m_geo.Size(); }
	WgRect			Geo() const { return m_geo; }

	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgMenuHook *	Prev() const { return _prev(); }
	WgMenuHook *	Next() const { return _next(); }

	WgGizmoContainer* Parent() const;

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgMenuHook );

	WgMenuHook( WgGizmoMenulayer * pParent, WgGizmo * pOpener, const WgRect& launcherGeo, WgOrientation attachPoint, WgSize maxSize );

	void		_requestRender();
	void		_requestRender( const WgRect& rect );
	void		_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;
	WgGizmoContainer * _parent() const;

	bool		_updateGeo();


	WgGizmoMenulayer * m_pParent;

	WgRect			m_geo;				// Gizmos geo relative parent
	WgRect			m_launcherGeo;		// Launcher geo relative sibling or parent.
	WgOrientation	m_attachPoint;
	WgSize			m_maxSize;
	WgGizmoWeakPtr	m_pOpener;			// Gizmo that opened this menu.
	WgGizmoWeakPtr	m_pKeyFocus;		// Pointer at gizmo that held focus when this menu was ontop.
};



class WgGizmoMenulayer : public WgGizmoContainer
{
	friend class BaseHook;
	friend class WgMenuHook;

public:
	WgGizmoMenulayer();
	~WgGizmoMenulayer();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoMenulayer(); };


	WgHook *		SetBase( WgGizmo * pGizmo );
	WgGizmo *		Base();
	bool			DeleteBase();
	WgGizmo *		ReleaseBase();

	WgMenuHook *	OpenMenu( WgGizmo * pMenu, WgGizmo * pOpener, const WgRect& launcherGeo, WgOrientation attachPoint = WG_NORTHEAST, WgSize maxSize = WgSize(INT_MAX,INT_MAX) );

	bool			CloseMenu( WgGizmo * pMenu );
	bool			CloseAllMenus();

	WgMenuHook *	FirstMenu();
	WgMenuHook *	LastMenu();


	// Overloaded from WgGizmo

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			DefaultSize() const;

	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCoord& ofs, WgSearchMode mode );

	bool			DeleteChild( WgGizmo * pGizmo ) { return 0; }
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo ) { return 0; }

	bool			DeleteAllChildren() { return 0; }
	bool			ReleaseAllChildren() { return 0; }

private:

	class BaseHook : public WgHook
	{
		friend class WgGizmoMenulayer;

	public:

		const char *Type( void ) const;
		static const char * ClassType();

		// Standard Hook methods

		WgCoord		Pos() const { return m_pParent->Pos(); }
		WgSize		Size() const { 	return m_pParent->Size(); }
		WgRect		Geo() const { return m_pParent->Geo(); }

		WgCoord		ScreenPos() const { return m_pParent->ScreenPos(); }
		WgRect		ScreenGeo() const { return m_pParent->ScreenGeo(); }

		WgGizmoMenulayer* Parent() const { return m_pParent; }

	protected:
		BaseHook( WgGizmoMenulayer * pParent ) : m_pParent(pParent) {}

		void		_requestRender();
		void		_requestRender( const WgRect& rect );
		void		_requestResize();

		WgHook *	_prevHook() const { return 0; }
		WgHook *	_nextHook() const { return m_pParent->FirstMenu(); }
		WgGizmoParent * _parent() const { return m_pParent; }

		WgGizmoMenulayer * 	m_pParent;
	};


	WgGizmoMenulayer *	_getMenuLayer() const { return const_cast<WgGizmoMenulayer*>(this); }

	void			_stealKeyboardFocus();
	void			_restoreKeyboardFocus();

	//

	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );

	void			_onRequestRender( const WgRect& rect, const WgMenuHook * pHook );	// rect is in our coordinate system.

	WgHook*			_firstHook() const;		// Fist Hook returned is the normal child, then follows the modal ones.
	WgHook*			_lastHook() const;		//

	WgHook *		_firstHookWithGeo( WgRect& geo ) const;
	WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& geo ) const;
	WgHook *		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	BaseHook		m_baseHook;
	WgChain<WgMenuHook>		m_menuHooks;		// First menu lies at the bottom.

	WgSize			m_size;
	WgGizmoWeakPtr	m_pKeyFocus;		// Pointer at child that held focus before any menu was opened.


};

#endif //WG_GIZMO_MENULAYER_DOT_H
