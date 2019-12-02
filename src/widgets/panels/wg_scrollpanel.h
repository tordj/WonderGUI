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
#include <wg_islot.h>
#include <wg_paddedslot.h>


namespace wg
{

	class ScrollPanel;
	typedef	StrongPtr<ScrollPanel>	ScrollPanel_p;
	typedef	WeakPtr<ScrollPanel>	ScrollPanel_wp;

	//____ ScrollbarSlot ______________________________________________________

	class ScrollbarSlot : public PaddedSlot		/** @private */
	{
		friend class ScrollbarEntry;
		friend class ScrollPanel;
	public:
		ScrollbarSlot() : PaddedSlot(nullptr) {}
	protected:
		RectI			geo;
		bool			bAutoHide = false;
		bool			bAutoScroll = false;
		Direction		placement;
	};

	//____ ViewSlotHolder ______________________________________________________

	class ViewSlotHolder : public PaddedSlotHolder
	{

	};


	//____ ViewSlot ______________________________________________________

	class ViewSlot : public PaddedSlot		/** @private */
	{
		friend class ScrollPanel;
	public:
		ViewSlot(ViewSlotHolder *pHolder) : PaddedSlot(pHolder) {}

		int			paddedWindowPixelLenX();				// Width of view after childs window padding has been applied.
		int			paddedWindowPixelLenY();				// Height of view after childs window padding has been applied.
		float		paddedWindowLenX();
		float		paddedWindowLenY();

		float		windowFractionX() const;
		float		windowFractionY() const;
		float		windowOffsetX() const;
		float		windowOffsetY() const;

		bool		updateCanvasGeo();
		SizeI		sizeFromPolicy(SizeI specifiedSize) const;

		bool		setWindowPos(CoordI pos);
		bool		setWindowOffset(CoordF ofs);
		bool		setWindowOffsetX(float ofs);
		bool		setWindowOffsetY(float ofs);

		RectI			windowGeo;		// Geometry of Widgets window inside parent.
		RectI			canvasGeo;		// Geometry of Widgets canvas.

		SizePolicy		widthPolicy = SizePolicy::Default;
		SizePolicy		heightPolicy = SizePolicy::Default;
		Origo			contentOrigo = Origo::NorthWest;		// Origo when content is smaller than window
		SizeI			contentSize;
		CoordI			viewPixOfs;

		BorderI			scrollBorder;
		BorderI			rubberBorder;
		MouseButton		dragButton = MouseButton::None;



	};

	//____ IViewSlotHolder _______________________________________________

	class IViewSlotHolder : public SlotHolder /** @private */
	{
	public:
		virtual bool		_setWindowPos(CoordI pos) = 0;
		virtual bool		_setWindowOffset(CoordF ofs) = 0;
		virtual bool		_step(Direction dir, int nSteps = 1) = 0;
		virtual bool		_jump(Direction dir, int nJumps = 1) = 0;
		virtual void		_requestRender(const RectI& rect) = 0;
		virtual void		_updateViewGeo() = 0;
	};


	//____ IViewSlot ______________________________________________________

	class IViewSlot : public ISlot
	{
	public:
		/** @private */

		IViewSlot(ViewSlot * pSlot, IViewSlotHolder * pHolder) : ISlot(pSlot, pHolder) {}

		//.____ Operators _____________________________________________________

		IViewSlot operator=(Widget * pWidget);

		//.____ Geometry ______________________________________________________

		Size		canvasSize() const { return _slot()->contentSize; };

		void		setOrigo(Origo origo);
		Origo		origo() const { return _slot()->contentOrigo; }

		void		setWidthPolicy(SizePolicy policy);
		SizePolicy	widthPolicy() const { return _slot()->widthPolicy; }

		void		setHeightPolicy(SizePolicy policy);
		SizePolicy	heightPolicy() const { return _slot()->heightPolicy; }

		Rect		windowRect() const;
		Coord		windowPos() const { return _slot()->viewPixOfs; };
		Size		windowSize() const;

		RectF		windowSection() const;
		CoordF		windowOffset() const;
		SizeF		windowFraction() const;

		bool		setWindowPos( Coord pos );
		bool		setWindowOffset( CoordF ofs);

		bool		setWindowFocus(Origo canvasOrigo, Coord canvasOfs, Origo viewOrigo = Origo::Center, Coord viewOfs = { 0,0 });

		//.____ Control _______________________________________________________

		bool		step( Direction dir );
		bool		jump( Direction dir );

		void		setScrollBorder(Border border);

		void		setRubberBorder(Border border);

		void		setDragButton(MouseButton button);

	private:

		inline IViewSlotHolder * _holder() { return static_cast<IViewSlotHolder*>(m_pHolder); }
		inline const IViewSlotHolder * _holder() const { return static_cast<IViewSlotHolder*>(m_pHolder); }

		inline ViewSlot * _slot() { return static_cast<ViewSlot*>(m_pSlot); }
		inline const ViewSlot * _slot() const { return static_cast<ViewSlot*>(m_pSlot); }

	};


	//____ ScrollbarEntry ______________________________________________________

	class ScrollbarEntry : public Interface
	{
	public:
		/** @private */

		ScrollbarEntry(ScrollbarSlot * pSlot, ScrollPanel * pPanel) : m_pSlot(pSlot), m_pHolder(pPanel) {}

		//.____ State _________________________________________________________

		bool		isVisible() const { return m_pSlot->bVisible; }

		//.____ Behavior ______________________________________________________

		void		setAutoHide(bool autohide);
		bool		autoHide() const { return m_pSlot->bAutoHide; }

		bool		setAutoScroll(bool bAuto);
		bool		autoScroll() const { return m_pSlot->bAutoScroll; }

		//.____ Geometry ______________________________________________________

		bool		setPlacement( Direction );
		Direction	placement() const { return m_pSlot->placement; }


		//.____ Content _______________________________________________________

		inline Widget_p get() const { return m_pSlot->widget(); }
		void clear();

		//.____ Operators _____________________________________________________

		ScrollbarEntry operator=(Scrollbar * pWidget);
		inline operator Scrollbar_p() const { return Scrollbar_p((Scrollbar*)m_pSlot->_widget()); }

		inline bool operator==(Widget * other) const { return other == m_pSlot->_widget(); }
		inline bool operator!=(Widget * other) const { return other != m_pSlot->_widget(); }
		 
		inline operator bool() const { return m_pSlot->_widget() != nullptr; }

		//		inline Widget& operator*() const{ return * m_pSlotCan->pWidget; };
		inline Widget* operator->() const { return m_pSlot->_widget(); }


	protected:
		Object * _object() const override;

		ScrollbarSlot *	m_pSlot;
		ScrollPanel * m_pHolder;

	};


	//____ ScrollPanel ________________________________________________________

	class ScrollPanel : public Panel, protected IViewSlotHolder, protected ViewSlotHolder
	{
		friend class IViewSlot;
		friend class ScrollbarEntry;
	public:

		//.____ Creation __________________________________________

		static ScrollPanel_p	create() { return ScrollPanel_p(new ScrollPanel()); }

		//.____ Interfaces _______________________________________

		IViewSlot		view;
		ScrollbarEntry	hscrollbar, vscrollbar;

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const override;
		const char *			className(void) const override;
		static const char		CLASSNAME[];
		static ScrollPanel_p	cast(Object * pObject);

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

		// Overloaded from SlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }

		CoordI		_childGlobalPos(BasicSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(BasicSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }

		bool		_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		CoordI		_childPos(BasicSlot * pSlot) const override;

		void		_childRequestRender(BasicSlot * pSlot) override;
		void		_childRequestRender(BasicSlot * pSlot, const RectI& rect) override;
		void		_childRequestResize(BasicSlot * pSlot) override;

		void		_childRequestInView(BasicSlot * pSlot) override;
		void		_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override;


		Widget *	_prevChild(const BasicSlot * pSlot) const override;
		Widget *	_nextChild(const BasicSlot * pSlot) const override;

		RectI		_childWindowSection(BasicSlot * pSlot) const override;

		void		_releaseChild(BasicSlot * pSlot) override;
		void		_replaceChild(BasicSlot * pSlot, Widget * pWidget) override;

		// Overloaded from ChildHolder

		inline void		_requestRender(const RectI& rect) override { Panel::_requestRender(rect); }
		inline void		_requestRender() { Panel::_requestRender(); }
		virtual void	_updateViewGeo() override { _updateElementGeo(m_size); }

		// Overloaded from Container

		Widget *	_findWidget(const CoordI& pos, SearchMode mode) override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

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
		void		_updateElementGeo(SizeI mySize, BasicSlot * pForceUpdate = nullptr );
		bool		_setWindowPos(CoordI pos) override;
		bool		_setWindowOffset(CoordF ofs) override;


		bool		_step(Direction dir, int nSteps = 1) override;
		bool		_jump(Direction dir, int nJumps = 1) override;
		bool		_wheelRoll(Direction dir, int nSteps = 1);

		int			_defaultStepFunction(Direction dir, int steps);
		int			_defaultJumpFunction(Direction dir, int steps);


		std::function<int(Direction dir, int steps)> m_pStepFunction;
		std::function<int(Direction dir, int steps)> m_pJumpFunction;
		std::function<int(Direction dir, int steps)> m_pWheelRollFunction;


		ViewSlot	m_viewSlot;
		ScrollbarSlot	m_scrollbarSlots[2];			// xScroll and yScroll widgets in that order.

		Skin_p		m_pCornerSkin;
		RectI		m_cornerGeo;

		int			m_wheelForScroll;					// What wheel should be used for scrolling. (0=none)
		bool		m_bOverlayScrollbars;

		MyScrollbarTarget	m_scrollbarTargets[2];		// Order: Vertical, horizontal

	};


} // namespace wg
#endif //WG_SCROLLPANEL_DOT_H

