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

	//____ typeInfo() _________________________________________________________

	const TypeInfo& DoubleSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setFrontSkin() _____________________________________________________

	void DoubleSkin::setFrontSkin(Skin * pSkin)
	{
		m_pFrontSkin = pSkin;
		_onModified();
	}

	//____ setBackSkin() _____________________________________________________

	void DoubleSkin::setBackSkin(Skin * pSkin)
	{
		m_pBackSkin = pSkin;
		_onModified();
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
		if (m_bSkinInSkin)
			return m_pBackSkin->sizeForContent(m_pFrontSkin->minSize());
		else
			return Size::max(m_pFrontSkin->minSize(), m_pBackSkin->minSize());
	}

	//____ preferredSize() _________________________________________________

	Size DoubleSkin::preferredSize() const
	{
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

	//____ contentPadding() _______________________________________________

	Border DoubleSkin::contentPadding(State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bSkinInSkin)
			return m_pFrontSkin->contentPadding(state) + m_pBackSkin->contentPadding(state);
		else
			return m_pFrontSkin->contentPadding(state);
	}


	//____ contentPaddingSize() _______________________________________________

	Size DoubleSkin::contentPaddingSize() const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bSkinInSkin)
			return m_pFrontSkin->contentPaddingSize() + m_pBackSkin->contentPaddingSize();
		else
			return m_pFrontSkin->contentPaddingSize();
	}

	//____ contentOfs() _______________________________________________________

	Coord DoubleSkin::contentOfs(State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bSkinInSkin)
			return m_pFrontSkin->contentOfs(state) + m_pBackSkin->contentOfs(state);
		else
			return m_pFrontSkin->contentOfs(state);
	}

	//____ contentRect() _____________________________________________________

	Rect DoubleSkin::contentRect(const Rect& canvas, State state) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

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

	//____ isStateIdentical() _________________________________________________

	bool DoubleSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		return (m_pBackSkin->isStateIdentical(state, comparedTo, fraction) && m_pFrontSkin->isStateIdentical(state, comparedTo, fraction));
	}

	//____ markTest() _________________________________________________________

	bool DoubleSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_pBackSkin->markTest(ofs, canvas, state, opacityTreshold, fraction))
			return true;

		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, state) : canvas;
		return canvas2.contains(ofs) && m_pFrontSkin->markTest(ofs, canvas2, state, opacityTreshold, fraction);
	}

	//____ render() ___________________________________________________________

	void DoubleSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		Rect canvas = _canvas;

		m_pBackSkin->render(pDevice, canvas, state, fraction);
		if (m_bSkinInSkin)
			canvas = m_pBackSkin->contentRect(canvas, state);

		m_pFrontSkin->render(pDevice, canvas, state, fraction);
	}

	//____ fractionChangeRect() _______________________________________________

	Rect DoubleSkin::fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bIgnoresFraction)
			return Rect();

		Rect change1 = m_pBackSkin->fractionChangeRect(canvas, state, oldFraction, newFraction);

		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, state) : canvas;
		Rect change2 = m_pFrontSkin->fractionChangeRect(canvas2, state, oldFraction, newFraction);

		if (change1.isEmpty() && change2.isEmpty())
			return Rect();

		if (change1.isEmpty())
			return change2;

		if (change2.isEmpty())
			return change1;

		return Rect::getUnion(change1, change2);
	}

	//____ _onModified() ______________________________________________________

	void DoubleSkin::_onModified()
	{
		m_bIgnoresFraction = true;
		m_bOpaque = false;
		m_bContentShifting = false;

		if (m_pBackSkin)
		{
			if (!m_pBackSkin->ignoresFraction())
				m_bIgnoresFraction = false;
			if (m_pBackSkin->isOpaque())
				m_bOpaque = true;

			if (m_bSkinInSkin)
			{
				if (m_pBackSkin->isContentShifting())
					m_bContentShifting = true;
			}


		}

		if (m_pFrontSkin)
		{
			if (!m_pFrontSkin->ignoresFraction())
				m_bIgnoresFraction = false;
			if (m_pFrontSkin->isOpaque())
				m_bOpaque = true;
			if (m_pFrontSkin->isContentShifting())
				m_bContentShifting = true;
		}

	}



} // namespace wg
