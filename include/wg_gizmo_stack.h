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

#ifndef WG_GIZMO_STACK_DOT_H
#define WG_GIZMO_STACK_DOT_H

#ifndef WG_ORDERED_LAYOUT_DOT_H
#	include <wg_orderedlayout.h>
#endif

class WgGizmoStack;

class WgStackHook : public WgOrderedHook
{
	friend class WgGizmoStack;
public:

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

	WgGizmoStack * 	Parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgStackHook );

	WgStackHook( WgGizmoStack * pParent );

	WgGizmoContainer * _parent() const;

	WgRect			_getGeo( const WgRect& parentGeo ) const;

	WgGizmoStack *	m_pParent;
	
	WgBorders		m_borders;
	WgOrientation	m_orientation;
	SizePolicy		m_sizePolicy;
	
};

class WgGizmoStack : public WgOrderedLayout
{
friend class WgStackHook;	
	
public:
	WgGizmoStack();
	~WgGizmoStack();

	const char * Type() const;
	static const char * GetMyType();

	inline WgStackHook * AddChild( WgGizmo * pGizmo ) { return static_cast<WgStackHook*>(WgOrderedLayout::AddChild(pGizmo)); }
	inline WgStackHook * InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling ) { return static_cast<WgStackHook*>(WgOrderedLayout::InsertChild(pGizmo,pSibling)); }
	inline WgStackHook * InsertChildSorted( WgGizmo * pGizmo ) { return static_cast<WgStackHook*>(WgOrderedLayout::InsertChildSorted(pGizmo)); }

	inline WgStackHook* FirstHook() const { return static_cast<WgStackHook*>(m_hooks.First()); }
	inline WgStackHook* LastHook() const { return static_cast<WgStackHook*>(m_hooks.Last()); }

//	inline WgVBoxHook* FirstSelectedHook() const { return static_cast<WgVBoxHook*>(_firstSelectedHook()); }
//	inline WgVBoxHook* LastSelectedHook() const { return static_cast<WgVBoxHook*>(_lastSelectedHook()); }


	// Overloaded from WgGizmo

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

	WgSize	MinSize() const;
	WgSize	DefaultSize() const;
	WgSize	MaxSize() const;

protected:

	// Overloaded from Gizmo

//	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onNewSize( const WgSize& size );

	// Overloaded from WgContainer

	WgHook *	_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	// Overloaded from WgOrderedLayout

	WgRect	_hookGeo( const WgOrderedHook * pHook );
	void	_onResizeRequested( WgOrderedHook * pHook );
	void	_onRenderRequested( WgOrderedHook * pHook );
	void	_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect );
	void	_onGizmoAppeared( WgOrderedHook * pInserted );				// so parent can update geometry and possibly request render.
	void	_onGizmoDisappeared( WgOrderedHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
	void	_onGizmosReordered();
	void	_refreshAllGizmos();
	WgOrderedHook * _newHook();

	// Internal to WgGizmoStack

	void 	_refreshDefaultSize();
	void	_adaptChildrenToSize();
	void	_renderFromChildOnward( WgOrderedHook * pHook );


	WgSize	m_size;
	WgSize	m_bestSize;

};



#endif //WG_GIZMO_STACK_DOT_H
