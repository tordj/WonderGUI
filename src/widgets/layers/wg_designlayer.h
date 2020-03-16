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

	//____ DesignToolboxSlot ___________________________________________________________

	class DesignToolboxSlot : public Layer::Slot
	{
		friend class DesignLayer;
		friend class CDesignToolboxSlotVector;
		template<class S> friend class CStaticSlotVector;
		template<class S> friend class SlotVector;

	public:


	protected:
		DesignToolboxSlot(SlotHolder * pHolder) : Layer::Slot(pHolder) {}

		inline SlotHolder * _holder() { return static_cast<SlotHolder*>(Layer::Slot::_holder()); }
		inline const SlotHolder * _holder() const { return static_cast<const SlotHolder*>(Layer::Slot::_holder()); }

		const static bool safe_to_relocate = false;

		bool		m_bVisible = true;
		Origo		m_placement = Origo::NorthWest;
		CoordI		m_placementPos;			// Widgets pos relative selected widget and origo.
	};


	//____ CDesignToolboxVector _________________________________________________

	class CDesignToolboxVector : public CStaticSlotVector<DesignToolboxSlot>
	{
		friend class DesignLayer;

		CDesignToolboxVector(SlotHolder * pHolder) : CStaticSlotVector<DesignToolboxSlot>(pHolder) {}
	};


	class DesignLayer;
	typedef	StrongPtr<DesignLayer>	DesignLayer_p;
	typedef	WeakPtr<DesignLayer>	DesignLayer_wp;

	//____ DesignLayer __________________________________________________________

	class DesignLayer : public Layer
	{

	public:

		//.____ Creation __________________________________________

		static DesignLayer_p	create() { return DesignLayer_p(new DesignLayer()); }

		//.____ Components _______________________________________

		CDesignToolboxVector	palettes;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static DesignLayer_p	cast( Object * pObject );

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

		Widget *		_findWidget( const CoordI& ofs, SearchMode mode ) override;
		void			_render(GfxDevice * pDevice, const RectI& canvas, const RectI& window) override;

		// Overloaded from SlotHolder

		Container *		_container() override { return this; }
		RootPanel *		_root() override { return Container::_root(); }
		Object *		_object() override { return this; }
		const Object *	_object() const override { return this; }

		CoordI			_childPos(const StaticSlot * pSlot) const override { return Layer::_childPos(pSlot); }
		CoordI			_childGlobalPos(const StaticSlot * pSlot) const override { return Layer::_childGlobalPos(pSlot); }
		bool			_isChildVisible(const StaticSlot * pSlot) const override { return Layer::_isChildVisible(pSlot); }
		RectI			_childWindowSection(const StaticSlot * pSlot) const override { return Layer::_childWindowSection(pSlot); }

		void			_childRequestRender(StaticSlot * pSlot) override { return Layer::_childRequestRender(pSlot); }
		void			_childRequestRender(StaticSlot * pSlot, const RectI& rect) override { return Layer::_childRequestRender(pSlot); }

		bool			_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Layer::_childRequestFocus(pSlot, pWidget); }
		bool			_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Layer::_childReleaseFocus(pSlot, pWidget); }

		void			_childRequestInView(StaticSlot * pSlot) override { return Layer::_childRequestInView(pSlot); }
		void			_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Layer::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		Widget *		_prevChild(const StaticSlot * pSlot) const override { return Layer::_prevChild(pSlot); }
		Widget *		_nextChild(const StaticSlot * pSlot) const override { return Layer::_nextChild(pSlot); }

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( StaticSlot * pSlot ) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		// Overloaded from Layer

		const Layer::Slot * 	_beginLayerSlots() const override;
		const Layer::Slot * 	_endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		// Overloaded from Widget

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_resize( const SizeI& size ) override;
		void			_receive( Msg * pMsg ) override;

		// Toolbox creators

		Widget_p		_createGenericSlotTool(const StaticSlot& slot);
		Widget_p		_createGenericWidgetTool(Widget * pWidget);


		//

		class MainSlotAccess : public CStandardSlot { friend class DesignLayer; };
		MainSlotAccess * _baseSlot() { return static_cast<MainSlotAccess*>(&mainSlot); }
		const MainSlotAccess * _baseSlot() const { return static_cast<const MainSlotAccess*>(&mainSlot); }

		RectI			_selectionGeo() const;
		void			_refreshRealGeo(DesignToolboxSlot * pSlot);
		void			_selectWidget(Widget * pWidget);
		//

		bool		m_bEditMode = false;

		Widget_wp	m_pSelectedWidget;
		Skin_p		m_pSelectionSkin;
		Skin_p		m_pToolboxSkin;

		PackPanel_p	m_pSlotToolbox;
		PackPanel_p	m_pWidgetToolbox;

		int			m_pressedToolbox = -1;			// Index for palette that is pressed.
		CoordI		m_pressedToolboxStartOfs;
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
