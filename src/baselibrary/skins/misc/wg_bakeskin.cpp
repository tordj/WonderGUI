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
#include <wg_skin.impl.h>

namespace wg
{
	template class CDynamicVector<Skin_p>;

	using namespace Util;

	const TypeInfo BakeSkin::TYPEINFO = { "BakeSkin", &Skin::TYPEINFO };


	//____ create() ___________________________________________________________

	BakeSkin_p BakeSkin::create( const Blueprint& blueprint )
	{
		return BakeSkin_p(new BakeSkin(blueprint));
	}

	BakeSkin_p BakeSkin::create(Surface* pBakeSurface, std::initializer_list<Skin_p> skins)
	{
		return BakeSkin_p(new BakeSkin(pBakeSurface,skins));
	}

	//____ constructor ________________________________________________________

	BakeSkin::BakeSkin(const Blueprint& blueprint)
	{
		m_pBakeSurface = blueprint.surface;

		m_blendMode = blueprint.blendMode;
		m_tintColor = blueprint.color;
		m_gradient = blueprint.gradient;
		m_bSkinInSkin = blueprint.skinInSkin;
		m_layer = blueprint.layer;

		m_contentPadding = blueprint.padding;
		m_bContentPaddingSet = !blueprint.padding.isEmpty();

		//TODO: CDynamicVector should have a pushBack( iterator begin, interator end ) which we should use.

		m_skins = blueprint.skins;

		_onModified();
	}

	BakeSkin::BakeSkin(Surface* pBakeSurface, std::initializer_list<Skin_p> skinsIn)
	{
		m_pBakeSurface = pBakeSurface;

		m_skins = skinsIn;

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

	//____ _minSize() __________________________________________________________

	SizeSPX BakeSkin::_minSize(int scale) const
	{
		if (m_cachedScale != scale)
			_updateCachedGeo(scale);

		return m_cachedMinSize;
	}

	//____ _preferredSize() ____________________________________________________

	SizeSPX BakeSkin::_preferredSize(int scale) const
	{
		if (m_cachedScale != scale)
			_updateCachedGeo(scale);

		return m_cachedPreferredSize;

	}

	//____ _contentPadding() _______________________________________________

	BorderSPX BakeSkin::_contentPadding(int scale, State state) const
	{
		if (m_cachedScale != scale)
			_updateCachedGeo(scale);

		return m_cachedContentPadding[_stateToIndex(state)];
	}


	//____ _contentPaddingSize() _______________________________________________

	SizeSPX BakeSkin::_contentPaddingSize(int scale ) const
	{
		if (m_cachedScale != scale)
			_updateCachedGeo(scale);

		return m_cachedContentPadding[0].size();
	}

	//____ _contentOfs() _______________________________________________________

	CoordSPX BakeSkin::_contentOfs(int scale, State state) const
	{
		if (m_cachedScale != scale)
			_updateCachedGeo(scale);

		int index = _stateToIndex(state);

		return CoordSPX(m_cachedContentPadding[index].left, m_cachedContentPadding[index].top);
	}

	//____ _contentRect() _____________________________________________________

	RectSPX BakeSkin::_contentRect(const RectSPX& canvas, int scale, State state) const
	{
		if (m_cachedScale != scale)
			_updateCachedGeo(scale);

		return canvas - m_cachedContentPadding[_stateToIndex(state)];
	}

	//____ _isOpaque() _____________________________________________________________

	bool BakeSkin::_isOpaque(State state) const
	{
		return m_opaqueStates.bit(_stateToIndex(state));
	}

	bool BakeSkin::_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		if (m_opaqueStates.bit(_stateToIndex(state)))
			return true;

		for (auto& pSkin : m_skins)
		{
			if (pSkin &&  pSkin->_isOpaque(rect, canvasSize, scale, state))
				return true;
		}

		return false;
	}

	//____ _markTest() _________________________________________________________

	bool BakeSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2) const
	{
		for (auto& pSkin : m_skins)
		{
			if (pSkin && pSkin->_markTest(ofs,canvas,scale,state,value,value2))
				return true;
		}
		return false;
	}

	//____ _render() ______________________________________________________________

	void BakeSkin::_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		// Sanity checking

		if (!m_pBakeSurface)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::FailedPrerequisite, "Can't render: missing bake surface", this, BakeSkin::TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		if (m_skins.empty())
			return;

		// 

		RectSPX bakeCanvas = canvas.size();
//		RectI bakeCanvasPX = bakeCanvas.px();

		// Generate and set new cliplist.

		int		nClipRects = pDevice->clipListSize();
		RectI	clip[8];

		if (nClipRects <= 8)
		{
			auto pOldClip = pDevice->clipList();

			for( int i = 0 ; i < nClipRects ; i++ )
				clip[i] = RectSPX( pOldClip[i] - canvas.pos(), bakeCanvas );
		}
		else
		{
			clip[0] = bakeCanvas;
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
			RectSPX canvas = bakeCanvas;

			for (auto it = m_skins.rbegin(); it != m_skins.rend(); it++ )
			{
				if ((*it) != nullptr)
				{
					(*it)->_render(pDevice, canvas, scale, state, value, value2, animPos, pStateFractions);
					canvas = (*it)->_contentRect(canvas, scale, state);
				}
			}
		}
		else
		{
			for (auto it = m_skins.rbegin(); it != m_skins.rend(); it++)
			{
				if ((*it) != nullptr)
					(*it)->_render(pDevice, bakeCanvas, scale, state, value, value2, animPos, pStateFractions);
			}
		}

		// Reset canvas and cliplist

		pDevice->endCanvasUpdate();

		// Blit baked graphics to canvas.

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_tintColor, canvas, m_gradient);

		pDevice->setBlitSource(m_pBakeSurface);
		pDevice->blit(canvas.pos(), { 0,0,canvas.size()});
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX BakeSkin::_dirtyRect(const RectSPX& _canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		RectSPX canvas = _canvas;
		RectSPX r;

		for (auto it = m_skins.rbegin(); it != m_skins.rend(); it++)
		{
			if ((*it) != nullptr)
			{
				RectSPX r2 = (*it)->_dirtyRect(canvas, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2,
					newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
				if (!r2.isEmpty())
				{
					if (r.isEmpty())
						r = r2;
					else
						r.growToContain(r2);
				}

				if( m_bSkinInSkin )
					canvas = (*it)->_contentRect(canvas, scale, newState);
			}
		}

		return r;
	}

	//____ _animationLength() __________________________________________________

	int BakeSkin::_animationLength(State state) const
	{
		return m_animationLengths[_stateToIndex(state)];
	}

	//____ _transitioningStates() _______________________________________________

	Bitmask<uint8_t> BakeSkin::_transitioningStates() const
	{
		return m_transitioningStates;
	}

	//____ _transitionTimes() __________________________________________________

	const int* BakeSkin::_transitionTimes() const
	{
		return m_transitionTimes;
	}


	//____ _updateCachedGeo() _________________________________________________

	void BakeSkin::_updateCachedGeo(int scale) const
	{
		m_cachedScale = scale;

		// Update cached preferred and min size.

		SizeSPX preferred;
		SizeSPX min;

		for (auto& pSkin : m_skins)
		{
			if (pSkin)
			{
				preferred = SizeSPX::max(preferred, pSkin->_preferredSize(scale));
				min = SizeSPX::max(min, pSkin->_minSize(scale));
			}
		}

		m_cachedPreferredSize = preferred;
		m_cachedMinSize = min;

		// Update cached content padding.

		if (m_bContentPaddingSet || m_skins.empty())
		{
			// Content padding is specified.

			BorderSPX padding = align(ptsToSpx(m_contentPadding,scale));

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
						m_cachedContentPadding[index] = _stateContentPadding(scale,_indexToState(index));
				}
				else
				{
					BorderSPX padding = _stateContentPadding(scale,StateEnum::Normal);

					for (int index = 0; index < StateEnum_Nb; index++)
						m_cachedContentPadding[index] = padding;
				}
			}
			else
			{
				// Content padding is that of top skin.

				BorderSPX contentPadding;

				for (auto& pSkin : m_skins)
				{
					if (pSkin)
					{
						for (int index = 0; index < StateEnum_Nb; index++)
							m_cachedContentPadding[index] = pSkin->_contentPadding(scale,_indexToState(index));
						break;
					}
				}
			}
		}
	}

	//____ _stateContentPadding() _____________________________________________

	BorderSPX BakeSkin::_stateContentPadding(int scale, State state) const
	{
		// Only called if we are skin-in-skin.

		BorderSPX padding;

		for (auto& pSkin : m_skins)
		{
			if (pSkin)
				padding += pSkin->_contentPadding(scale,state);
		}
		
		return padding;
	}


	//____ _onModified() ______________________________________________________

	void BakeSkin::_onModified()
	{
		// Update various flags.

		m_bContentShifting = false;
		m_bIgnoresValue = true;

		for (auto& pSkin : m_skins)
		{
			if (pSkin)
			{
				m_bContentShifting = m_bContentShifting || pSkin->_isContentShifting();
				m_bIgnoresValue = m_bIgnoresValue && pSkin->_ignoresValue();
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
		else if (m_blendMode != BlendMode::Blend || m_tintColor.a < 4096 || (m_gradient.isValid && !m_gradient.isOpaque()) )
		{
			bOpaque = false;
			opaqueStates = 0;
		}
		else
		{
			// Do global opacity check

			if (m_bSkinInSkin)
			{
				for (auto it = m_skins.end(); it != m_skins.begin(); )
				{
					it--;
					if ((*it) != nullptr)
					{
						if ((*it)->isOpaque())
						{
							// Skin is opaque and has not been padded by outer skin, thus we are opaque.
							break;
						}

						if (!(*it)->_hasContentPadding())
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
				for (auto& pSkin : m_skins)
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
					for (auto it = m_skins.end(); it != m_skins.begin(); )
					{
						it--;
						if ((*it) != nullptr)
						{
							if ((*it)->isOpaque(state))
							{
								bStateOpaque = true;
								break;
							}

							if (m_bSkinInSkin && !(*it)->_hasContentPadding())
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

		m_cachedScale = 0;

		// Update transition data

		for (int i = 0; i < StateBits_Nb; i++)
			m_transitionTimes[i] = 0;

		m_transitioningStates = 0;

		for (auto& pSkin : m_skins)
		{
			if (pSkin)
			{
				m_transitioningStates |= pSkin->_transitioningStates();
				auto p = pSkin->_transitionTimes();

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

			for (auto& pSkin : m_skins)
			{
				if (pSkin)
				{
					int skinLength = pSkin->_animationLength(state);
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

	//____ _incUseCount() _________________________________________________________

	void BakeSkin::_incUseCount()
	{
		for (auto& pSkin : m_skins)
		{
			if (pSkin)
                _doIncUseCount(pSkin);
		}

		m_useCount++;
	}

	//____ _decUseCount() ______________________________________________________

	void BakeSkin::_decUseCount()
	{
		for (auto& pSkin : m_skins)
		{
			if (pSkin)
				_doDecUseCount(pSkin);
		}

		m_useCount--;
	}

} // namespace wg
