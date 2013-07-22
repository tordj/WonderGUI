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

#ifndef WG_VECTORPANEL_DOT_H
#define WG_VECTORPANEL_DOT_H

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

class WgVectorPanel;
typedef	WgSmartChildPtr<WgVectorPanel,WgPanelPtr>		WgVectorPanelPtr;
typedef	WgWeakChildPtr<WgVectorPanel,WgPanelWeakPtr>	WgVectorPanelWeakPtr;

//____ WgVectorHook ___________________________________________________________

class WgVectorHook : public WgPanelHook, protected WgLink
{
	friend class WgVectorPanel;
	friend class WgChain<WgVectorHook>;


public:
	WgCoord	Pos() const;
	WgSize	Size() const;
	WgRect	Geo() const;
	WgCoord	ScreenPos() const;
	WgRect	ScreenGeo() const;

	WgVectorHook*	Prev() const { return _prev(); }
	WgVectorHook*	Next() const { return _next(); }
	WgVectorPanelPtr Parent() const;

	bool			MoveForward();
	bool			MoveBackward();
	bool			MoveBefore( WgVectorHook * pSibling );
	bool			MoveAfter( WgVectorHook * pSibling );
	bool			MoveFirst();
	bool			MoveLast();

	bool			SetVisible( bool bVisible );

protected:
	PROTECTED_LINK_METHODS( WgVectorHook );

	WgVectorHook();
	~WgVectorHook();

	void	_requestRender();
	void	_requestRender( const WgRect& rect );
	void	_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;
};

//____ WgVectorPanel __________________________________________________________

class WgVectorPanel : public WgPanel
{
	friend class WgVectorHook;
public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgVectorPanelPtr	Cast( const WgObjectPtr& pObject );

	WgVectorHook * AddChild( const WgWidgetPtr& pWidget );
	WgVectorHook * InsertChild( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling );

	bool			RemoveChild( const WgWidgetPtr& pWidget );
	bool			Clear();

protected:
	WgVectorPanel();
	virtual ~WgVectorPanel();

	void			_onCloneContent( const WgWidget * _pOrg );

	WgHook*			_firstHook() const { return m_hooks.First(); }
	WgHook*			_lastHook() const { return m_hooks.Last(); }

	// To be overloaded by subclasses

	virtual WgRect	_hookGeo( const WgVectorHook * pHook ) = 0;
	virtual void	_onResizeRequested( WgVectorHook * pHook ) = 0;
	virtual void	_onRenderRequested( WgVectorHook * pHook ) = 0;
	virtual void	_onRenderRequested( WgVectorHook * pHook, const WgRect& rect ) = 0;
	virtual void	_onWidgetAppeared( WgVectorHook * pInserted ) = 0;				// so parent can update geometry and possibly request render.
	virtual void	_onWidgetDisappeared( WgVectorHook * pToBeRemoved ) = 0;		// so parent can update geometry and possibly request render.
	virtual void	_onWidgetsReordered() = 0;
	virtual void	_refreshAllWidgets() = 0;
	virtual WgVectorHook * _newHook() = 0;

	//

	WgChain<WgVectorHook>	m_hooks;

	int					m_nChildPanels;					// Number of VISIBLE children that are panels.
};



#endif //WG_VECTORPANEL_DOT_H