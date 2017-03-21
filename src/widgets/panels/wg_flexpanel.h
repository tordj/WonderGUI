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
	
	
	//____ FlexPos ____________________________________________________________
	

	class FlexPos
	{
	public:
		inline FlexPos() {}

		inline FlexPos( Origo _origo ) : origo(s_origoTab[(int)_origo][0], s_origoTab[(int)_origo][1]) {}
		inline FlexPos( Origo _origo, Coord _offset ) : origo(s_origoTab[(int)_origo][0], s_origoTab[(int)_origo][1]), offset(_offset) {}
		inline FlexPos( Origo _origo, int _ofsX, int _ofsY ) : origo(s_origoTab[(int)_origo][0], s_origoTab[(int)_origo][1]), offset(_ofsX,_ofsY) {}

		inline FlexPos( CoordF _origo ) : origo(_origo) {}
		inline FlexPos( CoordF _origo, Coord _offset ) : origo(_origo), offset(_offset) {}
		inline FlexPos( CoordF _origo, int _ofsX, int _ofsY )  : origo(_origo), offset(_ofsX,_ofsY) {}

		inline FlexPos( float _origoX, float _origoY ) : origo(_origoX, _origoY) {}
		inline FlexPos( float _origoX, float _origoY, Coord _offset ) : origo(_origoX, _origoY), offset(_offset) {}
		inline FlexPos( float _origoX, float _origoY, int _ofsX, int _ofsY ) : origo(_origoX, _origoY), offset(_ofsX,_ofsY) {}


		CoordF	origo;
		Coord	offset;
			
		Coord	pos( Size canvas ) const { return Coord( ((int)(origo.x*canvas.w+0.5f)) + offset.x, ((int)(origo.y*canvas.h+0.5f)) + offset.y ); }
			
		bool			operator==(const FlexPos& other) const { return origo == other.origo &&
																			offset == other.offset; }
		bool			operator!=(const FlexPos& other) const { return origo != other.origo ||
																			offset != other.offset; }
	private:
		static float	s_origoTab[9][2];
	};




	//____ FlexPanelSlot ____________________________________________________________
	
	class FlexPanelSlot : public Slot
	{
	public:
		FlexPanelSlot() : bPinned(false), bVisible(false), origo(Origo::NorthWest), hotspot(Origo::NorthWest) {}
	
		bool			bPinned;		
		bool			bVisible;
		Rect			realGeo;			// Widgets geo relative parent
	
//		union
//		{
//			struct // Positioned children
//			{
				FlexPos			origo;
				FlexPos			hotspot;
				Rect			placementGeo;	// Widgets geo relative anchor and hotspot.
//			};
//			struct	// Stretched children
//			{
				FlexPos			topLeftPin;
				FlexPos			bottomRightPin;
//			};
//		};
	
	
	};
	
	
	class FlexPanelChildren;
	typedef	StrongInterfacePtr<FlexPanelChildren,Interface_p>	FlexPanelChildren_p;
	typedef	WeakInterfacePtr<FlexPanelChildren,Interface_wp>	FlexPanelChildren_wp;
	
	//____ FlexPanelChildren ________________________________________________________

	class FlexPanelChildren : public PanelChildren<FlexPanelSlot,FlexPanel>
	{
	public:
		FlexPanelChildren( SlotArray<FlexPanelSlot> * pSlotArray, FlexPanel * pHolder ) : PanelChildren<FlexPanelSlot,FlexPanel>(pSlotArray,pHolder) {}

		inline FlexPanelChildren_p	ptr() { return FlexPanelChildren_p(_object(),this); }


		bool		addPinned( const Widget_p& pWidget, const FlexPos& topLeft, 
									const FlexPos& bottomRight );
		bool		addMovable( const Widget_p& pWidget, const Rect& geometry = Rect(), const FlexPos& origo = Origo::NorthWest, 
									const FlexPos& hotspot = Origo::NorthWest );
	
		bool		insertPinned( int index, const Widget_p& pWidget, const FlexPos& topLeft,
									const FlexPos& bottomRight );
		bool		insertMovable( int index, const Widget_p& pWidget, const Rect& geometry, 
									const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest );

		bool		setPinned( int index );
		bool		setPinned( int index, const FlexPos& topLeft, const FlexPos& bottomRight );

		bool		setMovable( int index, const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest );
		bool		setMovable( int index, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot );
	
		bool		moveToBack( int index );								// Put us ontop all our silbings.
		bool		moveToFront( int index );							// Put us below all our siblings.	
		bool		moveAbove( int index, int otherWidget );
		bool		moveBelow( int index, int otherWidget );
	
		bool		isMovable( int index ) const;
		bool		isPinned( int index ) const;
	
		// Methods for movable children
	
		bool		setOrigo( int index, const FlexPos& origo );
		FlexPos		origo( int index ) const;

		bool		setHotspot( int index, const FlexPos& hotspot );
		FlexPos		hotspot( int index ) const;
		
		bool		setGeo( int index, const Rect& geometry );
		Rect		geo( int index ) const;
	
		bool		setOfs( int index, const Coord& ofs );
		Coord		ofs( int index ) const;

		bool		setSize( int index, const Size& size );
		Rect		size( int index ) const;
	
		bool		move( int index, const Coord& ofs );
	
	
		// Methods for pinned children
	
		FlexPos	topLeftCorner( int index ) const;
		FlexPos	bottomRightCorner( int index ) const;
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
		friend class FlexPanelChildren;
		friend class ChildGroup<FlexPanelSlot,FlexPanel>;
	
	public:

		//.____ Creation __________________________________________

		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }

		//.____ Components _______________________________________

		FlexPanelChildren	children;

		//.____ Identification __________________________________________
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FlexPanel_p	cast( const Object_p& pObject );
	
		//.____ Behavior ________________________________________________________


		void				setConfineWidgets( bool bConfineWidgets );
		bool				isConfiningWidgets() const { return m_bConfineWidgets; }
		
		//.____ Geometry ____________________________________________
		
		Size				preferredSize() const;
	
	protected:
		FlexPanel();
		virtual ~FlexPanel();
		virtual Widget* _newOfMyType() const { return new FlexPanel(); };
	

		// Overloaded from Container
		
		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstChildWithGeo( WidgetWithGeo& package ) const;
		void		_nextChildWithGeo( WidgetWithGeo& package ) const;


		// Methods for FlexPanelChildren

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_willRemoveSlots( Slot * pSlot, int nb );
		void		_hideSlots( FlexPanelSlot *, int nb );
		void		_unhideSlots( FlexPanelSlot *, int nb );

		// Overloaded from WidgetHolder

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;

	
	private:
	
		void		_cloneContent( const Widget * _pOrg );
		void		_setSize( const Size& size );

		void		_onRequestRender( const Rect& rect, const FlexPanelSlot * pSlot );

		void		_refreshRealGeo( FlexPanelSlot * pSlot );
		Size		_sizeNeededForGeo( FlexPanelSlot * pSlot ) const;
	
		void		_moveSlot(int oldPos, int newPos);
	
		SlotArray<FlexPanelSlot>	m_children;
	
		bool			m_bConfineWidgets;
	};
	
	

} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
