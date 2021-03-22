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

#ifndef WG_DESIGNLAYER_DOT_H
#define WG_DESIGNLAYER_DOT_H
#pragma once

#include <wg_layer.h>
#include <wg_cstaticslotvector.h>
#include <wg_packpanel.h>

namespace wg
{


	class DesignLayer;
	typedef	StrongPtr<DesignLayer>	DesignLayer_p;
	typedef	WeakPtr<DesignLayer>	DesignLayer_wp;

	//____ DesignLayer __________________________________________________________

	class DesignLayer : public Layer
	{

	public:

		//____ ToolboxSlot ___________________________________________________________

		class ToolboxSlot : public Layer::Slot
		{
			friend class DesignLayer;
			friend class CDesignToolboxSlotVector;
			template<class S> friend class CStaticSlotVector;
			template<class S> friend class SlotVector;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

		protected:
			ToolboxSlot(SlotHolder * pHolder) : Layer::Slot(pHolder) {}

			inline SlotHolder * _holder() { return static_cast<SlotHolder*>(Layer::Slot::_holder()); }
			inline const SlotHolder * _holder() const { return static_cast<const SlotHolder*>(Layer::Slot::_holder()); }

			const static bool safe_to_relocate = false;

			bool		m_bVisible = true;
			Placement	m_placement = Placement::NorthWest;
			Coord		m_placementPos;			// Widgets pos relative selected widget and placement.
		};


		//____ CToolboxVector _________________________________________________

		class CToolboxVector : public CStaticSlotVector<ToolboxSlot>
		{
			friend class DesignLayer;

			CToolboxVector(SlotHolder * pHolder) : CStaticSlotVector<ToolboxSlot>(pHolder) {}
		};


		//.____ Creation __________________________________________

		static DesignLayer_p	create() { return DesignLayer_p(new DesignLayer()); }

		//.____ Components _______________________________________

		CToolboxVector	palettes;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		virtual MU		matchingHeight(MU width) const override;
		virtual MU		matchingWidth(MU height) const override;

		Size			preferredSize() const override;

		//.____ Appearance _________________________________________________

		void				setToolboxSkin(Skin * pSkin);
		inline Skin_p		paletteSkin() const;

		void				setSelectionSkin(Skin * pSkin);
		inline Skin_p		selectionSkin() const;

		//.____ Control ____________________________________________________

		void				setEditMode(bool bEditMode);
		inline bool			isEditMode() const;


	protected:
		DesignLayer();
		virtual ~DesignLayer();
		virtual Widget* _newOfMyType() const  override { return new DesignLayer(); };


	private:

		// Overloaded from Container

		Widget *		_findWidget( const Coord& ofs, SearchMode mode ) override;
		void			_render(GfxDevice * pDevice, const Rect& canvas, const Rect& window) override;

		// Overloaded from SlotHolder

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( StaticSlot * pSlot ) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		// Overloaded from Layer

		const Layer::Slot * 	_beginLayerSlots() const override;
		const Layer::Slot * 	_endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		// Overloaded from Widget

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_resize( const Size& size ) override;
		void			_receive( Msg * pMsg ) override;

		// Toolbox creators

		Widget_p		_createGenericSlotTool(const StaticSlot& slot);
		Widget_p		_createGenericWidgetTool(Widget * pWidget);

		//

		Rect			_selectionGeo() const;
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
		Coord		m_pressedToolboxStartOfs;
	};

	//____ paletteSkin() ______________________________________________________

	Skin_p DesignLayer::paletteSkin() const
	{
		return m_pToolboxSkin;
	}

	//____ selectionSkin() ____________________________________________________

	Skin_p DesignLayer::selectionSkin() const
	{
		return m_pSelectionSkin;
	}

	//____ isEditMode() _______________________________________________________

	bool DesignLayer::isEditMode() const
	{
		return m_bEditMode;
	}


} // namespace wg
#endif //WG_DESIGNLAYER_DOT_H
