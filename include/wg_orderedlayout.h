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

#ifndef WG_ORDERED_LAYOUT_DOT_H
#define WG_ORDERED_LAYOUT_DOT_H

#ifndef WG_HOOK_DOT_H
#	include <wg_hook.h>
#endif

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif

#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_RECTCHAIN_DOT_H
#	include <wg_rectchain.h>
#endif

class WgOrderedLayout;


class WgOrderedHook : public WgHook, protected WgLink
{
	friend class WgOrderedLayout;
	friend class WgChain<WgOrderedHook>;


public:
	WgCoord	Pos() const;
	WgSize	Size() const;
	WgRect	Geo() const;
	WgCoord	ScreenPos() const;
	WgRect	ScreenGeo() const;

	inline WgOrderedHook*	Prev() const { return _prev(); }
	inline WgOrderedHook*	Next() const { return _next(); }
	inline WgOrderedLayout * Parent() const;

	bool			MoveUp();
	bool			MoveDown();
	bool			MoveBefore( WgOrderedHook * pSibling );
	bool			MoveAfter( WgOrderedHook * pSibling );
	bool			MoveFirst();
	bool			MoveLast();

	inline void		Hide() { SetHidden(true); }
	inline void		Unhide() { SetHidden(false); }
	void			SetHidden( bool bHide );
	bool			Hidden() { return m_bHidden; }

	// Needs to be here for now since Emitters are inherrited by Widgets. Shouldn't be hooks business in the future...

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	PROTECTED_LINK_METHODS( WgOrderedHook );

	WgOrderedHook();
	~WgOrderedHook();

	void	_requestRender();
	void	_requestRender( const WgRect& rect );
	void	_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;

	bool				m_bHidden;
};


class WgOrderedLayout : public WgGizmo, public WgGizmoContainer
{
	friend class WgOrderedHook;
public:
	WgOrderedLayout();
	virtual ~WgOrderedLayout();


	WgOrderedHook * AddChild( WgGizmo * pGizmo );
	WgOrderedHook * InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling );
	WgOrderedHook * InsertChildSorted( WgGizmo * pGizmo );

	bool			DeleteChild( WgGizmo * pGizmo );
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();

	void			SortChildren();
	void			SetSortOrder( WgSortOrder order );
	inline WgSortOrder	GetSortOrder() const { return m_sortOrder; }

	void			SetSortFunction( WgGizmoSortFunc pSortFunc );
	WgGizmoSortFunc	SortFunction() const { return m_pSortFunc; }

	// Overloaded from WgGizmo

	bool			IsView() const { return false; }
	bool			IsContainer() const { return true; }

	WgGizmoContainer *	CastToContainer() { return this; }
	const WgGizmoContainer *	CastToContainer() const { return this; }

	inline WgGizmo*	CastToGizmo() { return this; }

	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCoord& ofs, WgSearchMode mode );	// Default OrderedLayout implementation, assuming front-gizmos overlapping end-gizmos in case of overlap.

protected:
	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
									{ WgGizmoContainer::_renderPatches( pDevice, _canvas, _window, _pPatches, _layer ); }

	void			_onCloneContent( const WgGizmo * _pOrg );

	inline void		_onEnable() { WgGizmoContainer::_onEnable(); }		// Needed until WgGizmoContainer inerits from WgGizmo
	inline void		_onDisable() { WgGizmoContainer::_onDisable(); }		// Needed until WgGizmoContainer inerits from WgGizmo
	inline bool 	_onAlphaTest( const WgCoord& ofs ) { WgGizmoContainer::_onAlphaTest(ofs); }

	inline WgHook*	_firstHook() const { return m_hooks.First(); }
	inline WgHook*	_lastHook() const { return m_hooks.Last(); }

	inline int		_compareGizmos(const WgGizmo * p1, const WgGizmo * p2) { return m_pSortFunc?m_pSortFunc(p1,p2):0; }

	// To be overloaded by subclasses

	virtual WgRect	_hookGeo( const WgOrderedHook * pHook ) = 0;
	virtual void	_onResizeRequested( WgOrderedHook * pHook ) = 0;
	virtual void	_onRenderRequested( WgOrderedHook * pHook ) = 0;
	virtual void	_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect ) = 0;
	virtual void	_onGizmoAppeared( WgOrderedHook * pInserted ) = 0;				// so parent can update geometry and possibly request render.
	virtual void	_onGizmoDisappeared( WgOrderedHook * pToBeRemoved ) = 0;		// so parent can update geometry and possibly request render.
	virtual void	_onGizmosReordered() = 0;
	virtual void	_refreshAllGizmos() = 0;
	virtual WgOrderedHook * _newHook() = 0;

	//

	WgChain<WgOrderedHook>	m_hooks;
	WgRectChain				m_dirt;

	WgSortOrder		m_sortOrder;
	WgGizmoSortFunc	m_pSortFunc;
	int				m_nChildContainers;					// Number of VISIBLE children that are containers.
};



#endif //WG_ORDERED_LAYOUT_DOT_H
