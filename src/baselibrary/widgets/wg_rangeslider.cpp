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

#include <wg_rangeslider.h>
#include <wg_gfxdevice.h>
#include <wg_msg.h>
#include <wg_base.h>
#include <wg_msgrouter.h>

namespace wg
{
	using namespace Util;

	const TypeInfo RangeSlider::TYPEINFO = { "RangeSlider", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	RangeSlider::RangeSlider() : m_beginHandleSkin(this), m_endHandleSkin(this)
	{
	}

	//____ Destructor _____________________________________________________________

	RangeSlider::~RangeSlider()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& RangeSlider::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPreferredSlideLength() _________________________________________

	void RangeSlider::setPreferredSlideLength(pts length)
	{
		if (length < 0)
		{
			//TODO: Error handling!
			return;
		}

		m_preferredSlideLength = length;
		_updatePreferredSize();
	}

	//____ setAxis() _________________________________________________________

	void RangeSlider::setAxis(Axis axis)
	{
		if (axis != m_axis)
		{
			m_axis = axis;
			_requestRender();
			_updatePreferredSize();
		}
	}

	//____ setBeginHandleSkin() _______________________________________________

	void RangeSlider::setBeginHandleSkin(Skin* pSkin)
	{
		if( pSkin != m_beginHandleSkin.get() )
		{
			m_beginHandleSkin.set(pSkin);
			_requestRender();
			_updatePreferredSize();
		}
	}

	//____ setEndHandleSkin() _______________________________________________

	void RangeSlider::setEndHandleSkin(Skin* pSkin)
	{
		if (pSkin != m_endHandleSkin.get())
		{
			m_endHandleSkin.set(pSkin);
			_requestRender();
			_updatePreferredSize();
		}
	}

	//____ setSteps() ________________________________________________________

	void RangeSlider::setSteps(int nbSteps)
	{
		if (nbSteps < 0)
		{
			//TODO: Error handling!
			return;
		}

		if (nbSteps != m_nbSteps)
		{
			m_nbSteps = nbSteps;
			if (nbSteps > 0)
				_setRange(m_rangeBegin, m_rangeEnd);		// Have the range aligned to the steps.
		}
	}

	//____ setRange() _________________________________________________________

	void RangeSlider::setRange(float begin, float end)
	{
		limit(begin, 0.f, 1.f - m_minRange);
		limit(end, begin + m_minRange, 1.f);

		_setRange(begin, end, false);
	}

	//____ _preferredSize() __________________________________________________________

	SizeSPX RangeSlider::_preferredSize(int scale) const
	{
		if (m_preferredSize.w >= 0 && m_preferredSize.h >= 0)
			return m_preferredSize;
		else
			return Widget::_preferredSize(scale);
	}

	//____ _receive() __________________________________________________________

	void RangeSlider::_receive(Msg* _pMsg)
	{
		switch (_pMsg->type())
		{
		case MsgType::MouseEnter:
		case MsgType::MouseMove:
		{
			auto pMsg = static_cast<InputMsg*>(_pMsg);
			CoordSPX pos = _toLocal(pMsg->_pointerPos());

			if (_handleGeo(m_size, true).contains(pos) != m_beginHandleState.isHovered())
				_setHandleState(m_beginHandleState + StateEnum::Hovered, true);

			if (_handleGeo(m_size, false).contains(pos) != m_endHandleState.isHovered())
				_setHandleState(m_endHandleState + StateEnum::Hovered, false);
			break;
		}

		case MsgType::MouseLeave:
			if (m_beginHandleState.isHovered() && !m_beginHandleState.isPressed())
				_setHandleState(m_beginHandleState - StateEnum::Hovered, true);

			if (m_endHandleState.isHovered() && !m_endHandleState.isPressed())
				_setHandleState(m_endHandleState - StateEnum::Hovered, false);

			break;

		case MsgType::MousePress:
		{
			auto pMsg = static_cast<MousePressMsg*>(_pMsg);

			if (pMsg->button() != MouseButton::Left)
				break;

			CoordSPX pos = _toLocal(pMsg->_pointerPos());
			RectSPX  beginHandle = _handleGeo(m_size, true);
			RectSPX  endHandle = _handleGeo(m_size, false);
			if (beginHandle.contains(pos))
			{
				_setHandleState(m_beginHandleState + StateEnum::Pressed, true);
				m_valueAtPress = m_rangeBegin;
			}
			else if (endHandle.contains(pos))
			{
				_setHandleState(m_endHandleState + StateEnum::Pressed, false);
				m_valueAtPress = m_rangeEnd;
			}
			else
			{
				//TODO: Handle click on background.
			}
			break;
		}

		case MsgType::MouseRelease:
		{
			auto pMsg = static_cast<MousePressMsg*>(_pMsg);

			if (pMsg->button() != MouseButton::Left)
				break;

			//				Coord pos = toLocal(pMsg->pointerPos());
			//				Rect  handle = _handleGeo(m_size);
			if (m_beginHandleState.isPressed())
			{
				_setHandleState(m_beginHandleState - StateEnum::Pressed, true);
			}
			else if (m_endHandleState.isPressed())
			{
				_setHandleState(m_endHandleState - StateEnum::Pressed, false);
			}
			else
			{
				//TODO: Handle click on background.
			}
			break;

		}

		case MsgType::MouseRepeat:
			break;

		case MsgType::MouseDrag:
		{
			if (m_beginHandleState.isPressed() || m_endHandleState.isPressed())
			{
				auto pMsg = static_cast<MouseDragMsg*>(_pMsg);

				bool isBeginHandle = m_beginHandleState.isPressed();

				SizeSPX contentSize = m_size - m_skin.contentPaddingSize(m_scale);
				SizeSPX handleSize = _handleGeo(m_size, isBeginHandle).size();
				CoordSPX totalDrag = pMsg->_draggedTotal();

				spx slideLen;
				spx slided;

				if (m_axis == Axis::X)
				{
					slideLen = contentSize.w - handleSize.w;
					slided = totalDrag.x;
				}
				else
				{
					slideLen = contentSize.h - handleSize.h;
					slided = -totalDrag.y;
				}

				float newValue = m_valueAtPress + slided / float(slideLen);
				limit(newValue, 0.f, 1.f);

				if (isBeginHandle)
				{
					if (newValue > m_rangeEnd - m_minRange)
						newValue = m_rangeEnd - m_minRange;
					_setRange(newValue, m_rangeEnd);
				}
				else
				{
					if (newValue < m_rangeBegin + m_minRange)
						newValue = m_rangeBegin + m_minRange;
					_setRange(m_rangeBegin, newValue);
				}
			}
		}
		default:
			break;
		}
	}


	//____ _cloneContent() _______________________________________________________

	void RangeSlider::_cloneContent(const Widget* _pOrg)
	{
		Widget::_cloneContent(_pOrg);

		RangeSlider* pOrg = (RangeSlider*)_pOrg;

		m_preferredSize = pOrg->m_preferredSize;
	}

	//____ _render() __________________________________________________________

	void RangeSlider::_render(GfxDevice* pDevice, const RectSPX& canvas, const RectSPX& window)
	{
		m_skin.render(pDevice, canvas, m_scale, m_state, m_rangeBegin, m_rangeEnd);
		m_beginHandleSkin.render(pDevice, _handleGeo(canvas, true), m_scale, m_beginHandleState, m_rangeBegin);
		m_endHandleSkin.render(pDevice, _handleGeo(canvas, false), m_scale, m_endHandleState, m_rangeEnd);
	}

	//____ _alphaTest() ________________________________________________________

	bool RangeSlider::_alphaTest(const CoordSPX& ofs)
	{
		bool bMarked = m_skin.markTest(ofs, RectSPX(m_size), m_scale, m_state, m_rangeBegin, m_rangeEnd);

		if (!bMarked)
			bMarked = m_beginHandleSkin.markTest(ofs, _handleGeo(m_size, true), m_scale, m_beginHandleState, m_rangeBegin);

		if (!bMarked)
			bMarked = m_endHandleSkin.markTest(ofs, _handleGeo(m_size, false), m_scale, m_endHandleState, m_rangeEnd);

		return bMarked;
	}


	//____ _updatePreferredSize() ________________________________________________

	void RangeSlider::_updatePreferredSize()
	{
		SizeSPX sz = _calcPreferredSize(m_scale);
		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			_requestResize();
		}

	}

	//____ _calcPreferredSize() __________________________________________________

	SizeSPX RangeSlider::_calcPreferredSize(int scale) const
	{
		SizeSPX sz = m_beginHandleSkin.preferredSize(scale);
		SizeSPX sz2 = m_endHandleSkin.preferredSize(scale);

		if (m_axis == Axis::X)
			sz.w += align(ptsToSpx(m_preferredSlideLength,scale)) + sz2.w;
		else
			sz.h += align(ptsToSpx(m_preferredSlideLength,scale)) + sz2.h;

		sz += m_skin.contentPaddingSize(scale);
		sz = SizeSPX::max(sz, m_skin.preferredSize(scale));

		return sz;
	}

	//____ _setRange() __________________________________________________________

	void RangeSlider::_setRange(float begin, float end, bool bPostMsg)
	{
		if (m_nbSteps > 0)
		{
			int val = int(begin * m_nbSteps + 0.5f);
			begin = 1.f / val;

			val = int(end * m_nbSteps + 0.5f);
			end = 1.f / val;
		}

		if (begin != m_rangeBegin || end != m_rangeEnd)
		{
			float oldBegin = m_rangeBegin;
			float oldEnd = m_rangeEnd;

			RectSPX changeRect;

			if (begin != m_rangeBegin)
			{
				RectSPX oldGeo = _handleGeo(m_size, true);
				m_rangeBegin = begin;
				RectSPX newGeo = _handleGeo(m_size, true);

				changeRect = RectSPX::getUnion(oldGeo, newGeo);
			}

			if (end != m_rangeEnd)
			{
				RectSPX oldGeo = _handleGeo(m_size, false);
				m_rangeEnd = end;
				RectSPX newGeo = _handleGeo(m_size, false);

				changeRect = RectSPX::getUnion(changeRect, RectSPX::getUnion(oldGeo, newGeo));
			}
			_requestRender(changeRect);

			m_skin.valueChanged(begin, oldBegin, end, oldEnd);

			if (bPostMsg)
				Base::msgRouter()->post(RangeUpdateMsg::create(this, 0, 0, m_rangeBegin, m_rangeEnd - m_rangeBegin, false));

		}
	}

	//____ _setHandleState() __________________________________________________

	void RangeSlider::_setHandleState(State state, bool isBeginHandle)
	{
		if (isBeginHandle)
		{
			State oldState = m_beginHandleState;
			m_beginHandleState = state;

			m_beginHandleSkin.stateChanged(state, oldState);
		}
		else
		{
			State oldState = m_endHandleState;
			m_endHandleState = state;

			m_endHandleSkin.stateChanged(state, oldState);
		}
	}

	//____ _handleGeo() _______________________________________________________

	RectSPX RangeSlider::_handleGeo(const RectSPX& widgetGeo, bool isBeginHandle) const
	{
		RectSPX contentGeo = m_skin.contentRect(widgetGeo, m_scale, m_state);

		RectSPX handleGeo;

		SizeSPX handlePrefSize = isBeginHandle ? m_beginHandleSkin.preferredSize(m_scale) : m_endHandleSkin.preferredSize(m_scale);
		float value = isBeginHandle ? m_rangeBegin : m_rangeEnd;

		if (m_axis == Axis::X)
		{
			handleGeo.w = handlePrefSize.w * (contentGeo.h / float(handlePrefSize.h));
			handleGeo.h = contentGeo.h;

			handleGeo.x = contentGeo.x + (contentGeo.w - handleGeo.w) * value;
			handleGeo.y = contentGeo.y;
		}
		else
		{
			handleGeo.w = contentGeo.w;
			handleGeo.h = handlePrefSize.h * (contentGeo.w / float(handlePrefSize.w));

			handleGeo.x = contentGeo.x;
			handleGeo.y = contentGeo.y + contentGeo.h - handleGeo.h - (contentGeo.h - handleGeo.h) * value;
		}

		return align(handleGeo);
	}

	//____ _skinState() _______________________________________________________

	State RangeSlider::_skinState(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_beginHandleSkin)
			return m_beginHandleState;
		else if (pSlot == &m_endHandleSkin)
			return m_endHandleState;
		else
			return m_state;
	}

	//____ _skinSize() ___________________________________________________

	SizeSPX RangeSlider::_skinSize(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_beginHandleSkin)
			return _handleGeo(m_size, true).size();
		else if (pSlot == &m_endHandleSkin)
			return _handleGeo(m_size, false).size();

		return SizeSPX();
	}

	//____ _skinRequestRender() _______________________________________________

	void RangeSlider::_skinRequestRender(const SkinSlot* pSlot)
	{
		if (pSlot == &m_beginHandleSkin)
			_requestRender(_handleGeo(m_size, true));
		else if (pSlot == &m_endHandleSkin)
			_requestRender(_handleGeo(m_size, false));
		else
			_requestRender();
	}

	void RangeSlider::_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect)
	{
		CoordSPX ofs;

		if (pSlot == &m_beginHandleSkin)
			ofs = _handleGeo(m_size, true).pos();
		else if (pSlot == &m_endHandleSkin)
			ofs = _handleGeo(m_size, false).pos();

		_requestRender( rect + ofs);
	}

	//____ _skinValue() _______________________________________________________

	float RangeSlider::_skinValue(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_endHandleSkin)
			return m_rangeEnd;
		else
			return m_rangeBegin;
	}

	//____ _skinValue2() ______________________________________________________

	float RangeSlider::_skinValue2(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_skin)
			return m_rangeEnd;
		else
			return -1;
	}




} // namespace wg
