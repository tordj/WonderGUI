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
#	include <wg_rectchain.h>
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
	bool	SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );

	bool	SetFloating();
	bool	SetFloating( const WgCoord& pos, WgLocation origo = WG_NORTHWEST );
	bool	SetFloating( const WgCoord& pos, int anchor, WgLocation hotspot );
	bool	SetFloating( const WgRect& geometry, WgLocation origo = WG_NORTHWEST );
	bool	SetFloating( const WgRect& geometry, int anchor, WgLocation hotspot );


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

	bool	SetAnchor( int anchor );
	bool	SetHotspot( WgLocation hotspot );

	bool	SetSizePolicy( WgSizePolicy policy );
	bool	SetMinSize( const WgSize& size );
	bool	SetMaxSize( const WgSize& size );

	bool	SetGeo( const WgRect& geometry );

	bool	SetOfs( const WgCoord& ofs );
	bool	SetOfsX( int x );
	bool	SetOfsY( int y );

	bool	SetSize( const WgSize& size );
	bool	SetWidth( int width );
	bool	SetHeight( int height );

	bool	Move( const WgCoord& ofs );
	bool	MoveX( int x );
	bool	MoveY( int y );

	inline WgSize		MaxSize() const { return m_maxSize; }
	inline WgSize		MinSize() const { return m_minSize; }
	inline WgSizePolicy	SizePolicy() const { return m_sizePolicy; }
	inline int			Origo() const { return m_anchor; }
	inline WgLocation	Hotspot() const { return m_hotspot; }
	inline WgRect		FloatGeo() const { return m_placementGeo; }
	inline WgCoord		FloatOfs() const { return m_placementGeo.pos(); }
	inline WgRect		FloatSize() const { return m_placementGeo.size(); }

	// Methods for anchored hooks

	inline int			AnchorTopLeft() const { return m_anchorTopLeft; }
	inline int			AnchorBottomRight() const { return m_anchorBottomRight; }
	inline WgBorders	Borders() const { return m_borders; }


	// Standard Hook methods

	inline WgCoord		Pos() const { return m_realGeo.pos(); }
	inline WgSize		Size() const { 	return m_realGeo.size(); }
	inline WgRect		Geo() const { return m_realGeo; }
;
	WgCoord		ScreenPos() const;
	WgRect		ScreenGeo() const;

	inline WgFlexHook *	Prev() const { return _prev(); }
	inline WgFlexHook *	Next() const { return _next(); }

	inline WgGizmoFlexGeo * Parent() const { return m_pParent; }

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgFlexHook );

	WgFlexHook( WgGizmoFlexGeo * pParent, const WgRect& placementGeo );

	bool		RefreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.

	void		RequestRender();
	void		RequestRender( const WgRect& rect );
	void		RequestResize();

	bool		LimitPlacementSize();
	void		_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut );
	void		_renderDirtyRects( WgGfxDevice * pDevice, const WgCoord& parentPos, Uint8 _layer );


	WgGizmoHook *	_prevHook() const;
	WgGizmoHook *	_nextHook() const;
	WgGizmoContainer * _parent() const;

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

	WgRectChain	m_dirt;		// Dirty areas to be rendered, in screen coordinates!

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
	inline WgCoord	offset() const { return m_pixelOfs; }

	inline WgCoord	position( const WgSize& parentSize ) const { return WgCoord((int)(m_xRelative*parentSize.w), (int)(m_yRelative*parentSize.h)) + m_pixelOfs; }

private:
	WgFlexAnchor() : m_xRelative(0.f), m_yRelative(0.f), m_pixelOfs(0,0) {};
	WgFlexAnchor( float xRelative, float yRelative, const WgCoord& pixelOfs ) : m_xRelative(xRelative), m_yRelative(yRelative), m_pixelOfs(pixelOfs) {}

	float	m_xRelative;
	float	m_yRelative;
	WgCoord	m_pixelOfs;
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


	WgFlexHook *	AddChild( WgGizmo * pGizmo );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgCoord& pos, WgLocation origo = WG_NORTHWEST );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgCoord& pos, int anchor, WgLocation hotspot );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgRect& geometry, WgLocation origo = WG_NORTHWEST );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgRect& geometry, int anchor, WgLocation hotspot );

	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCoord& pos, WgLocation origo = WG_NORTHWEST );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCoord& pos, int anchor, WgLocation hotspot );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation origo = WG_NORTHWEST );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, int anchor, WgLocation hotspot );

	bool			DeleteChild( WgGizmo * pGizmo );
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();

	int				AddAnchor( float relativeX, float relativeY, const WgCoord& pixelOfs );
	bool			ReplaceAnchor( int index, float relativeX, float relativeY, const WgCoord& pixelOfs );
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

	WgGizmo*		CastToGizmo() { return this; }

	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCoord& ofs, WgSearchMode mode );



private:

	void			_onCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip );
	void			_onMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip );
	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			_onNewSize( const WgSize& size );
	void			_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool			_onAlphaTest( const WgCoord& ofs );

	inline void		_onEnable() { WgGizmoContainer::_onEnable(); }
	inline void		_onDisable() { WgGizmoContainer::_onDisable(); }

	void			_onRequestRender( const WgRect& rect, const WgFlexHook * pHook );	// rect is in our coordinate system.

	void			_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut );
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
