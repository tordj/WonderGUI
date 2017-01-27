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

namespace wg 
{
	
	class FlexPanel;
	typedef	StrongPtr<FlexPanel,Panel_p>		FlexPanel_p;
	typedef	WeakPtr<FlexPanel,Panel_wp>	FlexPanel_wp;
	
	class FlexHook;
	typedef	HookTypePtr<FlexHook,PanelHook_p>	FlexHook_p;
	
	
	//____ FlexOrigo ____________________________________________________________
	
	class FlexOrigo
	{
	public:
		inline FlexOrigo() : x(0.f), y(0.f) {}
		inline FlexOrigo( float _x, float _y ) : x(_x), y(_y) {}
		inline FlexOrigo( Origo origo ) : x(s_origoTab[(int)origo][0]), y(s_origoTab[(int)origo][1]) {}
	
		inline Coord position( Size sz ) { return Coord((int)(x*(sz.w+0.5f)),(int)(y*(sz.h+0.5f))); }
	
		float	x;
		float	y;
	
		bool			operator==(const FlexOrigo& origo) const { return x == origo.x &&
																			y == origo.y; }
		bool			operator!=(const FlexOrigo& origo) const { return x != origo.x ||
																			y != origo.y; }
	private:
		static float	s_origoTab[9][2];
	};
	
	//____ FlexHook _____________________________________________________________
	
	class FlexHook : public PanelHook, protected Link
	{
		friend class Widget;
		friend class Panel;
		friend class FlexPanel;
		friend class Chain<FlexHook>;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static FlexHook_p	cast( const Hook_p& pInterface );
	
		// Flex-specific methods
	
		bool	setStretching( const FlexOrigo& topLeftOrigo, const FlexOrigo& bottomRightOrigo, Border padding = 0 );
		bool	setStretching( const FlexOrigo& topLeftOrigo, const Coord& topLeftOfs, const FlexOrigo& bottomRightOrigo, const Coord& bottomRightOfs, Border padding = 0 );
	
		bool	setFloating( const Coord& pos, const FlexOrigo& origo = Origo::NorthWest );
		bool	setFloating( const Coord& pos, const FlexOrigo& origo, const FlexOrigo& hotspot );
		bool	setFloating( const Rect& geometry, const FlexOrigo& origo = Origo::NorthWest );
		bool	setFloating( const Rect& geometry, const FlexOrigo& origo, const FlexOrigo& hotspot );
	
	
		void	top();								// Put us ontop all our silbings.
		void	bottom();							// Put us below all our siblings.
		bool	up();								// Move us one step up among siblings.
		bool	down();								// Move us one step down among siblings.
	
		bool	moveOver( const FlexHook_p& pSibling );
		bool	moveUnder( const FlexHook_p& pSibling );
	
		bool	isFloating() const { return m_bFloating; }
		bool	isStretching() const { return !m_bFloating; }
	
		// Methods for floating hooks
	
		bool	setOrigo( const FlexOrigo& origo );
		bool	setHotspot( const FlexOrigo& hotspot );
	
		bool	setSizePolicy( SizePolicy width, SizePolicy height );
	
		bool	setGeo( const Rect& geometry );
	
		bool	setOfs( const Coord& ofs );
		bool	setOfsX( int x );
		bool	setOfsY( int y );
	
		bool	setSize( const Size& size );
		bool	setWidth( int width );
		bool	setHeight( int height );
	
		bool	move( const Coord& ofs );
		bool	moveX( int x );
		bool	moveY( int y );
	
		SizePolicy	widthPolicy() const { return m_widthPolicy; }
		SizePolicy	heightPolicy() const { return m_heightPolicy; }
		FlexOrigo		floatOrigo() const { return m_origo; }
		FlexOrigo		floatHotspot() const { return m_hotspot; }
		Rect			floatGeo() const { return m_placementGeo; }
		Coord			floatOfs() const { return m_placementGeo.pos(); }
		Rect			floatSize() const { return m_placementGeo.size(); }
	
		// Methods for stretching hooks
	
		FlexOrigo		topLeftOrigo() const { return m_topLeftOrigo; }
		FlexOrigo		bottomRightOrigo() const { return m_bottomRightOrigo; }
		Coord			topLeftOfs() const { return m_topLeftOfs; }
		Coord			bottomRightOfs() const { return m_bottomRightOfs; }
	
		// Standard MyHook methods
	
		Coord			pos() const { return m_realGeo.pos(); }
		Size			size() const { 	return m_realGeo.size(); }
		Rect			geo() const { return m_realGeo; }
	
	
		Coord			globalPos() const;
		Rect			globalGeo() const;
	
		FlexHook_p	prev() const { return _prev(); }
		FlexHook_p	next() const { return _next(); }
	
		FlexPanel_p	parent() const { return m_pParent; }
	
	protected:
		// TODO: Constructor should in the future call setHook() on Widget, once we are totally rid of widgets...
	
		PROTECTED_LINK_METHODS( FlexHook );
	
		FlexHook( FlexPanel * pParent, const Rect& placementGeo, Border padding );
	
		void			_refreshRealGeo();
		Size			_sizeNeededForGeo();
	
		void			_requestResize();
	
		Hook *		_prevHook() const;
		Hook *		_nextHook() const;
		Container *	_parent() const;
	
		FlexPanel* m_pParent;
	
		bool			m_bFloating;		// true = floating, false = anchored
		Rect			m_realGeo;			// Widgets geo relative parent
	
	//	union
	//	{
	//		struct // Floating hooks
	//		{
				FlexOrigo		m_origo;
				FlexOrigo		m_hotspot;
				Rect			m_placementGeo;	// Widgets geo relative anchor and hotspot.
	//		};
	//		struct	// Stretching hooks
	//		{
				FlexOrigo		m_topLeftOrigo;
				FlexOrigo		m_bottomRightOrigo;
				Coord			m_topLeftOfs;
				Coord			m_bottomRightOfs;
	//		};
	//	};
	
		// Only used for floating hooks, but kept safe when set to stretch.
	
		SizePolicy	m_widthPolicy;
		SizePolicy	m_heightPolicy;
	};
	
	
	//____ FlexPanel _________________________________________________________
	
	/**
	 * @brief	A widget for individual, flexible arrangement of children.
	 *
	 * A widget for individual, flexible arrangement of children.
	 * 
	 */
	
	
	class FlexPanel : public Panel
	{
	friend class FlexHook;
	
	public:
		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static FlexPanel_p	cast( const Object_p& pObject );
	
		void			setConfineWidgets( bool bConfineWidgets );
		bool			isConfiningWidgets() const { return m_bConfineWidgets; }
	
	
		FlexHook *	addWidget( const Widget_p& pWidget );
		FlexHook *	addWidget( const Widget_p& pWidget, const FlexOrigo& topLeftOrigo, const FlexOrigo& bottomRightOrigo, Border padding = 0 );
		FlexHook *	addWidget( const Widget_p& pWidget, const FlexOrigo& topLeftOrigo, const Coord& topLeftOfs, 
								  const FlexOrigo& bottomRightOrigo, const Coord& bottomRightOfs, Border padding = 0 );
	
		FlexHook *	addWidget( const Widget_p& pWidget, const Coord& pos, const FlexOrigo& origo = Origo::NorthWest, Border padding = 0 );
		FlexHook *	addWidget( const Widget_p& pWidget, const Coord& pos, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding = 0 );
		FlexHook *	addWidget( const Widget_p& pWidget, const Rect& geometry, const FlexOrigo& origo = Origo::NorthWest, Border padding = 0 );
		FlexHook *	addWidget( const Widget_p& pWidget, const Rect& geometry, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding = 0 );
	
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling );
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const FlexOrigo& topLeftOrigo,
									 const FlexOrigo& bottomRightOrigo, const Border padding = 0 );
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const FlexOrigo& topLeftOrigo, const Coord& topLeftOfs, 
									 const FlexOrigo& bottomRightOrigo, const Coord& bottomRightOfs, Border padding = 0 );
	
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Coord& pos, const FlexOrigo& origo = Origo::NorthWest, Border padding = 0 );
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Coord& pos, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding = 0 );
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Rect& geometry, const FlexOrigo& origo = Origo::NorthWest, Border padding = 0 );
		FlexHook *	insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Rect& geometry, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding = 0 );
	
		bool			removeWidget( const Widget_p& pWidget );
		bool			clear();
	
	
		FlexHook*		firstHook() const { return m_hooks.first(); }
		FlexHook*		lastHook() const { return m_hooks.last(); }
	
		// Overloaded from Widget
	
		Size			preferredSize() const;
	
	protected:
		FlexPanel();
		virtual ~FlexPanel();
		virtual Widget* _newOfMyType() const { return new FlexPanel(); };
	
		//

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;

	
	private:
	
		void			_cloneContent( const Widget * _pOrg );
		void			_setSize( const Size& size );
	
	
		void			_onRequestRender( const Rect& rect, const FlexHook * pHook );	// rect is in our coordinate system.
	
		Hook*			_firstHook() const { return firstHook(); }
		Hook*			_lastHook() const { return lastHook(); }
	
		Hook *		_firstHookWithGeo( Rect& writeGeo ) const;
		Hook *		_nextHookWithGeo( Rect& writeGeo, Hook * pHook ) const;
	
		Hook *		_lastHookWithGeo( Rect& writeGeo ) const;
		Hook *		_prevHookWithGeo( Rect& writeGeo, Hook * pHook ) const;
	
		Chain<FlexHook>			m_hooks;
	
		bool			m_bConfineWidgets;
	};
	
	

} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
