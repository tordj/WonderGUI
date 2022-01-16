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

#include <wg_piemeterskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo PieMeterSkin::TYPEINFO = { "PieMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	PieMeterSkin_p PieMeterSkin::create( const Blueprint& bp )
	{
		return PieMeterSkin_p(new PieMeterSkin(bp));
	}

	//____ constructor ____________________________________________________________

	PieMeterSkin::PieMeterSkin( const Blueprint& bp )
	{

		m_rangeStart = bp.rotation;
		m_minRange = bp.minLength;
		m_maxRange = bp.maxLength;
		m_hubSize = bp.hubSize;
		
		m_hubColor = bp.hubColor;
		m_backColor = bp.backColor;
		m_emptyColor = bp.emptyColor;

		m_gfxPadding = bp.gfxPadding;
		m_contentPadding = bp.contentPadding;
		m_bStaticSections = !bp.movingSlices;
		m_bRectangular = bp.rectangular;

		// Copy slices

		m_nSlices = 0;
		float	pieSize = 0.f;

		for (auto& slice : bp.slices)
		{
			if (slice.size <= 0)
				break;

			m_slices[m_nSlices++] = slice;
			pieSize += slice.size;
		}

		// Normalize slice sizes

		if (pieSize > 0 && pieSize != 1.f)
		{
			float factor = 1.f / pieSize;

			for (int i = 0; i < m_nSlices; i++)
				m_slices[i].size *= factor;
		}
		
		_updateOpacity();
		
		m_layer = bp.layer;
		m_blendMode = bp.blendMode;
		m_markAlpha = bp.markAlpha;
		m_overflow = bp.overflow;
		m_preferredSize = bp.preferredSize.isEmpty() ? Size(64, 64) : bp.preferredSize;
		m_bIgnoresValue = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PieMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void PieMeterSkin::_render(GfxDevice * pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		bool	bFramed = false;

		RectSPX canvas = _canvas;

		// Shrink canvas with padding

		if (!m_gfxPadding.isEmpty())
		{
			canvas -= align(ptsToSpx(m_gfxPadding,scale));

			if (canvas.w <= 0 || canvas.h <= 0)
			{
				pDevice->fill(_canvas, m_backColor);
				return;
			}
			bFramed = true;
		}

		// 

		// Force canvas to be square.

		if (_canvas.w != _canvas.h)
		{
			if (canvas.w > canvas.h)
			{
				canvas.x += (canvas.w - canvas.h) / 2;
				canvas.w = canvas.h;
			}
			else if (canvas.h > canvas.w)
			{
				canvas.y += (canvas.h - canvas.w) / 2;
				canvas.h = canvas.w;
			}
			bFramed = true;
		}

		// Fill borders with background color.

		if (bFramed && m_backColor.a > 0.f)
		{
			RectSPX	outer = _canvas;
			RectSPX	inner = canvas;
			BorderSPX frame = { inner.y - outer.y, outer.right() - inner.right(), outer.bottom() - inner.bottom(), inner.x - outer.x };

			RectSPX top(outer.x, outer.y, outer.w, frame.top);
			RectSPX left(outer.x, outer.y + frame.top, frame.left, outer.h - frame.height());
			RectSPX right(outer.x + outer.w - frame.right, outer.y + frame.top, frame.right, outer.h - frame.height());
			RectSPX bottom(outer.x, outer.y + outer.h - frame.bottom, outer.w, frame.bottom);

			pDevice->fill(top, m_backColor);
			pDevice->fill(left, m_backColor);
			pDevice->fill(right, m_backColor);
			pDevice->fill(bottom, m_backColor);
		}

		//

		float sliceSizes[c_maxSlices];
		HiColor sliceColors[c_maxSlices];

		HiColor hubColor = m_hubColor.a == 4096 ? m_hubColor : HiColor::blend(m_backColor, m_hubColor, BlendMode::Blend);

		float totalLength = m_minRange + value * (m_maxRange - m_minRange);

//		if (totalLength == 0.f)
//			return;

		int nSlices = 0;

		if (m_bStaticSections)
		{
			float length = 0;

			for (int i = 0; i < m_nSlices; i++)
			{
				sliceColors[i] = HiColor::mix(m_slices[i].minColor, m_slices[i].maxColor, int(value*4096));

				float size = m_slices[i].size*m_maxRange;
				if (length + size >= totalLength)
				{
					sliceSizes[i] = totalLength - length;
					nSlices++;
					break;
				}
				sliceSizes[i] = size;
				length += size;
				nSlices++;
			}
		}
		else
		{
			float length = 0;

			float gapSize = m_maxRange - totalLength;

			for (int i = 0; i < m_nSlices; i++)
			{
				float size = m_slices[i].size*m_maxRange;
				if (length + size > gapSize )
				{
					sliceSizes[nSlices] = length < gapSize ? length + size - gapSize : size;
					sliceColors[nSlices] = HiColor::mix(m_slices[i].minColor, m_slices[i].maxColor, int(value * 4096));
					nSlices++;
				}
				length += size;
			}
		}

		// Possibly fill out with empty section

		if (value < 1.f)
		{
			sliceColors[nSlices] = m_emptyColor;
			sliceSizes[nSlices++] = (1.f - value) * (m_maxRange - m_minRange);
		}

		pDevice->drawPieChart(canvas, m_rangeStart, nSlices, sliceSizes, sliceColors, m_hubSize, hubColor, m_backColor, m_bRectangular);
	}

	//____  _preferredSize() ___________________________________________________

	SizeSPX	PieMeterSkin::_preferredSize(int scale) const
	{
		return align(ptsToSpx(m_preferredSize, scale));
	}

	//____ _markTest() _________________________________________________________

	bool PieMeterSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		if (m_bOpaque)
			return true;

		//TODO: Implement!

		return true;
	}

	//____ _dirtyRect() ________________________________________________________

	RectSPX PieMeterSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		//TODO: Implement more optimized version.

		if (newValue != oldValue)
			return canvas;

		return RectSPX();
	}

	//____ _updateOpacity() ___________________________________________________

	void PieMeterSkin::_updateOpacity()
	{
		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode != BlendMode::Blend || m_backColor.a < 4096 || (m_hubSize > 0.f && m_hubColor.a < 4096) )
			m_bOpaque = false;
		else
		{
			int totalAlpha = 0;

			for (int i = 0; i < m_nSlices; i++)
			{
				totalAlpha += m_slices[i].minColor.a;
				totalAlpha += m_slices[i].maxColor.a;
			}

			m_bOpaque = (totalAlpha == m_nSlices * 2 * 4096);
		}
	}



} // namespace wg
