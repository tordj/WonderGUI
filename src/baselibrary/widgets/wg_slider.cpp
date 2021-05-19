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

	Slider::Slider() : handleSkin(this)
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

	//____ _setPreferredSlideLength() _________________________________________

	void Slider::setPreferredSlideLength(pts length)
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

	//____ _preferredSize() __________________________________________________________

	SizeSPX Slider::_preferredSize(int scale) const
	{
		if( scale == m_scale && m_preferredSize.w >= 0 && m_preferredSize.h >= 0)
			return m_preferredSize;
		else
			return _calcPreferredSize(scale);
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
				CoordSPX pos = _toLocal(pMsg->_pointerPos());
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

				CoordSPX pos = _toLocal(pMsg->_pointerPos());
				RectSPX  handle = _handleGeo(m_size);
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

					SizeSPX contentSize = m_size - OO(skin)._contentPaddingSize(m_scale);
					SizeSPX handleSize = _handleGeo(m_size).size();
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

	void Slider::_render(GfxDevice * pDevice, const RectSPX& canvas, const RectSPX& window)
	{
			OO(skin)._render(pDevice, canvas, m_scale, m_state, m_value);

			OO(handleSkin)._render(pDevice, _handleGeo(canvas), m_scale, m_handleState, m_value);
	}

	//____ _alphaTest() ________________________________________________________

	bool Slider::_alphaTest(const CoordSPX& ofs)
	{
		bool bMarked = OO(skin)._markTest(ofs, RectSPX(m_size), m_scale, m_state, m_markOpacity, m_value);

		if (!bMarked && !handleSkin.isEmpty())
			bMarked = OO(handleSkin)._markTest(ofs, _handleGeo(m_size), m_scale, m_handleState, m_markOpacity, m_value);

		return bMarked;
	}

	//____ _updatePreferredSize() ________________________________________________

	void Slider::_updatePreferredSize(bool bRequestResize)
	{	
		SizeSPX sz = _calcPreferredSize(m_scale);
		if (sz != m_preferredSize)
		{
			m_preferredSize = sz;
			if( bRequestResize )
				_requestResize();
		}
	}

	//____ _calcPreferredSize() ________________________________________________

	SizeSPX Slider::_calcPreferredSize(int scale) const
	{
		SizeSPX sz = handleSkin.isEmpty() ? SizeSPX(4, 4) : OO(handleSkin)._preferredSize(scale);

		if (m_axis == Axis::X)
			sz.w += align(ptsToSpx(m_preferredSlideLength,scale));
		else
			sz.h += align(ptsToSpx(m_preferredSlideLength,scale));

		sz += OO(skin)._contentPaddingSize(scale);
		sz = SizeSPX::max(sz, OO(skin)._preferredSize(scale));

		return sz;
	}


	//____ _resize() ____________________________________________________________

	void Slider::_resize(const SizeSPX& size, int scale)
	{
		Widget::_resize(size, scale);
		_updatePreferredSize(false);			// Size is already decided for us, we just need to update m_preferredSize.
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
			
			RectSPX oldGeo = _handleGeo(m_size);
			float	oldValue = m_value;
			m_value = value;
			RectSPX newGeo = _handleGeo(m_size);

			RectSPX changeRect = RectSPX::getUnion(oldGeo, newGeo);
			_requestRender(changeRect);

			OO(skin)._valueChanged(m_value, oldValue);

			if(bPostMsg)
				Base::msgRouter()->post(ValueUpdateMsg::create(this, 0, m_value, false));
		}
	}

	//____ _setHandleState() __________________________________________________

	void Slider::_setHandleState(State state)
	{
		State oldState = m_state;
		m_handleState = state;

		OO(handleSkin)._stateChanged(m_state, oldState);
	}


	//____ _handleGeo() _______________________________________________________

	RectSPX Slider::_handleGeo(const RectSPX& widgetGeo) const
	{
		RectSPX contentGeo =  OO(skin)._contentRect(widgetGeo, m_scale, m_state);

		RectSPX handleGeo;

		SizeSPX handlePrefSize = OO(handleSkin)._preferredSize(m_scale);

		if (m_axis == Axis::X)
		{
			handleGeo.w = handlePrefSize.w * (contentGeo.h / float(handlePrefSize.h));
			handleGeo.h = contentGeo.h;

			handleGeo.x = contentGeo.x + (contentGeo.w - handleGeo.w) * m_value;
			handleGeo.y = contentGeo.y;
		}
		else
		{
			handleGeo.w = contentGeo.w;
			handleGeo.h = handlePrefSize.h * (contentGeo.w / float(handlePrefSize.w));

			handleGeo.x = contentGeo.x;
			handleGeo.y = contentGeo.y + contentGeo.h - handleGeo.h - (contentGeo.h - handleGeo.h) * m_value;
		}

		return align(handleGeo);
	}

		//____ _componentState() _______________________________________________________

	State Slider::_componentState(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return m_handleState;
		else
			return m_state;
	}

	//____ _componentPos() ___________________________________________________

	CoordSPX Slider::_componentPos(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return _handleGeo(m_size).pos();
		else
			return CoordSPX();
	}

	//____ _componentSize() ___________________________________________________

	SizeSPX Slider::_componentSize(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return _handleGeo(m_size).size();
		else
			return m_size;
	}

	//____ _componentGeo() ___________________________________________________

	RectSPX Slider::_componentGeo(const GeoComponent* pComponent) const
	{
		if (pComponent == &handleSkin)
			return _handleGeo(m_size);
		else
			return m_size;
	}

	//____ _componentRequestRender() _______________________________________________

	void Slider::_componentRequestRender(const GeoComponent* pComponent)
	{
		if (pComponent == &handleSkin)
			_requestRender(_handleGeo(m_size));
		else
			_requestRender();
	}

	void Slider::_componentRequestRender(const GeoComponent* pComponent, const RectSPX& rect)
	{
		if (pComponent == &handleSkin)
			_requestRender(rect + _handleGeo(m_size).pos());
		else
			_requestRender(rect);
	}

	//____ Slider::_skinChanged() ________________________________________________

	void Slider::_skinChanged(const CSkinSlot* pSlot, Skin* pNewSkin, Skin* pOldSkin)
	{
		Widget::_skinChanged(pSlot, pNewSkin, pOldSkin);
		_updatePreferredSize();
	}

	//____ _skinValue() _______________________________________________________

	float Slider::_skinValue(const CSkinSlot* pSlot) const
	{
		return m_value;
	}

} // namespace wg
