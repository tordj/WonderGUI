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



#include <wg_panel.h>
#include <wg_cstandardslot.h>
#include <wg_cscrollbar.h>

namespace wg
{

	class ScrollPanel;
	typedef	StrongPtr<ScrollPanel>	ScrollPanel_p;
	typedef	WeakPtr<ScrollPanel>	ScrollPanel_wp;


	//____ ScrollPanel ________________________________________________________

	class ScrollPanel : public Panel, protected CScrollbar::Holder
	{
	public:

		//.____ Creation __________________________________________

		static ScrollPanel_p	create() { return ScrollPanel_p(new ScrollPanel()); }

		//.____ Components _______________________________________

		CStandardSlot	slot;
		CScrollbar		scrollbarX;
		CScrollbar		scrollbarY;

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc ____________________________________________________

		void			setPlacement(Placement placement);

		bool			setSizeConstraints(SizeConstraint width, SizeConstraint height);

		void			setAutohideScrollbars(bool scrollbarX, bool scrollbarY);
		void			setOverlayScrollbars(bool scrollbarX, bool scrollbarY);

		void			setStepSize(pts stepSize);
		void			setStepSize(pts x, pts y);
		void			setWheelStepSize(pts stepSize);
		void			setWheelStepSize(pts x, pts y);
		void			setPageOverlap(pts overlap);
		void			setPageOverlap(pts x, pts y);

		void			setScrollWheels(int wheelForX, int wheelForY);
		void			setScrollWheelAxisShift(ModifierKeys axisShift);
		void			setStealWheelFromScrollbars(bool bSteal);

		//void			setDragCombo(MouseButton button, ModifierKeys modkeys); // NOT POSSIBLE YET, NEEDS TO BE ABLE TO INTERCEPT MESSAGES.

	protected:
		ScrollPanel();
		virtual ~ScrollPanel();
		virtual Widget* _newOfMyType() const override { return new ScrollPanel(); };

		void		_updateRegions();

		void		_childWindowCorrection();

		void		_updateCanvasSize();
		void		_updateScrollbars();

		SizeSPX		_calcCanvasSize( SizeSPX viewMinSize, SizeSPX viewMaxSize );


		// Overloaded from Widget

		virtual void _resize(const SizeSPX& size, int scale) override;

		void		_receive(Msg * pMsg) override;
		void		_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void		_collectPatches(Patches& container, const RectSPX& geo, const RectSPX& clip) override;
		void		_maskPatches(Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode) override;

		bool		_alphaTest(const CoordSPX& ofs) override;
		void		_cloneContent(const Widget * _pOrg) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_findWidget(const CoordSPX& pos, SearchMode mode) override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordSPX	_childPos(const StaticSlot * pSlot) const override;
		RectSPX		_childWindowSection(const StaticSlot * pSlot) const override;


		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		void		_childRequestInView(StaticSlot * pSlot) override;
		void		_childRequestInView(StaticSlot * pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pWidget) override;

		//

		CoordSPX		_componentPos(const GeoComponent* pComponent) const override;
		SizeSPX			_componentSize(const GeoComponent* pComponent) const override;
		RectSPX			_componentGeo(const GeoComponent* pComponent) const override;



		// Needed for CScrollbar

		void	_scrollbarStep(const CScrollbar* pComponent, int dir) override;
		void	_scrollbarPage(const CScrollbar* pComponent, int dir) override;
		void	_scrollbarWheel(const CScrollbar* pComponent, int dir) override;
		spx		_scrollbarMove(const CScrollbar* pComponent, spx pos) override;
		std::tuple<spx, spx, spx> _scrollbarOfsLenContent(const CScrollbar* pComponent) override;

		RectSPX			m_viewRegion;
		RectSPX			m_scrollbarXRegion;
		RectSPX			m_scrollbarYRegion;

		RectSPX			m_childCanvas;								// Child canvas in our coordinate system
		RectSPX			m_childWindow;								// Child view in our coordinate system.

		Placement		m_smallChildPlacement = Placement::NorthWest;	// Child placement within view if smaller than view.
		SizeConstraint	m_widthConstraint = SizeConstraint::None;		// Constraint of child width.
		SizeConstraint	m_heightConstraint = SizeConstraint::None;		// Constraint of child height.

		bool			m_bAutohideScrollbarX = true;
		bool			m_bAutohideScrollbarY = true;

		bool			m_bOverlayScrollbarX = false;
		bool			m_bOverlayScrollbarY = false;

		spx				m_viewXOfs, m_viewXLen, m_canvasXLen;
		spx				m_viewYOfs, m_viewYLen, m_canvasYLen;

		pts				m_stepSizeX = 8, m_stepSizeY = 8;
		pts				m_wheelStepSizeX = 8*3, m_wheelStepSizeY = 8*3;
		pts				m_pageOverlapX = 8, m_pageOverlapY = 8;

		int				m_wheelForScrollX = 2;		// What wheel should be used for horizontal scrolling. (0=none)
		int				m_wheelForScrollY = 1;		// What wheel should be used for vertical scrolling. (0=none)
		ModifierKeys	m_wheelAxisShiftCombo = ModifierKeys::MODKEY_ALT;
		bool			m_bStealWheelFromScrollbars = false;


//		bool		m_bOverlayScrollbars;


	};


} // namespace wg
#endif //WG_SCROLLPANEL_DOT_H

