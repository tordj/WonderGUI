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

#ifndef WG_MENULAYER_DOT_H
#define WG_MENULAYER_DOT_H

#ifndef WG_LAYER_DOT_H
#	include <wg_layer.h>
#endif


class WgMenuLayer;
typedef	WgSmartPtr<WgMenuLayer,WgLayerPtr>		WgMenuLayerPtr;
typedef	WgWeakPtr<WgMenuLayer,WgLayerWeakPtr>	WgMenuLayerWeakPtr;

class WgMenuHook : public WgLayerHook, protected WgLink
{
	friend class WgMenuLayer;
	friend class WgChain<WgMenuHook>;

public:

	const char *Type( void ) const;
	static const char * ClassType();

	// Standard Hook methods

	WgMenuHook *	Prev() const { return _prev(); }
	WgMenuHook *	Next() const { return _next(); }

	WgMenuLayerPtr	Parent() const;

protected:
	// TODO: Constructor should in the future call SetHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgMenuHook );

	WgMenuHook( WgMenuLayer * pParent, WgWidget * pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize );

	void		_requestRender();
	void		_requestRender( const WgRect& rect );
	void		_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;
	WgContainer* _parent() const;

	bool		_updateGeo();


	WgMenuLayer * m_pParent;

	WgRect			m_launcherGeo;		// Launcher geo relative sibling or parent.
	WgOrigo	m_attachPoint;
	WgSize			m_maxSize;
	WgWidgetWeakPtr	m_pOpener;			// Widget that opened this menu.
	WgWidgetWeakPtr	m_pKeyFocus;		// Pointer at widget that held focus when this menu was ontop.
};


//____ WgMenuLayer ____________________________________________________________

class WgMenuLayer : public WgLayer
{
	friend class WgMenuHook;

public:
	static WgMenuLayerPtr	Create() { return WgMenuLayerPtr(new WgMenuLayer()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMenuLayerPtr	Cast( const WgObjectPtr& pObject );

	WgMenuHook *	OpenMenu( const WgWidgetPtr& pMenu, const WgWidgetPtr& pOpener, const WgRect& launcherGeo, WgOrigo attachPoint = WG_NORTHEAST, WgSize maxSize = WgSize(INT_MAX,INT_MAX) );

	bool			CloseMenu( const WgWidgetPtr& pMenu );
	bool			CloseAllMenus();

	WgMenuHook *	FirstMenu();
	WgMenuHook *	LastMenu();

	// Overloaded from WgPanel

	bool			RemoveChild( const WgWidgetPtr& pWidget ) { return false; }
	bool			Clear() { return false; }

protected:
	WgMenuLayer();
	virtual ~WgMenuLayer();
	virtual WgWidget* _newOfMyType() const { return new WgMenuLayer(); };

	WgWidget *		_findWidget( const WgCoord& ofs, WgSearchMode mode );

private:

	// Overloaded from WgLayer

	WgLayerHook *	_firstLayerHook() const { return m_menuHooks.First(); }

	//

	WgMenuLayer *	_getMenuLayer() const { return const_cast<WgMenuLayer*>(this); }

	void			_stealKeyboardFocus();
	void			_restoreKeyboardFocus();

	//

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );

	void			_onRequestRender( const WgRect& rect, const WgMenuHook * pHook );	// rect is in our coordinate system.

	WgHook*			_firstHook() const;		// Fist Hook returned is the normal child, then follows the modal ones.
	WgHook*			_lastHook() const;		//

	WgHook *		_firstHookWithGeo( WgRect& geo ) const;
	WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& geo ) const;
	WgHook *		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgChain<WgMenuHook>		m_menuHooks;		// First menu lies at the bottom.

	WgWidgetWeakPtr	m_pKeyFocus;		// Pointer at child that held focus before any menu was opened.


};

#endif //WG_MENULAYER_DOT_H
