/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#ifndef WG_FROSTOVERLAY_DOT_H
#define WG_FROSTOVERLAY_DOT_H
#pragma once

#include <wg_overlay.h>
#include <wg_frost.h>
#include <wg_dynamicslotvector.h>

namespace wg
{

	class FrostOverlay;
	typedef	StrongPtr<FrostOverlay>	FrostOverlay_p;
	typedef	WeakPtr<FrostOverlay>	FrostOverlay_wp;

	//____ FrostOverlay __________________________________________________________
	/**
	 * @brief	Overlay with freely placed panes on frosted glass.
	 *
	 * The main child is rendered into a backdrop that is cached in a surface. The
	 * children in frostSlots are rendered on top of it, each with a frosted copy of
	 * the backdrop behind it as background (see Frost). The first slot is the topmost.
	 *
	 * Since the backdrop is cached, panes can be moved without the main child being
	 * rendered again, and content in the panes can change without the frost being
	 * recalculated.
	 *
	 * The main child takes up the whole overlay. The skin of the overlay is rendered
	 * below the backdrop and is not frosted.
	 */

	class FrostOverlay : public Overlay
	{
		friend class Slot;

	public:

		//____ Slot ___________________________________________________________

		class Slot : public Overlay::Slot
		{
			friend class FrostOverlay;
			template<class S> friend class DynamicSlotVector;
			template<class S> friend class SlotVector;

		public:

			//.____ Blueprint _____________________________________________________

			struct Blueprint
			{
				bool		frost = true;
				Coord		pos;
				Size		size;			// Width or height of 0 is taken from the widget's default or matching size.
			};

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry _________________________________________________

			void			setGeo(const Rect& geometry);
			void			setPos(const Coord& pos);
			void			setSize(const Size& size);
			void			move(const Coord& ofs);

			//.____ Appearance _______________________________________________

			void			setFrost(bool bFrost);
			bool			hasFrost() const { return m_bFrost; }

		protected:

			Slot(SlotHolder * pHolder) : Overlay::Slot(pHolder) {}

			bool _setBlueprint( const Blueprint& bp );

			inline FrostOverlay * _holder() { return static_cast<FrostOverlay*>(Overlay::Slot::_holder()); }
			inline const FrostOverlay * _holder() const { return static_cast<const FrostOverlay*>(Overlay::Slot::_holder()); }

			const static bool safe_to_relocate = false;

			Rect			m_placementGeo;			// Position relative to the overlay and requested size.
			bool			m_bFrost = true;
			Frost::Pane		m_pane;
		};

		//.____ Blueprint _____________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			disabled = false;
			bool			dropTarget = false;
			Finalizer_p		finalizer = nullptr;
			Frost::Blueprint frost;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::Undefined;
			bool			pickable = false;
			uint8_t			pickCategory = 0;
			bool			pickHandle = false;
			PointerStyle	pointer = PointerStyle::Undefined;
			bool			selectable = false;
			Skin_p			skin;
			bool			stickyFocus = false;
			bool			tabLock = false;
			bool			takesFocusFromChild = false;
			String			tooltip;
			bool			usePickHandles = false;
		};

		//.____ Creation __________________________________________

		static FrostOverlay_p	create() { return FrostOverlay_p(new FrostOverlay()); }
		static FrostOverlay_p	create( const Blueprint& blueprint ) { return FrostOverlay_p(new FrostOverlay(blueprint)); }

		//.____ Components _______________________________________

		DynamicSlotVector<Slot>		frostSlots;
		Frost						frost;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		FrostOverlay();

		template<class BP> FrostOverlay(const BP& bp) : Overlay(bp), frostSlots(this), frost(this)
		{
			frost._initFromBlueprint(bp.frost);
		}

		virtual ~FrostOverlay();

	private:

		void			_refreshRealGeo(Slot * pSlot, bool bForceResize = false);

		// Overloaded from Widget

		void			_resize(const SizeSPX& size, int scale) override;
		void			_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		void			_childRequestRender( StaticSlot * pSlot, const RectSPX& rect ) override;
		void			_childRequestResize( StaticSlot * pSlot ) override;

		void			_releaseChild(StaticSlot * pSlot) override;
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;

		// Overloaded from Overlay

		const Overlay::Slot * _beginOverlaySlots() const override;
		const Overlay::Slot * _endOverlaySlots() const override;
		int					_sizeOfOverlaySlot() const override;
	};


} // namespace wg
#endif //WG_FROSTOVERLAY_DOT_H
