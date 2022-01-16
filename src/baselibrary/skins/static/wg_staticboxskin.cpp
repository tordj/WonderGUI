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

#include <wg_staticboxskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo StaticBoxSkin::TYPEINFO = { "StaticBoxSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticBoxSkin_p StaticBoxSkin::create(Border outline, HiColor fillColor, HiColor outlineColor)
	{
		Blueprint blueprint;
		blueprint.outline = outline;
		blueprint.color = fillColor;
		blueprint.outlineColor = outlineColor;
		return StaticBoxSkin_p(new StaticBoxSkin(blueprint));
	}

	StaticBoxSkin_p StaticBoxSkin::create(const Blueprint& blueprint)
	{
		return StaticBoxSkin_p(new StaticBoxSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	StaticBoxSkin::StaticBoxSkin(const Blueprint& blueprint)
	{
		m_outline				= blueprint.outline;
		m_fillColor			= blueprint.color;
		m_outlineColor		= blueprint.outlineColor;
		m_blendMode			= blueprint.blendMode;
		m_contentPadding	= blueprint.contentPadding;
		m_layer				= blueprint.layer;
		m_markAlpha			= blueprint.markAlpha;
		m_overflow			= blueprint.overflow;

		_updateOpaqueFlag();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticBoxSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _minSize() __________________________________________________________

	SizeSPX StaticBoxSkin::_minSize(int scale) const
	{
		SizeSPX content = Skin::_minSize(scale);
		SizeSPX outline = align(ptsToSpx(m_outline,scale));

		return SizeSPX::max(content, outline);
	}


	//____ _preferredSize() ____________________________________________________

	SizeSPX StaticBoxSkin::_preferredSize(int scale) const
	{
		SizeSPX content = Skin::_minSize(scale);
		SizeSPX outline = align(ptsToSpx(m_outline,scale));

		return SizeSPX::max(content, outline);
	}

	//____ _markTest() _________________________________________________________

	bool StaticBoxSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		int opacity;

		if (m_bOpaque)
			opacity = 4096;
		else
		{
			RectSPX center = canvas - align(ptsToSpx(m_outline,scale));
			opacity =  center.contains(ofs) ? m_fillColor.a : m_outlineColor.a;
		}

		return (opacity >= m_markAlpha);
	}

	//____ _render() ______________________________________________________________

	void StaticBoxSkin::_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		//TODO: Optimize! Clip patches against canvas first.

		RenderSettings settings(pDevice, m_layer, m_blendMode);

		BorderSPX outline = align(ptsToSpx(m_outline,scale));

		if (outline.isEmpty() || m_outlineColor == m_fillColor)
		{
			pDevice->fill(canvas, m_fillColor);
		}
		else
		{
			if (outline.width() >= canvas.w || outline.height() >= canvas.h)
			{
				pDevice->fill(canvas, m_outlineColor);
			}
			else
			{
				RectI top(canvas.x, canvas.y, canvas.w, outline.top);
				RectI left(canvas.x, canvas.y + outline.top, outline.left, canvas.h - outline.height());
				RectI right(canvas.x + canvas.w - outline.right, canvas.y + outline.top, outline.right, canvas.h - outline.height());
				RectI bottom(canvas.x, canvas.y + canvas.h - outline.bottom, canvas.w, outline.bottom);
				RectI center(canvas - outline);

				pDevice->fill(top, m_outlineColor);
				pDevice->fill(left, m_outlineColor);
				pDevice->fill(right, m_outlineColor);
				pDevice->fill(bottom, m_outlineColor);

				if (center.w > 0 || center.h > 0)
					pDevice->fill(center, m_fillColor);
			}
		}
	}


	//____ _updateOpaqueFlag() _____________________________________________________

	void StaticBoxSkin::_updateOpaqueFlag()
	{
		switch (m_blendMode)
		{
		case BlendMode::Replace:
			m_bOpaque = true;
			break;

		case BlendMode::Blend:
		{
			m_bOpaque = (m_fillColor.a == 4096 && (m_outline.isEmpty() || m_outlineColor.a == 4096));
			break;
		}

		default:
			m_bOpaque = false;
		}
	}


} // namespace wg
