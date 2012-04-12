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

class WgGizmoFlexGeo;

//____ WgFlexHook _____________________________________________________________

class WgFlexHook : public WgHook, protected WgLink
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
	bool	SetFloating( const WgCoord& pos, WgOrientation origo = WG_NORTHWEST );
	bool	SetFloating( const WgCoord& pos, int anchor, WgOrientation hotspot );
	bool	SetFloating( const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	bool	SetFloating( const WgRect& geometry, int anchor, WgOrientation hotspot );


	void	Top();								// Put us ontop all our silbings.
	void	Bottom();							// Put us below all our siblings.
	bool	Up();								// Move us one step up among siblings.
	bool	Down();								// Move us one step down among siblings.

	bool	MoveOver( WgFlexHook * pSibling );
	bool	MoveUnder( WgFlexHook * pSibling );

	void	Hide();
	void	Show();

	bool	IsFloating() const { return m_bFloating; }
	bool	IsAnchored() const { return !m_bFloating; }
	bool	IsHidden() const { return m_bHidden; }

	// Methods for floating hooks

	bool	SetAnchor( int anchor );
	bool	SetHotspot( WgOrientation hotspot );

	bool	SetSizePolicy( WgSizePolicy width, WgSizePolicy height );

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

	WgSizePolicy	WidthPolicy() const { return m_widthPolicy; }
	WgSizePolicy	HeightPolicy() const { return m_heightPolicy; }
	int				Origo() const { return m_anchor; }
	WgOrientation	Hotspot() const { return m_hotspot; }
	WgRect			FloatGeo() const { return m_placementGeo; }
	WgCoord			FloatOfs() const { return m_placementGeo.Pos(); }
	WgRect			FloatSize() const { return m_placementGeo.Size(); }

	// Methods for anchored hooks

	int				AnchorTopLeft() const { return m_anchorTopLeft; }
	int				AnchorBottomRight() const { return m_anchorBottomRight; }
	WgBorders		Borders() const { return m_borders; }


	// Standard Hook methods

	WgCoord			Pos() const { return m_realGeo.Pos(); }
	WgSize			Size() const { 	return m_realGeo.Size(); }
	WgRect			Geo() const { return m_realGeo; }


	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgFlexHook *	Prev() const { return _prev(); }
	WgFlexHook *	Next() const { return _next(); }

	WgGizmoFlexGeo* Parent() const { return m_pParent; }

	WgWidget*		GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgFlexHook );

	WgFlexHook( WgGizmoFlexGeo * pParent, const WgRect& placementGeo );

	void		_refreshRealGeo();

	void		_requestRender();
	void		_requestRender( const WgRect& rect );
	void		_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgGizmoParent * _parent() const;

	WgGizmoFlexGeo* m_pParent;

	bool			m_bHidden;
	bool			m_bFloating;		// true = floating, false = anchored
	WgRect			m_realGeo;			// Gizmos geo relative parent

	// Only used for floating hooks.

	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;

	int				m_anchor;
	WgOrientation	m_hotspot;
	WgRect			m_placementGeo;	// Gizmos geo relative anchor and hotspot.

	//Only used for anchored hooks.

	int				m_anchorTopLeft;
	int				m_anchorBottomRight;
	WgBorders		m_borders;

};


//____ WgFlexAnchor ___________________________________________________________

class WgFlexAnchor
{
	friend class WgGizmoFlexGeo;

public:
	float	RelativeX() const { return m_xRelative; }
	float	RelativeY() const { return m_yRelative; }
	int		OffsetX() const { return m_pixelOfs.x; }
	int		OffsetY() const { return m_pixelOfs.y; }
	WgCoord	Offset() const { return m_pixelOfs; }

	WgCoord	Position( const WgSize& parentSize ) const { return WgCoord((int)(m_xRelative*parentSize.w), (int)(m_yRelative*parentSize.h)) + m_pixelOfs; }

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
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoFlexGeo(); };

	void			SetConfineChildren( bool bRestrictChildren );
	bool			IsConfiningChildren() const { return m_bConfineChildren; }


	WgFlexHook *	AddChild( WgGizmo * pGizmo );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgCoord& pos, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgCoord& pos, int anchor, WgOrientation hotspot );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	AddChild( WgGizmo * pGizmo, const WgRect& geometry, int anchor, WgOrientation hotspot );

	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCoord& pos, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCoord& pos, int anchor, WgOrientation hotspot );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, int anchor, WgOrientation hotspot );

	bool			DeleteChild( WgGizmo * pGizmo );
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();

	int				AddAnchor( float relativeX, float relativeY, const WgCoord& pixelOfs );
	bool			ReplaceAnchor( int index, float relativeX, float relativeY, const WgCoord& pixelOfs );
	bool			DeleteAnchor( int index );
	void			DeleteAllAnchors();

	int				NbAnchors() const { return m_anchors.size()+9; }
	const WgFlexAnchor *	Anchor( int index );


	WgFlexHook*		FirstHook() const { return m_hooks.First(); }
	WgFlexHook*		LastHook() const { return m_hooks.Last(); }

	// Overloaded from WgGizmo

	WgSize			DefaultSize() const;

	bool			IsView() const { return false; }
	bool			IsContainer() const { return true; }

	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }

	WgGizmo*		CastToGizmo() { return this; }
	const WgGizmo*	CastToGizmo() const { return this; }


private:

	// These are needed until WgGizmoContainer inherits from WgGizmo

	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
									{ WgGizmoContainer::_renderPatches( pDevice, _canvas, _window, _pPatches, _layer ); }
	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
									{ WgGizmoContainer::_onCollectPatches(container, geo, clip); }
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
									{ WgGizmoContainer::_onMaskPatches(patches, geo, clip, blendMode ); }
	void			_onEnable() { WgGizmoContainer::_onEnable(); }
	void			_onDisable() { WgGizmoContainer::_onDisable(); }
	bool 			_onAlphaTest( const WgCoord& ofs ) { return WgGizmoContainer::_onAlphaTest(ofs); }

	//

	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );


	void			_onRequestRender( const WgRect& rect, const WgFlexHook * pHook );	// rect is in our coordinate system.

	WgHook*			_firstHook() const { return FirstHook(); }
	WgHook*			_lastHook() const { return LastHook(); }

	WgHook *		_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *		_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& writeGeo ) const;
	WgHook *		_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgChain<WgFlexHook>			m_hooks;
	std::vector<WgFlexAnchor>	m_anchors;

	bool			m_bConfineChildren;

	static WgFlexAnchor	g_baseAnchors[9];
};


#endif //WG_GIZMO_FLEXGEO_DOT_H
