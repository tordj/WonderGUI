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

namespace wg
{
	using namespace Util;

	const TypeInfo RangeSlider::TYPEINFO = { "RangeSlider", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	RangeSlider::RangeSlider()
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

	//____ preferredSize() __________________________________________________________

	Size RangeSlider::preferredSize() const
	{
		if( m_preferredSize.w >= 0 && m_preferredSize.h >= 0 )
			return m_preferredSize;
		else
			return Widget::preferredSize();
	}

	//____ _setPreferredSlideLength() _________________________________________

	void RangeSlider::setPreferredSlideLength(MU length)
	{
		if (length < 0)
		{
			//TODO: Error handling!
			return;
		}

		m_preferredSlideLength = length;
		_updatePreferredSize();
	}

	//____ setSkin() _________________________________________________________

	void RangeSlider::setSkin(Skin * pSkin)
	{
		Widget::setSkin(pSkin);
	}

	//____ setBeginHandleSkin() ___________________________________________________

	void RangeSlider::setBeginHandleSkin(const Skin_p& pSkin)
	{
		if (pSkin != m_pBeginHandleSkin)
		{
			m_pBeginHandleSkin = pSkin;
			_requestRender();
			_updatePreferredSize();
		}
	}

	//____ setEndHandleSkin() ___________________________________________________

	void RangeSlider::setEndHandleSkin(const Skin_p& pSkin)
	{
		if (pSkin != m_pEndHandleSkin)
		{
			m_pEndHandleSkin = pSkin;
			_requestRender();
			_updatePreferredSize();
		}
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
			if( nbSteps > 0 )
				setRange(m_rangeBegin, m_rangeEnd);		// Have the range aligned to the steps.
		}
	}

	//____ setRange() _________________________________________________________

	void RangeSlider::setRange(float begin, float end)
	{
		limit(begin, 0.f, 1.f - m_minRange);
		limit(end, begin + m_minRange, 1.f);

		if (begin != m_rangeBegin || end != m_rangeEnd)
			_setRange(begin,end);
	}

	//____ _receive() __________________________________________________________

	void RangeSlider::_receive(Msg * _pMsg)
	{
		switch (_pMsg->type())
		{
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				auto pMsg = static_cast<InputMsg*>(_pMsg);
				Coord pos = toLocal(pMsg->pointerPos());

				if( _handleGeo(m_size,true).contains(pos) != m_beginHandleState.isHovered() )
					_setHandleState(m_beginHandleState + StateEnum::Hovered, true);

				if (_handleGeo(m_size, false).contains(pos) != m_endHandleState.isHovered())
					_setHandleState(m_endHandleState + StateEnum::Hovered, false);
				break;
			}

			case MsgType::MouseLeave:
				if (m_beginHandleState.isHovered() && !m_beginHandleState.isPressed() )
					_setHandleState(m_beginHandleState - StateEnum::Hovered, true);

				if (m_endHandleState.isHovered() && !m_endHandleState.isPressed())
					_setHandleState(m_endHandleState - StateEnum::Hovered, false);

				break;

			case MsgType::MousePress:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);

				if (pMsg->button() != MouseButton::Left)
					break;

				Coord pos = toLocal(pMsg->pointerPos());
				Rect  beginHandle = _handleGeo(m_size,true);
				Rect  endHandle = _handleGeo(m_size, false);
				if (beginHandle.contains(pos))
				{
					_setHandleState(m_beginHandleState + StateEnum::Pressed,true);
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

					Size contentSize = m_pSkin ? m_size - m_pSkin->contentPaddingSize() : m_size;
					Size handleSize = _handleGeo(m_size, isBeginHandle).size();
					Coord totalDrag = pMsg->draggedTotal();

					MU slideLen;
					MU slided;

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

					float newValue = m_valueAtPress + slided / slideLen;
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
		}
	}


	//____ _cloneContent() _______________________________________________________

	void RangeSlider::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		RangeSlider * pOrg = (RangeSlider*) _pOrg;

		m_preferredSize = pOrg->m_preferredSize;
	}

	//____ _render() __________________________________________________________

	void RangeSlider::_render(GfxDevice * pDevice, const Rect& canvas, const Rect& window)
	{
		if (m_pSkin)
			m_pSkin->render(pDevice, canvas, m_state, m_rangeBegin, m_rangeEnd);

		if (m_pBeginHandleSkin)
			m_pBeginHandleSkin->render(pDevice, _handleGeo(canvas,true), m_beginHandleState, m_rangeBegin);

		if (m_pEndHandleSkin)
			m_pEndHandleSkin->render(pDevice, _handleGeo(canvas,false), m_endHandleState, m_rangeEnd);

	}

	//____ _alphaTest() ________________________________________________________

	bool RangeSlider::_alphaTest(const Coord& ofs)
	{
		bool bMarked = false;

		if (m_pSkin)
			bMarked = m_pSkin->markTest(ofs, Rect(m_size), m_state, m_markOpacity, m_rangeBegin, m_rangeEnd);

		if (!bMarked && m_pBeginHandleSkin)
			bMarked = m_pBeginHandleSkin->markTest(ofs, _handleGeo(m_size,true), m_beginHandleState, m_markOpacity, m_rangeBegin);

		if (!bMarked && m_pEndHandleSkin)
			bMarked = m_pEndHandleSkin->markTest(ofs, _handleGeo(m_size, false), m_endHandleState, m_markOpacity, m_rangeEnd);

		return bMarked;
	}


	//____ _updatePreferredSize() ________________________________________________

	void RangeSlider::_updatePreferredSize()
	{
		Size sz =	m_pBeginHandleSkin ? m_pBeginHandleSkin->preferredSize() : Size( 4, 4 );
		Size sz2 =	m_pEndHandleSkin ? m_pEndHandleSkin->preferredSize() : Size(4, 4);

		if (m_axis == Axis::X)
			sz.w += m_preferredSlideLength + sz2.w;
		else
			sz.h += m_preferredSlideLength + sz2.h;

		if (m_pSkin)
		{
			sz += m_pSkin->contentPaddingSize();
			sz = Size::max(sz, m_pSkin->preferredSize());
		}

		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			_requestResize();
		}
	}

	//____ _setRange() __________________________________________________________

	void RangeSlider::_setRange(float begin, float end)
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
			Rect changeRect;

			if (begin != m_rangeBegin)
			{
				Rect oldGeo = _handleGeo(m_size,true);

				m_rangeBegin = begin;
				Rect newGeo = _handleGeo(m_size,true);

				changeRect = Rect::getUnion(oldGeo, newGeo);
			}

			if (end != m_rangeEnd)
			{
				Rect oldGeo = _handleGeo(m_size, false);

				m_rangeEnd = end;
				Rect newGeo = _handleGeo(m_size, false);

				changeRect = Rect::getUnion(changeRect, Rect::getUnion(oldGeo, newGeo));
			}


			if( m_pSkin )
				changeRect.growToContain( m_pSkin->fractionChangeRect(m_size, m_state, m_rangeBegin, begin, m_rangeEnd, end) );

			_requestRender( changeRect );
		}
	}

	//____ _setHandleState() __________________________________________________

	void RangeSlider::_setHandleState(State state, bool isBeginHandle)
	{
		if (isBeginHandle )
		{
			if (m_pBeginHandleSkin && !m_pBeginHandleSkin->isStateIdentical(state, m_beginHandleState, m_rangeBegin))
				_requestRender(_handleGeo(m_size, isBeginHandle));

			m_beginHandleState = state;
		}
		else
		{
			if (m_pEndHandleSkin && !m_pEndHandleSkin->isStateIdentical(state, m_endHandleState, m_rangeEnd))
				_requestRender(_handleGeo(m_size, isBeginHandle));

			m_endHandleState = state;
		}
	}

	//____ _handleGeo() _______________________________________________________

	Rect RangeSlider::_handleGeo(const Rect& widgetGeo, bool isBeginHandle)
	{
		Rect contentGeo = m_pSkin ? m_pSkin->contentRect(widgetGeo, m_state) : widgetGeo;

		Rect handleGeo;

		Size handlePrefSize = isBeginHandle ? m_pBeginHandleSkin->preferredSize() : m_pEndHandleSkin->preferredSize();
		float value = isBeginHandle ? m_rangeBegin : m_rangeEnd;

		if (m_axis == Axis::X)
		{
			handleGeo.w = handlePrefSize.w * (contentGeo.h.qpix / float(handlePrefSize.h.qpix));
			handleGeo.h = contentGeo.h;

			handleGeo.x = contentGeo.x + (contentGeo.w - handleGeo.w) * value;
			handleGeo.y = contentGeo.y;
		}
		else
		{
			handleGeo.w = contentGeo.w;
			handleGeo.h = handlePrefSize.h * (contentGeo.w.qpix / float(handlePrefSize.w.qpix));

			handleGeo.x = contentGeo.x;
			handleGeo.y = contentGeo.y + contentGeo.h - handleGeo.h - (contentGeo.h - handleGeo.h) * value;
		}

		return handleGeo.aligned();
	}


} // namespace wg
