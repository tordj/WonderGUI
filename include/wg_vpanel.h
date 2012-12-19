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

#ifndef WG_VPANEL_DOT_H
#define WG_VPANEL_DOT_H

#ifndef WG_BOXLAYOUT_DOT_H
#	include <wg_boxlayout.h>
#endif

class WgVPanel;

class WgVHook : public WgSortableHook
{
	friend class WgVPanel;

public:
	const char *Type( void ) const;
	static const char * ClassType();

	inline WgVHook * Prev() const { return _prev(); }
	inline WgVHook * Next() const { return _next(); }
	inline WgVPanel * Parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgVHook );

	WgVHook( WgVPanel * pParent );

	WgPanel * _parent() const;

	WgVPanel *	m_pParent;
	int				m_height;
	WgSize			m_bestSize;			// Cached best size from the child.
};

class WgVPanel : public WgBoxLayout
{
public:
	WgVPanel();
	~WgVPanel();

	const char * Type() const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgVPanel(); };
	

	inline WgVHook * AddChild( WgWidget * pWidget ) { return static_cast<WgVHook*>(WgSortablePanel::AddChild(pWidget)); }
	inline WgVHook * InsertChild( WgWidget * pWidget, WgWidget * pSibling ) { return static_cast<WgVHook*>(WgSortablePanel::InsertChild(pWidget,pSibling)); }
	inline WgVHook * InsertChildSorted( WgWidget * pWidget ) { return static_cast<WgVHook*>(WgSortablePanel::InsertChildSorted(pWidget)); }

	inline WgVHook* FirstHook() const { return static_cast<WgVHook*>(m_hooks.First()); }
	inline WgVHook* LastHook() const { return static_cast<WgVHook*>(m_hooks.Last()); }

	// Overloaded from WgWidget

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

	WgSize	DefaultSize() const;

protected:

	// Overloaded from Widget

//	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onNewSize( const WgSize& size );

	// Overloaded from WgPanel

	WgHook *	_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	// Overloaded from WgSortablePanel

	WgRect	_hookGeo( const WgSortableHook * pHook );
	void	_onResizeRequested( WgSortableHook * pHook );
	void	_onRenderRequested( WgSortableHook * pHook );
	void	_onRenderRequested( WgSortableHook * pHook, const WgRect& rect );
	void	_onWidgetAppeared( WgSortableHook * pInserted );				// so parent can update geometry and possibly request render.
	void	_onWidgetDisappeared( WgSortableHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
	void	_onWidgetsReordered();
	void	_refreshAllWidgets();
	WgSortableHook * _newHook();

	// Overloaded from WgBoxLayout

	void	_onSpaceReallocated();

	// Internal to WgVPanel

	void	_adaptChildrenToWidth( int width );
	void 	_refreshDefaultSize();
	void	_refreshDefaultWidth();
	void	_renderFromChildOnward( WgSortableHook * pHook );


	WgSize	m_size;
	WgSize	m_bestSize;
	int		m_nBestWidth;				// Number of Widgets who have exactly m_bestSize.w as their prefered width.

	

};



#endif //WG_VPANEL_DOT_H
