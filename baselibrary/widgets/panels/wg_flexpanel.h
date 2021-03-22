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
#include <wg_cdynamicslotvector.h>

namespace wg
{

	class FlexPanel;
	typedef	StrongPtr<FlexPanel>	FlexPanel_p;
	typedef	WeakPtr<FlexPanel>		FlexPanel_wp;


	//____ FlexPos ____________________________________________________________


	class FlexPos
	{
	public:

		//.____ Creation _______________________________________________________

		inline FlexPos() {}

		inline FlexPos( Placement _origo ) : origo(s_origoTab[(int)_origo][0], s_origoTab[(int)_origo][1]) {}
		inline FlexPos( Placement _origo, Coord _offset ) : origo(s_origoTab[(int)_origo][0], s_origoTab[(int)_origo][1]), offset(_offset) {}

		inline FlexPos( CoordF _origo ) : origo(_origo) {}
		inline FlexPos( CoordF _origo, Coord _offset ) : origo(_origo), offset(_offset) {}

		inline FlexPos( float _origoX, float _origoY ) : origo(_origoX, _origoY) {}
		inline FlexPos( float _origoX, float _origoY, Coord _offset ) : origo(_origoX, _origoY), offset(_offset) {}

		//.____ Geometry _______________________________________________________

		Coord	pos(Size canvas) const 
		{ 
			Coord c( origo.x*canvas.w + offset.x, origo.y*canvas.h + offset.y);
			return c.aligned();
		}

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



	//____ FlexPanel _________________________________________________________

	/**
	 * @brief	A widget for individual, flexible arrangement of children.
	 *
	 * A widget for individual, flexible arrangement of children.
	 *
	 */


	class FlexPanel : public Panel
	{
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ____________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class FlexPanel;
			friend class CSlots;
			template<class S> friend class CDynamicSlotVector;

		public:
			 
			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Properties _________________________________________________________

			void			setPinned();
			void			setPinned(const FlexPos& topLeft, const FlexPos& bottomRight);
			inline bool		isPinned() const { return m_bPinned; }

			void			setMovable(const FlexPos& origo = Placement::NorthWest, const FlexPos& hotspot = Placement::NorthWest);
			void			setMovable(const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot);
			inline bool		isMovable() const { return !m_bPinned; }


			//.____ Geometry ______________________________________________________

			// Methods for movable children

			bool			setOrigo(const FlexPos& origo);
			inline FlexPos	origo() const { return m_origo; }

			bool			setHotspot(const FlexPos& hotspot);
			inline FlexPos	hotspot() const { return m_hotspot; }

			bool			setGeo(const Rect& geometry);

			bool			setOffset(const Coord& ofs);
			inline Coord	offset() const { return m_placementGeo.pos(); }

			bool			setSize(const Size& size);

			bool			move(const Coord& ofs);

			// Methods for pinned children

			inline FlexPos	topLeftPin() const { return m_topLeftPin; }
			inline FlexPos	bottomRightPin() const { return m_bottomRightPin; }

			//

			inline Coord	pos() const { return m_realGeo.pos(); }
			inline Size		size() const { return m_realGeo.size(); }
			inline Rect		geo() const { return m_realGeo; }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }


		protected:

			Slot(SlotHolder * pHolder) : DynamicSlot(pHolder), m_bPinned(false), m_bVisible(false), m_origo(Placement::NorthWest), m_hotspot(Placement::NorthWest) {}

			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;


			inline FlexPanel * _holder() { return static_cast<FlexPanel*>(DynamicSlot::_holder()); }
			inline const FlexPanel * _holder() const { return static_cast<const FlexPanel*>(DynamicSlot::_holder()); }

			bool			m_bPinned;
			bool			m_bVisible;
			Rect			m_realGeo;			// Widgets geo relative parent, pixel aligned.

			// Positioned children

			FlexPos			m_origo;
			FlexPos			m_hotspot;
			Rect			m_placementGeo;		// Widgets geo relative anchor and hotspot, not pixel aligned.

			//	Stretched children

			FlexPos			m_topLeftPin;
			FlexPos			m_bottomRightPin;

		};


		//____ CSlots ________________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CDynamicSlotVector<Slot>
		{
			friend class FlexPanel;
		public:

			//.____ Content _______________________________________________________

			iterator	pushFrontPinned(const Widget_p& pWidget, const FlexPos& topLeft,
				const FlexPos& bottomRight);
			iterator	pushFrontMovable(const Widget_p& pWidget, const Rect& geometry = Rect(), const FlexPos& origo = Placement::NorthWest,
				const FlexPos& hotspot = Placement::NorthWest);

			iterator	pushBackPinned(const Widget_p& pWidget, const FlexPos& topLeft,
				const FlexPos& bottomRight);
			iterator	pushBackMovable(const Widget_p& pWidget, const Rect& geometry = Rect(), const FlexPos& origo = Placement::NorthWest,
				const FlexPos& hotspot = Placement::NorthWest);

			iterator	insertPinned(int index, const Widget_p& pWidget, const FlexPos& topLeft,
				const FlexPos& bottomRight);
			iterator	insertMovable(int index, const Widget_p& pWidget, const Rect& geometry,
				const FlexPos& origo = Placement::NorthWest, const FlexPos& hotspot = Placement::NorthWest);

			//.____ Misc _______________________________________________________

			inline CSlots_p ptr() { return CSlots_p(this); }


		protected:

			CSlots(SlotHolder * pHolder) : CDynamicSlotVector<Slot>(pHolder) {}

			inline FlexPanel * _holder() { return static_cast<FlexPanel*>(CDynamicSlotVector<Slot>::_holder()); }
			inline const FlexPanel * _holder() const { return static_cast<const FlexPanel*>(CDynamicSlotVector<Slot>::_holder()); }
		};



		//.____ Creation __________________________________________

		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }

		//.____ Components _______________________________________

		CSlots			slots;

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		Size			preferredSize() const override;

		//.____ Behavior ________________________________________________________

		void			setConfineWidgets( bool bConfineWidgets );
		bool			isConfiningWidgets() const { return m_bConfineWidgets; }


	protected:
		FlexPanel();
		virtual ~FlexPanel();
		virtual Widget* _newOfMyType() const override{ return new FlexPanel(); };


		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;
		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		Coord		_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const Rect& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void		_hideSlots(StaticSlot *, int nb) override;
		void		_unhideSlots(StaticSlot *, int nb) override;

	private:
		void		_refreshRealGeo(Slot * pSlot, bool bForceRefresh = false);
		Size		_size() const { return m_size; }

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const Size& size ) override;

		void		_onRequestRender( const Rect& rect, const Slot * pSlot );

		Size		_sizeNeededForGeo( Slot * pSlot ) const;

		bool		m_bConfineWidgets = false;
//		int			m_qpixPerPoint;
	};



} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
