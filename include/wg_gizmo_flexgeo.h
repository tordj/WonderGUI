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

#ifndef WG_GIZMO_FLEXGEO_DOT_H
#define WG_GIZMO_FLEXGEO_DOT_H

#include <vector>

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif


#ifndef WG_DIRTYRECT_DOT_H
#	include <wg_dirtyrect.h>
#endif

class WgGizmoFlexGeo;

//____ WgFlexHook _____________________________________________________________

class WgFlexHook : public WgGizmoHook, protected WgLink
{
	friend class WgGizmo;
	friend class WgGizmoContainer;
	friend class WgGizmoFlexGeo;
	friend class WgChain<WgFlexHook>;

public:

	// Flex-specific methods

	bool	SetAnchored();
	bool	SetAnchored( int anchorTopLeft, int anchorBottomRight );
	bool	SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders borders );

	bool	SetFloating();
	bool	SetFloating( const WgRect& geometry );
	bool	SetFloating( const WgRect& geometry, WgLocation hotspot );
	bool	SetFloating( const WgRect& geometry, WgLocation hotspot, int anchor );


	void	Top();								// Put us ontop all our silbings.
	void	Bottom();							// Put us below all our siblings.
	bool	Up();								// Move us one step up among siblings.
	bool	Down();								// Move us one step down among siblings.

	bool	MoveOver( WgFlexHook * pSibling );
	bool	MoveUnder( WgFlexHook * pSibling );

	void	Hide();
	void	Show();

	inline bool	IsFloating() const { return m_bFloating; }
	inline bool	IsAnchored() const { return !m_bFloating; }
	inline bool IsHidden() const { return m_bHidden; }

	// Methods for floating hooks

	bool	SetOrigo( int anchor );
	bool	SetHotspot( WgLocation hotspot );

	bool	SetSizePolicy( WgSizePolicy policy );
	bool	SetMinSize( const WgSize& size );
	bool	SetMaxSize( const WgSize& size );

	bool	SetGeo( const WgRect& geometry );

	bool	SetOfs( const WgCord& ofs );
	bool	SetOfsX( int x );
	bool	SetOfsY( int y );

	bool	SetSize( const WgSize& size );
	bool	SetWidth( int width );
	bool	SetHeight( int height );

	bool	Move( const WgCord& ofs );
	bool	MoveX( int x );
	bool	MoveY( int y );

	inline WgSize		MaxSize() const { return m_maxSize; }
	inline WgSize		MinSize() const { return m_minSize; }
	inline WgSizePolicy	SizePolicy() const { return m_sizePolicy; }
	inline int			Origo() const { return m_anchor; }
	inline WgLocation	Hotspot() const { return m_hotspot; }
	inline WgRect		FloatGeo() const { return m_placementGeo; }
	inline WgCord		FloatOfs() const { return m_placementGeo.pos(); }
	inline WgRect		FloatSize() const { return m_placementGeo.size(); }

	// Methods for anchored hooks

	inline int			AnchorTopLeft() const { return m_anchorTopLeft; }
	inline int			AnchorBottomRight() const { return m_anchorBottomRight; }
	inline WgBorders	Borders() const { return m_borders; }


	// Standard Hook methods

	WgCord		Pos() const;
	WgSize		Size() const;
	WgRect		Geo() const;
	WgCord		ScreenPos() const;
	WgRect		ScreenGeo() const;

	inline WgFlexHook *	PrevHook() const { return Prev(); }
	inline WgFlexHook *	NextHook() const { return Next(); }

	WgGizmoContainer* Parent() const;

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	LINK_METHODS( WgFlexHook );

	WgFlexHook( WgGizmo * pGizmo, WgGizmoFlexGeo * pParent );

	bool		RefreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.

	void		RequestRender();
	void		RequestRender( const WgRect& rect );
	void		RequestResize();

	bool		LimitPlacementSize();
	void		_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgDirtyRect * pDirtIn, WgDirtyRectObj * pDirtOut );
	void		_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer );


	WgGizmoHook *	_prevHook() const;
	WgGizmoHook *	_nextHook() const;

	WgGizmoFlexGeo * m_pParent;

	bool		m_bHidden;
	bool		m_bFloating;		// true = floating, false = anchored
	WgRect		m_realGeo;			// Gizmos geo relative parent

	// Only used for floating hooks.

	WgSizePolicy	m_sizePolicy;
	WgSize		m_minSize;
	WgSize		m_maxSize;

	int			m_anchor;
	WgLocation	m_hotspot;
	WgRect		m_placementGeo;	// Gizmos geo relative anchor and hotspot.

	//Only used for anchored hooks.

	int			m_anchorTopLeft;
	int			m_anchorBottomRight;
	WgBorders	m_borders;

	//

	WgDirtyRectObj	m_dirt;		// Dirty areas to be rendered, in screen coordinates!

};


//____ WgFlexAnchor ___________________________________________________________

class WgFlexAnchor
{
	friend class WgGizmoFlexGeo;

public:
	inline float	relativeX() const { return m_xRelative; }
	inline float	relativeY() const { return m_yRelative; }
	inline int		offsetX() const { return m_pixelOfs.x; }
	inline int		offsetY() const { return m_pixelOfs.y; }
	inline WgCord	offset() const { return m_pixelOfs; }

	inline WgCord	position( const WgSize& parentSize ) const { return WgCord((int)(m_xRelative*parentSize.w), (int)(m_yRelative*parentSize.h)) + m_pixelOfs; }

private:
	WgFlexAnchor() : m_xRelative(0.f), m_yRelative(0.f), m_pixelOfs(0,0) {};
	WgFlexAnchor( float xRelative, float yRelative, const WgCord& pixelOfs );

	float	m_xRelative;
	float	m_yRelative;
	WgCord	m_pixelOfs;
};


//____ WgGizmoFlexGeo _________________________________________________________

class WgGizmoFlexGeo : public WgGizmo, public WgGizmoContainer
{
friend class WgFlexHook;

public:

	WgGizmoFlexGeo();
	virtual ~WgGizmoFlexGeo();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	void			SetConfineChildren( bool bRestrictChildren );
	inline bool		IsConfiningChildren() const { return m_bConfineChildren; }


	WgFlexHook *	AddGizmo( WgGizmo * pGizmo );
	WgFlexHook *	AddGizmo( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot, int anchor );
	WgFlexHook *	AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot = WG_NORTHWEST );

	WgFlexHook *	InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling );
	WgFlexHook *	InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot, int anchor );
	WgFlexHook *	InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot = WG_NORTHWEST );

	bool			DeleteGizmo( WgGizmo * pGizmo );
	bool			ReleaseGizmo( WgGizmo * pGizmo );

	void			DeleteAllGizmos();
	void			ReleaseAllGizmos();

	int				AddAnchor( float relativeX, float relativeY, const WgCord& pixelOfs );
	bool			ReplaceAnchor( int index, float relativeX, float relativeY, const WgCord& pixelOfs );
	bool			DeleteAnchor( int index );
	void			DeleteAllAnchors();

	inline int		NbAnchors() const { return m_anchors.size()+9; }
	const WgFlexAnchor *	Anchor( int index );


	inline WgFlexHook*	FirstHook() const { return m_hooks.First(); }
	inline WgFlexHook*	LastHook() const { return m_hooks.Last(); }

	// Overloaded from WgGizmo

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			MinSize() const;
	WgSize			BestSize() const;
	WgSize			MaxSize() const;

	bool			IsView() const { return false; }
	bool			IsContainer() const { return true; }

	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }


	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCord& ofs, WgSearchMode mode );



private:

	void			OnCollectRects( WgDirtyRectObj& rects, const WgRect& geo, const WgRect& clip );
	void			OnMaskRects( WgDirtyRectObj& rects, const WgRect& geo, const WgRect& clip );
	void			OnCloneContent( const WgGizmo * _pOrg );
	void			OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			OnNewSize( const WgSize& size );
	void			OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool			OnAlphaTest( const WgCord& ofs );

	inline void		OnEnable() { WgGizmoContainer::OnEnable(); }
	inline void		OnDisable() { WgGizmoContainer::OnDisable(); }

	void			OnRequestRender( const WgRect& rect, const WgFlexHook * pHook );	// rect is in our coordinate system.

	WgGizmo*		_castToGizmo() { return this; }

	void			_castDirtyRect( const WgRect& geo, const WgRect& clip, WgDirtyRect * pDirtIn, WgDirtyRectObj* pDirtOut );
	void			_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer );
	void			_clearDirtyRects();


	WgGizmoHook*	_firstHook() const { return FirstHook(); }
	WgGizmoHook*	_lastHook() const { return LastHook(); }

	WgChain<WgFlexHook>			m_hooks;
	std::vector<WgFlexAnchor>	m_anchors;

	bool			m_bConfineChildren;

	static WgFlexAnchor	g_baseAnchors[9];
};


#endif //WG_GIZMO_FLEXGEO_DOT_H
