/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_DESIGNOVERLAY_DOT_H
#define WG_DESIGNOVERLAY_DOT_H
#pragma once

#include <wg_overlay.h>
#include <wg_cstaticslotvector.h>
#include <wg_packpanel.h>

namespace wg
{


	class DesignOverlay;
	typedef	StrongPtr<DesignOverlay>	DesignOverlay_p;
	typedef	WeakPtr<DesignOverlay>	DesignOverlay_wp;

	//____ DesignOverlay __________________________________________________________

	class DesignOverlay : public Overlay
	{

	public:

		//____ ToolboxSlot ___________________________________________________________

		class ToolboxSlot : public Overlay::Slot
		{
			friend class DesignOverlay;
			friend class CDesignToolboxSlotVector;
			template<class S> friend class CStaticSlotVector;
			template<class S> friend class SlotVector;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

		protected:
			ToolboxSlot(SlotHolder * pHolder) : Overlay::Slot(pHolder) {}

			inline SlotHolder * _holder() { return static_cast<SlotHolder*>(Overlay::Slot::_holder()); }
			inline const SlotHolder * _holder() const { return static_cast<const SlotHolder*>(Overlay::Slot::_holder()); }

			const static bool safe_to_relocate = false;

			bool		m_bVisible = true;
			Placement	m_placement = Placement::NorthWest;
			CoordSPX	m_placementPos;			// Widgets pos relative selected widget and placement.
		};


		//____ CToolboxVector _________________________________________________

		class CToolboxVector : public CStaticSlotVector<ToolboxSlot>
		{
			friend class DesignOverlay;

			CToolboxVector(SlotHolder * pHolder) : CStaticSlotVector<ToolboxSlot>(pHolder) {}
		};


		//.____ Creation __________________________________________

		static DesignOverlay_p	create() { return DesignOverlay_p(new DesignOverlay()); }

		//.____ Components _______________________________________

		CToolboxVector	palettes;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void				setToolboxSkin(Skin * pSkin);
		inline Skin_p		paletteSkin() const;

		void				setSelectionSkin(Skin * pSkin);
		inline Skin_p		selectionSkin() const;

		//.____ Control ____________________________________________________

		void				setEditMode(bool bEditMode);
		inline bool			isEditMode() const;

		//.____ Internal ______________________________________________________

		virtual spx		_matchingHeight(spx width, int scale = -1) const override;
		virtual spx		_matchingWidth(spx height, int scale = -1) const override;

		SizeSPX			_defaultSize(int scale = -1) const override;


	protected:
		DesignOverlay();
		virtual ~DesignOverlay();

	private:

		// Overloaded from Container

		Widget *		_findWidget( const CoordSPX& ofs, SearchMode mode ) override;
		void			_render(GfxDevice * pDevice, const RectSPX& canvas, const RectSPX& window) override;

		// Overloaded from SlotHolder

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Overlay::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( StaticSlot * pSlot ) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		// Overloaded from Overlay

		const Overlay::Slot * 	_beginOverlaySlots() const override;
		const Overlay::Slot * 	_endOverlaySlots() const override;
		int					_sizeOfOverlaySlot() const override;

		// Overloaded from Widget

		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_receive( Msg * pMsg ) override;

		// Toolbox creators

		Widget_p		_createGenericSlotTool(const StaticSlot& slot);
		Widget_p		_createGenericWidgetTool(Widget * pWidget);

		//

		RectSPX			_selectionGeo() const;
		void			_refreshRealGeo(ToolboxSlot * pSlot);
		void			_selectWidget(Widget * pWidget);
		//

		bool		m_bEditMode = false;

		Widget_wp	m_pSelectedWidget;
		Skin_p		m_pSelectionSkin;
		Skin_p		m_pToolboxSkin;

		PackPanel_p	m_pSlotToolbox;
		PackPanel_p	m_pWidgetToolbox;

		int			m_pressedToolbox = -1;			// Index for palette that is pressed.
		CoordSPX	m_pressedToolboxStartOfs;
	};

	//____ paletteSkin() ______________________________________________________

	Skin_p DesignOverlay::paletteSkin() const
	{
		return m_pToolboxSkin;
	}

	//____ selectionSkin() ____________________________________________________

	Skin_p DesignOverlay::selectionSkin() const
	{
		return m_pSelectionSkin;
	}

	//____ isEditMode() _______________________________________________________

	bool DesignOverlay::isEditMode() const
	{
		return m_bEditMode;
	}


} // namespace wg
#endif //WG_DESIGNOVERLAY_DOT_H
