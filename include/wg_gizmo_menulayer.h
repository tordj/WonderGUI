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


class WgGizmoMenuLayer;

class WgMenuHook : public WgHook, protected WgLink
{
	friend class WgGizmoMenuLayer;
	friend class WgChain<WgMenuHook>;

public:

	// Standard Hook methods

	WgCoord			Pos() const { return m_geo.Pos(); }
	WgSize			Size() const { 	return m_geo.Size(); }
	WgRect			Geo() const { return m_geo; }

	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgMenuHook *	Prev() const { return _prev(); }
	WgMenuHook *	Next() const { return _next(); }

	WgGizmoContainer* Parent() const;

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgMenuHook );

	WgMenuHook( WgGizmoMenuLayer * pParent, const WgRect& launcherGeo, WgOrientation attachPoint, WgSize maxSize );

	void		_requestRender();
	void		_requestRender( const WgRect& rect );
	void		_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;
	WgGizmoContainer * _parent() const;

	bool		_updateGeo();


	WgGizmoMenuLayer * m_pParent;

	WgRect			m_geo;				// Gizmos geo relative parent
	WgRect			m_launcherGeo;		// Launcher geo relative sibling or parent.
	WgOrientation	m_attachPoint;
	WgSize			m_maxSize;
	WgGizmoWeakPtr	m_pKeyFocus;		// Pointer at gizmo that held focus when this menu was ontop.
};



class WgGizmoMenuLayer : public WgGizmo, public WgGizmoContainer
{
	friend class BaseHook;
	friend class WgMenuHook;

public:
	WgGizmoMenuLayer();
	~WgGizmoMenuLayer();

	virtual const char *Type( void ) const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoMenuLayer(); };


	WgHook *		SetBase( WgGizmo * pGizmo );
	WgGizmo *		Base();
	bool			DeleteBase();
	WgGizmo *		ReleaseBase();

	WgMenuHook *	OpenMenu( WgGizmo * pMenu, const WgRect& launcherGeo, WgOrientation attachPoint = WG_NORTHEAST, WgSize maxSize = WgSize(INT_MAX,INT_MAX) );

	bool			CloseMenu( WgGizmo * pMenu );
	bool			CloseAllMenus();

	WgMenuHook *	FirstMenu();
	WgMenuHook *	LastMenu();


	// Overloaded from WgGizmo

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			DefaultSize() const;

	bool			IsView() const { return false; }
	bool			IsContainer() const { return true; }
	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }

	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCoord& ofs, WgSearchMode mode );

	bool			DeleteChild( WgGizmo * pGizmo ) { return 0; }
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo ) { return 0; }

	bool			DeleteAllChildren() { return 0; }
	bool			ReleaseAllChildren() { return 0; }

	WgGizmo*		CastToGizmo() { return this; }
	const WgGizmo*	CastToGizmo() const { return this; }

private:

	class BaseHook : public WgHook
	{
		friend class WgGizmoMenuLayer;

	public:
		// Standard Hook methods

		WgCoord		Pos() const { return m_pParent->Pos(); }
		WgSize		Size() const { 	return m_pParent->Size(); }
		WgRect		Geo() const { return m_pParent->Geo(); }

		WgCoord		ScreenPos() const { return m_pParent->ScreenPos(); }
		WgRect		ScreenGeo() const { return m_pParent->ScreenGeo(); }

		WgGizmoMenuLayer* Parent() const { return m_pParent; }

		WgWidget*	GetRoot() { return 0; }			// Should in the future not return a widget, but a gizmo.

	protected:
		BaseHook( WgGizmoMenuLayer * pParent ) : m_pParent(pParent) {}

		void		_requestRender();
		void		_requestRender( const WgRect& rect );
		void		_requestResize();

		WgHook *	_prevHook() const { return 0; }
		WgHook *	_nextHook() const { return m_pParent->FirstMenu(); }
		WgGizmoParent * _parent() const { return m_pParent; }

		WgGizmoMenuLayer * 	m_pParent;
	};


	WgGizmoMenuLayer *	_getMenuLayer() const { return const_cast<WgGizmoMenuLayer*>(this); }

	void			_stealKeyboardFocus();
	void			_restoreKeyboardFocus();


	// These are needed until WgGizmoContainer inherits from WgGizmo

	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
									{ WgGizmoContainer::_renderPatches( pDevice, _canvas, _window, _pPatches, _layer ); }
	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
									{ WgGizmoContainer::_onCollectPatches(container, geo, clip); }
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
									{ WgGizmoContainer::_onMaskPatches(patches, geo, clip, blendMode); }
	void			_onEnable() { WgGizmoContainer::_onEnable(); }
	void			_onDisable() { WgGizmoContainer::_onDisable(); }
	bool 			_onAlphaTest( const WgCoord& ofs ) { return WgGizmoContainer::_onAlphaTest(ofs); }

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
