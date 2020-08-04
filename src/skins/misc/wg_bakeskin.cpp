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
#include <cassert>
#include <wg_cdynamicvector.impl.h>

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

	//____ Constructor ________________________________________________________

	BakeSkin::BakeSkin(Surface* pBakeSurface) : skins(this)
	{
		m_pBakeSurface = pBakeSurface;
	}

	BakeSkin::BakeSkin(Surface* pBakeSurface, std::initializer_list<Skin_p> skinsIn) : skins(this)
	{
		m_pBakeSurface = pBakeSurface;

		skins.pushBack(skinsIn);

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
	}

	//____ setTintColor() _____________________________________________________

	void BakeSkin::setTintColor(Color color)
	{
		m_tintColor = color;
	}

	//____ setSkinInSkin() ____________________________________________________

	void BakeSkin::setSkinInSkin(bool bInside)
	{
		if( m_bSkinInSkin != bInside )
			m_bSkinInSkin = bInside;
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

	//____ isStateIdentical() _________________________________________________

	bool BakeSkin::isStateIdentical(State state, State comparedTo, float fraction, float fraction2) const
	{
		for (auto& pSkin : skins)
		{
			if (pSkin && !pSkin->isStateIdentical(state, comparedTo, fraction,fraction2))
				return false;
		}

		return true;
	}

	//____ markTest() _________________________________________________________

	bool BakeSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction, float fraction2) const
	{
		for (auto& pSkin : skins)
		{
			if (pSkin && pSkin->markTest(ofs,canvas,state,opacityTreshold,fraction,fraction2))
				return true;
		}
		return false;
	}

	//____ render() ______________________________________________________________

	void BakeSkin::render(GfxDevice* pDevice, const Rect& canvas, State state, float fraction, float fraction2) const
	{
		// Sanity checking

		if (!m_pBakeSurface)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::FailedPrerequisite, "Can't render: missing bake surface", this, BakeSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		if (skins.isEmpty())
			return;

		// Save old cliplist and canvas

		auto oldClip = Util::pushClipList(pDevice);
		auto oldCanvas = pDevice->canvas();

		// Set new canvas (surface and rect).

		pDevice->setCanvas(m_pBakeSurface, CanvasInit::Discard,false);

		Rect bakeCanvas = canvas.size();
		RectI bakeCanvasPX = bakeCanvas.px();

		// Generate and set new cliplist.

		RectI	clip[8];

		if (oldClip.nRects <= 8)
		{
			for( int i = 0 ; i < oldClip.nRects ; i++ )
				clip[i] = RectI( oldClip.pRects[i] - canvas.pos().px(), bakeCanvasPX );
			pDevice->setClipList(oldClip.nRects, clip);
		}
		else
		{
			clip[0] = bakeCanvasPX;
			pDevice->setClipList(1, clip);
		}

		// Render skins to bake surface, from back to front

		if (!skins[0]->isOpaque())
		{
			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->fill(Color::Transparent);
			pDevice->setBlendMode(BlendMode::Blend);
		}

		if (m_bSkinInSkin)
		{
			Rect canvas = bakeCanvas;

			for (auto it = skins.end(); it != skins.begin(); )
			{
				it--;
				if ((*it) != nullptr)
				{
					(*it)->render(pDevice, canvas, state, fraction, fraction2);
					canvas = (*it)->contentRect(canvas, state);
				}
			}
		}
		else
		{
			for (auto it = skins.end(); it != skins.begin(); )
			{
				it--;
				if ((*it) != nullptr)
					(*it)->render(pDevice, bakeCanvas, state, fraction, fraction2);
			}
		}

		// Reset canvas and cliplist

		pDevice->setCanvas(oldCanvas, CanvasInit::Keep, false);
		Util::popClipList(pDevice, oldClip);

		// Blit baked graphics to canvas.

		pDevice->setBlitSource(m_pBakeSurface);
		pDevice->blit(canvas.pos().px(), { 0,0,canvas.size().px() });
	}

	//____ fractionChangeRect() _______________________________________________

	Rect BakeSkin::fractionChangeRect(	const Rect& canvas, State state, float oldFraction, float newFraction,
										float oldFraction2, float newFraction2 ) const
	{
		if (m_bIgnoresFraction)
			return Rect();

		Rect r;

		for (auto& pSkin : skins)
		{
			if (pSkin && !pSkin->ignoresFraction() )
			{
				Rect r2 = pSkin->fractionChangeRect(canvas, state, oldFraction, newFraction, oldFraction2, newFraction2);
				if (!r2.isEmpty())
				{
					if (r.isEmpty())
						r = r2;
					else
						r.growToContain(r2);
				}
			}
		}

		return r;
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

		bool		bContentShifting = false;
		bool		bSupportsFraction = false;
		bool		bOpaque = false;

		for (auto& pSkin : skins)
		{
			if (pSkin)
			{
				bContentShifting = bContentShifting || pSkin->isContentShifting();
				bSupportsFraction = bSupportsFraction || !pSkin->ignoresFraction();
				bOpaque = bOpaque || pSkin->isOpaque();
			}
		}

		// Redo opacity check if we have skin in skin.

		if (m_bSkinInSkin && bOpaque)
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

		m_bContentShifting = bContentShifting;
		m_bIgnoresFraction = !bSupportsFraction;
		m_bOpaque = bOpaque;

		// Update opaqueStates bitmask.

		Bitmask<uint32_t> opaqueStates;

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
						bStateOpaque = bStateOpaque || (*it)->isOpaque(state);
						if (m_bSkinInSkin && !(*it)->contentPaddingSize().isEmpty())
							break;			// is padding inner/upper skins, thus they are irrelevant.
					}
				}
				opaqueStates.setBit(i, bStateOpaque);
			}
		}

		m_opaqueStates = opaqueStates;

		// Invalidate our geo cache

		m_cachedQPixPerPoint = 0;
	}

	//____ didAddEntries() _______________________________________________________

	void BakeSkin::_didAddEntries(Skin_p* pEntry, int nb)
	{
		_onModified();
	}

	// ____ _didMoveEntries() ____________________________________________________

	void BakeSkin::_didMoveEntries(Skin_p* pFrom, Skin_p* pTo, int nb)
	{
		// Do nothing. Order doesn't affect any internal state.
	}

	//____ _willEraseEntries() ________________________________________________

	void BakeSkin::_willEraseEntries(Skin_p* pEntry, int nb)
	{
		while (nb > 0)
			*pEntry++ = nullptr;

		_onModified();
	}

} // namespace wg
