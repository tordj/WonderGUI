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
#include <wg_cdynamicslotarray.h>

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


	//____ FlexSlot ____________________________________________________________

	class FlexSlot : public DynamicSlot
	{
		friend class FlexPanel;
		friend class CFlexSlotArray;
		template<class S> friend class CDynamicSlotArray;

	public:

		//.____ Properties _________________________________________________________

		void			setPinned();	
		void			setPinned(const FlexPos& topLeft, const FlexPos& bottomRight);
		inline bool		isPinned() const { return m_bPinned; }

		void			setMovable(const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest);
		void			setMovable(const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot );
		inline bool		isMovable() const { return !m_bPinned; }


		//.____ Geometry ______________________________________________________

		// Methods for movable children

		bool			setOrigo(const FlexPos& origo);
		inline FlexPos	origo() const { return m_origo; }

		bool			setHotspot(const FlexPos& hotspot);
		inline FlexPos	hotspot() const { return m_hotspot; }

		bool			setGeo(const Rect& geometry);

		bool			setOffset(const Coord& ofs);
		inline Coord	offset() const { return Util::rawToQpix(m_placementGeo.pos()); }

		bool			setSize(const Size& size);

		bool			move(const Coord& ofs);

		// Methods for pinned children

		inline FlexPos	topLeftPin() const { return m_topLeftPin; }
		inline FlexPos	bottomRightPin() const { return m_bottomRightPin; }

		//

		inline Coord	pos() const { return Util::rawToQpix(m_realGeo.pos()); }
		inline Size		size() const { return Util::rawToQpix(m_realGeo.size()); }
		inline Rect		geo() const { return Util::rawToQpix(m_realGeo); }

	protected:

		class Holder : public DynamicSlot::Holder
		{
		public:
			virtual void	_refreshRealGeo(FlexSlot * pSlot, bool bForceRefresh = false) = 0;
			virtual SizeI	_size() const = 0;
		};

		FlexSlot(Holder * pHolder) : DynamicSlot(pHolder), m_bPinned(false), m_bVisible(false), m_origo(Origo::NorthWest), m_hotspot(Origo::NorthWest) {}


		inline Holder * _holder() { return static_cast<Holder*>(DynamicSlot::_holder()); }
		inline const Holder * _holder() const { return static_cast<const Holder*>(DynamicSlot::_holder()); }

		bool			m_bPinned;
		bool			m_bVisible;
		RectI			m_realGeo;			// Widgets geo relative parent, pixel aligned.

		// Positioned children

		FlexPos			m_origo;
		FlexPos			m_hotspot;
		RectI			m_placementGeo;		// Widgets geo relative anchor and hotspot, not pixel aligned.

		//	Stretched children

		FlexPos			m_topLeftPin;
		FlexPos			m_bottomRightPin;

	};


	//____ CFlexSlotArray ________________________________________________________

	class CFlexSlotArray;
	typedef	StrongComponentPtr<CFlexSlotArray>	CFlexSlotArray_p;
	typedef	WeakComponentPtr<CFlexSlotArray>	CFlexSlotArray_wp;

	class CFlexSlotArray : public CDynamicSlotArray<FlexSlot>
	{
		friend class FlexPanel;
	public:

		//.____ Content _______________________________________________________

		iterator	pushFrontPinned(const Widget_p& pWidget, const FlexPos& topLeft,
									const FlexPos& bottomRight );
		iterator	pushFrontMovable(const Widget_p& pWidget, const Rect& geometry = Rect(), const FlexPos& origo = Origo::NorthWest,
									const FlexPos& hotspot = Origo::NorthWest );

		iterator	pushBackPinned(const Widget_p& pWidget, const FlexPos& topLeft,
									const FlexPos& bottomRight );
		iterator	pushBackMovable(const Widget_p& pWidget, const Rect& geometry = Rect(), const FlexPos& origo = Origo::NorthWest,
									const FlexPos& hotspot = Origo::NorthWest );

		iterator	insertPinned( int index, const Widget_p& pWidget, const FlexPos& topLeft,
									const FlexPos& bottomRight );
		iterator	insertMovable( int index, const Widget_p& pWidget, const Rect& geometry,
									const FlexPos& origo = Origo::NorthWest, const FlexPos& hotspot = Origo::NorthWest );

		//.____ Misc _______________________________________________________

		inline CFlexSlotArray_p ptr() { return CFlexSlotArray_p(this); }


	protected:
		class Holder : public  CDynamicSlotArray<FlexSlot>::Holder
		{
		};

		CFlexSlotArray(Holder * pHolder) : CDynamicSlotArray<FlexSlot>(pHolder) {}


		inline CFlexSlotArray::Holder * _holder() { return static_cast<CFlexSlotArray::Holder*>(CDynamicSlotArray<FlexSlot>::_holder()); }
		inline const CFlexSlotArray::Holder * _holder() const { return static_cast<const CFlexSlotArray::Holder*>(CDynamicSlotArray<FlexSlot>::_holder()); }
	};



	//____ FlexPanel _________________________________________________________

	/**
	 * @brief	A widget for individual, flexible arrangement of children.
	 *
	 * A widget for individual, flexible arrangement of children.
	 *
	 */


	class FlexPanel : public Panel, protected CFlexSlotArray::Holder
	{
	public:

		//.____ Creation __________________________________________

		static FlexPanel_p	create() { return FlexPanel_p(new FlexPanel()); }

		//.____ Components _______________________________________

		CFlexSlotArray		slots;

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

		void		_didAddSlots( StaticSlot * pSlot, int nb ) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots( StaticSlot * pSlot, int nb ) override;
		void		_hideSlots( StaticSlot *, int nb ) override;
		void		_unhideSlots( StaticSlot *, int nb ) override;
		void		_refreshRealGeo(FlexSlot * pSlot, bool bForceRefresh = false) override;
		SizeI		_size() const override { return m_size; }

		// Overloaded from WidgetHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }
		const Object *	_object() const override { return this; }

		CoordI		_childPos(const StaticSlot * pSlot) const override;
		CoordI		_childGlobalPos(const StaticSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const StaticSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const StaticSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(StaticSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

	private:

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;

		void		_onRequestRender( const RectI& rect, const FlexSlot * pSlot );

		SizeI		_sizeNeededForGeo( FlexSlot * pSlot ) const;

		bool		m_bConfineWidgets = false;
//		int			m_qpixPerPoint;
	};



} // namespace wg
#endif //WG_FLEXPANEL_DOT_H
