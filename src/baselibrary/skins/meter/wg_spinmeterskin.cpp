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

#include <wg_spinmeterskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_skin.impl.h>

#include <cmath>

namespace wg
{

	const TypeInfo SpinMeterSkin::TYPEINFO = { "SpinMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	SpinMeterSkin_p SpinMeterSkin::create(	const Blueprint& blueprint )
	{
		return SpinMeterSkin_p(new SpinMeterSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	SpinMeterSkin::SpinMeterSkin( const Blueprint& blueprint ) : 
		m_pSurface(blueprint.surface),
		m_defaultSize(blueprint.defaultSize),
		m_pivot(blueprint.pivot),
		m_placement(blueprint.placement),
		m_fromDegrees(blueprint.angleBegin),
		m_toDegrees(blueprint.angleEnd),
		m_zoom(blueprint.zoom),
		m_gfxPadding(blueprint.gfxPadding),
		m_color(blueprint.color),
		m_gradient(blueprint.gradient),
		m_blendMode(blueprint.blendMode)
	{
		//TODO: Also take frame opacity into account.

		m_bOpaque = blueprint.surface->isOpaque();
		m_bIgnoresValue = false;
		m_contentPadding = blueprint.padding;
		m_layer = blueprint.layer;
		m_markAlpha = blueprint.markAlpha;
		m_overflow = blueprint.overflow;


		_updateOpacityFlag();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SpinMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void SpinMeterSkin::_render(GfxDevice* pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		float	zoom = m_zoom * scale / 64.f;

		// Scale zoom to fit content of default size into canvas size.


		RectSPX canvas = _canvas;
		if (!m_defaultSize.isEmpty() )
		{
			SizeSPX defaultSize = align(ptsToSpx(m_defaultSize, scale));
			if (canvas.size() != defaultSize)
			{
				float xScale = float(_canvas.w) / float(defaultSize.w);
				float yScale = float(_canvas.h) / float(defaultSize.h);
				float scaleSrc = std::min(xScale, yScale);

				spx w = defaultSize.w * scaleSrc;
				spx h = defaultSize.h * scaleSrc;

				canvas.x += (w - canvas.w) / 2;
				canvas.y += (h - canvas.h) / 2;
				canvas.w = w;
				canvas.h = h;

				zoom *= scaleSrc;
			}
		}

		//

		canvas -= align(ptsToSpx(m_gfxPadding, scale));

		float	degrees = m_fromDegrees + (m_toDegrees - m_fromDegrees) * value;

		if (degrees < 0.f)
			degrees = 360.f + (float)fmod(degrees, 360.f);
		else if (degrees >= 360.f)
			degrees = (float)fmod(degrees, 360.f);

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient);

		pDevice->setBlitSource(m_pSurface);
		pDevice->rotScaleBlit(_canvas, degrees, zoom, m_pivot, m_placement);
	}

	//____ _defaultSize() ______________________________________________________________

	SizeSPX SpinMeterSkin::_defaultSize(int scale) const
	{
		return align(ptsToSpx(m_defaultSize, scale));
	}

	//____ _markTest() _________________________________________________________

	bool SpinMeterSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2, int alphaOverride) const
	{
		if (!canvas.contains(ofs))
			return false;

		if (m_bOpaque)
			return true;

		//TODO: Implement!

		return true;
	}

	//____ _dirtyRect() ________________________________________________________

	RectSPX SpinMeterSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newValue != oldValue)
			return canvas;

		return RectSPX();
	}

	//____ _updateOpacityFlag() _______________________________________________

	void SpinMeterSkin::_updateOpacityFlag()
	{
		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
		{
			if ((m_gradient.isValid && !m_gradient.isOpaque()) || m_color.a != 4096)
				m_bOpaque = false;
			else
				m_bOpaque = m_pSurface->isOpaque();
		}
		else
			m_bOpaque = false;
	}

} // namespace wg
