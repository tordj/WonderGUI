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
#pragma once

#include <vector>

#include <wg_panel.h>

namespace wg 
{
	
	class FlexPanel;
	typedef	StrongPtr<FlexPanel>		FlexPanel_p;
	typedef	WeakPtr<FlexPanel>	FlexPanel_wp;
	
	
	//____ FlexPos ____________________________________________________________
	

	class FlexPos
	{
	public:

		//.____ Creation _______________________________________________________

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

		//.____ Geometry _______________________________________________________

		Coord	pos( Size canvas ) const { return Coord( ((int)(origo.x*canvas.w+0.5f)) + offset.x, ((int)(origo.y*canvas.h+0.5f)) + offset.y ); }

		//.____ Properties _____________________________________________________

		CoordF	origo;
		Coord	offset;
			
		//.____ Operators ______________________________________________________
			
		bool			operator==(const FlexPos& other) const { return origo == other.origo &&
																			offset == other.offset; }
		bool			operator!=(const FlexPos& other) const { return origo != other.origo ||
																			offset != other.offset; }
	private:
		static float	s_origoTab[9][2];
	};




	//____ FlexPanelSlot ____________________________________________________________

	class FlexPanelSlot : public Slot		/** @private */
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
	typedef	StrongInterfacePtr<FlexPanelChildren>	FlexPanelChildren_p;
	typedef	WeakInterfacePtr<FlexPanelChildren>	FlexPanelChildren_wp;
	
	//____ FlexPanelChildren ________________________________________________________

	class FlexPanelChildren : public PanelChildren<FlexPanelSlot,FlexPanel>
	{
	public:
		/** @private */

		FlexPanelChildren( SlotArray<FlexPanelSlot> * pSlotArray, FlexPanel * pHolder ) : PanelChildren<FlexPanelSlot,FlexPanel>(pSlotArray,pHolder) {}

		//.____ Misc __________________________________________________________

		inline FlexPanelChildren_p	ptr() { return FlexPanelChildren_p(this); }

		//.____ Content _______________________________________________________

		iterator	addPinned( Widget * pWidget, const FlexPos& topLeft, 
									const FlexPos& bottomRight );
		iterator	addMovable( Widget * pWidget, const Rect& geometry = Rect(), const FlexPos& origo = Origo::NorthWest, 
									const FlexPos& hotspot = Origo::NorthWest );
	
		iterator	insertPinned( int index, Widget * pWidget, const FlexPos& topLeft,
									const FlexPos& bottomRight );
		iterator	insertMovable( int index, Widget * pWidget, const Rect& geometry, 
									const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest );

		//.____ State _________________________________________________________

		void		setPinned( int index );
		void		setPinned( iterator it );

		void		setPinned( int index, const FlexPos& topLeft, const FlexPos& bottomRight );
		void		setPinned( iterator it, const FlexPos& topLeft, const FlexPos& bottomRight);

		void		setMovable( int index, const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest );
		void		setMovable(iterator it, const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest);

		void		setMovable( int index, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot );
		void		setMovable(iterator it, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot);

		bool		isMovable(int index) const;
		bool		isMovable(iterator it) const;

		bool		isPinned(int index) const;
		bool		isPinned(iterator it) const;

		//.____ Ordering ______________________________________________________

		void		moveToBack( int index );								// Put us ontop all our silbings.
		iterator	moveToBack( iterator it );								// Put us ontop all our silbings.

		void		moveToFront( int index );							// Put us below all our siblings.	
		iterator	moveToFront( iterator it );							// Put us below all our siblings.	

		void		moveAbove( int index, int sibling );
		iterator	moveAbove( iterator it, iterator sibling );

		void		moveBelow( int index, int sibling );
		iterator	moveBelow( iterator it, iterator sibling );


		//.____ Geometry ______________________________________________________

		// Methods for movable children
	
		bool		setOrigo( int index, const FlexPos& origo );
		bool		setOrigo( iterator it, const FlexPos& origo);

		FlexPos		origo( int index ) const;
		FlexPos		origo( iterator it ) const;

		bool		setHotspot( int index, const FlexPos& hotspot );
		bool		setHotspot( iterator it, const FlexPos& hotspot);

		FlexPos		hotspot( int index ) const;
		FlexPos		hotspot( iterator it ) const;

		bool		setGeo( int index, const Rect& geometry );
		bool		setGeo( iterator it, const Rect& geometry);

		Rect		geo( int index ) const;
		Rect		geo( iterator it ) const;

		bool		setOfs( int index, const Coord& ofs );
		bool		setOfs( iterator it, const Coord& ofs);

		Coord		ofs( int index ) const;
		Coord		ofs( iterator it ) const;

		bool		setSize( int index, const Size& size );
		bool		setSize( iterator it, const Size& size);

		Rect		size( int index ) const;
		Rect		size( iterator it ) const;

		bool		move( int index, const Coord& ofs );
		bool		move( iterator it, const Coord& ofs);

	
		// Methods for pinned children
	
		FlexPos	topLeftCorner( int index ) const;
		FlexPos	topLeftCorner( iterator it ) const;

		FlexPos	bottomRightCorner( int index ) const;
		FlexPos	bottomRightCorner( iterator it ) const;

	protected:
		void		_setPinned(FlexPanelSlot * p);
		void		_setPinned(FlexPanelSlot * p, const FlexPos& topLeft, const FlexPos& bottomRight);

		void		_setMovable(FlexPanelSlot * p, const FlexPos& origo, const FlexPos& hotspot);
		void		_setMovable(FlexPanelSlot * p, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot);

		FlexPanelSlot * _moveAbove(FlexPanelSlot * p, FlexPanelSlot * sibling);
		FlexPanelSlot *	_moveBelow(FlexPanelSlot * p, FlexPanelSlot * sibling);

		// Methods for movable children

		bool		_setOrigo(FlexPanelSlot * p, const FlexPos& origo);
		FlexPos		_origo(FlexPanelSlot * p) const;

		bool		_setHotspot(FlexPanelSlot * p, const FlexPos& hotspot);
		FlexPos		_hotspot(FlexPanelSlot * p) const;

		bool		_setGeo(FlexPanelSlot * p, const Rect& geometry);
		Rect		_geo(FlexPanelSlot * p) const;

		bool		_setOfs(FlexPanelSlot * p, const Coord& ofs);
		Coord		_ofs(FlexPanelSlot * p) const;

		bool		_setSize(FlexPanelSlot * p, const Size& size);
		Rect		_size(FlexPanelSlot * p) const;

		bool		_move(FlexPanelSlot * p, const Coord& ofs);

		// Methods for pinned children

		FlexPos	_topLeftCorner(FlexPanelSlot * p) const;
		FlexPos	_bottomRightCorner(FlexPanelSlot * p) const;


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
		friend class PanelChildren<FlexPanelSlot,FlexPanel>;
		friend class HideableChildren<FlexPanelSlot,FlexPanel>;
		friend class DynamicChildGroup<FlexPanelSlot,FlexPanel>;
		friend class ChildGroup<FlexPanelSlot,FlexPanel>;
	
	public:

		//.____ Creation __________________________________________

		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }

		//.____ Interfaces _______________________________________

		FlexPanelChildren	children;

		//.____ Identification __________________________________________
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FlexPanel_p	cast( Object * pObject );
	
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

		void		_firstSlotWithGeo( SlotWithGeo& package ) const;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const;


		// Methods for FlexPanelChildren

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_willRemoveSlots( Slot * pSlot, int nb );
		void		_hideSlots( FlexPanelSlot *, int nb );
		void		_unhideSlots( FlexPanelSlot *, int nb );

		// Overloaded from WidgetHolder

		Coord		_childPos( Slot * pSlot ) const;
		Size		_childSize( Slot * pSlot ) const;

		void		_childRequestRender( Slot * pSlot );
		void		_childRequestRender( Slot * pSlot, const Rect& rect );
		void		_childRequestResize( Slot * pSlot );

		Widget *	_prevChild( Slot * pSlot ) const;
		Widget *	_nextChild( Slot * pSlot ) const;

	
	private:
	
		void		_cloneContent( const Widget * _pOrg );
		void		_setSize( const Size& size );

		void		_onRequestRender( const Rect& rect, const FlexPanelSlot * pSlot );

		void		_refreshRealGeo( FlexPanelSlot * pSlot );
		Size		_sizeNeededForGeo( FlexPanelSlot * pSlot ) const;
	
		void		_moveSlot(FlexPanelSlot * pFrom, FlexPanelSlot * pTo);
	
		SlotArray<FlexPanelSlot>	m_children;
	
		bool			m_bConfineWidgets;
	};
	
	

} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
