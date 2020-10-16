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

#include <wg_doubleskin.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_internal.h>
#include <cassert>

namespace wg
{

	using namespace Util;

	const TypeInfo DoubleSkin::TYPEINFO = { "DoubleSkin", &Skin::TYPEINFO };

	//____ create() _______________________________________________________________

	DoubleSkin_p DoubleSkin::create()
	{
		return DoubleSkin_p(new DoubleSkin());
	}

	DoubleSkin_p DoubleSkin::create(Skin * pFrontSkin, Skin * pBackSkin, bool bSkinInSkin)
	{
		return DoubleSkin_p(new DoubleSkin(pFrontSkin, pBackSkin, bSkinInSkin));
	}


	//____ constructor ________________________________________________________

	DoubleSkin::DoubleSkin()
	{
	}

	DoubleSkin::DoubleSkin(Skin * pFrontSkin, Skin * pBackSkin, bool bSkinInSkin) :
		m_pFrontSkin(pFrontSkin),
		m_pBackSkin(pBackSkin),
		m_bSkinInSkin(bSkinInSkin)
	{
		_onModified();
	}

	//____ destructor __________________________________________________________

	DoubleSkin::~DoubleSkin()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& DoubleSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setFrontSkin() _____________________________________________________

	bool DoubleSkin::setFrontSkin(Skin * pSkin)
	{
		m_pFrontSkin = pSkin;
		_onModified();
		return true;
	}

	//____ setBackSkin() _____________________________________________________

	bool DoubleSkin::setBackSkin(Skin* pSkin)
	{
		m_pBackSkin = pSkin;
		_onModified();
		return true;
	}

	//____ setSkinInSkin() _____________________________________________________

	void DoubleSkin::setSkinInSkin(bool bInside)
	{
		m_bSkinInSkin = bInside;
		_onModified();
	}

	//____ minSize() __________________________________________________________

	Size DoubleSkin::minSize() const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bSkinInSkin)
			return m_pBackSkin->sizeForContent(m_pFrontSkin->minSize());
		else
			return Size::max(m_pFrontSkin->minSize(), m_pBackSkin->minSize());
	}

	//____ preferredSize() _________________________________________________

	Size DoubleSkin::preferredSize() const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bSkinInSkin)
			return Size::max(m_pBackSkin->sizeForContent(m_pFrontSkin->preferredSize()), m_pBackSkin->preferredSize());
		else
			return Size::max(m_pFrontSkin->preferredSize(), m_pBackSkin->preferredSize());
	}

	//____ sizeForContent() ________________________________________________

	Size DoubleSkin::sizeForContent(const Size& contentSize) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bSkinInSkin)
			return m_pBackSkin->sizeForContent(m_pFrontSkin->sizeForContent(contentSize));
		else
		{
			return Size::max(m_pFrontSkin->sizeForContent(contentSize), m_pBackSkin->minSize());
		}
	}

	//____ setContentPadding() ____________________________________________

	void DoubleSkin::setContentPadding(const BorderI& padding)
	{
		m_contentPadding = padding;
		m_bContentPaddingSet = !padding.isEmpty();
	}


	//____ contentPadding() _______________________________________________

	Border DoubleSkin::contentPadding(State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bContentPaddingSet)
			return m_contentPadding;
		else if (m_bSkinInSkin)
			return m_pFrontSkin->contentPadding(state) + m_pBackSkin->contentPadding(state);
		else
			return m_pFrontSkin->contentPadding(state);
	}


	//____ contentPaddingSize() _______________________________________________

	Size DoubleSkin::contentPaddingSize() const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bContentPaddingSet)
			return Size(Border(m_contentPadding).aligned());
		else if (m_bSkinInSkin)
			return m_pFrontSkin->contentPaddingSize() + m_pBackSkin->contentPaddingSize();
		else
			return m_pFrontSkin->contentPaddingSize();
	}

	//____ contentOfs() _______________________________________________________

	Coord DoubleSkin::contentOfs(State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if( m_bContentPaddingSet )
			return Coord(m_contentPadding.left, m_contentPadding.top).aligned();
		else if( m_bSkinInSkin )
			return m_pFrontSkin->contentOfs(state) + m_pBackSkin->contentOfs(state);
		else
			return m_pFrontSkin->contentOfs(state);
	}

	//____ contentRect() _____________________________________________________

	Rect DoubleSkin::contentRect(const Rect& canvas, State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bContentPaddingSet)
			return (canvas - Border(m_contentPadding).aligned()).aligned();

		Rect content = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, state) : canvas;

		return m_pFrontSkin->contentRect(content, state);
	}

	//____ isOpaque() _________________________________________________________

	bool DoubleSkin::isOpaque(State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bOpaque)
			return true;

		return m_pBackSkin->isOpaque(state) || m_pFrontSkin->isOpaque(state);
	}

	bool DoubleSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bOpaque)
			return true;

		if (m_pBackSkin->isOpaque(rect, canvasSize, state))
			return true;
	 
		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvasSize, state) : Rect(canvasSize);
		Rect rect2 = Rect::getUnion(rect, canvas2) - canvas2.pos();

		return (!rect2.isEmpty() && m_pFrontSkin->isOpaque(rect2, canvas2.size(), state));
	}

	//____ markTest() _________________________________________________________

	bool DoubleSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_pBackSkin->markTest(ofs, canvas, state, opacityTreshold, value,value2))
			return true;

		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, state) : canvas;
		return canvas2.contains(ofs) && m_pFrontSkin->markTest(ofs, canvas2, state, opacityTreshold, value, value2);
	}

	//____ render() ___________________________________________________________

	void DoubleSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		Rect canvas = _canvas;

		m_pBackSkin->render(pDevice, canvas, state, value, value2, animPos, pStateFractions);
		if (m_bSkinInSkin)
			canvas = m_pBackSkin->contentRect(canvas, state);

		m_pFrontSkin->render(pDevice, canvas, state, value, value2, animPos, pStateFractions);
	}

	//____ dirtyRect() ______________________________________________________

	Rect DoubleSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		Rect dirt1 = m_pBackSkin->dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);

		if (dirt1 == canvas)
			return dirt1;

		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, newState) : canvas;
		Rect dirt2 = m_pFrontSkin->dirtyRect(canvas2, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);

		if (dirt1.isEmpty())
			return dirt2;

		if (dirt2.isEmpty())
			return dirt1;

		return Rect::getUnion(dirt1, dirt2);
	}

	//____ animationLength() __________________________________________________

	int DoubleSkin::animationLength(State state) const
	{
		int frontLength = m_pFrontSkin ? m_pFrontSkin->animationLength(state) : 0;
		int backLength = m_pBackSkin ? m_pBackSkin->animationLength(state) : 0;

		int length = frontLength * backLength;

		if (length == 0)
			return frontLength + backLength;

		int g = Util::gcd(frontLength, backLength);

		return frontLength / g * backLength;
	}

	//____ transitioningStates() ______________________________________________

	Bitmask<uint8_t>  DoubleSkin::transitioningStates() const
	{
		Bitmask<uint8_t> x = 0;

		if (m_pFrontSkin)
			x |= m_pFrontSkin->transitioningStates();

		if (m_pBackSkin)
			x |= m_pBackSkin->transitioningStates();

		return x;
	}

	//____ transitionTimes() __________________________________________________

	const int* DoubleSkin::transitionTimes() const
	{
		return m_transitionTimes;
	}

	//____ _onModified() ______________________________________________________

	void DoubleSkin::_onModified()
	{
		m_bIgnoresValue = true;
		m_bIgnoresState = true;
		m_bOpaque = false;
		m_bContentShifting = false;

		if (m_pBackSkin)
		{
			m_bIgnoresValue = m_pBackSkin->ignoresValue();
			m_bIgnoresState = m_pBackSkin->ignoresState();
			m_bOpaque = m_pBackSkin->isOpaque();

			if (m_bSkinInSkin)
				m_bContentShifting = m_pBackSkin->isContentShifting();

			const int* p = m_pBackSkin->transitionTimes();
			for (int i = 0; i < StateBits_Nb ; i++)
				m_transitionTimes[i] = p[i];
		}
		else
		{
			for (int i = 0; i < StateBits_Nb; i++)
				m_transitionTimes[i] = 0;
		}

		if (m_pFrontSkin)
		{
			if (!m_pFrontSkin->ignoresValue())
				m_bIgnoresValue = false;
			if (!m_pFrontSkin->ignoresState())
				m_bIgnoresState = false;
			if (m_pFrontSkin->isOpaque())
				m_bOpaque = true;
			if (m_pFrontSkin->isContentShifting())
				m_bContentShifting = true;

			const int* p = m_pFrontSkin->transitionTimes();
			for (int i = 0; i < StateBits_Nb; i++)
			{
				if( p[i] != 0)
					m_transitionTimes[i] = std::max(m_transitionTimes[i],p[i]);
			}
		}

	}

	//____ _incUseCount() _________________________________________________________

	void DoubleSkin::_incUseCount()
	{
		if (m_pFrontSkin)
			OO(m_pFrontSkin)->_incUseCount();

		if (m_pBackSkin)
			OO(m_pBackSkin)->_incUseCount();

		m_useCount++;
	}

	//____ _decUseCount() ______________________________________________________

	void DoubleSkin::_decUseCount()
	{
		if (m_pFrontSkin)
			OO(m_pFrontSkin)->_decUseCount();

		if (m_pBackSkin)
			OO(m_pBackSkin)->_decUseCount();

		m_useCount--;
	}


} // namespace wg
