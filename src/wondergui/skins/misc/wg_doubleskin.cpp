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
#include <wg_gfxdevice.h>
#include <cassert>
#include <algorithm>

namespace wg
{

	using namespace Util;

	const TypeInfo DoubleSkin::TYPEINFO = { "DoubleSkin", &Skin::TYPEINFO };

	//____ create() _______________________________________________________________

	DoubleSkin_p DoubleSkin::create( const Blueprint& blueprint )
	{
		return DoubleSkin_p(new DoubleSkin(blueprint));
	}

	DoubleSkin_p DoubleSkin::create(Skin * pFrontSkin, Skin * pBackSkin, bool bSkinInSkin)
	{
		return DoubleSkin_p(new DoubleSkin(pFrontSkin, pBackSkin, bSkinInSkin));
	}


	//____ constructor ________________________________________________________

	DoubleSkin::DoubleSkin( const Blueprint& bp ) : Skin(bp)
	{
		m_pFrontSkin = bp.skins[0];
		m_pBackSkin = bp.skins[1];
		m_bSkinInSkin = bp.skinInSkin;

		m_bContentPaddingSet = !bp.padding.isEmpty();

		_onModified();
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

	//____ _minSize() __________________________________________________________

	SizeSPX DoubleSkin::_minSize(int scale) const
	{
		if (m_bSkinInSkin)
			return m_pBackSkin->_sizeForContent(m_pFrontSkin->_minSize(scale),scale) + align(ptsToSpx(m_margin, scale));
		else
			return SizeSPX::max(m_pFrontSkin->_minSize(scale), m_pBackSkin->_minSize(scale)) + align(ptsToSpx(m_margin, scale));
	}

	//____ _defaultSize() _________________________________________________

	SizeSPX DoubleSkin::_defaultSize(int scale) const
	{
		if (m_bSkinInSkin)
			return SizeSPX::max(m_pBackSkin->_sizeForContent(m_pFrontSkin->_defaultSize(scale),scale), m_pBackSkin->_defaultSize(scale));
		else
			return SizeSPX::max(m_pFrontSkin->_defaultSize(scale), m_pBackSkin->_defaultSize(scale)) + align(ptsToSpx(m_margin, scale));
	}

	//____ _sizeForContent() ________________________________________________

	SizeSPX DoubleSkin::_sizeForContent(const SizeSPX& contentSize, int scale) const
	{
		if (m_bSkinInSkin)
			return m_pBackSkin->_sizeForContent(m_pFrontSkin->_sizeForContent(contentSize, scale), scale) + align(ptsToSpx(m_margin, scale));
		else
		{
			return SizeSPX::max(m_pFrontSkin->_sizeForContent(contentSize, scale), m_pBackSkin->_minSize(scale)) + align(ptsToSpx(m_margin, scale));
		}
	}

	//____ _padding() _______________________________________________

	BorderSPX DoubleSkin::_padding(int scale, State state) const
	{
		// We read content border of our contained skins instead of padding, since that in
		// reality becomes the padding in our DoubleSkin.
		
		if (m_bContentPaddingSet)
			return align(ptsToSpx(m_padding,scale));
		else if (m_bSkinInSkin)
			return m_pFrontSkin->_contentBorder(scale, state) + m_pBackSkin->_contentBorder(scale, state);
		else
			return m_pFrontSkin->_contentBorder(scale, state);
	}

	//____ _paddingSize() _______________________________________________

	SizeSPX DoubleSkin::_paddingSize(int scale) const
	{
		// We read content border of our contained skins instead of padding, since that in
		// reality becomes the padding in our DoubleSkin.

		if (m_bContentPaddingSet)
			return align(ptsToSpx(m_padding, scale));
		else if (m_bSkinInSkin)
			return m_pFrontSkin->_contentBorderSize(scale) + m_pBackSkin->_contentBorderSize(scale);
		else
			return m_pFrontSkin->_contentBorderSize(scale);
	}

	//____ _contentOfs() _______________________________________________________

	CoordSPX DoubleSkin::_contentOfs(int scale, State state) const
	{
		if( m_bContentPaddingSet )
			return align(ptsToSpx(Coord(m_padding.left, m_padding.top),scale)) + align(ptsToSpx(Coord(m_margin.left, m_margin.top), scale));
		else if( m_bSkinInSkin )
			return m_pFrontSkin->_contentOfs(scale, state) + m_pBackSkin->_contentOfs(scale, state) + align(ptsToSpx(Coord(m_margin.left, m_margin.top), scale));
		else
			return m_pFrontSkin->_contentOfs(scale, state) + align(ptsToSpx(Coord(m_margin.left, m_margin.top), scale));
	}

	//____ _contentRect() _____________________________________________________

	RectSPX DoubleSkin::_contentRect(const RectSPX& canvas, int scale, State state) const
	{
		if (m_bContentPaddingSet)
			return canvas - align(ptsToSpx(m_padding,scale)) - align(ptsToSpx(m_margin, scale));

		RectSPX content = m_bSkinInSkin ? m_pBackSkin->_contentRect(canvas, scale, state) : canvas;

		content -= align(ptsToSpx(m_margin, scale));
		
		return m_pFrontSkin->_contentRect(content, scale, state);
	}

	//____ _contentBorder() ______________________________________________________

	BorderSPX DoubleSkin::_contentBorder(int scale, State state) const
	{
		if (m_bContentPaddingSet)
			return align(ptsToSpx(m_padding,scale)) + align(ptsToSpx(m_margin, scale));

		BorderSPX border = align(ptsToSpx(m_margin, scale));
		if( m_bSkinInSkin )
			border += m_pBackSkin->_contentBorder(scale, state);
		
		return border + m_pFrontSkin->_contentBorder(scale, state);
	}

	//____ contentBorderSize() ___________________________________________________

	SizeSPX DoubleSkin::_contentBorderSize(int scale) const
	{
		if (m_bContentPaddingSet)
			return align(ptsToSpx(m_padding,scale)).size() + align(ptsToSpx(m_margin, scale)).size();

		SizeSPX sz = align(ptsToSpx(m_margin, scale)).size();
		if( m_bSkinInSkin )
			sz += m_pBackSkin->_contentBorderSize(scale);
		
		return sz + m_pFrontSkin->_contentBorderSize(scale);
	}

	//____ _isOpaque() _________________________________________________________

	bool DoubleSkin::_isOpaque(State state) const
	{
		if (m_bOpaque)
			return true;

		if( !m_margin.isEmpty() )
			return false;
		
		if (m_pBackSkin->_isOpaque(state))
			return true;

		return m_pFrontSkin->_isOpaque(state) && (!m_bSkinInSkin || (!m_pBackSkin->_hasPadding() && !m_pBackSkin->_hasMargin()));
	}

	bool DoubleSkin::_isOpaque(const RectSPX& _rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		if (m_bOpaque)
			return true;

		RectSPX rect;
		if( !m_margin.isEmpty() )
		{
			RectSPX canvas = RectSPX(canvasSize) - align(ptsToSpx(m_margin, scale));
		
			if( !canvas.contains(_rect) )
				return false;				// Rect includes our margin
		
			rect = _rect - canvas.pos();
		}
		else
			rect = _rect;
		
		if (m_pBackSkin->_isOpaque(rect, canvasSize, scale, state))
			return true;
	 
		RectSPX canvas2 = m_bSkinInSkin ? m_pBackSkin->_contentRect(canvasSize, scale, state) : RectSPX(canvasSize)  - align(ptsToSpx(m_margin, scale));
		RectSPX rect2 = RectSPX::bounds(rect, canvas2) - canvas2.pos();

		return (!rect2.isEmpty() && m_pFrontSkin->_isOpaque(rect2, canvas2.size(), scale, state));
	}

	//____ _markTest() _________________________________________________________

	bool DoubleSkin::_markTest(const CoordSPX& ofs, const RectSPX& _canvas, int scale, State state, float value, float value2, int alphaOverride) const
	{
		RectSPX canvas = _canvas  - align(ptsToSpx(m_margin, scale));
		
		if (m_pBackSkin->_markTest(ofs, canvas, scale, state, value, value2, alphaOverride ))
			return true;

		RectSPX canvas2 = m_bSkinInSkin ? m_pBackSkin->_contentRect(canvas, scale, state) : canvas;
		return canvas2.contains(ofs) && m_pFrontSkin->_markTest(ofs, canvas2, scale, state, value, value2, alphaOverride);
	}

	//____ _render() ___________________________________________________________

	void DoubleSkin::_render(GfxDevice * pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RectSPX canvas = _canvas  - align(ptsToSpx(m_margin, scale));

		int oldLayer = -1;
		if (m_layer != -1)
		{
			oldLayer = pDevice->renderLayer();
			pDevice->setRenderLayer(m_layer);
		}

		m_pBackSkin->_render(pDevice, canvas, scale, state, value, value2, animPos, pStateFractions);
		if (m_bSkinInSkin)
			canvas = m_pBackSkin->_contentRect(canvas, scale, state);

		m_pFrontSkin->_render(pDevice, canvas, scale, state, value, value2, animPos, pStateFractions);

		if (oldLayer != -1)
			pDevice->setRenderLayer(oldLayer);
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX DoubleSkin::_dirtyRect(const RectSPX& _canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		RectSPX canvas = _canvas  - align(ptsToSpx(m_margin, scale));

		RectSPX dirt1 = m_pBackSkin->_dirtyRect(canvas, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);

		if (dirt1 == canvas)
			return dirt1;

		RectSPX canvas2 = m_bSkinInSkin ? m_pBackSkin->_contentRect(canvas, scale, newState) : canvas;
		RectSPX dirt2 = m_pFrontSkin->_dirtyRect(canvas2, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);

		if (dirt1.isEmpty())
			return dirt2;

		if (dirt2.isEmpty())
			return dirt1;

		return RectSPX::bounds(dirt1, dirt2);
	}

	//____ _animationLength() __________________________________________________

	int DoubleSkin::_animationLength(State state) const
	{
		int frontLength = m_pFrontSkin ? m_pFrontSkin->_animationLength(state) : 0;
		int backLength = m_pBackSkin ? m_pBackSkin->_animationLength(state) : 0;

		int length = frontLength * backLength;

		if (length == 0)
			return frontLength + backLength;

		int g = Util::gcd(frontLength, backLength);

		return frontLength / g * backLength;
	}

	//____ _transitioningStates() ______________________________________________

	Bitmask<uint8_t>  DoubleSkin::_transitioningStates() const
	{
		Bitmask<uint8_t> x = 0;

		if (m_pFrontSkin)
			x |= m_pFrontSkin->_transitioningStates();

		if (m_pBackSkin)
			x |= m_pBackSkin->_transitioningStates();

		return x;
	}

	//____ _transitionTimes() __________________________________________________

	const int* DoubleSkin::_transitionTimes() const
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
			if( m_margin.isEmpty() && m_pBackSkin->isOpaque() )
				m_bOpaque = true;
			
			m_bIgnoresValue = m_pBackSkin->_ignoresValue();
			m_bIgnoresState = m_pBackSkin->_ignoresState();
			m_bOpaque = m_pBackSkin->isOpaque();

			if (m_bSkinInSkin)
				m_bContentShifting = m_pBackSkin->_isContentShifting();

			const int* p = m_pBackSkin->_transitionTimes();
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
			if (m_pFrontSkin->isOpaque() && (!m_pBackSkin || !m_bSkinInSkin || (!m_pBackSkin->_hasPadding() && !m_pBackSkin->_hasMargin()) ))
				m_bOpaque = true;

			if (!m_pFrontSkin->_ignoresValue())
				m_bIgnoresValue = false;
			if (!m_pFrontSkin->_ignoresState())
				m_bIgnoresState = false;
			if (m_pFrontSkin->_isContentShifting())
				m_bContentShifting = true;

			const int* p = m_pFrontSkin->_transitionTimes();
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
            _doIncUseCount(m_pFrontSkin);

		if (m_pBackSkin)
            _doIncUseCount(m_pBackSkin);

		m_useCount++;
	}

	//____ _decUseCount() ______________________________________________________

	void DoubleSkin::_decUseCount()
	{
		if (m_pFrontSkin)
            _doDecUseCount(m_pFrontSkin);

		if (m_pBackSkin)
			_doIncUseCount(m_pBackSkin);

		m_useCount--;
	}


} // namespace wg
