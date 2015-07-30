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
typedef	WgStrongPtr<WgFlexPanel,WgPanel_p>		WgFlexPanel_p;
typedef	WgWeakPtr<WgFlexPanel,WgPanel_wp>	WgFlexPanel_wp;

class WgFlexHook;
typedef	WgHookTypePtr<WgFlexHook,WgPanelHook_p>	WgFlexHook_p;


//____ WgFlexOrigo ____________________________________________________________

class WgFlexOrigo
{
public:
	inline WgFlexOrigo() : x(0.f), y(0.f) {}
	inline WgFlexOrigo( float _x, float _y ) : x(_x), y(_y) {}
	inline WgFlexOrigo( WgOrigo origo ) : x(s_origoTab[origo][0]), y(s_origoTab[origo][1]) {}

	inline WgCoord position( WgSize sz ) { return WgCoord((int)(x*(sz.w+0.5f)),(int)(y*(sz.h+0.5f))); }

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
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgFlexHook_p	cast( const WgHook_p& pInterface );

	// Flex-specific methods

	bool	setStretching( const WgFlexOrigo& topLeftOrigo, const WgFlexOrigo& bottomRightOrigo, WgBorder padding = 0 );
	bool	setStretching( const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorder padding = 0 );

	bool	setFloating( const WgCoord& pos, const WgFlexOrigo& origo = WG_NORTHWEST );
	bool	setFloating( const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot );
	bool	setFloating( const WgRect& geometry, const WgFlexOrigo& origo = WG_NORTHWEST );
	bool	setFloating( const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot );


	void	top();								// Put us ontop all our silbings.
	void	bottom();							// Put us below all our siblings.
	bool	up();								// Move us one step up among siblings.
	bool	down();								// Move us one step down among siblings.

	bool	moveOver( const WgFlexHook_p& pSibling );
	bool	moveUnder( const WgFlexHook_p& pSibling );

	bool	isFloating() const { return m_bFloating; }
	bool	isStretching() const { return !m_bFloating; }

	// Methods for floating hooks

	bool	setOrigo( const WgFlexOrigo& origo );
	bool	setHotspot( const WgFlexOrigo& hotspot );

	bool	setSizePolicy( WgSizePolicy width, WgSizePolicy height );

	bool	setGeo( const WgRect& geometry );

	bool	setOfs( const WgCoord& ofs );
	bool	setOfsX( int x );
	bool	setOfsY( int y );

	bool	setSize( const WgSize& size );
	bool	setWidth( int width );
	bool	setHeight( int height );

	bool	move( const WgCoord& ofs );
	bool	moveX( int x );
	bool	moveY( int y );

	WgSizePolicy	widthPolicy() const { return m_widthPolicy; }
	WgSizePolicy	heightPolicy() const { return m_heightPolicy; }
	WgFlexOrigo		floatOrigo() const { return m_origo; }
	WgFlexOrigo		floatHotspot() const { return m_hotspot; }
	WgRect			floatGeo() const { return m_placementGeo; }
	WgCoord			floatOfs() const { return m_placementGeo.pos(); }
	WgRect			floatSize() const { return m_placementGeo.size(); }

	// Methods for stretching hooks

	WgFlexOrigo		topLeftOrigo() const { return m_topLeftOrigo; }
	WgFlexOrigo		bottomRightOrigo() const { return m_bottomRightOrigo; }
	WgCoord			topLeftOfs() const { return m_topLeftOfs; }
	WgCoord			bottomRightOfs() const { return m_bottomRightOfs; }

	// Standard Hook methods

	WgCoord			pos() const { return m_realGeo.pos(); }
	WgSize			size() const { 	return m_realGeo.size(); }
	WgRect			geo() const { return m_realGeo; }


	WgCoord			globalPos() const;
	WgRect			globalGeo() const;

	WgFlexHook_p	prev() const { return _prev(); }
	WgFlexHook_p	next() const { return _next(); }

	WgFlexPanel_p	parent() const { return m_pParent; }

protected:
	// TODO: Constructor should in the future call setHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgFlexHook );

	WgFlexHook( WgFlexPanel * pParent, const WgRect& placementGeo, WgBorder padding );

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
 * A widget for individual, flexible arrangement of children.
 * 
 */


class WgFlexPanel : public WgPanel
{
friend class WgFlexHook;

public:
	static WgFlexPanel_p	create() { return WgFlexPanel_p(new WgFlexPanel()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgFlexPanel_p	cast( const WgObject_p& pObject );

	void			setConfineWidgets( bool bConfineWidgets );
	bool			isConfiningWidgets() const { return m_bConfineWidgets; }


	WgFlexHook *	addWidget( const WgWidget_p& pWidget );
	WgFlexHook *	addWidget( const WgWidget_p& pWidget, const WgFlexOrigo& topLeftOrigo, const WgFlexOrigo& bottomRightOrigo, WgBorder padding = 0 );
	WgFlexHook *	addWidget( const WgWidget_p& pWidget, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
							  const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorder padding = 0 );

	WgFlexHook *	addWidget( const WgWidget_p& pWidget, const WgCoord& pos, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorder padding = 0 );
	WgFlexHook *	addWidget( const WgWidget_p& pWidget, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding = 0 );
	WgFlexHook *	addWidget( const WgWidget_p& pWidget, const WgRect& geometry, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorder padding = 0 );
	WgFlexHook *	addWidget( const WgWidget_p& pWidget, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding = 0 );

	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling );
	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgFlexOrigo& topLeftOrigo,
								 const WgFlexOrigo& bottomRightOrigo, const WgBorder padding = 0 );
	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
								 const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorder padding = 0 );

	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgCoord& pos, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorder padding = 0 );
	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding = 0 );
	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgRect& geometry, const WgFlexOrigo& origo = WG_NORTHWEST, WgBorder padding = 0 );
	WgFlexHook *	insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding = 0 );

	bool			removeWidget( const WgWidget_p& pWidget );
	bool			clear();


	WgFlexHook*		firstHook() const { return m_hooks.first(); }
	WgFlexHook*		lastHook() const { return m_hooks.last(); }

	// Overloaded from WgWidget

	WgSize			preferredSize() const;

protected:
	WgFlexPanel();
	virtual ~WgFlexPanel();
	virtual WgWidget* _newOfMyType() const { return new WgFlexPanel(); };


private:

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );


	void			_onRequestRender( const WgRect& rect, const WgFlexHook * pHook );	// rect is in our coordinate system.

	WgHook*			_firstHook() const { return firstHook(); }
	WgHook*			_lastHook() const { return lastHook(); }

	WgHook *		_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *		_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& writeGeo ) const;
	WgHook *		_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgChain<WgFlexHook>			m_hooks;

	bool			m_bConfineWidgets;
};


#endif //WG_FLEXPANEL_DOT_H
