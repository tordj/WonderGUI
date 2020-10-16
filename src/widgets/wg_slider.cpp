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
#include <wg_base.h>
#include <wg_msgrouter.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Slider::TYPEINFO = { "Slider", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Slider::Slider() : m_handleSkin(this)
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
		m_handleSkin.setSkin(pSkin);
		_updatePreferredSize();
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
				_setValue(m_value);		// Have the value aligned to the steps.
		}
	}

	//____ setValue() _________________________________________________________

	void Slider::setValue(float value)
	{
		limit(value, 0.f, 1.f);
		_setValue(value,false);
	}

	//____ _receive() __________________________________________________________

	void Slider::_receive(Msg * _pMsg)
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

				if (m_handleState.isPressed())
				{
					_setHandleState(m_handleState - StateEnum::Pressed);
				}
				else
				{
//					Coord pos = toLocal(pMsg->pointerPos());
//					Rect  handle = _handleGeo(m_size);

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

					Size contentSize = m_size - m_skin.contentPaddingSize();
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
				
			default:
				break;
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
			m_skin.render(pDevice, canvas, m_state, m_value);

			m_handleSkin.render(pDevice, _handleGeo(canvas), m_handleState, m_value);
	}

	//____ _alphaTest() ________________________________________________________

	bool Slider::_alphaTest(const Coord& ofs)
	{
		bool bMarked = m_skin.markTest(ofs, Rect(m_size), m_state, m_markOpacity, m_value);

		if (!bMarked && !m_handleSkin.isEmpty())
			bMarked = m_handleSkin.markTest(ofs, _handleGeo(m_size), m_handleState, m_markOpacity, m_value);

		return bMarked;
	}


	//____ _updatePreferredSize() ________________________________________________

	void Slider::_updatePreferredSize()
	{
		Size sz = m_handleSkin.isEmpty() ? Size(4,4) : m_handleSkin.preferredSize();

		if (m_axis == Axis::X)
			sz.w += m_preferredSlideLength;
		else
			sz.h += m_preferredSlideLength;

		sz += m_skin.contentPaddingSize();
		sz = Size::max(sz, m_skin.preferredSize());

		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			_requestResize();
		}
	}

	//____ _setValue() __________________________________________________________

	void Slider::_setValue(float value, bool bPostMsg)
	{
		if (m_nbSteps > 0)
		{
			int val = int(value * m_nbSteps + 0.5f);
			value = 1.f / val;
		}

		if (value != m_value)
		{
			
			Rect oldGeo = _handleGeo(m_size);
			float	oldValue = m_value;
			m_value = value;
			Rect newGeo = _handleGeo(m_size);

			Rect changeRect = Rect::getUnion(oldGeo, newGeo);
			_requestRender(changeRect);

			m_skin.valueChanged(m_value, oldValue);

			if(bPostMsg)
				Base::msgRouter()->post(ValueUpdateMsg::create(this, 0, m_value, false));
		}
	}

	//____ _setHandleState() __________________________________________________

	void Slider::_setHandleState(State state)
	{
		State oldState = m_state;
		m_handleState = state;

		m_handleSkin.stateChanged(m_state, oldState);
	}


	//____ _handleGeo() _______________________________________________________

	Rect Slider::_handleGeo(const Rect& widgetGeo) const
	{
		Rect contentGeo =  m_skin.contentRect(widgetGeo, m_state);

		Rect handleGeo;

		Size handlePrefSize = m_handleSkin.preferredSize();

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

		return handleGeo.aligned();
	}

	//____ _skinRequestRender() _______________________________________________

	void Slider::_skinRequestRender(const SkinSlot* pSlot)
	{
		if (pSlot == &m_handleSkin)
			_requestRender(_handleGeo(m_size));
		else
			_requestRender();
	}

	void Slider::_skinRequestRender(const SkinSlot* pSlot, const Rect& rect)
	{
		if (pSlot == &m_handleSkin)
			_requestRender( rect + _handleGeo(m_size).pos() );
		else
			_requestRender( rect );
	}

	//____ _skinSize() ________________________________________________________

	Size Slider::_skinSize(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_handleSkin)
			return _handleGeo(m_size).size();
		else
			return m_size;

	}

	//____ _skinGlobalPos() ___________________________________________________

	Coord Slider::_skinGlobalPos(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_handleSkin)
			return globalPos() + _handleGeo(m_size).pos();
		else
			return globalPos();
	}

	//____ _skinState() _______________________________________________________

	State Slider::_skinState(const SkinSlot* pSlot) const
	{
		if (pSlot == &m_handleSkin)
			return m_handleState;
		else
			return m_state;
	}

	//____ _skinValue() _______________________________________________________

	float Slider::_skinValue(const SkinSlot* pSlot) const
	{
		return m_value;
	}

} // namespace wg
