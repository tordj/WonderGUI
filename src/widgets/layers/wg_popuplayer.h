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

#ifndef WG_POPUPLAYER_DOT_H
#define WG_POPUPLAYER_DOT_H

#ifndef WG_LAYER_DOT_H
#	include <wg_layer.h>
#endif


class WgPopupLayer;
typedef	WgStrongPtr<WgPopupLayer,WgLayerPtr>		WgPopupLayerPtr;
typedef	WgWeakPtr<WgPopupLayer,WgLayerWeakPtr>	WgPopupLayerWeakPtr;

class WgPopupHook;
typedef	WgHookTypePtr<WgPopupHook,WgLayerHookPtr>	WgPopupHookPtr;

class WgPopupHook : public WgLayerHook, protected WgLink
{
	friend class WgPopupLayer;
	friend class WgChain<WgPopupHook>;

public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgPopupHookPtr	Cast( const WgHookPtr& pInterface );

	// Standard Hook methods

	WgPopupHookPtr	Prev() const { return _prev(); }
	WgPopupHookPtr	Next() const { return _next(); }

	WgPopupLayerPtr	Parent() const;

protected:
	// TODO: Constructor should in the future call SetHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgPopupHook );

	WgPopupHook( WgPopupLayer * pParent, WgWidget * pOpener, const WgRect& launcherGeo, WgOrigo attachPoint, WgSize maxSize );

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgLayerHook *	_prevLayerHook() const;			// Iterate through all hooks except the base hook
	WgLayerHook *	_nextLayerHook() const;			// Iterate through all hooks except the base hook
	WgContainer*	_parent() const;

	bool			_updateGeo();


	WgPopupLayer *	m_pParent;

	WgRect			m_launcherGeo;		// Launcher geo relative sibling or parent.
	WgOrigo			m_attachPoint;
	WgSize			m_maxSize;
	WgWidgetWeakPtr	m_pOpener;			// Widget that opened this popup.
	WgWidgetWeakPtr	m_pKeyFocus;		// Pointer at widget that held focus when this popup was ontop.
};


//____ WgPopupLayer ____________________________________________________________

class WgPopupLayer : public WgLayer
{
	friend class WgPopupHook;

public:
	static WgPopupLayerPtr	Create() { return WgPopupLayerPtr(new WgPopupLayer()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgPopupLayerPtr	Cast( const WgObjectPtr& pObject );

	WgPopupHookPtr	OpenPopup( const WgWidgetPtr& pPopup, const WgWidgetPtr& pOpener, const WgRect& launcherGeo, WgOrigo attachPoint = WG_NORTHEAST, WgSize maxSize = WgSize(INT_MAX,INT_MAX) );

	bool			ClosePopup( const WgWidgetPtr& pPopup );
	bool			CloseAllPopups();

	WgPopupHookPtr	FirstPopupHook();
	WgPopupHookPtr	LastPopupHook();

	// Overloaded from WgPanel

	bool			RemoveWidget( const WgWidgetPtr& pWidget ) { return false; }
	bool			Clear() { return false; }

protected:
	WgPopupLayer();
	virtual ~WgPopupLayer();
	virtual WgWidget* _newOfMyType() const { return new WgPopupLayer(); };

	WgWidget *		_findWidget( const WgCoord& ofs, WgSearchMode mode );

private:

	// Overloaded from WgLayer

	WgLayerHook *	_firstLayerHook() const { return m_popupHooks.First(); }
	WgLayerHook *	_lastLayerHook() const { return m_popupHooks.Last(); }

	//

	WgPopupLayer *	_getPopupLayer() const { return const_cast<WgPopupLayer*>(this); }

	void			_stealKeyboardFocus();
	void			_restoreKeyboardFocus();

	//

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onMsg( const WgMsgPtr& pMsg );

	void			_onRequestRender( const WgRect& rect, const WgPopupHook * pHook );	// rect is in our coordinate system.

	WgChain<WgPopupHook>		m_popupHooks;		// First popup lies at the bottom.

	WgWidgetWeakPtr	m_pKeyFocus;		// Pointer at child that held focus before any menu was opened.


};

#endif //WG_POPUPLAYER_DOT_H
