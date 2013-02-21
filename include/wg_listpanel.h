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

#ifndef WG_LISTPANEL_DOT_H
#define WG_LISTPANEL_DOT_H

#ifndef WG_VECTORPANEL_DOT_H
#	include <wg_vectorpanel.h>
#endif

class WgListPanel;

class WgListHook : public WgVectorHook
{
	friend class WgListPanel;

public:
	const char *Type( void ) const;
	static const char * ClassType();

	inline WgListHook * Prev() const { return _prev(); }
	inline WgListHook * Next() const { return _next(); }
	inline WgListPanel * Parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgListHook );

	WgListHook( WgListPanel * pParent );

	WgContainer * _parent() const;

	WgListPanel *		m_pParent;
	int				m_height;
	WgSize			m_preferredSize;			// Cached best size from the child.
};

class WgListPanel : public WgVectorPanel
{
public:
	WgListPanel();
	~WgListPanel();

	const char * Type() const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgListPanel(); };
	

	inline WgListHook * AddChild( WgWidget * pWidget ) { return static_cast<WgListHook*>(WgVectorPanel::AddChild(pWidget)); }
	inline WgListHook * InsertChild( WgWidget * pWidget, WgWidget * pSibling ) { return static_cast<WgListHook*>(WgVectorPanel::InsertChild(pWidget,pSibling)); }
	inline WgListHook * InsertChildSorted( WgWidget * pWidget ) { return static_cast<WgListHook*>(WgVectorPanel::InsertChildSorted(pWidget)); }

	inline WgListHook* FirstHook() const { return static_cast<WgListHook*>(m_hooks.First()); }
	inline WgListHook* LastHook() const { return static_cast<WgListHook*>(m_hooks.Last()); }

	// Overloaded from WgWidget

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

	WgSize	PreferredSize() const;

protected:

	// Overloaded from Widget

//	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onNewSize( const WgSize& size );

	// Overloaded from WgPanel

	WgHook *	_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	// Overloaded from WgVectorPanel

	WgRect	_hookGeo( const WgVectorHook * pHook );
	void	_onResizeRequested( WgVectorHook * pHook );
	void	_onRenderRequested( WgVectorHook * pHook );
	void	_onRenderRequested( WgVectorHook * pHook, const WgRect& rect );
	void	_onWidgetAppeared( WgVectorHook * pInserted );				// so parent can update geometry and possibly request render.
	void	_onWidgetDisappeared( WgVectorHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
	void	_onWidgetsReordered();
	void	_refreshAllWidgets();
	WgVectorHook * _newHook();

	// Internal to WgListPanel

	void	_adaptChildrenToWidth( int width );
	void 	_refreshPreferredSize();
	void	_refreshDefaultWidth();
	void	_renderFromChildOnward( WgVectorHook * pHook );


	WgSize	m_size;
	WgSize	m_preferredSize;
	int		m_nPreferredWidth;				// Number of Widgets who have exactly m_preferredSize.w as their prefered width.

	

};



#endif //WG_LISTPANEL_DOT_H
