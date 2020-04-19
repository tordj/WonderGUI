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

#include <wg_slider.h>
#include <wg_gfxdevice.h>
#include <wg_msg.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Slider::TYPEINFO = { "Slider", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Slider::Slider()
	{
	}

	//____ Destructor _____________________________________________________________

	Slider::~Slider()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Slider::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() __________________________________________________________

	Size Slider::preferredSize() const
	{
		if( m_preferredSize.w >= 0 && m_preferredSize.h >= 0 )
			return m_preferredSize;
		else
			return Widget::preferredSize();
	}

	//____ _setPreferredSlideLength() _________________________________________

	void Slider::setPreferredSlideLength(MU length)
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

	void Slider::setSkin(Skin * pSkin)
	{
		Widget::setSkin(pSkin);
	}

	//____ setHandleSkin() ___________________________________________________

	void Slider::setHandleSkin(const Skin_p& pSkin)
	{
		if (pSkin != m_pHandleSkin)
		{
			m_pHandleSkin = pSkin;
			_requestRender();
			_updatePreferredSize();
		}
	}

	//____ setAxis() _________________________________________________________

	void Slider::setAxis(Axis axis)
	{
		if (axis != m_axis)
		{
			m_axis = axis;
			_requestRender();
			_updatePreferredSize();
		}
	}

	//____ setSteps() ________________________________________________________

	void Slider::setSteps(int nbSteps)
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
				setValue(m_value);		// Have the value aligned to the steps.
		}
	}

	//____ setValue() _________________________________________________________

	void Slider::setValue(float value)
	{
		limit(value, 0.f, 1.f);

		if (value != m_value)
			_setValue(value);
	}

	//____ receive() __________________________________________________________

	void Slider::receive(Msg * _pMsg)
	{
		switch (_pMsg->type())
		{
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				auto pMsg = static_cast<InputMsg*>(_pMsg);
				Coord pos = toLocal(pMsg->pointerPos());
				if( _handleGeo(m_size).contains(pos) != m_handleState.isHovered() )
				{
					_setHandleState(m_handleState + StateEnum::Hovered);
				}
				break;
			}

			case MsgType::MouseLeave:
				if (m_handleState.isHovered() && !m_handleState.isPressed() )
					_setHandleState(m_handleState - StateEnum::Hovered);
				break;

			case MsgType::MousePress:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);

				if (pMsg->button() != MouseButton::Left)
					break;

				Coord pos = toLocal(pMsg->pointerPos());
				Rect  handle = _handleGeo(m_size);
				if (handle.contains(pos))
				{
					_setHandleState(m_handleState + StateEnum::Pressed);
					m_valueAtPress = m_value;
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

				Coord pos = toLocal(pMsg->pointerPos());
				Rect  handle = _handleGeo(m_size);
				if (m_handleState.isPressed())
				{
					_setHandleState(m_handleState - StateEnum::Pressed);
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
				if (m_handleState.isPressed())
				{
					auto pMsg = static_cast<MouseDragMsg*>(_pMsg);

					Size contentSize = m_pSkin ? m_size - m_pSkin->contentPaddingSize() : m_size;
					Size handleSize = _handleGeo(m_size).size();
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
					_setValue(newValue);
				}
			}
		}
	}


	//____ _cloneContent() _______________________________________________________

	void Slider::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Slider * pOrg = (Slider*) _pOrg;

		m_preferredSize = pOrg->m_preferredSize;
	}

	//____ _render() __________________________________________________________

	void Slider::_render(GfxDevice * pDevice, const Rect& canvas, const Rect& window)
	{
		if (m_pSkin)
			m_pSkin->render(pDevice, canvas, m_state, m_value);

		if (m_pHandleSkin)
			m_pHandleSkin->render(pDevice, _handleGeo(canvas), m_handleState, m_value);
	}

	//____ _updatePreferredSize() ________________________________________________

	void Slider::_updatePreferredSize()
	{
		Size sz = m_pHandleSkin ? m_pHandleSkin->preferredSize() : Size( 4, 4 );

		if (m_axis == Axis::X)
			sz.w += m_preferredSlideLength;
		else
			sz.h += m_preferredSlideLength;

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

	//____ _setValue() __________________________________________________________

	void Slider::_setValue(float value)
	{
		if (m_nbSteps > 0)
		{
			int val = value * m_nbSteps + 0.5f;
			value = 1.f / val;
		}

		if (value != m_value)
		{
			if (!m_pSkin || m_pSkin->ignoresFraction())
			{
				Rect oldGeo = _handleGeo(m_size);
				m_value = value;
				Rect newGeo = _handleGeo(m_size);
				_requestRender(Rect::getUnion(oldGeo, newGeo));
			}
			else
			{
				m_value = value;
				_requestRender();
			}
		}
	}

	//____ _setHandleState() __________________________________________________

	void Slider::_setHandleState(State state)
	{
		if (m_pHandleSkin)
		{
			if (!m_pHandleSkin->isStateIdentical(state, m_handleState, m_value));
			{
				_requestRender(_handleGeo(m_size));
			}
		}

		m_handleState = state;
	}


	//____ _handleGeo() _______________________________________________________

	Rect Slider::_handleGeo(const Rect& widgetGeo)
	{
		Rect contentGeo = m_pSkin ? m_pSkin->contentRect(widgetGeo, m_state) : widgetGeo;

		Rect handleGeo;

		Size handlePrefSize = m_pHandleSkin->preferredSize();

		if (m_axis == Axis::X)
		{
			handleGeo.w = handlePrefSize.w * (contentGeo.h.qpix / float(handlePrefSize.h.qpix));
			handleGeo.h = contentGeo.h;

			handleGeo.x = contentGeo.x + (contentGeo.w - handleGeo.w) * m_value;
			handleGeo.y = contentGeo.y;
		}
		else
		{
			handleGeo.w = contentGeo.w;
			handleGeo.h = handlePrefSize.h * (contentGeo.w.qpix / float(handlePrefSize.w.qpix));

			handleGeo.x = contentGeo.x;
			handleGeo.y = contentGeo.y + contentGeo.h - handleGeo.h - (contentGeo.h - handleGeo.h) * m_value;
		}

		return handleGeo;
	}


} // namespace wg
