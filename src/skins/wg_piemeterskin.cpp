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

namespace wg
{

	const TypeInfo PieMeterSkin::TYPEINFO = { "PieMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	PieMeterSkin_p PieMeterSkin::create(float start, float min, float max, float thickness, Color minColor, Color maxColor,
										Color hubColor, Color backColor, const BorderI& piePadding, const BorderI& contentPadding, 
										bool bFixedSections, bool bRounded)
	{
		return PieMeterSkin_p(new PieMeterSkin(start, min, max, thickness, minColor, maxColor, hubColor, backColor, piePadding, contentPadding, bFixedSections, bRounded));
	}

	//____ constructor ____________________________________________________________

	PieMeterSkin::PieMeterSkin(	float start, float min, float max, float thickness, Color minColor, Color maxColor,
								Color hubColor, Color backColor, const BorderI& piePadding, const BorderI& contentPadding,
								bool bFixedSections, bool bRounded)
	{
		m_rangeStart = start;
		m_minRange = min;
		m_maxRange = max;
		m_thickness = thickness;
		
		m_hubColor = hubColor;
		m_backColor = backColor;

		m_slices[0].size = 1.f;
		m_slices[0].minColor = minColor;
		m_slices[0].maxColor = maxColor;
		m_nSlices = 1;

		m_piePadding = piePadding;
		m_contentPadding = contentPadding;
		m_bFixedSections = bFixedSections;
		m_bRounded = bRounded;

		m_bOpaque = (minColor.a == 255 && maxColor.a == 255 && (hubColor.a == 255 || thickness >= 1.f) && backColor.a == 255);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PieMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isStateIdentical() _________________________________________________

	bool PieMeterSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		return true;
	}

	//____ isOpaque() ______________________________________________________________

	bool PieMeterSkin::isOpaque() const
	{
		return m_bOpaque;
	}

	bool PieMeterSkin::isOpaque(State state) const
	{
		return m_bOpaque;
	}

	//____ isOpaque() ______________________________________________________________

	bool PieMeterSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return m_bOpaque;
	}

	//____ render() ______________________________________________________________

	void PieMeterSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
	{
		Rect canvas = _canvas - m_contentPadding;

		if (canvas.w <= 0 || canvas.h <= 0)
			return;

		float sliceSizes[c_maxSlices];
		Color sliceColors[c_maxSlices];

		float totalLength = m_minRange + fraction * (m_maxRange - m_minRange);

		if (totalLength == 0.f)
			return;

		int nSlices = 1;
		float length = 0;

		if (m_bFixedSections)
		{
			for (int i = 0; i < m_nSlices; i++)
			{
				sliceColors[i] = Color::mix(m_slices[i].minColor, m_slices[i].maxColor, uint8_t(fraction*255));

				float size = m_slices[i].size*m_maxRange;
				if (length + size >= totalLength)
				{
					sliceSizes[i] = totalLength - length;
					break;
				}
				sliceSizes[i] = size;
				length += size;
				nSlices++;
			}
		}
		else
		{

		}


		pDevice->drawPieChart(canvas, m_rangeStart, nSlices, sliceSizes, sliceColors, m_thickness, m_bRounded, m_hubColor, m_backColor);
	}

	//____ minSize() ______________________________________________________________

	Size PieMeterSkin::minSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ setPreferredSize() _________________________________________________

	void PieMeterSkin::setPreferredSize(const SizeI& preferred)
	{
		if (preferred.w < 0 || preferred.h < 0)
		{
			//TODO: Error handling
			return;
		}

		m_preferredSize = preferred;
	}

	//____ preferredSize() ______________________________________________________________

	Size PieMeterSkin::preferredSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ setContentPadding() ________________________________________________

	void PieMeterSkin::setContentPadding(const BorderI& padding)
	{
		m_contentPadding = padding;
	}

	//____ contentPadding() ______________________________________________________________

	Border PieMeterSkin::contentPadding(State state) const
	{
		return Border(m_contentPadding).aligned();
	}

	//____ contentPaddingSize() ______________________________________________________________

	Size PieMeterSkin::contentPaddingSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ contentOfs() ______________________________________________________________

	Coord PieMeterSkin::contentOfs(State state) const
	{
		return Coord(m_contentPadding.left, m_contentPadding.top).aligned();
	}

	//____ sizeForContent() ___________________________________________________

	Size PieMeterSkin::sizeForContent(const Size& contentSize) const
	{
		return contentSize + Size(Border(m_contentPadding).aligned());
	}

	//____ contentRect() ______________________________________________________

	Rect PieMeterSkin::contentRect(const Rect& canvas, State state) const
	{
		return (canvas - Border(m_contentPadding).aligned()).aligned();
	}

	//____ setSlices() ________________________________________________________

	bool PieMeterSkin::setSlices(std::initializer_list<Slice> slices)
	{
		// Sanity checking

		if (slices.size() > c_maxSlices)
		{
			//TODO: Error handling!
			return false;
		}

		// Copy slices

		m_nSlices = 0;
		float	pieSize = 0.f;
		int		totalAlpha = 0;

		for (auto& slice : slices)
		{
			m_slices[m_nSlices++] = slice;
			pieSize += slice.size;
			totalAlpha += slice.minColor.a;
			totalAlpha += slice.maxColor.a;
		}

		// Normalize slice sizes

		if (pieSize != 1.f)
		{
			float factor = pieSize / 1.f;

			for (int i = 0; i < m_nSlices; i++)
				m_slices[i].size *= factor;
		}

		// Set m_bOpaque

		m_bOpaque = (m_backColor.a == 255 && (m_thickness >= 1.f || m_hubColor.a == 255) && totalAlpha == m_nSlices * 2 * 255);

	}

	//____ markTest() _________________________________________________________

	bool PieMeterSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if (!canvas.contains(ofs))
			return false;

		if (m_bOpaque)
			return true;

		//TODO: Implement!

		return true;
	}

	//____ ignoresFraction() __________________________________________________

	bool PieMeterSkin::ignoresFraction() const
	{
		return false;
	}

	//____ fractionChangeRect() ______________________________________

	Rect PieMeterSkin::fractionChangeRect(const Rect& _canvas, State state, float oldFraction, float newFraction) const
	{
		//TODO: Implement!

		return _canvas;
	}

} // namespace wg
