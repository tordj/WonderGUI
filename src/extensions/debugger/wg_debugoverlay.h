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

#ifndef WG_DEBUGOVERLAY_DOT_H
#define WG_DEBUGOVERLAY_DOT_H
#pragma once

#include <wg_overlay.h>
#include <wg_staticslotvector.h>
#include <wg_packpanel.h>

namespace wg
{


	class DebugOverlay;
	typedef	StrongPtr<DebugOverlay>	DebugOverlay_p;
	typedef	WeakPtr<DebugOverlay>	DebugOverlay_wp;

	//____ DebugOverlay __________________________________________________________

	class DebugOverlay : public Overlay
	{

	public:

		//____ ToolboxSlot ___________________________________________________________

		class ToolboxSlot : public Overlay::Slot
		{
			friend class DebugOverlay;
			friend class CDesignToolboxSlotVector;
			template<class S> friend class StaticSlotVector;
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

		class CToolboxVector : public StaticSlotVector<ToolboxSlot>
		{
			friend class DebugOverlay;

			CToolboxVector(SlotHolder * pHolder) : StaticSlotVector<ToolboxSlot>(pHolder) {}
		};


		//.____ Creation __________________________________________

		static DebugOverlay_p	create() { return DebugOverlay_p(new DebugOverlay()); }

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

		virtual spx		_matchingHeight(spx width, int scale) const override;
		virtual spx		_matchingWidth(spx height, int scale) const override;

		SizeSPX			_defaultSize(int scale) const override;


	protected:
		DebugOverlay();
		virtual ~DebugOverlay();

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

		void			_resize( const SizeSPX& size, int scale ) override;
		void			_receive( Msg * pMsg ) override;

		// Toolbox creators

		std::tuple<Widget_p, PackPanel_p> _createToolbox( const char * pTitle );

		Widget_p		_createMainTool();

		Widget_p		_createGenericSlotTool(const StaticSlot& slot);
		Widget_p		_createGenericWidgetTool(Widget * pWidget);

		//

		RectSPX			_selectionGeo() const;
		void			_refreshRealGeo(ToolboxSlot * pSlot, bool bForceResize = false);
		void			_selectWidget(Widget * pWidget);
		//

		bool		m_bEditMode = false;
		bool		m_bSelectMode = false;

		Widget_wp	m_pSelectedWidget;
		Skin_p		m_pSelectionSkin;
		Skin_p		m_pToolboxSkin;

		PackPanel_p	m_pMainToolbox;
		PackPanel_p	m_pSlotToolbox;
		PackPanel_p	m_pWidgetToolbox;

		int			m_pressedToolbox = -1;			// Index for palette that is pressed.
		CoordSPX	m_pressedToolboxStartOfs;
	};

	//____ paletteSkin() ______________________________________________________

	Skin_p DebugOverlay::paletteSkin() const
	{
		return m_pToolboxSkin;
	}

	//____ selectionSkin() ____________________________________________________

	Skin_p DebugOverlay::selectionSkin() const
	{
		return m_pSelectionSkin;
	}

	//____ isEditMode() _______________________________________________________

	bool DebugOverlay::isEditMode() const
	{
		return m_bEditMode;
	}


} // namespace wg
#endif //WG_DEBUGOVERLAY_DOT_H
