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
#include <wg_childentry.h>


namespace wg 
{
		
	class ScrollPanel;
	typedef	StrongPtr<ScrollPanel>	ScrollPanel_p;
	typedef	WeakPtr<ScrollPanel>	ScrollPanel_wp;

	//____ ScrollbarSlot ______________________________________________________

	class ScrollbarSlot : public PanelSlot		/** @private */
	{
	public:
		ScrollbarSlot() : bAutoHide(false), bAutoScroll(false) {}

		Rect			geo;
		bool			bAutoHide;
		bool			bAutoScroll;
		Direction		placement;
	};

	//____ ViewSlot ______________________________________________________

	class ViewSlot : public PanelSlot		/** @private */
	{
	public:
		ViewSlot() : widthPolicy(SizePolicy::Default), heightPolicy(SizePolicy::Default), contentOrigo(Origo::NorthWest) {}

		int			paddedWindowPixelLenX();				// Width of view after childs window padding has been applied.
		int			paddedWindowPixelLenY();				// Height of view after childs window padding has been applied.
		float		paddedWindowLenX();
		float		paddedWindowLenY();

		float		windowFractionX() const;
		float		windowFractionY() const;
		float		windowOffsetX() const;
		float		windowOffsetY() const;

		bool		updateCanvasGeo();
		Size		sizeFromPolicy(Size specifiedSize) const;

		bool		setWindowPos(Coord pos);
		bool		setWindowOffset(CoordF ofs);
		bool		setWindowOffsetX(float ofs);
		bool		setWindowOffsetY(float ofs);

		Rect			windowGeo;		// Geometry of Widgets window inside parent.
		Rect			canvasGeo;		// Geometry of Widgets canvas.

		SizePolicy		widthPolicy;
		SizePolicy		heightPolicy;
		Origo			contentOrigo;		// Origo when content is smaller than window

		Size			contentSize;
		Coord			viewPixOfs;
	};

	//____ ViewEntry ______________________________________________________

	class ViewEntry : public ChildEntry<ViewSlot, ScrollPanel>
	{
	public:
		ViewEntry(ViewSlot * pSlot, ScrollPanel * pPanel) : ChildEntry(pSlot, pPanel) {}

		ViewEntry operator=(Widget * pWidget);
		Size		canvasSize() const { return m_pSlot->contentSize; };

		void		setOrigo(Origo origo);
		Origo		origo() const { return m_pSlot->contentOrigo; }

		void		setWidthPolicy(SizePolicy policy);
		SizePolicy	widthPolicy() const { return m_pSlot->widthPolicy; }

		void		setHeightPolicy(SizePolicy policy);
		SizePolicy	heightPolicy() const { return m_pSlot->heightPolicy; }

		Rect		windowRect() const;
		Coord		windowPos() const { return m_pSlot->viewPixOfs; };
		Size		windowSize() const;

		RectF		windowSection() const;
		CoordF		windowOffset() const;
		SizeF		windowFraction() const;

		bool		setWindowPos( Coord pos );
		bool		setWindowOffset( CoordF ofs);

		bool		step( Direction dir );
		bool		jump( Direction dir );
	};

	//____ ScrollbarEntry ______________________________________________________

	class ScrollbarEntry : public Interface
	{
	public:
		ScrollbarEntry(ScrollbarSlot * pSlot, ScrollPanel * pPanel) : m_pSlot(pSlot), m_pHolder(pPanel) {}

		bool		isVisible() const { return m_pSlot->bVisible; }

		void		setAutoHide(bool autohide);
		bool		autoHide() const { return m_pSlot->bAutoHide; }

		bool		setAutoScroll(bool bAuto);
		bool		autoScroll() const { return m_pSlot->bAutoScroll; }

		bool		setPlacement( Direction );
		Direction	placement() const { return m_pSlot->placement; }

		ScrollbarEntry operator=(Scrollbar * pWidget);
		inline operator Scrollbar_p() const { return Scrollbar_p((Scrollbar*)m_pSlot->pWidget); }

		inline bool operator==(Widget * other) const { return other == m_pSlot->pWidget; }
		inline bool operator!=(Widget * other) const { return other != m_pSlot->pWidget; }

		inline operator bool() const { return m_pSlot->pWidget != nullptr; }

		//		inline Widget& operator*() const{ return * m_pSlotCan->pWidget; };
		inline Widget* operator->() const { return m_pSlot->pWidget; }


		inline Widget_p get() const { return Widget_p(m_pSlot->pWidget); }
		void clear();

	protected:
		Object * _object() const;

		ScrollbarSlot *	m_pSlot;
		ScrollPanel * m_pHolder;

	};


	//____ ScrollPanel ________________________________________________________
	
	class ScrollPanel : public Panel, protected ChildEntryHolder
	{
		friend class ViewEntry;
		friend class ScrollbarEntry;
	public:

		//.____ Creation __________________________________________

		static ScrollPanel_p	create() { return ScrollPanel_p(new ScrollPanel()); }

		//.____ Interfaces _______________________________________

		ViewEntry		view;
		ScrollbarEntry	hscrollbar, vscrollbar;

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const;
		const char *			className(void) const;
		static const char		CLASSNAME[];
		static ScrollPanel_p	cast(Object * pObject);

		//.____ Geometry _________________________________________________

		Size				preferredSize() const;				// = preferred size of scrollbars in the geometry, fixed value if scrollbars are missing.

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

		virtual void setSkin(Skin * pSkin);

		//.____ Misc _________________________________________________________________

		/*
		bool	viewCenterOnPixel( int x, int y );
		bool	viewCenterOnPixelX( int x );
		bool	viewCenterOnPixelY( int y );

		bool	viewInclude( Rect& rect );
		bool	viewIncludeX( Rect& rect );
		bool	viewIncludeY( Rect& rect );
		*/




	protected:
		ScrollPanel();
		virtual ~ScrollPanel();
		virtual Widget* _newOfMyType() const { return new ScrollPanel(); };

		// Overloaded from Widget

		virtual void _setSize(const Size& size);

		void		_receive(Msg * pMsg);
		void		_renderPatches(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches);
		void		_collectPatches(Patches& container, const Rect& geo, const Rect& clip);
		void		_maskPatches(Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode);

		bool		_alphaTest(const Coord& ofs);
		void		_cloneContent(const Widget * _pOrg);

		// Overloaded from WidgetHolder

		Coord		_childPos(Slot * pSlot) const;
		Size		_childSize(Slot * pSlot) const;

		void		_childRequestRender(Slot * pSlot);
		void		_childRequestRender(Slot * pSlot, const Rect& rect);
		void		_childRequestResize(Slot * pSlot);

		Widget *	_prevChild(Slot * pSlot) const;
		Widget *	_nextChild(Slot * pSlot) const;

		Rect		_childWindowSection(Slot * pSlot) const;

		// Overloaded from ChildEntryHolder

		void		_setWidget(Slot * pSlot, Widget * pWidget);

		// Overloaded from Container

		Widget *	_findWidget(const Coord& pos, SearchMode mode);

		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstSlotWithGeo(SlotWithGeo& package) const;
		void		_nextSlotWithGeo(SlotWithGeo& package) const;

		// Internal

		class MyScrollbarTarget : public ScrollbarTarget
		{
		public:
			float		_stepFwd();
			float		_stepBwd();
			float		_jumpFwd();
			float		_jumpBwd();
			float		_wheelRolled(int distance);

			float		_setPosition(float fraction);

			Widget*		_getWidget();
			float		_getHandlePosition();
			float		_getHandleSize();

			void		_updateScrollbar(float pos, float size) { ScrollbarTarget::_updateScrollbar(pos, size); }

			bool		m_bHorizontal;
			ScrollPanel *	m_pParent;
		};


		Size		_calcContentSize(Size mySize);
		void		_updateElementGeo(Size mySize);
		bool		_setWindowPos(Coord pos);
		bool		_setWindowOffset(CoordF ofs);


		bool		_step(Direction dir, int nSteps = 1);
		bool		_jump(Direction dir, int nJumps = 1);
		bool		_wheelRoll(Direction dir, int nSteps = 1);

		int			_defaultStepFunction(Direction dir, int steps);
		int			_defaultJumpFunction(Direction dir, int steps);


		std::function<int(Direction dir, int steps)> m_pStepFunction;
		std::function<int(Direction dir, int steps)> m_pJumpFunction;
		std::function<int(Direction dir, int steps)> m_pWheelRollFunction;


		ViewSlot	m_viewSlot;
		ScrollbarSlot	m_scrollbarSlots[2];			// xScroll and yScroll widgets in that order.
	
		Skin_p		m_pCornerSkin;
		Rect		m_cornerGeo;

		int			m_wheelForScroll;					// What wheel should be used for scrolling. (0=none)
		bool		m_bOverlayScrollbars;

		MyScrollbarTarget	m_scrollbarTargets[2];		// Order: Vertical, horizontal
		
	};
	

} // namespace wg
#endif //WG_SCROLLPANEL_DOT_H

