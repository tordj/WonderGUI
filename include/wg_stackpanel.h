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

#ifndef WG_STACKPANEL_DOT_H
#define WG_STACKPANEL_DOT_H

#ifndef WG_SORTABLEPANEL_DOT_H
#	include <wg_sortablepanel.h>
#endif

class WgStackPanel;

class WgStackHook : public WgSortableHook
{
	friend class WgStackPanel;
public:
	const char *Type( void ) const;
	static const char * ClassType();

	enum SizePolicy
	{
		DEFAULT,
		STRETCH,
		SCALE
	};

	void			SetSizePolicy( SizePolicy policy );
	SizePolicy		GetSizePolicy() const { return m_sizePolicy; }
	
	void			SetBorders( WgBorders borders );
	WgBorders		Borders() const { return m_borders; }
	
	void			SetOrientation( WgOrientation orientation );
	WgOrientation	Orientation() const { return m_orientation; }

	WgStackHook * 	Prev() const { return _prev(); }
	WgStackHook * 	Next() const { return _next(); }

	WgStackPanel * 	Parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgStackHook );

	WgStackHook( WgStackPanel * pParent );

	WgPanel * _parent() const;

	WgRect			_getGeo( const WgRect& parentGeo ) const;

	WgStackPanel *	m_pParent;
	
	WgBorders		m_borders;
	WgOrientation	m_orientation;
	SizePolicy		m_sizePolicy;
	
};

class WgStackPanel : public WgSortablePanel
{
friend class WgStackHook;	
	
public:
	WgStackPanel();
	~WgStackPanel();

	const char * Type() const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgStackPanel(); };
	

	inline WgStackHook * AddChild( WgWidget * pWidget ) { return static_cast<WgStackHook*>(WgSortablePanel::AddChild(pWidget)); }
	inline WgStackHook * InsertChild( WgWidget * pWidget, WgWidget * pSibling ) { return static_cast<WgStackHook*>(WgSortablePanel::InsertChild(pWidget,pSibling)); }
	inline WgStackHook * InsertChildSorted( WgWidget * pWidget ) { return static_cast<WgStackHook*>(WgSortablePanel::InsertChildSorted(pWidget)); }

	inline WgStackHook* FirstHook() const { return static_cast<WgStackHook*>(m_hooks.First()); }
	inline WgStackHook* LastHook() const { return static_cast<WgStackHook*>(m_hooks.Last()); }

	// Overloaded from WgWidget

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

	WgSize	MinSize() const;
	WgSize	DefaultSize() const;
	WgSize	MaxSize() const;

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

	// Internal to WgStackPanel

	void 	_refreshDefaultSize();
	void	_adaptChildrenToSize();
	void	_renderFromChildOnward( WgSortableHook * pHook );


	WgSize	m_size;
	WgSize	m_bestSize;

};



#endif //WG_STACKPANEL_DOT_H
