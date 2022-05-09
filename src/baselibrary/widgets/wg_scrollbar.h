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

#ifndef	WG_SCROLLBAR_DOT_H
#define	WG_SCROLLBAR_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_cscrollbar.h>

namespace wg
{


	class Scrollbar;
	typedef	StrongPtr<Scrollbar>		Scrollbar_p;
	typedef	WeakPtr<Scrollbar>		Scrollbar_wp;

	//____ Class: Scrollbar _____________________________________________________
	/**
	 * @brief Scrollbar widget.
	 *
	 * Scrollbar is a normal scrollbar widget with buttons for stepping forward/backward
	 * and a resizable handle that can be dragged to move the view.
	 *
	 * The same scrollbar class is used for vertical and horizontal scrollbars with the axis
	 * setting deciding if scrollbar is displayed vertically or horizontally.
	 *
	 **/

	class	Scrollbar : public Widget, private CScrollbar::Holder
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Axis			axis = Axis::X;
			Object_p		baggage;
			pts				contentLength = 0;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			pts				pageOverlap = 8;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = -1;
			CScrollbar::Blueprint	scrollbar;
			bool			selectable = true;
			pts				singleStepMovement = 8;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
			pts				viewLength = 0;
			pts				viewPos = 0;
			pts				wheelRollMovement = 8 * 3;
		};

		//.____ Creation __________________________________________

		static Scrollbar_p	create() { return Scrollbar_p(new Scrollbar()); }
		static Scrollbar_p	create(const Blueprint& blueprint) { return Scrollbar_p(new Scrollbar(blueprint)); }

		//.____ Components _______________________________________

		CScrollbar	scrollbar;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis( Axis axis );						///< @brief Set scrollbar to vertical or horizontal.
		inline Axis		axis() const; 								///< @brief Check if scrollbar is vertical or horizontal.

		//.____ Control ____________________________________________

		void			setMovementAmounts(pts singleStep, pts wheelRoll, pts pageOverlap);
		void			setView(pts viewPos, pts viewLength, pts contentLength);

		inline pts		viewPos() const;
		inline pts		viewLength() const;
		inline pts		contentLength() const;

		inline float	fracViewPos() const;
		inline float	fracViewLength() const;

	protected:
		Scrollbar();
		Scrollbar(const Blueprint& bp);
		virtual	~Scrollbar();

		virtual SizeSPX	_defaultSize(int scale = -1) const override;

		void	_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void	_receive( Msg * pMsg ) override;
		void	_refresh() override;
		bool	_alphaTest( const CoordSPX& ofs ) override;
		void	_setState( State state ) override;
		void	_setViewPos(pts pos);
		void	_sendMessage();

		void	_scrollbarStep(const CScrollbar* pComponent, int dir) override;
		void	_scrollbarPage(const CScrollbar* pComponent, int dir) override;
		void	_scrollbarWheel(const CScrollbar* pComponent, int dir) override;
		spx		_scrollbarMove(const CScrollbar* pComponent, spx pos) override;
		std::tuple<spx, spx, spx> _scrollbarOfsLenContent(const CScrollbar* pComponent) override;

		pts				m_singleStep = 8;
		pts				m_wheelRoll = 8*3;
		pts				m_pageOverlap = 8;

		pts				m_viewPos = 0;
		pts				m_viewLen = 0;
		pts				m_contentLen = 0;
	};


	//______________________________________________________________________________
	/**
	 * 
	 **/

	pts Scrollbar::viewPos() const
	{
		return m_viewPos;
	}

	//______________________________________________________________________________
	/**
	 *
	 **/

	pts Scrollbar::viewLength() const
	{
		return m_viewLen;
	}

	//______________________________________________________________________________
	/**
	 *
	 **/

	pts Scrollbar::contentLength() const
	{
		return m_contentLen;
	}

	//______________________________________________________________________________
	/**
	 *
	 **/

	float Scrollbar::fracViewPos() const
	{
		return float(m_viewPos) / (m_contentLen - m_viewLen);
	}

	//______________________________________________________________________________
	/**
	 *
	 **/

	float Scrollbar::fracViewLength() const
	{
		return m_viewLen / float(m_contentLen);
	}


	//____ axis() _____________________________________________________________
	/**
	 * Check if scrollbar is vertical or horizontal
	 *
	 *
	 *
	 * @return 	Axis::X if scrollbar is horizontal,
	 * 		   	Axis::Y if scrollbar is vertical.
	 **/

	Axis Scrollbar::axis() const
	{
		return scrollbar._axis();
	}

} // namespace wg
#endif //	WG_SCROLLBAR_DOT_H