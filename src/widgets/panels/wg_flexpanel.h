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
#include <wg_cslotarray.h>

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


	//____ FlexSlotHolder ____________________________________________________________

	class FlexSlotHolder : public SlotHolder
	{

	};


	//____ FlexSlot ____________________________________________________________

	class FlexSlot : public BasicSlot		/** @private */
	{
		friend class FlexPanel;
		friend class IFlexSlots;
		template<class S> friend class CSlotArray;


	public:
		FlexSlot(SlotHolder * pHolder) : BasicSlot(pHolder), bPinned(false), bVisible(false), origo(Origo::NorthWest), hotspot(Origo::NorthWest) {}

	protected:
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


	//____ IFlexSlots ________________________________________________________

	class IFlexSlots;
	typedef	StrongComponentPtr<IFlexSlots>	IFlexSlots_p;
	typedef	WeakComponentPtr<IFlexSlots>	IFlexSlots_wp;

	class IFlexSlots : public CSlotArray<FlexSlot>
	{
		friend class FlexPanel;
	public:

		class Holder : public  CSlotArray<FlexSlot>::Holder			/** @private */
		{
		public:
			virtual void	_refreshRealGeo(FlexSlot * pSlot, bool bForceRefresh = false) = 0;
			virtual SizeI	_size() const = 0;

		};


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

		//.____ Misc _______________________________________________________

		inline IFlexSlots_p ptr() { return IFlexSlots_p(this); }


	protected:
		IFlexSlots(SlotArray<FlexSlot> * pSlotArray, IFlexSlots::Holder * pHolder) : CSlotArray<FlexSlot>(pSlotArray, pHolder) {}

		void		_setPinned(FlexSlot * p);
		void		_setPinned(FlexSlot * p, const FlexPos& topLeft, const FlexPos& bottomRight);

		void		_setMovable(FlexSlot * p, const FlexPos& origo, const FlexPos& hotspot);
		void		_setMovable(FlexSlot * p, const RectI& geometry, const FlexPos& origo, const FlexPos& hotspot);

//		FlexSlot * _moveAbove(FlexSlot * p, FlexSlot * sibling);
//		FlexSlot *	_moveBelow(FlexSlot * p, FlexSlot * sibling);

		// Methods for movable children

		bool		_setOrigo(FlexSlot * p, const FlexPos& origo);
		FlexPos		_origo(FlexSlot * p) const;

		bool		_setHotspot(FlexSlot * p, const FlexPos& hotspot);
		FlexPos		_hotspot(FlexSlot * p) const;

		bool		_setGeo(FlexSlot * p, const RectI& geometry);
		RectI		_geo(FlexSlot * p) const;

		bool		_setOfs(FlexSlot * p, const CoordI& ofs);
		CoordI		_ofs(FlexSlot * p) const;

		bool		_setSize(FlexSlot * p, const SizeI& size);
		RectI		_size(FlexSlot * p) const;

		bool		_move(FlexSlot * p, const CoordI& ofs);

		// Methods for pinned children

		FlexPos	_topLeftCorner(FlexSlot * p) const;
		FlexPos	_bottomRightCorner(FlexSlot * p) const;

		//

		inline IFlexSlots::Holder * _holder() { return static_cast<IFlexSlots::Holder*>(m_pHolder); }
	};



	//____ FlexPanel _________________________________________________________

	/**
	 * @brief	A widget for individual, flexible arrangement of children.
	 *
	 * A widget for individual, flexible arrangement of children.
	 *
	 */


	class FlexPanel : public Panel, protected IFlexSlots::Holder, protected FlexSlotHolder
	{

	public:

		//.____ Creation __________________________________________

		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }

		//.____ Components _______________________________________

		IFlexSlots	children;

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

		void		_didAddSlots( BasicSlot * pSlot, int nb ) override;
		void		_didMoveSlots(BasicSlot * pFrom, BasicSlot * pTo, int nb) override;
		void		_willRemoveSlots( BasicSlot * pSlot, int nb ) override;
		void		_hideSlots( BasicSlot *, int nb ) override;
		void		_unhideSlots( BasicSlot *, int nb ) override;
		void		_refreshRealGeo(FlexSlot * pSlot, bool bForceRefresh = false) override;
		SizeI		_size() const override{ return m_size; }

		// Overloaded from WidgetHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }

		CoordI		_childPos(const BasicSlot * pSlot) const override;
		CoordI		_childGlobalPos(const BasicSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const BasicSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const BasicSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(BasicSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		void		_childRequestRender( BasicSlot * pSlot ) override;
		void		_childRequestRender( BasicSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( BasicSlot * pSlot ) override;

		Widget *	_prevChild( const BasicSlot * pSlot ) const override;
		Widget *	_nextChild( const BasicSlot * pSlot ) const override;

		void		_releaseChild(BasicSlot * pSlot) override;
		void		_replaceChild(BasicSlot * pSlot, Widget * pNewChild ) override;

	private:

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;

		void		_onRequestRender( const RectI& rect, const FlexSlot * pSlot );

		SizeI		_sizeNeededForGeo( FlexSlot * pSlot ) const;


		SlotArray<FlexSlot>	m_children;

		bool			m_bConfineWidgets = false;
		int				m_qpixPerPoint;
	};



} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
