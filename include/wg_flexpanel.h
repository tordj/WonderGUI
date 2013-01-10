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

#ifndef WG_FLEXPANEL_DOT_H
#define WG_FLEXPANEL_DOT_H

#include <vector>

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

class WgFlexPanel;

//____ WgFlexHook _____________________________________________________________

class WgFlexHook : public WgPanelHook, protected WgLink
{
	friend class WgWidget;
	friend class WgPanel;
	friend class WgFlexPanel;
	friend class WgChain<WgFlexHook>;

public:

	const char *Type( void ) const;
	static const char * ClassType();

	// Flex-specific methods

	bool	SetAnchored();
	bool	SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders padding = 0 );

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

	// Standard Hook methods

	WgCoord			Pos() const { return m_realGeo.Pos(); }
	WgSize			Size() const { 	return m_realGeo.Size(); }
	WgRect			Geo() const { return m_realGeo; }


	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgFlexHook *	Prev() const { return _prev(); }
	WgFlexHook *	Next() const { return _next(); }

	WgFlexPanel* Parent() const { return m_pParent; }

protected:
	// TODO: Constructor should in the future call SetHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgFlexHook );

	WgFlexHook( WgFlexPanel * pParent, const WgRect& placementGeo );

	void		_refreshRealGeo();
	WgSize		_sizeNeededForGeo();

	void		_requestRender();
	void		_requestRender( const WgRect& rect );
	void		_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgWidgetHolder * _parent() const;

	WgFlexPanel* m_pParent;

	bool			m_bHidden;
	bool			m_bFloating;		// true = floating, false = anchored
	WgRect			m_realGeo;			// Widgets geo relative parent

	// Only used for floating hooks.

	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;

	int				m_anchor;
	WgOrientation	m_hotspot;
	WgRect			m_placementGeo;	// Widgets geo relative anchor and hotspot.

	//Only used for anchored hooks.

	int				m_anchorTopLeft;
	int				m_anchorBottomRight;

};


//____ WgFlexAnchor ___________________________________________________________

class WgFlexAnchor
{
	friend class WgFlexPanel;

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


//____ WgFlexPanel _________________________________________________________

class WgFlexPanel : public WgPanel
{
friend class WgFlexHook;

public:

	WgFlexPanel();
	virtual ~WgFlexPanel();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgFlexPanel(); };

	void			SetConfineChildren( bool bRestrictChildren );
	bool			IsConfiningChildren() const { return m_bConfineChildren; }


	WgFlexHook *	AddChild( WgWidget * pWidget );
	WgFlexHook *	AddChild( WgWidget * pWidget, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	AddChild( WgWidget * pWidget, const WgCoord& pos, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	AddChild( WgWidget * pWidget, const WgCoord& pos, int anchor, WgOrientation hotspot );
	WgFlexHook *	AddChild( WgWidget * pWidget, const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	AddChild( WgWidget * pWidget, const WgRect& geometry, int anchor, WgOrientation hotspot );

	WgFlexHook *	InsertChild( WgWidget * pWidget, WgWidget * pSibling );
	WgFlexHook *	InsertChild( WgWidget * pWidget, WgWidget * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 );
	WgFlexHook *	InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgCoord& pos, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgCoord& pos, int anchor, WgOrientation hotspot );
	WgFlexHook *	InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	WgFlexHook *	InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgRect& geometry, int anchor, WgOrientation hotspot );

	bool			DeleteChild( WgWidget * pWidget );
	WgWidget *		ReleaseChild( WgWidget * pWidget );

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

	// Overloaded from WgWidget

	WgSize			PreferredSize() const;

private:

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );


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


#endif //WG_FLEXPANEL_DOT_H
