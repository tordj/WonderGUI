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

#include <wg_panel.h>
#include <wg_ihideablechildren.h>

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
		inline FlexPos( Origo _origo, Coord _offset ) : origo(s_origoTab[(int)_origo][0], s_origoTab[(int)_origo][1]), offset(Util::qpixToRaw(_offset)) {}

		inline FlexPos( CoordF _origo ) : origo(_origo) {}
		inline FlexPos( CoordF _origo, Coord _offset ) : origo(_origo), offset(Util::qpixToRaw(_offset)) {}

		inline FlexPos( float _origoX, float _origoY ) : origo(_origoX, _origoY) {}
		inline FlexPos( float _origoX, float _origoY, Coord _offset ) : origo(_origoX, _origoY), offset(Util::qpixToRaw(_offset)) {}

		//.____ Geometry _______________________________________________________

		Coord	posX( Size canvas ) const { return Util::rawToQpix( rawPos( Util::qpixToRaw(canvas))); }
		CoordI	rawPos(SizeI canvas) const { return CoordI((((int)(origo.x*canvas.w + 0.5f)) + offset.x)&0xFFFFFFFC, (((int)(origo.y*canvas.h + 0.5f)) + offset.y)&0xFFFFFFFC); }

		//.____ Properties _____________________________________________________

		CoordF	origo;
		CoordI	offset;

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
		RectI			realGeo;			// Widgets geo relative parent, not pixel aligned.

//		union
//		{
//			struct // Positioned children
//			{
				FlexPos			origo;
				FlexPos			hotspot;
				RectI			placementGeo;	// Widgets geo relative anchor and hotspot. Pixel aligned.
//			};
//			struct	// Stretched children
//			{
				FlexPos			topLeftPin;
				FlexPos			bottomRightPin;
//			};
//		};

	};


	//____ FlexChildrenHolder ____________________________________________________

	class FlexChildrenHolder : public HideableChildrenHolder /** @private */
	{
	public:
		virtual void	_refreshRealGeo(FlexPanelSlot * pSlot) = 0;
		virtual SizeI	_size() const = 0;

	};


	class IFlexPanelChildren;
	typedef	StrongInterfacePtr<IFlexPanelChildren>	IFlexPanelChildren_p;
	typedef	WeakInterfacePtr<IFlexPanelChildren>	IFlexPanelChildren_wp;

	//____ IFlexPanelChildren ________________________________________________________

	class IFlexPanelChildren : public IHideableChildren<FlexPanelSlot,FlexChildrenHolder>
	{
	public:
		/** @private */

		IFlexPanelChildren( SlotArray<FlexPanelSlot> * pSlotArray, FlexChildrenHolder * pHolder ) : IHideableChildren<FlexPanelSlot,FlexChildrenHolder>(pSlotArray,pHolder) {}

		//.____ Misc __________________________________________________________

		inline IFlexPanelChildren_p	ptr() { return IFlexPanelChildren_p(this); }

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
		void		_setMovable(FlexPanelSlot * p, const RectI& geometry, const FlexPos& origo, const FlexPos& hotspot);

//		FlexPanelSlot * _moveAbove(FlexPanelSlot * p, FlexPanelSlot * sibling);
//		FlexPanelSlot *	_moveBelow(FlexPanelSlot * p, FlexPanelSlot * sibling);

		// Methods for movable children

		bool		_setOrigo(FlexPanelSlot * p, const FlexPos& origo);
		FlexPos		_origo(FlexPanelSlot * p) const;

		bool		_setHotspot(FlexPanelSlot * p, const FlexPos& hotspot);
		FlexPos		_hotspot(FlexPanelSlot * p) const;

		bool		_setGeo(FlexPanelSlot * p, const RectI& geometry);
		RectI		_geo(FlexPanelSlot * p) const;

		bool		_setOfs(FlexPanelSlot * p, const CoordI& ofs);
		CoordI		_ofs(FlexPanelSlot * p) const;

		bool		_setSize(FlexPanelSlot * p, const SizeI& size);
		RectI		_size(FlexPanelSlot * p) const;

		bool		_move(FlexPanelSlot * p, const CoordI& ofs);

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


	class FlexPanel : public Panel, protected FlexChildrenHolder
	{

	public:

		//.____ Creation __________________________________________

		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }

		//.____ Interfaces _______________________________________

		IFlexPanelChildren	children;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static FlexPanel_p	cast( Object * pObject );

		//.____ Behavior ________________________________________________________


		void				setConfineWidgets( bool bConfineWidgets );
		bool				isConfiningWidgets() const { return m_bConfineWidgets; }


	protected:
		FlexPanel();
		virtual ~FlexPanel();
		virtual Widget* _newOfMyType() const override{ return new FlexPanel(); };


		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;


		// Methods for FlexPanelChildren

		Slot *		_incSlot(Slot * pSlot) const override;
		Slot *		_decSlot(Slot * pSlot) const override;
		void		_didAddSlots( Slot * pSlot, int nb ) override;
		void		_didMoveSlots(Slot * pFrom, Slot * pTo, int nb) override;
		void		_willRemoveSlots( Slot * pSlot, int nb ) override;
		void		_hideSlots( Slot *, int nb ) override;
		void		_unhideSlots( Slot *, int nb ) override;
		Object *	_object() override { return this; }
		WidgetHolder *	_widgetHolder() override { return this; }
		void		_refreshRealGeo(FlexPanelSlot * pSlot) override;
		SizeI		_size() const override{ return m_size; }

		// Overloaded from WidgetHolder

		CoordI		_childPos( Slot * pSlot ) const override;
		SizeI		_childSize( Slot * pSlot ) const override;

		void		_childRequestRender( Slot * pSlot ) override;
		void		_childRequestRender( Slot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( Slot * pSlot ) override;

		Widget *	_prevChild( const Slot * pSlot ) const override;
		Widget *	_nextChild( const Slot * pSlot ) const override;

		void		_releaseChild(Slot * pSlot) override;

	private:

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_setSize( const SizeI& size ) override;

		void		_onRequestRender( const RectI& rect, const FlexPanelSlot * pSlot );

		SizeI		_sizeNeededForGeo( FlexPanelSlot * pSlot ) const;


		SlotArray<FlexPanelSlot>	m_children;

		bool			m_bConfineWidgets;
		int				m_qpixPerPoint;
	};



} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
