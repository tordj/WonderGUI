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

#include <memory.h>
#include <wg_scrollbar.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg
{

	using namespace Util;

	const TypeInfo Scrollbar::TYPEINFO = { "Scrollbar", &Widget::TYPEINFO };


	//____ Scrollbar() ________________________________________________________

	Scrollbar::Scrollbar() : scrollbar(this,this,Axis::Y)
	{

	}

	//____ ~Scrollbar() _______________________________________________________

	Scrollbar::~Scrollbar( void )
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& Scrollbar::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setAxis() __________________________________________________________
	/**
	 * Set if scrollbar should be horizontal or vertical.
	 *
	 * @param axis	Set to Axis::X to make widget horizontal or
	 * 				Axis::Y to make widget vertical.
	 *
	 * By default a scrollbar is vertical.
	 **/

	void Scrollbar::setAxis( Axis axis )
	{
		if( axis != scrollbar._axis() )
		{
			scrollbar._setAxis(axis);
			_requestRender();
			_requestResize();
		}
	}

	//____ setMovementAmounts() _______________________________________________

	void Scrollbar::setMovementAmounts(pts singleStep, pts wheelRoll, pts pageOverlap)
	{
		m_singleStep = singleStep;
		m_wheelRoll = wheelRoll;
		m_pageOverlap = pageOverlap;
	}

	//____ setView() __________________________________________________________

	void Scrollbar::setView(pts viewPos, pts viewLength, pts contentLength)
	{
		if (viewPos != m_viewPos || viewLength != m_viewLen || contentLength != m_contentLen)
		{
			spx oldPos = align(ptsToSpx(m_viewPos, m_scale));
			spx oldLen = align(ptsToSpx(m_viewLen, m_scale)); 
			spx oldContentLen = align(ptsToSpx(m_contentLen, m_scale));

			spx newPos = align(ptsToSpx(viewPos, m_scale));
			spx newLen = align(ptsToSpx(viewLength, m_scale));
			spx newContentLen = align(ptsToSpx(contentLength, m_scale));

			m_viewPos = viewPos;
			m_viewLen = viewLength;
			m_contentLen = contentLength;

			scrollbar._update( newPos, oldPos, newLen, oldLen, newContentLen, oldContentLen );
		}
	}

	//____ _cloneContent() ____________________________________________________

	void Scrollbar::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Scrollbar * pOrg = (Scrollbar *) _pOrg;

		m_singleStep	= pOrg->m_singleStep;
		m_wheelRoll		= pOrg->m_wheelRoll;
		m_pageOverlap	= pOrg->m_pageOverlap;

	  	m_viewPos		= pOrg->m_viewPos;
		m_viewLen		= pOrg->m_viewLen;
		m_contentLen	= pOrg->m_contentLen;
	}

	//____ _setState() ______________________________________________________

	void Scrollbar::_setState( State state )
	{
		Widget::_setState(state);
		scrollbar._setState(state);
	}


	//____ _refresh() _______________________________________________________

	void Scrollbar::_refresh( void )
	{
		Widget::_refresh();
	}

	//____ _preferredSize() ___________________________________________________

	SizeSPX Scrollbar::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX preferred = scrollbar._preferredSize(scale);

		if (!skin.isEmpty())
			preferred += skin->_contentPaddingSize(scale);

		return preferred;
	}


	//____ _render() ________________________________________________________

	void Scrollbar::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectSPX	contentCanvas = OO(skin)._contentRect(_canvas,m_scale, m_state);

		scrollbar._render(pDevice, contentCanvas, m_scale);
	}

	//____ _alphaTest() ______________________________________________________

	bool Scrollbar::_alphaTest( const CoordSPX& ofs )
	{
		RectSPX	contentCanvas = OO(skin)._contentRect(m_size, m_scale, m_state);

		if (contentCanvas.contains(ofs) && scrollbar._alphaTest(ofs-contentCanvas.pos(),contentCanvas.size(),m_markOpacity))
			return true;

		return Widget::_alphaTest(ofs);
	}

	//____ _receive() _________________________________________________________

	void Scrollbar::_receive( Msg * pMsg )
	{
		scrollbar._receive(pMsg);
	}

	//____ _setViewPos() ______________________________________________________

	void Scrollbar::_setViewPos(pts pos)
	{
		limit(pos, 0, m_contentLen - m_viewLen);

		if (pos != m_viewPos)
		{
			spx oldPos = align(ptsToSpx(m_viewPos, m_scale));
			spx newPos = align(ptsToSpx(pos, m_scale));

			spx len = align(ptsToSpx(m_viewLen, m_scale));
			spx contentLen = align(ptsToSpx(m_contentLen, m_scale));

			m_viewPos = pos;

			scrollbar._update(newPos, oldPos, len, len, contentLen, contentLen);

			_sendMessage();
		}
	}

	//____ _sendMessage() _____________________________________________________

	void Scrollbar::_sendMessage()
	{
		Base::msgRouter()->post(ScrollbarMoveMsg::create(this, spxToPts(m_viewPos, m_scale), fracViewPos() ));
	}


	//____ _scrollbarStep() ___________________________________________________

	void Scrollbar::_scrollbarStep(const CScrollbar* pComponent, int dir)
	{
		spx newViewPos = m_viewPos + m_singleStep * dir;
		limit(newViewPos, 0, m_contentLen - m_viewLen);

		_setViewPos(m_viewPos + m_singleStep * dir);
	}

	//____ _scrollbarPage() ___________________________________________________

	void Scrollbar::_scrollbarPage(const CScrollbar* pComponent, int dir)
	{
		_setViewPos(m_viewPos + (m_viewLen - m_pageOverlap) * dir);
	}

	//____ _scrollbarWheel() __________________________________________________

	void Scrollbar::_scrollbarWheel(const CScrollbar* pComponent, int dir)
	{
		_setViewPos(m_viewPos + m_wheelRoll * dir);
	}

	//____ _scrollbarMove() ___________________________________________________

	spx Scrollbar::_scrollbarMove(const CScrollbar* pComponent, spx pos)
	{
		spx viewLen = align(ptsToSpx(m_viewLen, m_scale));
		spx contentLen = align(ptsToSpx(m_contentLen, m_scale));

		scrollbar._update( pos, align(ptsToSpx(m_viewPos, m_scale)), viewLen, viewLen, contentLen, contentLen);

		m_viewPos = spxToPts(pos, m_scale);

		_sendMessage();
		return pos;
	}

	//____ _scrollbarOfsLenContent() __________________________________________

	std::tuple<spx, spx, spx> Scrollbar::_scrollbarOfsLenContent(const CScrollbar* pComponent)
	{
		return std::make_tuple(align(ptsToSpx(m_viewPos,m_scale)), align(ptsToSpx(m_viewLen, m_scale)), align(ptsToSpx(m_contentLen, m_scale)) );
	}

} // namespace wg
