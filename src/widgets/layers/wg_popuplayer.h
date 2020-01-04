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
#include <wg_cstaticslotarray.h>

#include <vector>

namespace wg
{

	//____ PopupSlot ___________________________________________________________

	class PopupSlot : public LayerSlot
	{
		friend class PopupLayer;
		friend class CPopupSlots;
		friend class CStaticSlotArray<PopupSlot>;
		friend class CStaticSlotArray<PopupSlot>::Holder;

	public:
		const static bool safe_to_relocate = false;

	protected:
		class Holder : public LayerSlot::Holder
		{
		};

		PopupSlot(Holder * pHolder) : LayerSlot(pHolder) {}

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

		RectI		m_launcherGeo;		// Launcher geo relative sibling or parent.
		Origo		m_attachPoint = Origo::NorthWest;
		bool		m_bAutoClose;		// Has been opened in auto-close mode.
		State		m_state;
		int			m_stateCounter;		// Counts millisec the slot has been in a transitative state (Delay, Opening, Coundown and Closing).
		SizeI		m_maxSize = { INT_MAX, INT_MAX };
		Widget_wp	m_pOpener;			// Widget that opened this popup.
		Widget_wp	m_pKeyFocus;		// Pointer at widget that held focus when this popup was ontop.
	};


	class CPopupSlots;
	typedef	StrongComponentPtr<CPopupSlots>	CPopupSlots_p;
	typedef	WeakComponentPtr<CPopupSlots>	CPopupSlots_wp;

	//____ CPopupSlots ________________________________________________________

class CPopupSlots : public CStaticSlotArray<PopupSlot>
	{
		friend class PopupLayer;
		
	public:
		
		//.____ Misc __________________________________________________________

		inline CPopupSlots_p	ptr() { return CPopupSlots_p(this); }

		//.____ Content _______________________________________________________
		
		void	pushFront(const Widget_p& pPopup, Widget * pOpener, const Rect& launcherGeo, Origo attachPoint = Origo::NorthEast, bool bAutoClose = false, Size maxSize = Size(INT_MAX>>8, INT_MAX>>8));
		void	pop(int nb = 1);
		void	pop(Widget * pPopup);
		void	clear();

	protected:
		class Holder : public CStaticSlotArray<PopupSlot>::Holder
		{
		public:
			virtual void		_removeSlots(int ofs, int nb) = 0;
			virtual void		_addSlot(Widget * pPopup, Widget * pOpener, const RectI& launcherGeo, Origo attachPoint, bool bAutoClose, SizeI maxSize) = 0;
		};

		CPopupSlots(Holder * pHolder) : CStaticSlotArray<PopupSlot>(pHolder) {}

		const Holder *	_holder() const { return static_cast<const Holder*>(CStaticSlotArray<PopupSlot>::_holder()); }
		Holder *	_holder() { return static_cast<Holder*>(CStaticSlotArray<PopupSlot>::_holder()); }
	};


	class PopupLayer;
	typedef	StrongPtr<PopupLayer>	PopupLayer_p;
	typedef	WeakPtr<PopupLayer>	PopupLayer_wp;

	//____ PopupLayer ____________________________________________________________

	class PopupLayer : public Layer, protected CPopupSlots::Holder
	{
		friend class CPopupSlots;

	public:

		//.____ Creation __________________________________________

		static PopupLayer_p	create() { return PopupLayer_p(new PopupLayer()); }

		//.____ Components _______________________________________

		CPopupSlots	popupSlots;

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

		// Only base slot can have child replaced, we should throw something later...
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize(StaticSlot * pSlot) override;
		void			_releaseChild(StaticSlot * pSlot) override;

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

		class MainSlotAccess : public CStandardSlot { friend class PopupLayer; };
		MainSlotAccess * _mainSlot() { return static_cast<MainSlotAccess*>(&mainSlot); }
		const MainSlotAccess * _mainSlot() const { return static_cast<const MainSlotAccess*>(&mainSlot); }

		Widget_wp			m_pKeyFocus;	// Pointer at child that held focus before any menu was opened.

		RouteId			m_tickRouteId = 0;

		int				m_openingDelayMs = 100;
		int				m_openingFadeMs = 100;
		int				m_closingDelayMs = 200;
		int				m_closingFadeMs = 200;
	};


} // namespace wg
#endif //WG_POPUPLAYER_DOT_H
