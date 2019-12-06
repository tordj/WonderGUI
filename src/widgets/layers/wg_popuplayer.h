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

#ifndef WG_POPUPLAYER_DOT_H
#define WG_POPUPLAYER_DOT_H
#pragma once

#include <wg_layer.h>
#include <wg_slotarray.h>

namespace wg
{

	class PopupLayer;
	typedef	StrongPtr<PopupLayer>	PopupLayer_p;
	typedef	WeakPtr<PopupLayer>	PopupLayer_wp;



	//____ PopupSlot ___________________________________________________________

	class PopupSlot : public LayerSlot		/** @private */
	{
		friend class PopupLayer;
		friend class CPopupSlotArray;

	public:

		class Holder : public LayerSlot::Holder
		{
		};

		PopupSlot(Holder * pHolder) : LayerSlot(pHolder), attachPoint(Origo::NorthWest), maxSize(INT_MAX,INT_MAX) {}

		const static bool safe_to_relocate = false;

	protected:
		enum class State
		{
			OpeningDelay,			// Popup is in "delayed opening" mode. Some ms before it starts to open.
			Opening,				// Popup is opening (fading in).
			PeekOpen,				// Popup is open until pointer leaves launcherGeo (mode only allowed if bAutoClose is set).
			WeakOpen,				// Popup is open, but closed if other entry of ancestors is peeked (mode only allowed if bAutoClose is set).
			FixedOpen,				// Popup is open until it is closed by a pop() call.
			ClosingDelay,			// Popup is in countdown to closing mode (mode only allowed if bAutoClose is set).
			Closing,				// Popup is closing (fading out).
		};

		RectI		launcherGeo;		// Launcher geo relative sibling or parent.
		Origo		attachPoint;
		bool		bAutoClose;			// Has been opened in auto-close mode.
		State		state;
		int			stateCounter;		// Counts millisec the slot has been in a transitative state (Delay, Opening, Coundown and Closing).
		SizeI		maxSize;
		Widget_wp	pOpener;			// Widget that opened this popup.
		Widget_wp	pKeyFocus;			// Pointer at widget that held focus when this popup was ontop.
	};


	class CPopupSlotArray;
	typedef	StrongComponentPtr<CPopupSlotArray>	CPopupSlotArray_p;
	typedef	WeakComponentPtr<CPopupSlotArray>	CPopupSlotArray_wp;

	//____ CPopupSlotArray ________________________________________________________

	class CPopupSlotArray : public Component
	{
	public:
		/** @private */

		CPopupSlotArray(PopupLayer * pHolder) : m_pHolder(pHolder) {}

		//.____ Misc __________________________________________________________

		inline CPopupSlotArray_p	ptr() { return CPopupSlotArray_p(this); }

		//.____ Operators _____________________________________________________

		Widget& operator[](int index) const;

		//.____ Content _______________________________________________________

		int		size() const;
		void	push(Widget * pPopup, Widget * pOpener, const Rect& launcherGeo, Origo attachPoint = Origo::NorthEast, bool bAutoClose = false, Size maxSize = Size(INT_MAX>>8, INT_MAX>>8));
		void	pop(int nb = 1);
		void	pop(Widget * pPopup);
		void	clear();

		Widget_p at(int index) const;

	protected:
		Object *		_object() override;
		const Object *	_object() const override;
		PopupLayer *	m_pHolder;

	};


	//____ PopupLayer ____________________________________________________________

	class PopupLayer : public Layer, protected PopupSlot::Holder
	{
		friend class CPopupSlotArray;

	public:

		//.____ Creation __________________________________________

		static PopupLayer_p	create() { return PopupLayer_p(new PopupLayer()); }

		//.____ Components _______________________________________

		CPopupSlotArray	popups;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static PopupLayer_p	cast( Object * pObject );


	protected:
		PopupLayer();
		virtual ~PopupLayer();
		virtual Widget* _newOfMyType() const override { return new PopupLayer(); };

		PopupLayer *	_getPopupLayer() const override { return const_cast<PopupLayer*>(this); }

		void			_stealKeyboardFocus();
		void			_restoreKeyboardFocus();
		void			_updateGeo(PopupSlot * pSlot, bool bForceResize = false);
		void			_removeSlots(int ofs, int nb);
		void			_addSlot(Widget * pPopup, Widget * pOpener, const RectI& launcherGeo, Origo attachPoint, bool bAutoClose, SizeI maxSize);

		// Overloaded from Container

		Widget *		_findWidget( const CoordI& ofs, SearchMode mode ) override;

		// Overloaded from WidgetHolder

		Container *		_container() override { return this; }
		RootPanel *		_root() override { return Container::_root(); }
		Object *		_object() override { return this; }

		CoordI			_childPos(const BasicSlot * pSlot) const { return Layer::_childPos(pSlot); }
		CoordI			_childGlobalPos(const BasicSlot * pSlot) const override { return Layer::_childGlobalPos(pSlot); }
		bool			_isChildVisible(const BasicSlot * pSlot) const override { return Layer::_isChildVisible(pSlot); }
		RectI			_childWindowSection(const BasicSlot * pSlot) const override { return Layer::_childWindowSection(pSlot); }

		void			_childRequestRender(BasicSlot * pSlot) { return Layer::_childRequestRender(pSlot); }
		void			_childRequestRender(BasicSlot * pSlot, const RectI& rect) { return Layer::_childRequestRender(pSlot); }

		bool			_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Layer::_childRequestFocus(pSlot, pWidget); }
		bool			_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Layer::_childReleaseFocus(pSlot, pWidget); }

		void			_childRequestInView(BasicSlot * pSlot) override { return Layer::_childRequestInView(pSlot); }
		void			_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Layer::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		Widget *		_prevChild(const BasicSlot * pSlot) const { return Layer::_prevChild(pSlot); }
		Widget *		_nextChild(const BasicSlot * pSlot) const { return Layer::_nextChild(pSlot); }

		// Only base slot can have child replaced, we should throw something later...
		void			_replaceChild(BasicSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize(BasicSlot * pSlot) override;
		void			_releaseChild(BasicSlot * pSlot) override;

		// Overloaded from Layer

		const LayerSlot * 	_beginLayerSlots() const override;
		const LayerSlot * 	_endLayerSlots() const override;
		int				_sizeOfLayerSlot() const override;

		void			_onRequestRender(const RectI& rect, const LayerSlot * pSlot) override;	// rect is in our coordinate system.

		// Overloaded from container

//		void			_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode);
//		void			_collectPatches(Patches& container, const RectI& geo, const RectI& clip);


		// Overloaded from Widget

		void            _render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_resize( const SizeI& size ) override;
		void			_receive( Msg * pMsg ) override;

		//

		class MainSlotAccess : public CMainSlot { friend class PopupLayer; };
		MainSlotAccess * _mainSlot() { return static_cast<MainSlotAccess*>(&mainSlot); }
		const MainSlotAccess * _mainSlot() const { return static_cast<const MainSlotAccess*>(&mainSlot); }


		SlotArray<PopupSlot>m_popups;		// First popup lies at the bottom.
		Widget_wp			m_pKeyFocus;	// Pointer at child that held focus before any menu was opened.

		RouteId			m_tickRouteId = 0;

		int				m_openingDelayMs = 100;
		int				m_openingFadeMs = 100;
		int				m_closingDelayMs = 200;
		int				m_closingFadeMs = 200;
	};


} // namespace wg
#endif //WG_POPUPLAYER_DOT_H
