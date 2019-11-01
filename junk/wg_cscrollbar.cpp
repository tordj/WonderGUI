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

//TODO: Possible enter/leave on mouse move message.
//TODO: No press, repeat, Wheel roll etc if outside.


#include <wg_cscrollbar.h>
#include <wg_msg.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

#include <algorithm>

namespace wg
{
	//____ Constructor ________________________________________________________

	CScrollbar::CScrollbar(ComponentHolder * pHolder) : Component(pHolder)
	{
		for (int i = 0; i < C_NUMBER_OF_PARTS; i++)
			m_states[i] = StateEnum::Normal;
	}

	//____ receive() __________________________________________________________

	void CScrollbar::receive(Msg * pMsg, const RectI& canvas, int markOpacity )
	{
		// We are only interested in button 1 and wheel 1 messages

		if (!(pMsg->type() == MsgType::MouseEnter || pMsg->type() == MsgType::MouseMove || pMsg->type() == MsgType::MouseLeave ||
			
			pMsg->isMouseButtonMsg() && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left ||
			pMsg->type() == MsgType::WheelRoll && WheelRollMsg::cast(pMsg)->wheel() == 1))
			return;

		int		handlePos, handleLen;
		_viewToPosLen(&handlePos, &handleLen, canvas.size() );

		MsgRouter_p	pHandler = Base::msgRouter();
		CoordI pos = InputMsg::cast(pMsg)->pointerPosRaw() - _globalPos();

		int		pointerOfs;
		int		length;

		RectI contentRect = RectI(0, 0, canvas.size());

		if (m_bHorizontal)
		{
			pointerOfs = pos.x - contentRect.x;
			length = contentRect.w;
		}
		else
		{
			pointerOfs = pos.y - contentRect.y;
			length = contentRect.h;
		}

		length -= m_headerLen + m_footerLen;
		pointerOfs -= m_headerLen;

		switch (pMsg->type())
		{
			case MsgType::MouseRelease:
			{
				if (MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left)
					return;

				// Just put them all to NORMAL and request render.
				// Release is followed by over before render anyway so right one will be highlighted.

				_unhoverReqRender();
				break;
			}

			case MsgType::MouseLeave:
			{
				// Turn any MARKED/SELECTED button/bg back to NORMAL.
				// Turn handle back to NORMAL only if MARKED (selected handle should remain selected).
				// Request render only if something changed (which it has unless bar was SELECTED...).

				if (m_states[C_HANDLE].isPressed())
					return;

				_unhoverReqRender();
				break;
			}

			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				if (m_states[C_HANDLE].isPressed())
					return;

				Part c = _findMarkedPart(pos, canvas.size(), markOpacity);

				if ((c != C_NONE && !m_states[c].isHovered()) || (c == C_BG && m_states[C_HANDLE].isHovered()))
				{
					_unhoverReqRender();
					m_states[c].setHovered(true);
					if (c == C_HANDLE)
						m_states[C_BG].setHovered(true);			// Always also mark bg if bar is marked.
				}
				else if (c == C_BG && m_states[C_HANDLE].isHovered())
				{
					m_states[C_HANDLE].setHovered(false);
					m_states[C_BG].setHovered(false);
					_requestRender();
				}


				break;
			}

			case MsgType::MousePress:
			{
				if (MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left)
					return;

				Part c = _findMarkedPart(pos, canvas.size(), markOpacity);

				_unhoverReqRender();
				m_states[c].setPressed(true);

				if (c == C_HANDLE)
				{
					m_handlePressOfs = pointerOfs - handlePos;
					m_states[C_BG].setHovered(true);			// Always mark bg if bar is pressed.
				}
				else if (c == C_BG)
				{
					if (m_bJumpToPress)
					{
						if (pointerOfs - handlePos < handleLen / 2)
							_notify(ComponentNotif::JumpBackward, 0, nullptr);
						else
							_notify(ComponentNotif::JumpForward, 0, nullptr);					
					}
					else
					{ 
						m_states[C_HANDLE].setPressed(true);
						m_states[C_BG].setHovered(true);
						m_handlePressOfs = handleLen / 2;
						setHandlePixelPos(pointerOfs, canvas.size() );
						_notify(ComponentNotif::ValueEdited, 0, nullptr);
					}

				}
				else if (c == C_HEADER_FWD || c == C_FOOTER_FWD)
					_notify(ComponentNotif::StepForward, 1, nullptr);
				else if (c == C_HEADER_BWD || c == C_FOOTER_BWD)
					_notify(ComponentNotif::StepBackward, 1, nullptr);
				break;
			}

			case MsgType::MouseRepeat:
			{
				if (MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left)
					return;

				if (m_states[C_HANDLE].isPressed())
					return;

				Part c = _findMarkedPart(pos, canvas.size(), markOpacity);

				if (c == C_BG)
				{
					if (pointerOfs - handlePos < handleLen / 2)
						_notify(ComponentNotif::JumpBackward, 0, nullptr);
					else
						_notify(ComponentNotif::JumpForward, 0, nullptr);
				}
				else if (c == C_HEADER_FWD || c == C_FOOTER_FWD)
					_notify(ComponentNotif::StepForward, 1, nullptr);
				else if (c == C_HEADER_BWD || c == C_FOOTER_BWD)
					_notify(ComponentNotif::StepBackward, 1, nullptr);

				break;
			}

			case MsgType::MouseDrag:
			{
				if (MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left)
					return;

				if (m_states[C_HANDLE].isPressed())
				{
					float	scrollhandlePos = 0.f;

					if (m_handleSize < 1.f)
						scrollhandlePos = ((float)(pointerOfs - m_handlePressOfs)) / (length - handleLen);

					limit(scrollhandlePos, 0.f, 1.f);

					if (scrollhandlePos != m_handlePos)
					{
						setHandlePos(scrollhandlePos);
						_notify(ComponentNotif::ValueEdited, 0, nullptr);
					}
				}
				break;
			}

			case MsgType::WheelRoll:
			{
				WheelRollMsg_p p = WheelRollMsg::cast(pMsg);

				if (p->wheel() == 1)
				{
					int distance = p->distance().y;		//TODO: Should we support horizontal wheel roll for horizontal scrollbar?

					if( distance < 0 )
						_notify(ComponentNotif::StepForward, -distance, nullptr);
					else
						_notify(ComponentNotif::StepBackward, distance, nullptr);
					pMsg->swallow();
				}
			}

			default:
				break;
		}

		// Swallow all button 1 messages.

		if (pMsg->isMouseButtonMsg() && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left)
			pMsg->swallow();
	}


	//____ _unhoverReqRender() ______________________________________________________

	void CScrollbar::_unhoverReqRender()
	{
		for (int i = 0; i < C_NUMBER_OF_PARTS; i++)
			m_states[i].setHovered(false);

		_requestRender();
	}


	//____ _render() _____________________________________________________________

	void CScrollbar::render(GfxDevice * pDevice, const RectI& _canvas)
	{
		RectI	dest = _canvas;

		// Render header buttons

		if (m_pBtnBwdSkin && (m_btnLayout & HEADER_BWD))
			_renderButton(pDevice, dest, m_pBtnBwdSkin, m_states[C_HEADER_BWD]);

		if (m_pBtnFwdSkin && (m_btnLayout & HEADER_FWD))
			_renderButton(pDevice, dest, m_pBtnFwdSkin, m_states[C_HEADER_FWD]);

		// Render background (if any).

		if (m_bHorizontal)
			dest.w = _canvas.w - m_headerLen - m_footerLen;
		else
			dest.h = _canvas.h - m_headerLen - m_footerLen;

		if (m_pBgSkin)
			m_pBgSkin->_render(pDevice, dest, m_states[C_BG]);

		// Render the handle

		if (m_pHandleSkin)
		{
			int handlePos;
			int handleLen;
			_viewToPosLen(&handlePos, &handleLen, _canvas.size());

			RectI	handleDest;
			if (m_bHorizontal)
				handleDest = RectI(dest.x + handlePos, dest.y, handleLen, dest.h);
			else
				handleDest = RectI(dest.x, dest.y + handlePos, dest.w, handleLen);

			m_pHandleSkin->_render(pDevice, handleDest, m_states[C_HANDLE]);
		}

		// Render footer buttons

		if (m_bHorizontal)
			dest.x += dest.w;
		else
			dest.y += dest.h;

		if (m_pBtnBwdSkin && (m_btnLayout & FOOTER_BWD))
			_renderButton(pDevice, dest, m_pBtnBwdSkin, m_states[C_FOOTER_BWD]);

		if (m_pBtnFwdSkin && (m_btnLayout & FOOTER_FWD))
			_renderButton(pDevice, dest, m_pBtnFwdSkin, m_states[C_FOOTER_FWD]);

	}

	//____ _renderButton() _________________________________________________________

	void CScrollbar::_renderButton(GfxDevice * pDevice, RectI& _dest, Skin * pSkin, State state)
	{
		if (m_bHorizontal)
			_dest.w = pSkin->_preferredSize().w;
		else
			_dest.h = pSkin->_preferredSize().h;

		pSkin->_render(pDevice, _dest, state);

		if (m_bHorizontal)
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;
	}

	//____ alphaTest() ___________________________________________________

	bool CScrollbar::alphaTest(const CoordI& ofs, SizeI size, int markOpacity )
	{
		if (_findMarkedPart(ofs, size, markOpacity) != C_NONE)
			return true;

		return false;
	}

	//____ setEnabled() _______________________________________________________

	void CScrollbar::setEnabled(bool bEnabled)
	{
		for (int i = 0; i < C_NUMBER_OF_PARTS; i++)
			m_states[i].setEnabled(bEnabled);
	}


	//____ setOrientation() ___________________________________________________

	void CScrollbar::setOrientation(Orientation orientation)
	{
		bool bHorizontal = (orientation == Orientation::Horizontal);

		if (m_bHorizontal != bHorizontal)
		{
			m_bHorizontal = bHorizontal;
			_headerFooterChanged();
			_updateMinSize();
			_requestRender();
		}
	}

	//____ preferredSize() ______________________________________________________

	SizeI CScrollbar::preferredSize() const
	{
		SizeI sz = m_minSize;

		// Add 50 pixels in the scrollbars direction for best size.

		if (m_bHorizontal)
			sz.w += 50 * QPix::pixelQuartersPerPoint();
		else
			sz.h += 50 * QPix::pixelQuartersPerPoint();

		return sz;
	}

	//____ setSkins() ___________________________________________________________

	void CScrollbar::setSkins(Skin * pBackgroundSkin, Skin * pHandleSkin,
							  Skin * pBwdButtonSkin, Skin * pFwdButtonSkin)
	{
		m_pBgSkin = pBackgroundSkin;
		m_pHandleSkin = pHandleSkin;
		m_pBtnFwdSkin = pFwdButtonSkin;
		m_pBtnBwdSkin = pBwdButtonSkin;

		_headerFooterChanged();
		_updateMinSize();
		_requestRender();
	}

	//____ setBackgroundSkin() _____________________________________________________

	void CScrollbar::setBackgroundSkin(Skin * pSkin)
	{
		if (pSkin != m_pBgSkin)
		{
			m_pBgSkin = pSkin;

			_updateMinSize();
			_requestRender();
		}
	}

	//____ setHandleSkin() ______________________________________________________

	void CScrollbar::setHandleSkin(Skin * pSkin)
	{
		if (pSkin != m_pHandleSkin)
		{
			m_pHandleSkin = pSkin;

			_updateMinSize();
			_requestRender();
		}
	}

	//____ setFwdButtonSkin() ______________________________________________________

	void CScrollbar::setFwdButtonSkin(Skin * pSkin)
	{
		if (pSkin != m_pBtnFwdSkin)
		{
			m_pBtnFwdSkin = pSkin;

			_headerFooterChanged();
			_updateMinSize();
			_requestRender();
		}
	}

	//____ setBwdButtonSkin() ______________________________________________________

	void CScrollbar::setBwdButtonSkin(Skin * pSkin)
	{
		if (pSkin != m_pBtnBwdSkin)
		{
			m_pBtnBwdSkin = pSkin;

			_headerFooterChanged();
			_updateMinSize();
			_requestRender();
		}
	}

	//____ setButtonLayout() ______________________________________________________

	void CScrollbar::setButtonLayout(BtnLayout layout)
	{
		m_btnLayout = layout;

		_headerFooterChanged();
	}

	//____ setJumpToPress() ______________________________________________________

	void CScrollbar::setJumpToPress(bool bJumpToPress)
	{
		m_bJumpToPress = bJumpToPress;
	}

	//____ setHandle() ________________________________________________________

	void CScrollbar::setHandle(float pos, float size)
	{
		limit(size, 0.0001f, 1.f);
		limit(pos, 0.f, 1.f);

		if (m_handlePos == pos && m_handleSize == size)
			return;

		m_handlePos = pos;
		m_handleSize = size;

		_requestRender();
	}

	//____ setHandlePos() _____________________________________________________

	void CScrollbar::setHandlePos(float pos)
	{
		limit(pos, 0.f, 1.f);

		if (pos > m_handlePos - 0.000001 && pos < m_handlePos + 0.000001)
			return;

		m_handlePos = pos;
		_requestRender();
	}

	//____ setHandlePixelPos() ________________________________________________

	void CScrollbar::setHandlePixelPos(int pos, SizeI size)
	{
		int		handlePos, handleLen;
		_viewToPosLen(&handlePos, &handleLen, size);

		int		length = m_bHorizontal ? size.w : size.h;

		length -= m_headerLen + m_footerLen;

		float scrollhandlePos = 0.f;
		if (m_handleSize < 1.f)
			scrollhandlePos = ((float)(pos - (handleLen >> 1))) / (length - handleLen);

		setHandlePos(scrollhandlePos);
	}

	//____ setHandleSize() ______________________________________________________

	void CScrollbar::setHandleSize(float size)
	{
		limit(size, 0.0001f, 1.f);

		if (size == m_handleSize)
			return;

		m_handleSize = size;
		_requestRender();
	}


	//____ _headerFooterChanged() _______________________________________________

	void CScrollbar::_headerFooterChanged()
	{
		int	fwdLen = 0, bwdLen = 0;

		if (m_bHorizontal)
		{
			if (m_pBtnFwdSkin)
				fwdLen = m_pBtnFwdSkin->_preferredSize().w;
			if (m_pBtnBwdSkin)
				bwdLen = m_pBtnBwdSkin->_preferredSize().w;
		}
		else
		{
			if (m_pBtnFwdSkin)
				fwdLen = m_pBtnFwdSkin->_preferredSize().h;
			if (m_pBtnBwdSkin)
				bwdLen = m_pBtnBwdSkin->_preferredSize().h;
		}

		int	headerLen = 0;
		int footerLen = 0;

		if ((m_btnLayout & HEADER_BWD))
			headerLen += bwdLen;

		if ((m_btnLayout & HEADER_FWD))
			headerLen += fwdLen;

		if ((m_btnLayout & FOOTER_BWD))
			footerLen += bwdLen;

		if ((m_btnLayout & FOOTER_FWD))
			footerLen += fwdLen;

		if (headerLen != m_headerLen || footerLen != m_footerLen)
		{
			m_headerLen = headerLen;
			m_footerLen = footerLen;

			_updateMinSize();
		}

		_requestRender();
	}

	//____ _updateMinSize() ________________________________________________________

	void CScrollbar::_updateMinSize()
	{
		int	minW = 4;
		int	minH = 4;

		// Check min w/h for BgGfx.

		if (m_pBgSkin)
		{
			SizeI sz = m_pBgSkin->_minSize();

			minW = max(minW, sz.w);
			minH = max(minH, sz.h);
		}

		// Check min w/h for BarGfx.

		if (m_pHandleSkin)
		{
			SizeI sz = m_pHandleSkin->_minSize();

			minW = max(minW, sz.w);
			minH = max(minH, sz.h);
		}


		// Add header and footer to minW/minH from scrollbar part.

		if (m_bHorizontal)
			minW += m_footerLen + m_headerLen;
		else
			minH += m_footerLen + m_headerLen;


		// Check min w/h for forward button.

		if (m_pBtnFwdSkin && (m_btnLayout & (HEADER_FWD | FOOTER_FWD)))
		{
			SizeI sz = m_pBtnFwdSkin->_preferredSize();

			minW = max(minW, sz.w);
			minH = max(minH, sz.h);
		}

		// Check min w/h for backward button.

		if (m_pBtnBwdSkin && (m_btnLayout & (HEADER_BWD | FOOTER_BWD)))
		{
			SizeI sz = m_pBtnBwdSkin->_preferredSize();

			minW = max(minW, sz.w);
			minH = max(minH, sz.h);
		}

		// Set if changed.

		if (minW != m_minSize.w || minH != m_minSize.h)
		{
			m_minSize.w = minW;
			m_minSize.h = minH;

			_requestResize();
		}
	}

	//____ _viewToPosLen() _________________________________________________________

	void CScrollbar::_viewToPosLen(int * _wpPos, int * _wpLen, SizeI size)
	{
		// changes by Viktor.

		// using floats here results in errors.
		//float pos, len;
		int pos, len;

		int maxLen = m_bHorizontal ? size.w : size.h;

		maxLen -= m_headerLen + m_footerLen;

		//len = m_handleSize * maxLen;
		len = (int)(m_handleSize * (float)maxLen);

		int		minLen;

		if (m_bHorizontal)
			minLen = m_pHandleSkin->_minSize().w;
		else
			minLen = m_pHandleSkin->_minSize().h;

		if (minLen < 4)
			minLen = 4;

		len = std::min(len, minLen);


		//pos = m_handlePos * (maxLen-len);
		pos = (int)(m_handlePos * (float)(maxLen - len));

		if (pos + len > maxLen)
			pos = maxLen - len;

		//	pos += m_headerLen;

			//* _wpPos = (int) pos;
			//* _wpLen = (int) len;
		* _wpPos = pos;
		*_wpLen = len;
		//	* _wpLen = ((int)(pos + len)) - (int) pos;
	}

	//____ _findMarkedPart() __________________________________________________

	CScrollbar::Part CScrollbar::_findMarkedPart(CoordI ofs, SizeI size, int markOpacity)
	{
		RectI canvas = RectI(0, 0, size);

		RectI dest = canvas;

		// First of all, do a mark test against the header buttons...

		if (m_pBtnBwdSkin && (m_btnLayout & HEADER_BWD))
		{
			if (_markTestButton(ofs, dest, m_pBtnBwdSkin, m_states[C_HEADER_BWD], markOpacity))
				return C_HEADER_BWD;
		}

		if (m_pBtnFwdSkin && (m_btnLayout & HEADER_FWD))
		{
			if (_markTestButton(ofs, dest, m_pBtnFwdSkin, m_states[C_HEADER_FWD], markOpacity))
				return C_HEADER_FWD;
		}

		// Then do a mark test against the footer buttons...

		if (m_bHorizontal)
			dest.x = canvas.x + canvas.w - m_footerLen;
		else
			dest.y = canvas.y + canvas.h - m_footerLen;

		if (m_pBtnBwdSkin && (m_btnLayout & FOOTER_BWD))
		{
			if (_markTestButton(ofs, dest, m_pBtnBwdSkin, m_states[C_FOOTER_BWD], markOpacity))
				return C_FOOTER_BWD;
		}

		if (m_pBtnFwdSkin && (m_btnLayout & FOOTER_FWD))
		{
			if (_markTestButton(ofs, dest, m_pBtnFwdSkin, m_states[C_FOOTER_FWD], markOpacity))
				return C_FOOTER_FWD;
		}

		// Then, do a mark test against the dragbar...

		if (_markTestHandle(ofs, size, markOpacity) == true)
			return C_HANDLE;

		// Do a mark test against the dragbar background.

		RectI	r = canvas;

		if (m_bHorizontal)
		{
			r.x += m_headerLen;
			r.w -= m_headerLen + m_footerLen;
		}
		else
		{
			r.y += m_headerLen;
			r.h -= m_headerLen + m_footerLen;
		}

		if (m_pBgSkin && m_pBgSkin->_markTest(ofs, r, m_states[C_BG], markOpacity))
			return C_BG;

		return C_NONE;
	}

	//____ _markTestButton() _______________________________________________________

	bool CScrollbar::_markTestButton(CoordI ofs, RectI& _dest, Skin * pSkin, State state, int markOpacity)
	{
		if (m_bHorizontal)
			_dest.w = pSkin->_preferredSize().w;
		else
			_dest.h = pSkin->_preferredSize().h;

		bool retVal = pSkin->_markTest(ofs, _dest, state, markOpacity);

		if (m_bHorizontal)
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;

		return retVal;
	}

	//____ _markTestHandle() _______________________________________________________

	bool CScrollbar::_markTestHandle(CoordI ofs, SizeI& componentSize, int markOpacity)
	{
		if (!m_pHandleSkin)
			return false;

		int   handlePos, handleLen;
		_viewToPosLen(&handlePos, &handleLen, componentSize);

		RectI area(0, 0, componentSize);

		if (m_bHorizontal)
		{
			area.x = handlePos + m_headerLen;
			area.w = handleLen;
		}
		else
		{
			area.y = handlePos + m_headerLen;
			area.h = handleLen;
		}

		return m_pHandleSkin->_markTest(ofs, area, m_states[C_HANDLE], markOpacity);
	}
};
