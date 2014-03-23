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
typedef	WgSmartPtr<WgFlexPanel,WgPanelPtr>		WgFlexPanelPtr;
typedef	WgWeakPtr<WgFlexPanel,WgPanelWeakPtr>	WgFlexPanelWeakPtr;

class WgFlexHook;
typedef	WgHookTypePtr<WgFlexHook,WgPanelHookPtr>	WgFlexHookPtr;


//____ WgFlexOrigo ____________________________________________________________

class WgFlexOrigo
{
public:
	inline WgFlexOrigo() : x(0.f), y(0.f) {}
	inline WgFlexOrigo( float _x, float _y ) : x(_x), y(_y) {}
	inline WgFlexOrigo( WgOrigo origo ) : x(s_origoTab[origo][0]), y(s_origoTab[origo][1]) {}

	inline WgCoord Position( WgSize sz ) { return WgCoord((int)(x*(sz.w+0.5f)),(int)(y*(sz.h+0.5f))); }

	float	x;
	float	y;

	bool			operator==(const WgFlexOrigo& origo) const { return x == origo.x &&
																		y == origo.y; }
	bool			operator!=(const WgFlexOrigo& origo) const { return x != origo.x ||
																		y != origo.y; }
private:
	static float	s_origoTab[9][2];
};

//____ WgFlexHook _____________________________________________________________

class WgFlexHook : public WgPanelHook, protected WgLink
{
	friend class WgWidget;
	friend class WgPanel;
	friend class WgFlexPanel;
	friend class WgChain<WgFlexHook>;

public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgFlexHookPtr	Cast( const WgHookPtr& pInterface );

	// Flex-specific methods

	bool	SetStretching( const WgFlexOrigo& topLeftOrigo, const WgFlexOrigo& bottomRightOrigo, WgBorders padding = 0 );
	bool	SetStretching( const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorders padding = 0 );

	bool	SetFloating( const WgCoord& pos, const WgFlexOrigo& origo = WG_NORTHWEST );
	bool	SetFloating( const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot );
	bool	SetFloating( const WgRect& geometry, const WgFlexOrigo& origo = WG_NORTHWEST );
	bool	SetFloating( const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot );


	void	Top();								// Put us ontop all our silbings.
	void	Bottom();							// Put us below all our siblings.
	bool	Up();								// Move us one step up among siblings.
	bool	Down();								// Move us one step down among siblings.

	bool	MoveOver( const WgFlexHookPtr& pSibling );
	bool	MoveUnder( const WgFlexHookPtr& pSibling );

	bool	IsFloating() const { return m_bFloating; }
	bool	IsStretching() const { return !m_bFloating; }

	// Methods for floating hooks

	bool	SetOrigo( const WgFlexOrigo& origo );
	bool	SetHotspot( const WgFlexOrigo& hotspot );

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
	WgFlexOrigo		FloatOrigo() const { return m_origo; }
	WgFlexOrigo		FloatHotspot() const { return m_hotspot; }
	WgRect			FloatGeo() const { return m_placementGeo; }
	WgCoord			FloatOfs() const { return m_placementGeo.Pos(); }
	WgRect			FloatSize() const { return m_placementGeo.Size(); }

	// Methods for stretching hooks

	WgFlexOrigo		TopLeftOrigo() const { return m_topLeftOrigo; }
	WgFlexOrigo		BottomRightOrigo() const { return m_bottomRightOrigo; }
	WgCoord			TopLeftOfs() const { return m_topLeftOfs; }
	WgCoord			BottomRightOfs() const { return m_bottomRightOfs; }

	// Standard Hook methods

	WgCoord			Pos() const { return m_realGeo.Pos(); }
	WgSize			Size() const { 	return m_realGeo.Size(); }
	WgRect			Geo() const { return m_realGeo; }


	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgFlexHookPtr	Prev() const { return _prev(); }
	WgFlexHookPtr	Next() const { return _next(); }

	WgFlexPanelPtr	Parent() const { return m_pParent; }

protected:
	// TODO: Constructor should in the future call SetHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgFlexHook );

	WgFlexHook( WgFlexPanel * pParent, const WgRect& placementGeo, WgBorders padding );

	void			_refreshRealGeo();
	WgSize			_sizeNeededForGeo();

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgContainer *	_parent() const;

	WgFlexPanel* m_pParent;

	bool			m_bFloating;		// true = floating, false = anchored
	WgRect			m_realGeo;			// Widgets geo relative parent

//	union
//	{
//		struct // Floating hooks
//		{
			WgFlexOrigo		m_origo;
			WgFlexOrigo		m_hotspot;
			WgRect			m_placementGeo;	// Widgets geo relative anchor and hotspot.
//		};
//		struct	// Stretching hooks
//		{
			WgFlexOrigo		m_topLeftOrigo;
			WgFlexOrigo		m_bottomRightOrigo;
			WgCoord			m_topLeftOfs;
			WgCoord			m_bottomRightOfs;
//		};
//	};

	// Only used for floating hooks, but kept safe when set to stretch.

	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;
};


//____ WgFlexPanel _________________________________________________________

/**
 * @brief	A widget for individual, flexible arrangement of children.
 * 
 */


class WgFlexPanel : public WgPanel
{
friend class WgFlexHook;

public:
	static WgFlexPanelPtr	Create() { return WgFlexPanelPtr(new WgFlexPanel()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFlexPanelPtr	Cast( const WgObjectPtr& pObject );

	void			SetConfineWidgets( bool bConfineWidgets );
	bool			IsConfiningWidgets() const { return m_bConfineWidgets; }


	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget );
	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget, const WgFlexOrigo& topLeftOrigo, const WgFlexOrigo& bottomRightOrigo, WgBorders padding = 0 );
	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
							  const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorders padding = 0 );

	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget, const WgCoord& pos, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorders padding = 0 );
	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding = 0 );
	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget, const WgRect& geometry, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorders padding = 0 );
	WgFlexHook *	AddWidget( const WgWidgetPtr& pWidget, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding = 0 );

	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling );
	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgFlexOrigo& topLeftOrigo,
								 const WgFlexOrigo& bottomRightOrigo, const WgBorders padding = 0 );
	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
								 const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorders padding = 0 );

	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgCoord& pos, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorders padding = 0 );
	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding = 0 );
	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgRect& geometry, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorders padding = 0 );
	WgFlexHook *	InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding = 0 );

	bool			RemoveWidget( const WgWidgetPtr& pWidget );
	bool			Clear();


	WgFlexHook*		FirstHook() const { return m_hooks.First(); }
	WgFlexHook*		LastHook() const { return m_hooks.Last(); }

	// Overloaded from WgWidget

	WgSize			PreferredSize() const;

protected:
	WgFlexPanel();
	virtual ~WgFlexPanel();
	virtual WgWidget* _newOfMyType() const { return new WgFlexPanel(); };


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

	bool			m_bConfineWidgets;
};


#endif //WG_FLEXPANEL_DOT_H
