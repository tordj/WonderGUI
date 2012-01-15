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

#ifndef WG_VBOXLAYOUT_DOT_H
#define WG_VBOXLAYOUT_DOT_H

#ifndef WG_ORDERED_LAYOUT_DOT_H
#	include <wg_orderedlayout.h>
#endif

class WgVBoxLayout;

class WgVBoxHook : public WgOrderedHook
{
	friend class WgVBoxLayout;

public:
	inline WgVBoxHook * Prev() const { return _prev(); }
	inline WgVBoxHook * Next() const { return _next(); }
	inline WgVBoxLayout * Parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgVBoxHook );

	WgVBoxHook( WgVBoxLayout * pParent );

	WgGizmoContainer * _parent() const;

	WgVBoxLayout *	m_pParent;
	int				m_height;
	WgSize			m_bestSize;			// Cached best size from the child.
};

class WgVBoxLayout : public WgOrderedLayout
{
public:
	WgVBoxLayout();
	~WgVBoxLayout();

	const char * Type() const;
	static const char * GetMyType();

	inline WgVBoxHook * AddChild( WgGizmo * pGizmo ) { return static_cast<WgVBoxHook*>(WgOrderedLayout::AddChild(pGizmo)); }
	inline WgVBoxHook * InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling ) { return static_cast<WgVBoxHook*>(WgOrderedLayout::InsertChild(pGizmo,pSibling)); }
	inline WgVBoxHook * InsertChildSorted( WgGizmo * pGizmo ) { return static_cast<WgVBoxHook*>(WgOrderedLayout::InsertChildSorted(pGizmo)); }

	inline WgVBoxHook* FirstHook() const { return static_cast<WgVBoxHook*>(m_hooks.First()); }
	inline WgVBoxHook* LastHook() const { return static_cast<WgVBoxHook*>(m_hooks.Last()); }

	// Overloaded from WgGizmo

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

	WgSize	DefaultSize() const;

protected:

	// Overloaded from Gizmo

//	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onNewSize( const WgSize& size );

	// Overloaded from WgOrderedLayout

	WgHook *	_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgRect	_hookGeo( const WgOrderedHook * pHook );
	void	_onResizeRequested( WgOrderedHook * pHook );
	void	_onRenderRequested( WgOrderedHook * pHook );
	void	_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect );
	void	_onGizmoAppeared( WgOrderedHook * pInserted );				// so parent can update geometry and possibly request render.
	void	_onGizmoDisappeared( WgOrderedHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
	void	_onGizmosReordered();
	void	_refreshAllGizmos();
	WgOrderedHook * _newHook();

	// Internal to WgVBoxLayout

	void	_adaptChildrenToWidth( int width );
	void 	_refreshDefaultSize();
	void	_refreshDefaultWidth();
	void	_renderFromChildOnward( WgOrderedHook * pHook );


	WgSize	m_size;
	WgSize	m_bestSize;
	int		m_nBestWidth;				// Number of Gizmos who have exactly m_bestSize.w as their prefered width.

};



#endif //WG_VBOXLAYOUT_DOT_H
