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

#include <wg_bakeskin.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_internal.h>
#include <cassert>
#include <wg_cdynamicvector.impl.h>
#include <wg_internal.h>
#include <wg_skin.impl.h>

namespace wg
{
	template class CDynamicVector<Skin_p>;

	using namespace Util;

	const TypeInfo BakeSkin::TYPEINFO = { "BakeSkin", &Skin::TYPEINFO };


	//____ create() ___________________________________________________________

	BakeSkin_p BakeSkin::create()
	{
		return BakeSkin_p(new BakeSkin(nullptr));
	}

	BakeSkin_p BakeSkin::create(Surface* pBakeSurface)
	{
		return BakeSkin_p(new BakeSkin(pBakeSurface));
	}

	BakeSkin_p BakeSkin::create(Surface* pBakeSurface, std::initializer_list<Skin_p> skins)
	{
		return BakeSkin_p(new BakeSkin(pBakeSurface,skins));
	}

	//____ constructor ________________________________________________________

	BakeSkin::BakeSkin(Surface* pBakeSurface) : skins(this)
	{
		m_pBakeSurface = pBakeSurface;
	}

	BakeSkin::BakeSkin(Surface* pBakeSurface, std::initializer_list<Skin_p> skinsIn) : skins(this)
	{
		m_pBakeSurface = pBakeSurface;

		skins.pushBack(skinsIn);

		_onModified();
	}

	//____ destructor _________________________________________________________

	BakeSkin::~BakeSkin()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BakeSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void BakeSkin::setBlendMode(BlendMode blend)
	{
		m_blendMode = blend;
		_onModified();
	}

	//____ setColor() _____________________________________________________

	void BakeSkin::setColor(HiColor color)
	{
		m_tintColor = color;
		_onModified();
	}

	//____ setGradient() ______________________________________________________

	void BakeSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_onModified();
	}

	//____ setSkinInSkin() ____________________________________________________

	void BakeSkin::setSkinInSkin(bool bInside)
	{
		if( m_bSkinInSkin != bInside )
			m_bSkinInSkin = bInside;

		_onModified();
	}

	//____ minSize() __________________________________________________________

	Size BakeSkin::minSize() const
	{
		if (m_cachedQPixPerPoint != MU::qpixPerPoint())
			_updateCachedGeo();

		return m_cachedMinSize;
	}

	//____ preferredSize() ____________________________________________________

	Size BakeSkin::preferredSize() const
	{
		if (m_cachedQPixPerPoint != MU::qpixPerPoint())
			_updateCachedGeo();

		return m_cachedPreferredSize;

	}

	//____ setContentPadding() ____________________________________________

	void BakeSkin::setContentPadding(const BorderI& padding)
	{
		if (padding != m_contentPadding)
		{
			m_contentPadding = padding;
			m_bContentPaddingSet = !padding.isEmpty();

			// Invalidate our geo cache

			m_cachedQPixPerPoint = 0;
		}
	}


	//____ contentPadding() _______________________________________________

	Border BakeSkin::contentPadding(State state) const
	{
		if (m_cachedQPixPerPoint != MU::qpixPerPoint())
			_updateCachedGeo();

		return m_cachedContentPadding[_stateToIndex(state)];
	}


	//____ contentPaddingSize() _______________________________________________

	Size BakeSkin::contentPaddingSize() const
	{
		if (m_cachedQPixPerPoint != MU::qpixPerPoint())
			_updateCachedGeo();

		return m_cachedContentPadding[0].size();
	}

	//____ contentOfs() _______________________________________________________

	Coord BakeSkin::contentOfs(State state) const
	{
		if (m_cachedQPixPerPoint != MU::qpixPerPoint())
			_updateCachedGeo();

		int index = _stateToIndex(state);

		return Coord(m_cachedContentPadding[index].left, m_cachedContentPadding[index].top);
	}

	//____ contentRect() _____________________________________________________

	Rect BakeSkin::contentRect(const Rect& canvas, State state) const
	{
		if (m_cachedQPixPerPoint != MU::qpixPerPoint())
			_updateCachedGeo();

		return canvas - m_cachedContentPadding[_stateToIndex(state)];
	}

	//____ setBakeSurface() ______________________________________________________

	void BakeSkin::setBakeSurface(Surface* pSurface)
	{
		m_pBakeSurface = pSurface;
	}

	//____ isOpaque() _____________________________________________________________

	bool BakeSkin::isOpaque(State state) const
	{
		return m_opaqueStates.bit(_stateToIndex(state));
	}

	bool BakeSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		if (m_opaqueStates.bit(_stateToIndex(state)))
			return true;

		for (auto& pSkin : skins)
		{
			if (pSkin &&  pSkin->isOpaque(rect, canvasSize, state))
				return true;
		}

		return false;
	}

	//____ markTest() _________________________________________________________

	bool BakeSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		for (auto& pSkin : skins)
		{
			if (pSkin && pSkin->markTest(ofs,canvas,state,opacityTreshold,value,value2))
				return true;
		}
		return false;
	}

	//____ render() ______________________________________________________________

	void BakeSkin::render(GfxDevice* pDevice, const Rect& canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		// Sanity checking

		if (!m_pBakeSurface)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::FailedPrerequisite, "Can't render: missing bake surface", this, BakeSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		if (skins.isEmpty())
			return;

		// 

		Rect bakeCanvas = canvas.size();
		RectI bakeCanvasPX = bakeCanvas.px();

		// Generate and set new cliplist.

		int		nClipRects = pDevice->clipListSize();
		RectI	clip[8];

		if (nClipRects <= 8)
		{
			auto pOldClip = pDevice->clipList();
			auto oldPos = canvas.pos().px();

			for( int i = 0 ; i < nClipRects ; i++ )
				clip[i] = RectI( pOldClip[i] - oldPos, bakeCanvasPX );
		}
		else
		{
			clip[0] = bakeCanvasPX;
			nClipRects = 1;
		}

		// Set new canvas (surface and rect).

		pDevice->beginCanvasUpdate(m_pBakeSurface, nClipRects, clip);

		// Clear bake surface, unless we are opaque.

		if (!m_bOpaque)
		{
			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->fill(Color::Transparent);
			pDevice->setBlendMode(BlendMode::Blend);
		}

		// Render skins to bake surface, from back to front

		if (m_bSkinInSkin)
		{
			Rect canvas = bakeCanvas;

			for (auto it = skins.rbegin(); it != skins.rend(); it++ )
			{
				if ((*it) != nullptr)
				{
					(*it)->render(pDevice, canvas, state, value, value2, animPos, pStateFractions);
					canvas = (*it)->contentRect(canvas, state);
				}
			}
		}
		else
		{
			for (auto it = skins.rbegin(); it != skins.rend(); it++)
			{
				if ((*it) != nullptr)
					(*it)->render(pDevice, bakeCanvas, state, value, value2, animPos, pStateFractions);
			}
		}

		// Reset canvas and cliplist

		pDevice->endCanvasUpdate();

		// Blit baked graphics to canvas.

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_tintColor, canvas, m_gradient, m_bGradient);

		pDevice->setBlitSource(m_pBakeSurface);
		pDevice->blit(canvas.pos().px(), { 0,0,canvas.size().px() });
	}

	//____ dirtyRect() ______________________________________________________

	Rect BakeSkin::dirtyRect(const Rect& _canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		Rect canvas = _canvas;
		Rect r;

		for (auto it = skins.rbegin(); it != skins.rend(); it++)
		{
			if ((*it) != nullptr)
			{
				Rect r2 = (*it)->dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
					newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
				if (!r2.isEmpty())
				{
					if (r.isEmpty())
						r = r2;
					else
						r.growToContain(r2);
				}

				if( m_bSkinInSkin )
					canvas = (*it)->contentRect(canvas, newState);
			}
		}

		return r;
	}

	//____ animationLength() __________________________________________________

	int BakeSkin::animationLength(State state) const
	{
		return m_animationLengths[_stateToIndex(state)];
	}

	//____ transitioningStates() _______________________________________________

	Bitmask<uint8_t> BakeSkin::transitioningStates() const
	{
		return m_transitioningStates;
	}

	//____ transitionTimes() __________________________________________________

	const int* BakeSkin::transitionTimes() const
	{
		return m_transitionTimes;
	}


	//____ _updateCachedGeo() _________________________________________________

	void BakeSkin::_updateCachedGeo() const
	{
		m_cachedQPixPerPoint = MU::qpixPerPoint();

		// Update cached preferred and min size.

		Size preferred;
		Size min;

		for (auto& pSkin : skins)
		{
			if (pSkin)
			{
				preferred = Size::max(preferred, pSkin->preferredSize());
				min = Size::max(min, pSkin->minSize());
			}
		}

		m_cachedPreferredSize = preferred;
		m_cachedMinSize = min;

		// Update cached content padding.

		if (m_bContentPaddingSet || skins.isEmpty())
		{
			// Content padding is specified.

			Border padding = Border(m_contentPadding).aligned();

			for (int index = 0; index < StateEnum_Nb; index++)
				m_cachedContentPadding[index] = padding;
		}
		else
		{
			if (m_bSkinInSkin)
			{
				// Content padding is the sum of all skins padding.

				if (m_bContentShifting)
				{
					for (int index = 0; index < StateEnum_Nb; index++)
						m_cachedContentPadding[index] = _stateContentPadding(_indexToState(index));
				}
				else
				{
					Border padding = _stateContentPadding(StateEnum::Normal);

					for (int index = 0; index < StateEnum_Nb; index++)
						m_cachedContentPadding[index] = padding;
				}
			}
			else
			{
				// Content padding is that of top skin.

				Border contentPadding;

				for (auto& pSkin : skins)
				{
					if (pSkin)
					{
						for (int index = 0; index < StateEnum_Nb; index++)
							m_cachedContentPadding[index] = pSkin->contentPadding(_indexToState(index));
						break;
					}
				}
			}
		}
	}

	//____ _stateContentPadding() _____________________________________________

	Border BakeSkin::_stateContentPadding(State state) const
	{
		// Only called if we are skin-in-skin.

		Border padding;

		for (auto& pSkin : skins)
		{
			if (pSkin)
				padding += pSkin->contentPadding(state);
		}
		
		return padding;
	}


	//____ _onModified() ______________________________________________________

	void BakeSkin::_onModified()
	{
		// Update various flags.

		m_bContentShifting = false;
		m_bIgnoresValue = true;

		for (auto& pSkin : skins)
		{
			if (pSkin)
			{
				m_bContentShifting = m_bContentShifting || pSkin->isContentShifting();
				m_bIgnoresValue = m_bIgnoresValue && pSkin->ignoresValue();
			}
		}

		//  Update bOpaque and opaqueStates

		bool		bOpaque = false;
		Bitmask<uint32_t> opaqueStates;

		if (m_blendMode == BlendMode::Replace)
		{
			bOpaque = true;
			opaqueStates = 0xFFFFFFFF;
		}
		else if (m_blendMode != BlendMode::Blend || m_tintColor.a < 4096 || (m_bGradient && !m_gradient.isOpaque()) )
		{
			bOpaque = false;
			opaqueStates = 0;
		}
		else
		{
			// Do global opacity check

			if (m_bSkinInSkin)
			{
				for (auto it = skins.end(); it != skins.begin(); )
				{
					it--;
					if ((*it) != nullptr)
					{
						if ((*it)->isOpaque())
						{
							// Skin is opaque and has not been padded by outer skin, thus we are opaque.
							break;
						}

						if (!(*it)->contentPaddingSize().isEmpty())
						{
							// Skin is not opaque and padds child, thus we are not opaque.
							bOpaque = false;
							break;
						}
					}
				}
			}
			else
			{
				for (auto& pSkin : skins)
				{
					if (pSkin)
						bOpaque = bOpaque || pSkin->isOpaque();
				}
			}

			// Update opaqueStates bitmask.

			if (bOpaque)
				opaqueStates = 0xFFFFFFFF;
			else
			{
				for (int i = 0; i < StateEnum_Nb; i++)
				{
					State state = _indexToState(i);

					bool bStateOpaque = false;
					for (auto it = skins.end(); it != skins.begin(); )
					{
						it--;
						if ((*it) != nullptr)
						{
							if ((*it)->isOpaque(state))
							{
								bStateOpaque = true;
								break;
							}

							if (m_bSkinInSkin && !(*it)->contentPaddingSize().isEmpty())
								break;			// is padding inner/upper skins, thus they are irrelevant.
						}
					}
					opaqueStates.setBit(i, bStateOpaque);
				}
			}
		}

		m_bOpaque			= bOpaque;
		m_opaqueStates		= opaqueStates;

		// Invalidate our geo cache

		m_cachedQPixPerPoint = 0;

		// Update transition data

		for (int i = 0; i < StateBits_Nb; i++)
			m_transitionTimes[i] = 0;

		m_transitioningStates = 0;

		for (auto& pSkin : skins)
		{
			if (pSkin)
			{
				m_transitioningStates |= pSkin->transitioningStates();
				auto p = pSkin->transitionTimes();

				for (int i = 0; i < StateBits_Nb; i++)
				{
					if( p[i] > m_transitionTimes[i] )
						m_transitionTimes[i] = p[i];
				}
			}
		}

		// Update animation lengths

		for (int index = 0; index < StateEnum_Nb; index++)
		{
			State state = _indexToState(index);

			int		combinedLength = 0;

			for (auto& pSkin : skins)
			{
				if (pSkin)
				{
					int skinLength = pSkin->animationLength(state);
					if (skinLength > 0)
					{
						if (combinedLength == 0)
							combinedLength = skinLength;
						else
						{
							int g = Util::gcd(combinedLength, skinLength);
							combinedLength = skinLength / g * combinedLength;
						}
					}
				}
			}

			m_animationLengths[index] = combinedLength;
		}
	}

	//____ didAddEntries() _______________________________________________________

	void BakeSkin::_didAddEntries(Skin_p* pEntry, int nb)
	{
		_onModified();
	}

	// ____ _didMoveEntries() ____________________________________________________

	void BakeSkin::_didMoveEntries(Skin_p* pFrom, Skin_p* pTo, int nb)
	{
		// Opacity might change when we have skin in skin.

		if( !m_bSkinInSkin )
			_onModified();
	}

	//____ _willEraseEntries() ________________________________________________

	void BakeSkin::_willEraseEntries(Skin_p* pEntry, int nb)
	{
		for (int i = 0; i < nb; i++)
			pEntry[i] = nullptr;

		_onModified();
	}

	//____ _incUseCount() _________________________________________________________

	void BakeSkin::_incUseCount()
	{
		for (auto& pSkin : skins)
		{
			if (pSkin)
				OO(pSkin)->_incUseCount();
		}

		m_useCount++;
	}

	//____ _decUseCount() ______________________________________________________

	void BakeSkin::_decUseCount()
	{
		for (auto& pSkin : skins)
		{
			if (pSkin)
				OO(pSkin)->_decUseCount();
		}

		m_useCount--;
	}

} // namespace wg
