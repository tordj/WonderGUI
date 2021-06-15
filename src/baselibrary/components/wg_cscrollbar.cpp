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

#include <wg_cscrollbar.h>


namespace wg
{
	const TypeInfo CScrollbar::TYPEINFO = { "CScrollbar", &GeoComponent::TYPEINFO };

	using namespace Util;

	//____ constructor ____________________________________________________________

	CScrollbar::CScrollbar(GeoComponent::Holder* pHolder, CScrollbar::Holder * pHolderInterface, Axis axis) : 
		GeoComponent(pHolder),m_pHolderInterface(pHolderInterface), m_axis(axis)
	{
	}

	//____ destructor _________________________________________________________

	CScrollbar::~CScrollbar()
	{

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CScrollbar::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSkins() _________________________________________________________
	/**
	 * Set all skins of this scrollbar in one go.
	 *
	 * @param pBackgroundSkin	Background skin for the scrollbar handle area.
	 * @param pHandleSkin		Skin for the scrollbar handle.
	 * @param pBwdButtonSkin	Skin for the backward (up or left) button.
	 * @param pFwdButtonSkin	Skin for the forward (down or right) button.
	 *
	 **/
	void CScrollbar::setSkins(Skin* pBackground, Skin* pBar, Skin* pPrevButton, Skin* pNextButton)
	{
		int scale = _scale();

		bool	bRequestRender = false;
		bool	bRequestResize = false;

		Skin* skin[4] = { pBackground, pBar, pPrevButton, pNextButton };

		for (int i = 0; i < 4; i++)
		{
			if (skin[i] != m_skins[i])
			{
				SizeSPX oldPref = m_skins[i] ? m_skins[i]->_preferredSize(scale) : SizeSPX();
				SizeSPX newPref = skin[i] ? skin[i]->_preferredSize(scale) : SizeSPX();

				m_skins[i] = skin[i];
				bRequestRender = true;

				if (oldPref != newPref)
					bRequestResize = true;
			}
		}

		if (bRequestRender)
			_requestRender();

		if (bRequestResize)
			_requestResize();
	}

	//____ setBackground() ____________________________________________________

	void CScrollbar::setBackground(Skin* pSkin)
	{
		_replaceSkin(pSkin, m_skins[Part::Back]);
	}

	//____ setBar() _______________________________________________________

	void CScrollbar::setBar(Skin* pSkin)
	{
		_replaceSkin(pSkin, m_skins[Part::Bar]);
	}

	//____ setForwardButton() _________________________________________________

	void CScrollbar::setForwardButton(Skin* pSkin)
	{
		_replaceSkin(pSkin, m_skins[Part::Next]);
	}

	//____ setBackwardButton() ________________________________________________

	void CScrollbar::setBackwardButton(Skin* pSkin)
	{
		_replaceSkin(pSkin, m_skins[Part::Prev]);
	}

	//____ setJumpToPress() ___________________________________________________

	void CScrollbar::setJumpToPress(bool bJump)
	{
		m_bJumpToPress = bJump;
	}

	//____ _preferredSize() ___________________________________________________

	SizeSPX CScrollbar::_preferredSize(int scale) const
	{
		SizeSPX prefSize = m_skins[Part::Bar] ? m_skins[Part::Bar]->_preferredSize(scale) : SizeSPX();

		if (m_skins[Part::Back])
			prefSize += m_skins[Part::Back]->_contentPaddingSize(scale);

		if (m_axis == Axis::X)
		{
			if (m_skins[Part::Next])
			{
				SizeSPX pref = m_skins[Part::Next]->_preferredSize(scale);
				prefSize.h = std::max(prefSize.h, pref.h);
				prefSize.w += pref.w;
			}

			if (m_skins[Part::Prev])
			{
				SizeSPX pref = m_skins[Part::Prev]->_preferredSize(scale);
				prefSize.h = std::max(prefSize.h, pref.h);
				prefSize.w += pref.w;
			}

			prefSize.w += 20 * scale;
		}
		else
		{
			if (m_skins[Part::Next])
			{
				SizeSPX pref = m_skins[Part::Next]->_preferredSize(scale);
				prefSize.w = std::max(prefSize.w, pref.w);
				prefSize.h += pref.h;
			}

			if (m_skins[Part::Prev])
			{
				SizeSPX pref = m_skins[Part::Prev]->_preferredSize(scale);
				prefSize.w = std::max(prefSize.w, pref.w);
				prefSize.h += pref.h;
			}

			prefSize.h += 20 * scale;
		}

		return prefSize;
	}

	//____ _render() __________________________________________________________

	void CScrollbar::_render(GfxDevice* pDevice, const RectSPX& canvas, int scale)
	{
		for (int i = 0; i < Part_size; i++)
		{
			if (m_skins[i])
				m_skins[i]->_render(pDevice, _partGeo((Part)i, canvas, scale), scale, m_states[i] );
		}
	}

	//____ _receive() _________________________________________________________

	void CScrollbar::_receive(Msg* _pMsg)
	{
		switch (_pMsg->type())
		{
			case MsgType::MouseEnter:		// Continue to MouseMove
			case MsgType::MouseLeave:		// Continue to MouseMove
			case MsgType::MouseMove:
			{
				auto pMsg = static_cast<MouseMoveMsg*>(_pMsg);

				RectSPX geo = _geo();
				Part markedPart = _getMarkedPart(geo.size(), pMsg->_pointerPos() - _globalPos() );

				if (markedPart != m_markedPart)
				{
					int scale = _scale();

					if (m_markedPart != Part::None)
						_setPartState(	m_markedPart, m_states[m_markedPart] - StateEnum::Hovered,
										m_states[m_markedPart], geo, scale);

					if (markedPart != Part::None)
						_setPartState(	markedPart, m_states[markedPart] + StateEnum::Hovered,
										m_states[markedPart], geo, scale);

					m_markedPart = markedPart;
				}

				break;
			}

			case MsgType::MousePress:
			{
				if (m_markedPart == Part::None)
					break;

				RectSPX geo = _geo();
				int scale = _scale();

				switch (m_markedPart)
				{			
					case Part::Bar:
						break;
					case Part::Back:
					{
						auto pMsg = static_cast<MousePressMsg*>(_pMsg);
						CoordSPX pointerPos = pMsg->_pointerPos() - _globalPos();

						auto barCenter = _partGeo(Part::Bar, geo, scale).center();
						if (m_axis == Axis::X && pointerPos.x < barCenter.x ||
							m_axis == Axis::Y && pointerPos.y < barCenter.y)
							m_pHolderInterface->_scrollbarPage(this, -1);
						else
							m_pHolderInterface->_scrollbarPage(this, 1);
						break;
					}
					case Part::Prev:
						m_pHolderInterface->_scrollbarStep(this, -1);
						break;
					case Part::Next:
						m_pHolderInterface->_scrollbarStep(this, 1);
						break;
					default:				// Just to avoid compiler warnings
						break;
				}

				_setPartState(m_markedPart, m_states[m_markedPart] + StateEnum::Pressed,
					m_states[m_markedPart], geo, scale);
				_pMsg->swallow();
				break;
			}
			case MsgType::MouseRepeat:
			{
				if (m_markedPart == Part::None)
					break;

				switch (m_markedPart)
				{
				case Part::Bar:
					break;
				case Part::Back:
				{
					RectSPX geo = _geo();
					int scale = _scale();

					auto pMsg = static_cast<MousePressMsg*>(_pMsg);
					CoordSPX pointerPos = pMsg->_pointerPos() - _globalPos();

					auto barCenter = _partGeo(Part::Bar, geo, scale).center();
					if (m_axis == Axis::X && pointerPos.x < barCenter.x ||
						m_axis == Axis::Y && pointerPos.y < barCenter.y)
						m_pHolderInterface->_scrollbarPage(this, -1);
					else
						m_pHolderInterface->_scrollbarPage(this, 1);
					break;
				}
				case Part::Prev:
					m_pHolderInterface->_scrollbarStep(this, -1);
					break;
				case Part::Next:
					m_pHolderInterface->_scrollbarStep(this, 1);
					break;
				default:				// Just to avoid compiler warnings
					break;
				}

				_pMsg->swallow();
				break;
			}
			case MsgType::MouseRelease:
			{
				if (m_markedPart == Part::None)
					break;

				RectSPX geo = _geo();
				int scale = _scale();

				_setPartState(m_markedPart, m_states[m_markedPart] - StateEnum::Pressed,
					m_states[m_markedPart], geo, scale);
				_pMsg->swallow();
				break;
			}
			case MsgType::MouseDrag:


				break;

			case MsgType::WheelRoll:
			{
				auto pMsg = static_cast<WheelRollMsg*>(_pMsg);
				int dir = pMsg->distance().y;

				if (dir != 0)
					m_pHolderInterface->_scrollbarWheel(this, dir);
				break;
			}
		}

	}

	//____ _setState() ________________________________________________________

	void CScrollbar::_setState(State state)
	{
		// We just copy Enabled/Focused/Selected to all our skins.
		// We ignore Targeted since that won't work without hover anyway.

		bool bEnabled = state.isEnabled();
		bool bFocused = state.isFocused();
		bool bSelected = state.isSelected();

		if (bEnabled != m_states[Part::Back].isEnabled() || bFocused != m_states[Part::Back].isFocused() ||
			bSelected != m_states[Part::Back].isSelected() )
		{
			for (int i = 0; i < Part_size; i++)
			{
				m_states[i].setEnabled(bEnabled);
				m_states[i].setFocused(bFocused);
				m_states[i].setSelected(bSelected);
			}

			_requestRender();
		}
	}

	//____ _setAxis() _________________________________________________________

	void CScrollbar::_setAxis(Axis axis)
	{
		// No request render here. We assume it is done anyway.

		m_axis = axis;
	}

	//____ _alphaTest() _______________________________________________________

	bool CScrollbar::_alphaTest(CoordSPX ofs, SizeSPX canvas, int markOpacity)
	{
		//TODO: Support rounded outer corners on buttons?

		return (markOpacity > 0);
	}

	//____ _update() __________________________________________________________

	void CScrollbar::_update(spx newViewPos, spx oldViewPos, spx newViewLen, spx oldViewLen, spx newContentLen, spx oldContentLen)
	{
		RectSPX canvas = _size();
		int scale = _scale();

		RectSPX sliderBack = _partGeo(Part::Back, canvas, scale);
		if (m_skins[Part::Back])
			sliderBack = m_skins[Part::Back]->_contentRect(sliderBack, scale, m_states[Part::Back]);

		RectSPX oldBar = _dragbarArea(sliderBack, scale, oldViewPos, oldViewLen, oldContentLen );
		RectSPX newBar = _dragbarArea(sliderBack, scale, newViewPos, newViewLen, newContentLen);

		RectSPX dirt = RectSPX::getUnion(oldBar, newBar);

		if( !dirt.isEmpty() )
			_requestRender(dirt);
	}


	//____ _getMarkedPart() ___________________________________________________
	//
	// We accept bar being marked even if we are beside it. This is per design.

	CScrollbar::Part CScrollbar::_getMarkedPart(const RectSPX& canvas, CoordSPX pos) const
	{
		if (!canvas.contains(pos))
			return Part::None;

		int scale = _scale();
		int nextButtonLen = _buttonLen(Part::Next, canvas, scale);
		int prevButtonLen = _buttonLen(Part::Prev, canvas, scale);


		if (m_axis == Axis::X)
		{
			if (pos.x < canvas.x + prevButtonLen)
				return Part::Prev;

			if (pos.x >= canvas.x + canvas.w - nextButtonLen)
				return Part::Next;

			RectSPX backContent = { canvas.x + prevButtonLen, canvas.y, canvas.w - prevButtonLen - nextButtonLen, canvas.h };
			if (m_skins[Part::Back])
				backContent = m_skins[Part::Back]->_contentRect(backContent, scale, m_states[Part::Back]);

			RectSPX bar = _dragbarArea(backContent, scale);
			if (pos.x >= bar.x && pos.x < bar.x + bar.w)
				return Part::Bar;
		}

		if (m_axis == Axis::Y)
		{
			if (pos.y < canvas.y + prevButtonLen)
				return Part::Prev;

			if (pos.y >= canvas.y + canvas.h - nextButtonLen)
				return Part::Next;

			RectSPX backContent = { canvas.x, canvas.y + prevButtonLen, canvas.w, canvas.h - prevButtonLen - nextButtonLen };
			if (m_skins[Part::Back])
				backContent = m_skins[Part::Back]->_contentRect(backContent, scale, m_states[Part::Back]);

			RectSPX bar = _dragbarArea(backContent, scale);
			if (pos.y >= bar.y && pos.y < bar.y + bar.h)
				return Part::Bar;
		}

		return Part::Back;
	}	

	//____ _partGeo() _______________________________________________

	RectSPX CScrollbar::_partGeo(Part part, const RectSPX& canvas, int scale) const
	{
		int nextButtonLen = _buttonLen(Part::Next, canvas, scale);
		int prevButtonLen = _buttonLen(Part::Prev, canvas, scale);

		switch (part)
		{
			case Part::Back:
				if (m_axis == Axis::X)
					return { canvas.x + prevButtonLen, canvas.y, canvas.w - prevButtonLen - nextButtonLen, canvas.h };
				else
					return { canvas.x, canvas.y + prevButtonLen, canvas.w, canvas.h - prevButtonLen - nextButtonLen };
			case Part::Bar:
			{
				RectSPX availableArea;
				if (m_axis == Axis::X)
					availableArea = { canvas.x + prevButtonLen, canvas.y, canvas.w - prevButtonLen - nextButtonLen, canvas.h };
				else
					availableArea = { canvas.x, canvas.y + prevButtonLen, canvas.w, canvas.h - prevButtonLen - nextButtonLen };

				if (m_skins[Part::Back])
					availableArea = m_skins[Part::Back]->_contentRect(availableArea, scale, m_states[Part::Back]);

				return  _dragbarArea(availableArea, scale);
			}
			case Part::Prev:
				if (m_axis == Axis::X)
					return { canvas.x, canvas.y, prevButtonLen, canvas.h };
				else
					return { canvas.x, canvas.y, canvas.w, prevButtonLen };
			case Part::Next:
				if (m_axis == Axis::X)
					return { canvas.x + canvas.w - nextButtonLen, canvas.y, nextButtonLen, canvas.h };
				else
					return { canvas.x, canvas.y + canvas.h - nextButtonLen, canvas.w, nextButtonLen };
				break;
		}
	}

	//____ _setPartState() _________________________________________________

	void CScrollbar::_setPartState(Part part, State newState, State oldState, const RectSPX& fullCanvas, int scale )
	{
		if (part == Part::None)
			return;

		if (m_skins[part])
		{
			RectSPX dirt = m_skins[part]->_dirtyRect(_partGeo(part, fullCanvas, scale), scale, newState, oldState );

			if (!dirt.isEmpty())
				_requestRender(dirt);

			m_states[part] = newState;
		}
	}

	//____ _replaceSkin() _____________________________________________________

	void CScrollbar::_replaceSkin(Skin* pNew, Skin_p& pOld)
	{
		if (pNew != pOld)
		{
			int scale = _scale();
			SizeSPX oldPref = pOld ? pOld->_preferredSize(scale) : SizeSPX();
			SizeSPX newPref = pNew ? pNew->_preferredSize(scale) : SizeSPX();

			pOld = pNew;
			_requestRender();

			if (oldPref != newPref)
				_requestResize();
		}
	}

	//____ _buttonLen() _____________________________________________________

	spx CScrollbar::_buttonLen(Part button, const RectSPX& canvas, int scale) const
	{
		if (m_skins[button])
		{
			SizeSPX buttonSize = m_skins[button]->_preferredSize(scale);

			if (m_axis == Axis::X)
			{
				if (buttonSize.h != canvas.h)
					buttonSize.w *= canvas.h / (float) buttonSize.h;

				return buttonSize.w;
			}
			else
			{
				if (buttonSize.w != canvas.w)
					buttonSize.h *= canvas.w / (float)buttonSize.w;

				return buttonSize.h;
			}
		}
		else 
			return 0;
	}

	//____ _dragbarArea() _____________________________________________________

	RectSPX CScrollbar::_dragbarArea(const RectSPX& availableArea, int scale) const
	{
			spx viewOfs;
			spx viewLen;
			spx contentLen;
			std::tie(viewOfs, viewLen, contentLen) = m_pHolderInterface->_scrollbarOfsLenContent(this);
			
			return _dragbarArea(availableArea, scale, viewOfs, viewLen, contentLen);
	}

	RectSPX CScrollbar::_dragbarArea(const RectSPX& availableArea, int scale, spx viewOfs, spx viewLen, spx contentLen) const
	{
		float fracLen = float(viewLen) / float(contentLen);
		float fracOfs = float(viewOfs) / float(contentLen - viewLen);

		if (m_axis == Axis::X)
		{
			spx length = align(availableArea.w * fracLen);
			spx ofs = align((availableArea.w - length) * fracOfs);

			return { availableArea.x + ofs, availableArea.y, length, availableArea.h };
		}
		else
		{
			spx length = align(availableArea.h * fracLen);
			spx ofs = align((availableArea.h - length) * fracOfs);

			return { availableArea.x, availableArea.y + ofs, availableArea.w, length };
		}
	}

}