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
	public:
		PopupSlot() : attachPoint(Origo::NorthWest), maxSize(INT_MAX,INT_MAX) {}

		const static bool safe_to_relocate = false;

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

		Rect		launcherGeo;		// Launcher geo relative sibling or parent.
		Origo		attachPoint;
		bool		bAutoClose;			// Has been opened in auto-close mode.
		State		state;
		int			stateCounter;		// Counts millisec the slot has been in a transitative state (Delay, Opening, Coundown and Closing).
		Size		maxSize;
		Widget_wp	pOpener;			// Widget that opened this popup.
		Widget_wp	pKeyFocus;			// Pointer at widget that held focus when this popup was ontop.
	};


	class PopupChildren;
	typedef	StrongInterfacePtr<PopupChildren>	PopupChildren_p;
	typedef	WeakInterfacePtr<PopupChildren>	PopupChildren_wp;

	//____ PopupChildren ________________________________________________________

	class PopupChildren : public Interface
	{
	public:
		/** @private */

		PopupChildren(PopupLayer * pHolder) : m_pHolder(pHolder) {}

		//.____ Misc __________________________________________________________

		inline PopupChildren_p	ptr() { return PopupChildren_p(this); }

		//.____ Operators _____________________________________________________

		Widget& operator[](int index) const;

		//.____ Content _______________________________________________________

		int		size() const;
		void	push(Widget * pPopup, Widget * pOpener, const Rect& launcherGeo, Origo attachPoint = Origo::NorthEast, bool bAutoClose = false, Size maxSize = Size(INT_MAX, INT_MAX));
		void	pop(int nb = 1);
		void	pop(Widget * pPopup);
		void	clear();
		
		Widget_p at(int index) const;

	protected:
		Object *		_object() const;
		PopupLayer *	m_pHolder;
		
	};

	
	//____ PopupLayer ____________________________________________________________
	
	class PopupLayer : public Layer
	{
		friend class PopupChildren;
	
	public:

		//.____ Creation __________________________________________

		static PopupLayer_p	create() { return PopupLayer_p(new PopupLayer()); }

		//.____ Interfaces _______________________________________

		PopupChildren	popups;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static PopupLayer_p	cast( Object * pObject );
	
	
	protected:
		PopupLayer();
		virtual ~PopupLayer();
		virtual Widget* _newOfMyType() const { return new PopupLayer(); };

		PopupLayer *	_getPopupLayer() const { return const_cast<PopupLayer*>(this); }

		void			_stealKeyboardFocus();
		void			_restoreKeyboardFocus();
		bool			_updateGeo(PopupSlot * pSlot, bool bInitialUpdate = false );
		void			_removeSlots(int ofs, int nb);
		void			_addSlot(Widget * pPopup, Widget * pOpener, const Rect& launcherGeo, Origo attachPoint, bool bAutoClose, Size maxSize);

		// Overloaded from Panel

		Widget *		_findWidget( const Coord& ofs, SearchMode mode );
	
		// Overloaded from WidgetHolder

		void			_childRequestResize(Slot * pSlot);

		// Overloaded from Layer

		LayerSlot * 	_beginLayerSlots() const;
		LayerSlot * 	_endLayerSlots() const;
		int				_sizeOfLayerSlot() const;

		void			_onRequestRender(const Rect& rect, const LayerSlot * pSlot);	// rect is in our coordinate system.

		// Overloaded from container

		void			_renderPatches(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches);
//		void			_maskPatches(Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode);
//		void			_collectPatches(Patches& container, const Rect& geo, const Rect& clip);


		// Overloaded from Widget
	
		void			_cloneContent( const Widget * _pOrg );
		void			_setSize( const Size& size );
		void			_receive( Msg * pMsg );
	
	
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
