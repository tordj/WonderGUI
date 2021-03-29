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
#include <wg_cstaticslotvector.h>

#include <vector>

namespace wg
{


	class PopupLayer;
	typedef	StrongPtr<PopupLayer>	PopupLayer_p;
	typedef	WeakPtr<PopupLayer>	PopupLayer_wp;

	//____ PopupLayer ____________________________________________________________

	class PopupLayer : public Layer
	{
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ___________________________________________________________

		class Slot : public Layer::Slot
		{
			friend class PopupLayer;
			friend class CSlots;
			friend class CStaticSlotVector<Slot>;

		public:
			const static bool safe_to_relocate = false;

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

		protected:

			Slot(SlotHolder * pHolder) : Layer::Slot(pHolder) {}

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

			Rect		m_launcherGeo;		// Launcher geo relative sibling or parent.
			Placement		m_attachPoint = Placement::NorthWest;
			bool		m_bAutoClose;		// Has been opened in auto-close mode.
			State		m_state;
			int			m_stateCounter;		// Counts millisec the slot has been in a transitative state (Delay, Opening, Coundown and Closing).
			Size		m_maxSize = { MU::Max, MU::Max };
			Widget_wp	m_pOpener;			// Widget that opened this popup.
			Widget_wp	m_pKeyFocus;		// Pointer at widget that held focus when this popup was ontop.
		};


		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CPopupSlots_p;
		typedef	WeakComponentPtr<CSlots>	CPopupSlots_wp;

		//____ CSlots ________________________________________________________

		class CSlots : public CStaticSlotVector<Slot>
		{
			friend class PopupLayer;

		public:

			//.____ Misc __________________________________________________________

			inline CPopupSlots_p	ptr() { return CPopupSlots_p(this); }

			//.____ Content _______________________________________________________

			void	pushFront(const Widget_p& pPopup, Widget * pOpener, const Rect& launcherGeo, Placement attachPoint = Placement::NorthEast, bool bAutoClose = false, Size maxSize = Size(MU::Max, MU::Max));
			void	pop(int nb = 1);
			void	pop(Widget * pPopup);
			void	clear();

		protected:

			CSlots(SlotHolder * pHolder) : CStaticSlotVector<Slot>(pHolder) {}

			const PopupLayer *	_holder() const { return static_cast<const PopupLayer*>(CStaticSlotVector<Slot>::_holder()); }
			PopupLayer *	_holder() { return static_cast<PopupLayer*>(CStaticSlotVector<Slot>::_holder()); }
		};


		//.____ Creation __________________________________________

		static PopupLayer_p	create() { return PopupLayer_p(new PopupLayer()); }

		//.____ Components _______________________________________

		CSlots	popupSlots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		PopupLayer();
		virtual ~PopupLayer();
		virtual Widget* _newOfMyType() const override { return new PopupLayer(); };

		PopupLayer *	_getPopupLayer() const override { return const_cast<PopupLayer*>(this); }

		void			_stealKeyboardFocus();
		void			_restoreKeyboardFocus();
		void			_updateGeo(Slot * pSlot, bool bForceResize = false);

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *		_findWidget( const Coord& ofs, SearchMode mode ) override;


		// Only base slot can have child replaced, we need error handling...
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize(StaticSlot * pSlot) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		// Overloaded from Layer

		const Layer::Slot * _beginLayerSlots() const override;
		const Layer::Slot * _endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		void				_onRequestRender(const Rect& rect, const Layer::Slot * pSlot) override;	// rect is in our coordinate system.

		// Overloaded from container

//		void			_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode);
//		void			_collectPatches(Patches& container, const RectI& geo, const RectI& clip);


		// Overloaded from Widget

		void            _render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_resize( const Size& size ) override;
		void			_receive( Msg * pMsg ) override;
		void			_update(int microPassed, int64_t microsecTimestamp) override;


		// Needed by CSlots

		void			_removeSlots(int ofs, int nb);
		void			_addSlot(Widget * pPopup, Widget * pOpener, const Rect& launcherGeo, Placement attachPoint, bool bAutoClose, Size maxSize);

		Widget_wp			m_pKeyFocus;	// Pointer at child that held focus before any menu was opened.

		int				m_openingDelayMs = 100;
		int				m_openingFadeMs = 100;
		int				m_closingDelayMs = 200;
		int				m_closingFadeMs = 200;
	};


} // namespace wg
#endif //WG_POPUPLAYER_DOT_H
