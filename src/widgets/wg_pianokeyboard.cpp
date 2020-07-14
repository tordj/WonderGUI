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

#include <wg_pianokeyboard.h>
#include <wg_gfxdevice.h>
#include <wg_msg.h>
#include <wg_base.h>
#include <wg_msgrouter.h>

//#include <wg_boxstateskin.h>

namespace wg
{
	using namespace Util;

	const TypeInfo PianoKeyboard::TYPEINFO = { "PianoKeyboard", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	PianoKeyboard::PianoKeyboard()
	{
	}

	//____ Destructor _____________________________________________________________

	PianoKeyboard::~PianoKeyboard()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PianoKeyboard::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setLayout() ________________________________________________________

	void PianoKeyboard::setLayout(int nbWhiteKeys, const std::bitset<7>& blackKeyPositions)
	{
		if (nbWhiteKeys > c_maxKeys || nbWhiteKeys < 2)
		{
			//TODO: Error handling!

			return;
		}

		m_nbWhiteKeys = nbWhiteKeys;
		m_blackKeyPositions = blackKeyPositions;

		// Count and enumerate the keys

		KeyInfo * pInfo = m_keyInfo;
		int blackKeyIdx = 6;

		for (int i = 0; i < nbWhiteKeys-1; i++)
		{
			pInfo->keyPos = i;
			pInfo->isBlack = false;
			pInfo->isSetToPressed = false;
			pInfo++;

			if (blackKeyPositions.test(blackKeyIdx))
			{
				pInfo->keyPos = i;
				pInfo->isBlack = true;
				pInfo->isSetToPressed = false;
				pInfo++;
			}

			if (blackKeyIdx == 0)
				blackKeyIdx = 6;
			else
				blackKeyIdx--;
		}

		pInfo->keyPos = nbWhiteKeys - 1;
		pInfo->isBlack = false;
		pInfo->isSetToPressed = false;
		pInfo++;

		m_nbKeys = pInfo - m_keyInfo;
	}

	//____ setSurfaces() ______________________________________________________

	void PianoKeyboard::setSurfaces(const Surface_p& pOddWhiteKeys, const Surface_p& pEvenWhiteKeys, 
									const Surface_p& pBlackKeys, std::initializer_list<State> states)
	{
		if (!pOddWhiteKeys || !pEvenWhiteKeys || (!pBlackKeys && m_blackKeyPositions.count() > 0))
		{
			//TODO: Error handling!

			return;
		}

		if (pOddWhiteKeys->size() != pEvenWhiteKeys->size() || (pBlackKeys && pBlackKeys->size().w != pOddWhiteKeys->size().w) )
		{
			//TODO: Error handling!
			
			return;
		}

		m_pOddWhiteKeys		= pOddWhiteKeys;
		m_pEvenWhiteKeys	= pEvenWhiteKeys;
		m_pBlackKeys		= pBlackKeys;

		SizeI pointSize = pOddWhiteKeys->pointSize();
		pointSize.h /= states.size();

		m_preferredKeyboardSize	= pointSize;
		m_keyboardSourceSize	= pointSize * pOddWhiteKeys->qpixPerPoint()/4;

		m_blackKeyHeight		= m_pBlackKeys ? (MU(m_pBlackKeys->pointSize().h) / states.size()).aligned() : MU(0);
		m_blackKeySourceHeight	= m_pBlackKeys ? m_pBlackKeys->size().h / states.size() : 0;

		// Fill in state offsets

		for (auto& ofs : m_stateOfsY)
			ofs = -1;

		Bitmask<uint32_t>	stateMask;
		int ofs = 0;
		for (auto state : states)
		{
			int i = _stateToIndex(state);
			stateMask.setBit(i);
			m_stateOfsY[i] = ofs++;
		}

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!stateMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, stateMask);
				m_stateOfsY[i] = m_stateOfsY[bestAlternative];
			}
		}
	}


	//____ pressKey() ________________________________________________________

	void PianoKeyboard::pressKey(int keyIdx)
	{
		if (keyIdx < 0 || keyIdx >= m_nbKeys)
		{
			//TODO: Error handling!
			return;
		}

		_setAPIPressed(keyIdx, true);
	}

	//____ releaseKey() ______________________________________________________

	void PianoKeyboard::releaseKey(int keyIdx)
	{
		if (keyIdx < 0 || keyIdx >= m_nbKeys)
		{
			//TODO: Error handling!
			return;
		}

		_setAPIPressed(keyIdx, false);
	}

	//____ isKeyPressed() ____________________________________________________

	bool PianoKeyboard::isKeyPressed(int keyIdx) const
	{
		if (keyIdx < 0 || keyIdx >= m_nbKeys)
		{
			//TODO: Error handling!
			return false;
		}

		return m_keyInfo[keyIdx].state.isPressed();
	}

	//____ setPressedKeys() __________________________________________________

	void PianoKeyboard::setPressedKeys(std::initializer_list<int> pressedKeys)
	{
		// Error handling

		for (auto idx : pressedKeys)
			if (idx < 0 || idx >= m_nbKeys)
			{
				//TODO: Error handling!
				return;
			}

		// First, release all that are not pressed anymore

		for (int i = 0; i < m_nbKeys; i++)
		{
			if (m_keyInfo[i].isSetToPressed)
			{
				bool bRemainPressed = false;

				for (auto idx : pressedKeys)
					if (idx == i)
					{
						bRemainPressed = true;
						break;
					}

				if (!bRemainPressed)
					_setAPIPressed(i,false);					
			}
		}

		// Secondly, select all newly selected

		for (auto idx : pressedKeys)
			_setAPIPressed(idx, true);
	}

	//____ releaseAllKeys() __________________________________________________

	void PianoKeyboard::releaseAllKeys()
	{
		for (int i = 0; i < m_nbKeys; i++)
			_setAPIPressed(i, false);
	}

	//____ setFlipKeyOnPress() ________________________________________________

	void PianoKeyboard::setFlipKeyOnPress(bool bFlip)
	{
		m_bFlipOnPress = bFlip;
	}

	//____ selectKey() ________________________________________________________

	void PianoKeyboard::selectKey(int keyIdx)
	{
		if (keyIdx < 0 || keyIdx >= m_nbKeys)
		{
			//TODO: Error handling!
			return;
		}

		if (!m_keyInfo[keyIdx].state.isSelected())
		{
			m_keyInfo[keyIdx].state.setSelected(true);
			_requestRenderKey(keyIdx);
		}
	}

	//____ unselectKey() ______________________________________________________

	void PianoKeyboard::unselectKey(int keyIdx)
	{
		if (keyIdx < 0 || keyIdx >= m_nbKeys)
		{
			//TODO: Error handling!
			return;
		}

		if (m_keyInfo[keyIdx].state.isSelected())
		{
			m_keyInfo[keyIdx].state.setSelected(false);
			_requestRenderKey(keyIdx);
		}
	}

	//____ isKeySelected() ____________________________________________________

	bool PianoKeyboard::isKeySelected(int keyIdx) const
	{
		if (keyIdx < 0 || keyIdx >= m_nbKeys)
		{
			//TODO: Error handling!
			return false;
		}

		return m_keyInfo[keyIdx].state.isSelected();
	}

	//____ setSelectedKeys() __________________________________________________

	void PianoKeyboard::setSelectedKeys(std::initializer_list<int> selectedKeys)
	{
		// Error handling

		for (auto idx : selectedKeys)
			if (idx < 0 || idx >= m_nbKeys)
			{
				//TODO: Error handling!
				return;
			}

		// First, unselect all that are not selected anymore

		for (int i = 0; i < m_nbKeys; i++)
		{
			if( m_keyInfo[i].state.isSelected() )
			{
				bool bRemainSelected = false;

				for( auto idx : selectedKeys )
					if (idx == i)
					{
						bRemainSelected = true;
						break;
					}

				if (!bRemainSelected)
				{
					m_keyInfo[i].state.setSelected(false);
					_requestRenderKey(i);
				}
			}
		}

		// Secondly, select all newly selected

		for (auto idx : selectedKeys)
			if( !m_keyInfo[idx].state.isSelected() )
			{
				m_keyInfo[idx].state.setSelected(false);
				_requestRenderKey(idx);
			}
	}

	//____ unselectAllKeys() __________________________________________________

	void PianoKeyboard::unselectAllKeys()
	{
		for (int i = 0; i < m_nbKeys; i++)
		{
			if (m_keyInfo[i].state.isSelected())
			{
				m_keyInfo[i].state.setSelected(false);
				_requestRenderKey(i);
			}
		}
	}

	//____ preferredSize() __________________________________________________________

	Size PianoKeyboard::preferredSize() const
	{
		return m_preferredKeyboardSize + (m_pSkin ? m_pSkin->contentPaddingSize() : Size());
	}

	//____ minSize() __________________________________________________________

	Size PianoKeyboard::minSize() const
	{
		return m_preferredKeyboardSize + (m_pSkin ? m_pSkin->contentPaddingSize() : Size());
	}

	//____ maxSize() __________________________________________________________

	Size PianoKeyboard::maxSize() const
	{
		return m_preferredKeyboardSize + (m_pSkin ? m_pSkin->contentPaddingSize() : Size());
	}


	//____ _cloneContent() _______________________________________________________

	void PianoKeyboard::_cloneContent(const Widget * _pOrg)
	{
		Widget::_cloneContent(_pOrg);

		PianoKeyboard * pOrg = (PianoKeyboard*)_pOrg;

	}

	//____ _render() __________________________________________________________

	void PianoKeyboard::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window)
	{
		//TODO: This could use _keyRect() to simplify and secure consistency.

		Widget::_render(pDevice, _canvas, _window);

		Rect canvas = m_pSkin ? m_pSkin->contentRect(_canvas, m_state) : _canvas;


		MU whiteKeySpacing = canvas.w / m_nbWhiteKeys;
		MU whiteKeyMargin = whiteKeySpacing/2;

		float xScaleFactor = m_keyboardSourceSize.w / float(canvas.w);
		float yScaleFactor = m_keyboardSourceSize.h / float(canvas.h);

		//

		if (m_pBlackKeys)
		{
			KeyInfo * pKey = m_keyInfo;

			pDevice->setBlitSource(m_pBlackKeys);

			for (int i = 0; i < m_nbWhiteKeys-1; i++)
			{
				pKey++;

				if (pKey->isBlack)
				{
					RectI dst = (Rect(i*whiteKeySpacing + whiteKeySpacing / 2, 0, whiteKeySpacing, m_blackKeyHeight)).px();

					float srcOfsY = float(m_stateOfsY[_stateToIndex(pKey->state)] * m_blackKeySourceHeight);

					pDevice->stretchBlit(dst + canvas.pos().px(), RectF(dst.x*xScaleFactor, srcOfsY, dst.w*xScaleFactor, dst.h*yScaleFactor));
					pKey++;
				}
			}
		}

		//


		KeyInfo * pFirst = &m_keyInfo[0];
		KeyInfo * pLast = &m_keyInfo[m_nbKeys-1];

		bool bForward = true;
		while (pFirst <= pLast)
		{
			KeyInfo * pKey = bForward ? pFirst : pLast;

			int i = pKey->keyPos;

			RectI dst = (Rect(i*whiteKeySpacing - whiteKeySpacing / 2, 0, whiteKeySpacing * 2, canvas.h)).px();

			if (i % 2 == 0)
				pDevice->setBlitSource(m_pOddWhiteKeys);
			else
				pDevice->setBlitSource(m_pEvenWhiteKeys);

			if (dst.x < 0)
			{
				dst.w += dst.x;
				dst.x = 0;
			}

			if (dst.x + dst.w > canvas.w.px() )
				dst.w = canvas.w.px() - dst.x;

			float srcOfsY = float(m_stateOfsY[_stateToIndex(pKey->state)] * m_keyboardSourceSize.h);

			pDevice->stretchBlit(dst + canvas.pos().px(), RectF(dst.x*xScaleFactor, srcOfsY, dst.w*xScaleFactor, dst.h*yScaleFactor));

			if (bForward)
			{
				pFirst++;
				if (pFirst->isBlack)
					pFirst++;
			}
			else
			{
				pLast--;
				if (pLast->isBlack)
					pLast--;
			}

			bForward = !bForward;
		}
	}

	//____ _receive() _________________________________________________________

	void PianoKeyboard::_receive(Msg * pMsg)
	{
		switch (pMsg->type())
		{
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				Coord pos = static_cast<InputMsg*>(pMsg)->pointerPos() - globalPos();
				int keyIdx = _markTestKey(pos);
				_setHoveredKey(keyIdx, static_cast<InputMsg*>(pMsg)->timestamp());
				break;
			}

			case MsgType::MouseLeave:
			{
				_setHoveredKey(-1, static_cast<InputMsg*>(pMsg)->timestamp());
				break;
			}

			case MsgType::MousePress:
			{
				if (m_hoveredKey >= 0)
				{
					if (m_bFlipOnPress)
					{
						_setAPIPressed(m_hoveredKey, !m_keyInfo[m_hoveredKey].isSetToPressed);
					
						if(m_keyInfo[m_hoveredKey].isSetToPressed)
							Base::msgRouter()->post(new PianoKeyPressMsg(this, m_hoveredKey, static_cast<InputMsg*>(pMsg)->timestamp()));
						else
							Base::msgRouter()->post(new PianoKeyReleaseMsg(this, m_hoveredKey, static_cast<InputMsg*>(pMsg)->timestamp()));
					}
					else
					{
						_setHoverPress(true, static_cast<InputMsg*>(pMsg)->timestamp());
					}
				}
				break;
			}

			case MsgType::MouseRelease:
			{
				if (m_hoveredKey >= 0)
				{
					if (!m_bFlipOnPress)
						_setHoverPress(false, static_cast<InputMsg*>(pMsg)->timestamp());
				}
				else
					m_bIsHoveredPressed = false;
				break;
			}
		}
	}

	//____ _setHoveredKey() ___________________________________________________

	void PianoKeyboard::_setHoveredKey(int keyIdx, uint64_t timestamp)
	{
		if (keyIdx == m_hoveredKey)
			return;

		if (m_hoveredKey >= 0 && !m_keyInfo[m_hoveredKey].isSetToPressed)
		{
			State& state = m_keyInfo[m_hoveredKey].state;
			State oldState = state;
			state.setHovered(false);

			if (m_bIsHoveredPressed)
				Base::msgRouter()->post(new PianoKeyReleaseMsg(this, m_hoveredKey, timestamp));

			if (m_stateOfsY[_stateToIndex(oldState)] != m_stateOfsY[_stateToIndex(state)])
				_requestRenderKey(m_hoveredKey);
		}

		m_hoveredKey = keyIdx;

		if (m_hoveredKey >= 0)
		{
			State& state = m_keyInfo[m_hoveredKey].state;
			State oldState = state;
			state.setHovered(true);

			if (m_bIsHoveredPressed)
			{
				state.setPressed(true);
				Base::msgRouter()->post(new PianoKeyPressMsg(this, m_hoveredKey, timestamp));
			}

			if (m_stateOfsY[_stateToIndex(oldState)] != m_stateOfsY[_stateToIndex(state)])
				_requestRenderKey(m_hoveredKey);
		}
	}

	//____ _setHoverPress() _______________________________________________

	void PianoKeyboard::_setHoverPress(bool bPressed, uint64_t timestamp)
	{
		if (m_hoveredKey == -1)
			return;

		m_bIsHoveredPressed = bPressed;

		auto& key = m_keyInfo[m_hoveredKey];

		if (key.state.isPressed() == bPressed)
			return;

		if( bPressed )
		{
			key.state.setPressed(true);
			_requestRenderKey(m_hoveredKey);

			Base::msgRouter()->post(new PianoKeyPressMsg(this, m_hoveredKey, timestamp));
		}
		else if( !key.isSetToPressed )
		{
			key.state.setPressed(false);
			_requestRenderKey(m_hoveredKey);

			Base::msgRouter()->post(new PianoKeyReleaseMsg(this, m_hoveredKey, timestamp));
		}
	}

	//____ _setAPIPressed() ___________________________________________________

	void PianoKeyboard::_setAPIPressed(int keyIdx, bool bPressed)
	{
		auto& key = m_keyInfo[keyIdx];

		if( key.isSetToPressed != bPressed )
		{
			key.isSetToPressed = bPressed;

			if (bPressed  )
			{
				if( !key.state.isPressed() )
				{
					key.state.setPressed(true);
					_requestRenderKey(keyIdx);
				}
			}
			else
			{
				if (!m_bIsHoveredPressed || m_hoveredKey != keyIdx)
				{
					key.state.setPressed(false);
					_requestRenderKey(keyIdx);
				}
			}
		}
	}

	//____ _alphaTest() _______________________________________________________

	bool PianoKeyboard::_alphaTest(const Coord& ofs)
	{
		if (Widget::_alphaTest(ofs) || _markTestKey(ofs) >= 0)
			return true;

		return false;
	}

	//____ _requestRenderKey() ________________________________________________

	void PianoKeyboard::_requestRenderKey(int keyIdx)
	{
		_requestRender(_keyRect(keyIdx));
	}

	//____ _markTestKey() _____________________________________________________

	int PianoKeyboard::_markTestKey(const Coord& pos)
	{
		// Convert coordinate from canvas to source

		Rect canvas = m_pSkin ? m_pSkin->contentRect(m_size, m_state) : Rect(0, 0, m_size);

		if (!canvas.contains(pos))
			return -1;

		float xScaleFactor = m_keyboardSourceSize.w / float(canvas.w);
		float yScaleFactor = m_keyboardSourceSize.h / float(canvas.h);

		CoordI srcPos = { int((pos.x - canvas.pos().x) * xScaleFactor), int((pos.y - canvas.pos().y) * yScaleFactor) };

		float whiteKeySpacing = m_keyboardSourceSize.w / (float) m_nbWhiteKeys;

		int keyPos = -2;
		bool isBlack = false;

		// alpha test on keys

		if (m_pOddWhiteKeys->alpha(srcPos))
			keyPos = int((srcPos.x + whiteKeySpacing / 2) / (whiteKeySpacing * 2)) * 2;
		else if (m_pEvenWhiteKeys->alpha(srcPos))
			keyPos = int((srcPos.x - whiteKeySpacing / 2) / (whiteKeySpacing * 2)) * 2 + 1;
		else if (m_pBlackKeys && (srcPos.y < m_pBlackKeys->size().h) && m_pBlackKeys->alpha(srcPos))
		{
			keyPos = int((srcPos.x - whiteKeySpacing / 2) / whiteKeySpacing);
			isBlack = true;
		}
		else
			return -1;

		// Find index for the key we have found

		for (auto& key : m_keyInfo)
		{
			if (key.keyPos == keyPos && key.isBlack == isBlack )
				return &key - m_keyInfo;
		}

		return -1;
	}

	//____ _keyRect() _________________________________________________________

	Rect PianoKeyboard::_keyRect(int keyIdx)
	{
		Rect canvas = m_pSkin ? m_pSkin->contentRect(m_size, m_state) : Rect(0, 0, m_size);

		MU whiteKeySpacing = canvas.w / m_nbWhiteKeys;

		Rect dst;
		
		if( m_keyInfo[keyIdx].isBlack )
			dst = { m_keyInfo[keyIdx].keyPos*whiteKeySpacing + whiteKeySpacing / 2, 0, whiteKeySpacing, m_blackKeyHeight };
		else
			dst = { m_keyInfo[keyIdx].keyPos*whiteKeySpacing - whiteKeySpacing / 2, 0, whiteKeySpacing * 2, canvas.h };

		if (dst.x < 0)
		{
			dst.w += dst.x;
			dst.x = 0;
		}

		if (dst.x + dst.w > canvas.w )
			dst.w = canvas.w - dst.x;

		return dst.aligned() + canvas.pos();
	}


} // namespace wg
