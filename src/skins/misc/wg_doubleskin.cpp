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
		if (pFrontSkin && OO(pFrontSkin)->_superSkin())
		{
			Base::handleError(ErrorSeverity::Serious, ErrorCode::FailedPrerequisite, "Parameter pFrontSkin is already part of a skin hierarchy.", nullptr, DoubleSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}

		if (pBackSkin && OO(pBackSkin)->_superSkin())
		{
			Base::handleError(ErrorSeverity::Serious, ErrorCode::FailedPrerequisite, "Parameter pBackSkin is already part of a skin hierarchy.", nullptr, DoubleSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}

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
		if( pFrontSkin )
			OO(pFrontSkin)->_setSuperSkin(this);
		if( pBackSkin )
			OO(pBackSkin)->_setSuperSkin(this);

		_onModified();
	}

	//____ destructor __________________________________________________________

	DoubleSkin::~DoubleSkin()
	{
		if (m_pFrontSkin)
			OO(m_pFrontSkin)->_setSuperSkin(nullptr);

		if (m_pBackSkin)
			OO(m_pBackSkin)->_setSuperSkin(nullptr);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& DoubleSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setFrontSkin() _____________________________________________________

	bool DoubleSkin::setFrontSkin(Skin * pSkin)
	{
		if ( pSkin && !OO(pSkin)->_setSuperSkin(this))
		{
			Base::handleError(ErrorSeverity::Serious, ErrorCode::FailedPrerequisite, "Parameter pSkin is already part of a skin hierarchy.", this, DoubleSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		if (m_pFrontSkin)
			OO(m_pFrontSkin)->_setSuperSkin(nullptr);

		m_pFrontSkin = pSkin;
		_onModified();
		return true;
	}

	//____ setBackSkin() _____________________________________________________

	bool DoubleSkin::setBackSkin(Skin* pSkin)
	{
		if (pSkin && !OO(pSkin)->_setSuperSkin(this))
		{
			Base::handleError(ErrorSeverity::Serious, ErrorCode::FailedPrerequisite, "Parameter pSkin is already part of a skin hierarchy.", this, DoubleSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		if (m_pBackSkin)
			OO(m_pBackSkin)->_setSuperSkin(nullptr);

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

	//____ isStateIdentical() _________________________________________________

	bool DoubleSkin::isStateIdentical(State state, State comparedTo, float fraction, float fraction2) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		return (m_pBackSkin->isStateIdentical(state, comparedTo, fraction, fraction2) && m_pFrontSkin->isStateIdentical(state, comparedTo, fraction, fraction2));
	}

	//____ markTest() _________________________________________________________

	bool DoubleSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction, float fraction2) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_pBackSkin->markTest(ofs, canvas, state, opacityTreshold, fraction,fraction2))
			return true;

		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, state) : canvas;
		return canvas2.contains(ofs) && m_pFrontSkin->markTest(ofs, canvas2, state, opacityTreshold, fraction,fraction2);
	}

	//____ render() ___________________________________________________________

	void DoubleSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction, float fraction2) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		Rect canvas = _canvas;

		m_pBackSkin->render(pDevice, canvas, state, fraction,fraction2);
		if (m_bSkinInSkin)
			canvas = m_pBackSkin->contentRect(canvas, state);

		m_pFrontSkin->render(pDevice, canvas, state, fraction,fraction2);
	}

	//____ fractionChangeRect() _______________________________________________

	Rect DoubleSkin::fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction, 
										float oldFraction2, float newFraction2) const
	{
		assert(m_pFrontSkin && m_pBackSkin);

		if (m_bIgnoresFraction)
			return Rect();

		Rect change1 = m_pBackSkin->fractionChangeRect(canvas, state, oldFraction, newFraction, oldFraction2, newFraction2);

		Rect canvas2 = m_bSkinInSkin ? m_pBackSkin->contentRect(canvas, state) : canvas;
		Rect change2 = m_pFrontSkin->fractionChangeRect(canvas2, state, oldFraction, newFraction, oldFraction2, newFraction2);

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

	//____ _subSkinGeo() _______________________________________________________

	Rect DoubleSkin::_subSkinGeo(Skin* pSubSkin, const Rect& myGeo, State state) const
	{
		if (!m_bSkinInSkin || pSubSkin == m_pBackSkin)
			return myGeo;

		return m_pBackSkin->contentRect(myGeo, state);
	}


} // namespace wg
