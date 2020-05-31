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

	PieMeterSkin_p PieMeterSkin::create()
	{
		return PieMeterSkin_p(new PieMeterSkin());
	}


	PieMeterSkin_p PieMeterSkin::create(float start, float min, float max, Color minColor, Color maxColor, Color emptyColor,
										float hubSize, Color hubColor, Color backColor, const BorderI& piePadding, const BorderI& contentPadding,
										bool bStaticSections, bool bRectangular)
	{
		return PieMeterSkin_p(new PieMeterSkin(start, min, max, minColor, maxColor, emptyColor, hubSize, hubColor, backColor, piePadding, contentPadding, bStaticSections, bRectangular));
	}

	//____ constructor ____________________________________________________________

	PieMeterSkin::PieMeterSkin()
	{
		m_slices[0].size = 1.f;
		m_slices[0].minColor = Color::Green;
		m_slices[0].maxColor = Color::Green;
		m_nSlices = 1;

		_updateOpacity();

		m_preferredSize = { 64,64 };
		m_bIgnoresFraction = false;
	}

	PieMeterSkin::PieMeterSkin(	float start, float min, float max, Color minColor, Color maxColor, Color emptyColor, float hubSize,
								Color hubColor, Color backColor, const BorderI& piePadding, const BorderI& contentPadding,
								bool bStaticSections, bool bRectangular)
	{
		m_rangeStart = start;
		m_minRange = min;
		m_maxRange = max;
		m_hubSize = hubSize;
		
		m_hubColor = hubColor;
		m_backColor = backColor;
		m_emptyColor = emptyColor;

		m_slices[0].size = 1.f;
		m_slices[0].minColor = minColor;
		m_slices[0].maxColor = maxColor;
		m_nSlices = 1;

		m_gfxPadding = piePadding;
		m_contentPadding = contentPadding;
		m_bStaticSections = bStaticSections;
		m_bRectangular = bRectangular;

		_updateOpacity();
		
		m_preferredSize = minSize() + SizeI(64, 64);
		m_bIgnoresFraction = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PieMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
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

		for (auto& slice : slices)
		{
			m_slices[m_nSlices++] = slice;
			pieSize += slice.size;
		}

		// Normalize slice sizes

		if (pieSize != 1.f)
		{
			float factor = pieSize / 1.f;

			for (int i = 0; i < m_nSlices; i++)
				m_slices[i].size *= factor;
		}

		//

		_updateOpacity();
		return true;
	}

	//____ setMeter() _________________________________________________________

	void PieMeterSkin::setRange(float min, float max)
	{
		m_minRange = min;
		m_maxRange = max;
	}

	//____ setStartAngle() ____________________________________________________

	void PieMeterSkin::setStartAngle(float start)
	{
		m_rangeStart = start;
	}

	//____ setMinLength() _____________________________________________________

	void PieMeterSkin::setMinLength(float min)
	{
		m_minRange = min;
	}

	//____ setMaxLength() _____________________________________________________

	void PieMeterSkin::setMaxLength(float max)
	{
		m_maxRange = max;
	}

	//____ setRectangular() _____________________________________________________

	void PieMeterSkin::setRectangular(bool bRectangular)
	{
		m_bRectangular = bRectangular;
	}

	//____ setStaticSections() _____________________________________________________

	void PieMeterSkin::setStaticSections(bool bStatic)
	{
		m_bStaticSections = bStatic;
	}

	//____ setGfxPadding() _____________________________________________________

	void PieMeterSkin::setGfxPadding(BorderI padding)
	{
		m_gfxPadding = padding;
	}

	//____ setEmptyColor() _____________________________________________________

	void PieMeterSkin::setEmptyColor(Color empty)
	{
		m_emptyColor = empty;
	}

	//____ setHub() _____________________________________________________

	void PieMeterSkin::setHub(float size, Color color)
	{
		m_hubSize = size;
		m_hubColor = color;
	}

	//____ setHubSize() _____________________________________________________

	void PieMeterSkin::setHubSize(float hubSize)
	{
		m_hubSize = hubSize;
	}

	//____ setHubColor() _____________________________________________________

	void PieMeterSkin::setHubColor(Color hubColor)
	{
		m_hubColor = hubColor;
	}

	//____ setBackColor() _____________________________________________________

	void PieMeterSkin::setBackColor(Color back)
	{
		m_backColor = back;
	}

	//____ render() ______________________________________________________________

	void PieMeterSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
	{
		bool	bFramed = false;

		Rect canvas = _canvas;

		// Shrink canvas with padding

		if (!m_gfxPadding.isEmpty())
		{
			canvas -= Border(m_gfxPadding).aligned();

			if (canvas.w <= 0 || canvas.h <= 0)
			{
				pDevice->fill(_canvas.px(), m_backColor);
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
			RectI	outer = _canvas.px();
			RectI	inner = canvas.px();
			BorderI frame = { inner.y - outer.y, outer.right() - inner.right(), outer.bottom() - inner.bottom(), inner.x - outer.x };

			RectI top(outer.x, outer.y, outer.w, frame.top);
			RectI left(outer.x, outer.y + frame.top, frame.left, outer.h - frame.height());
			RectI right(outer.x + outer.w - frame.right, outer.y + frame.top, frame.right, outer.h - frame.height());
			RectI bottom(outer.x, outer.y + outer.h - frame.bottom, outer.w, frame.bottom);

			pDevice->fill(top, m_backColor);
			pDevice->fill(left, m_backColor);
			pDevice->fill(right, m_backColor);
			pDevice->fill(bottom, m_backColor);
		}

		//

		float sliceSizes[c_maxSlices];
		Color sliceColors[c_maxSlices];

		Color hubColor = m_hubColor.a == 255 ? m_hubColor : Color::blend(m_backColor, m_hubColor, BlendMode::Blend);

		float totalLength = m_minRange + fraction * (m_maxRange - m_minRange);

//		if (totalLength == 0.f)
//			return;

		int nSlices = 0;

		if (m_bStaticSections)
		{
			float length = 0;

			for (int i = 0; i < m_nSlices; i++)
			{
				sliceColors[i] = Color::mix(m_slices[i].minColor, m_slices[i].maxColor, uint8_t(fraction*255));

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
					sliceColors[nSlices] = Color::mix(m_slices[i].minColor, m_slices[i].maxColor, uint8_t(fraction * 255));
					nSlices++;
				}
				length += size;
			}
		}

		// Possibly fill out with empty section

		if (fraction < 1.f)
		{
			sliceColors[nSlices] = m_emptyColor;
			sliceSizes[nSlices++] = (1.f - fraction) * (m_maxRange - m_minRange);
		}

		pDevice->drawPieChart(canvas.px(), m_rangeStart, nSlices, sliceSizes, sliceColors, m_hubSize, hubColor, m_backColor, m_bRectangular);
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
		return m_preferredSize;
	}

	//____ setContentPadding() ________________________________________________

	void PieMeterSkin::setContentPadding(const BorderI& padding)
	{
		m_contentPadding = padding;
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

	//____ fractionChangeRect() ______________________________________

	Rect PieMeterSkin::fractionChangeRect(const Rect& _canvas, State state, float oldFraction, float newFraction) const
	{
		//TODO: Implement!

		return _canvas;
	}

	//____ _updateOpacity() ___________________________________________________

	void PieMeterSkin::_updateOpacity()
	{
		if (m_backColor.a < 255 || (m_hubSize > 0.f && m_hubColor.a < 255) )
			m_bOpaque = false;
		else
		{
			int totalAlpha = 0;

			for (int i = 0; i < m_nSlices; i++)
			{
				totalAlpha += m_slices[i].minColor.a;
				totalAlpha += m_slices[i].maxColor.a;
			}

			m_bOpaque = (totalAlpha == m_nSlices * 2 * 255);
		}
	}



} // namespace wg
