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

#ifndef	WG_SCROLLPANEL_DOT_H
#define	WG_SCROLLPANEL_DOT_H
#pragma once


#include <functional>

#include <wg_panel.h>
#include <wg_scrollbar.h>
#include <wg_scrollbartarget.h>
#include <wg_cslot.h>
#include <wg_slot.h>


namespace wg
{

	class ScrollPanel;
	typedef	StrongPtr<ScrollPanel>	ScrollPanel_p;
	typedef	WeakPtr<ScrollPanel>	ScrollPanel_wp;


	//____ ScrollPanel ________________________________________________________

	class ScrollPanel : public Panel
	{
		friend class CViewSlot;
		friend class ScrollbarEntry;
	public:

		//____ ScrollbarSlot ______________________________________________________

		class ScrollbarSlot : public DynamicSlot
		{
			friend class ScrollbarEntry;
			friend class ScrollPanel;
		public:
		protected:
			ScrollbarSlot() : DynamicSlot(nullptr) {}

			SizeI		_paddedPreferredSize() const { return _preferredSize() + m_padding; }
			SizeI		_paddedMinSize() const { return _minSize() + m_padding; }
			SizeI		_paddedMaxSize() const { return _maxSize() + m_padding; }
			int			_paddedMatchingWidth(int paddedHeight) const { return _matchingWidth(paddedHeight - m_padding.height()) + m_padding.width(); }
			int			_paddedMatchingHeight(int paddedWidth) const { return _matchingHeight(paddedWidth - m_padding.width()) + m_padding.height(); }

			RectI			geo;
			bool			bAutoHide = false;
			bool			bAutoScroll = false;
			Direction		placement;

			BorderI		m_padding;
			bool		m_bVisible;
		};



		//____ ViewSlot ______________________________________________________

		class ViewSlot : public DynamicSlot
		{
			friend class ScrollPanel;
		public:

		protected:

			ViewSlot(SlotHolder *pHolder) : DynamicSlot(pHolder) {}


			int			_paddedWindowPixelLenX();				// Width of view after childs window padding has been applied.
			int			_paddedWindowPixelLenY();				// Height of view after childs window padding has been applied.
			float		_paddedWindowLenX();
			float		_paddedWindowLenY();

			float		_windowFractionX() const;
			float		_windowFractionY() const;
			float		_windowOffsetX() const;
			float		_windowOffsetY() const;

			bool		_updateCanvasGeo();
			SizeI		_sizeFromPolicy(SizeI specifiedSize) const;

			bool		_setWindowPos(CoordI pos);
			bool		_setWindowOffset(CoordF ofs);
			bool		_setWindowOffsetX(float ofs);
			bool		_setWindowOffsetY(float ofs);

			RectI			m_windowGeo;		// Geometry of Widgets window inside parent.
			RectI			m_canvasGeo;		// Geometry of Widgets canvas.

			SizePolicy		m_widthPolicy = SizePolicy::Default;
			SizePolicy		m_heightPolicy = SizePolicy::Default;
			Origo			m_contentOrigo = Origo::NorthWest;		// Origo when content is smaller than window
			SizeI			m_contentSize;
			CoordI			m_viewPixOfs;

			BorderI			m_scrollBorder;
			BorderI			m_rubberBorder;
			MouseButton		m_dragButton = MouseButton::None;
		};



		class CViewSlot;
		typedef	StrongComponentPtr<CViewSlot>	CViewSlot_p;
		typedef	WeakComponentPtr<CViewSlot>		CViewSlot_wp;


		//____ CViewSlot ______________________________________________________

		class CViewSlot : public CDynamicSlotImpl<ViewSlot>
		{
			friend class ScrollPanel;

		public:

			//.____ Geometry ______________________________________________________

			Size		canvasSize() const { return m_contentSize; };

			void		setOrigo(Origo origo);
			Origo		origo() const { return m_contentOrigo; }

			void		setWidthPolicy(SizePolicy policy);
			SizePolicy	widthPolicy() const { return m_widthPolicy; }

			void		setHeightPolicy(SizePolicy policy);
			SizePolicy	heightPolicy() const { return m_heightPolicy; }

			Rect		windowRect() const;
			Coord		windowPos() const { return m_viewPixOfs; };
			Size		windowSize() const;

			RectF		windowSection() const;
			CoordF		windowOffset() const;
			SizeF		windowFraction() const;

			bool		setWindowPos(Coord pos);
			bool		setWindowOffset(CoordF ofs);

			bool		setWindowFocus(Origo canvasOrigo, Coord canvasOfs, Origo viewOrigo = Origo::Center, Coord viewOfs = { 0,0 });

			//.____ Control _______________________________________________________

			bool		step(Direction dir);
			bool		jump(Direction dir);

			void		setScrollBorder(Border border);

			void		setRubberBorder(Border border);

			void		setDragButton(MouseButton button);

			//.____ Misc __________________________________________________________

			inline CViewSlot_p	ptr() { return CViewSlot_p(this); }

			//.____ Operators _____________________________________________________

			CViewSlot& operator=(const Widget_p& pWidget);

		private:


			CViewSlot(SlotHolder * pHolder) : CDynamicSlotImpl(pHolder) {}

			inline ScrollPanel * _holder() { return static_cast<ScrollPanel*>(m_pHolder); }
			inline const Holder * _holder() const { return static_cast<ScrollPanel*>(m_pHolder); }

		};


		//____ ScrollbarEntry ______________________________________________________

		class ScrollbarEntry : public Component
		{
		public:
			/** @private */

			ScrollbarEntry(ScrollbarSlot * pSlot, ScrollPanel * pPanel) : m_pSlot(pSlot), m_pHolder(pPanel) {}

			//.____ State _________________________________________________________

			bool		isVisible() const { return m_pSlot->m_bVisible; }

			//.____ Behavior ______________________________________________________

			void		setAutoHide(bool autohide);
			bool		autoHide() const { return m_pSlot->bAutoHide; }

			bool		setAutoScroll(bool bAuto);
			bool		autoScroll() const { return m_pSlot->bAutoScroll; }

			//.____ Geometry ______________________________________________________

			bool		setPlacement(Direction);
			Direction	placement() const { return m_pSlot->placement; }


			//.____ Content _______________________________________________________

			inline Widget_p get() const { return m_pSlot->widget(); }
			void clear();

			//.____ Operators _____________________________________________________

			ScrollbarEntry operator=(const Scrollbar_p& pWidget);
			inline operator Scrollbar_p() const { return Scrollbar_p((Scrollbar*)m_pSlot->_widget()); }

			inline bool operator==(Widget * other) const { return other == m_pSlot->_widget(); }
			inline bool operator!=(Widget * other) const { return other != m_pSlot->_widget(); }

			inline operator bool() const { return m_pSlot->_widget() != nullptr; }

			//		inline Widget& operator*() const{ return * m_pSlotCan->pWidget; };
			inline Widget* operator->() const { return m_pSlot->_widget(); }


		protected:
			Object * _object() override;
			const Object * _object() const override;

			ScrollbarSlot *	m_pSlot;
			ScrollPanel * m_pHolder;

		};


		//.____ Creation __________________________________________

		static ScrollPanel_p	create() { return ScrollPanel_p(new ScrollPanel()); }

		//.____ Components _______________________________________

		CViewSlot		viewSlot;
		ScrollbarEntry	hscrollbar, vscrollbar;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;
		/*
		NEED TO BE IMPLEMENTED!!!

		int		matchingHeight( int width ) const;	//
		int		matchingWidth( int height ) const;

		*/

		//.____ Behavior ________________________________________________________

		bool		setScrollWheel(int wheel);
		int			scrollWheel() const { return m_wheelForScroll; }

		void		setOverlayScrollbars(bool bOverlay);
		bool		overlayScrollbars() const { return m_bOverlayScrollbars; }

		void		setStepFunc(std::function<int (Direction,int steps)> func );
		void		setJumpFunc(std::function<int (Direction, int steps)> func);
		void		setWheelRollFunc(std::function<int(Direction, int steps)> func);

		//.____ Appearance _________________________________________________

		void		setCornerSkin(Skin * pSkin);
		Skin_p		cornerSkin() const { return m_pCornerSkin; }

		virtual void setSkin(Skin * pSkin) override;

		//.____ Misc _________________________________________________________________

		/*
		bool	viewCenterOnPixel( int x, int y );
		bool	viewCenterOnPixelX( int x );
		bool	viewCenterOnPixelY( int y );

		bool	viewInclude( RectI& rect );
		bool	viewIncludeX( RectI& rect );
		bool	viewIncludeY( RectI& rect );
		*/




	protected:
		ScrollPanel();
		virtual ~ScrollPanel();
		virtual Widget* _newOfMyType() const override { return new ScrollPanel(); };

		// Overloaded from Widget

		virtual void _resize(const SizeI& size) override;

		SizeI		_preferredSize() const override;				// = preferred size of scrollbars in the geometry, fixed value if scrollbars are missing.

		void		_receive(Msg * pMsg) override;
		void		_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window) override;
		void		_collectPatches(Patches& container, const RectI& geo, const RectI& clip) override;
		void		_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode) override;

		bool		_alphaTest(const CoordI& ofs) override;
		void		_cloneContent(const Widget * _pOrg) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_findWidget(const CoordI& pos, SearchMode mode) override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordI		_childPos(const StaticSlot * pSlot) const override;
		RectI		_childWindowSection(const StaticSlot * pSlot) const override;


		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectI& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		void		_childRequestInView(StaticSlot * pSlot) override;
		void		_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pWidget) override;



		// Internal

		class MyScrollbarTarget : public ScrollbarTarget
		{
		public:
			float		_stepFwd() override;
			float		_stepBwd() override;
			float		_jumpFwd() override;
			float		_jumpBwd() override;
			float		_wheelRolled(int distance) override;

			float		_setPosition(float fraction) override;

			Widget*		_getWidget() override;
			float		_getHandlePosition() override;
			float		_getHandleSize() override;

			void		_updateScrollbar(float pos, float size) { ScrollbarTarget::_updateScrollbar(pos, size); }

			bool		m_bHorizontal;
			ScrollPanel *	m_pParent;
		};


		SizeI		_calcContentSize(SizeI mySize);
		void		_updateElementGeo(SizeI mySize, StaticSlot * pForceUpdate = nullptr );

		void		_updateViewGeo() { _updateElementGeo(m_size); }

		bool		_setWindowPos(CoordI pos);
		bool		_setWindowOffset(CoordF ofs);


		bool		_step(Direction dir, int nSteps = 1);
		bool		_jump(Direction dir, int nJumps = 1);
		bool		_wheelRoll(Direction dir, int nSteps = 1);

		int			_defaultStepFunction(Direction dir, int steps);
		int			_defaultJumpFunction(Direction dir, int steps);


		std::function<int(Direction dir, int steps)> m_pStepFunction;
		std::function<int(Direction dir, int steps)> m_pJumpFunction;
		std::function<int(Direction dir, int steps)> m_pWheelRollFunction;


		ScrollbarSlot	m_scrollbarSlots[2];			// xScroll and yScroll widgets in that order.

		Skin_p		m_pCornerSkin;
		RectI		m_cornerGeo;

		int			m_wheelForScroll;					// What wheel should be used for scrolling. (0=none)
		bool		m_bOverlayScrollbars;

		MyScrollbarTarget	m_scrollbarTargets[2];		// Order: Vertical, horizontal

	};


} // namespace wg
#endif //WG_SCROLLPANEL_DOT_H

