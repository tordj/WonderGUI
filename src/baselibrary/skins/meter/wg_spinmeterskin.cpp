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

	SpinMeterSkin_p SpinMeterSkin::create(	Surface * pSurface, Size preferredSize, CoordF srcCenter,	CoordF dstCenter, 
											float fromDegrees, float toDegrees, float zoom, const Border& gfxPadding, 
											const Border& contentPadding)
	{
		return SpinMeterSkin_p(new SpinMeterSkin(pSurface, preferredSize, srcCenter, dstCenter, fromDegrees, toDegrees, zoom, gfxPadding, contentPadding));
	}

	//____ constructor ____________________________________________________________

	SpinMeterSkin::SpinMeterSkin(	Surface * pSurface, Size preferredSize, CoordF srcCenter, CoordF dstCenter, float fromDegrees, 
									float toDegrees, float zoom, const Border& gfxPadding, const Border& contentPadding) : 
		m_pSurface(pSurface),
		m_preferredSize(preferredSize),
		m_srcCenter(srcCenter),
		m_dstCenter(dstCenter),
		m_fromDegrees(fromDegrees),
		m_toDegrees(toDegrees),
		m_zoom(zoom),
		m_gfxPadding(gfxPadding)
	{
		//TODO: Also take frame opacity into account.

		m_bOpaque = pSurface->isOpaque();
		m_bIgnoresValue = false;
		m_contentPadding = contentPadding;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SpinMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setColor() _____________________________________________________

	void SpinMeterSkin::setColor(HiColor color)
	{
		m_color = color;
		_updateOpacityFlag();
	}

	//____ setGradient() ______________________________________________________

	void SpinMeterSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		_updateOpacityFlag();
	}


	//____ setBlendMode() _____________________________________________________

	void SpinMeterSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpacityFlag();
	}

	//____ _render() ______________________________________________________________

	void SpinMeterSkin::_render(GfxDevice* pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		float	zoom = m_zoom * scale / 64.f;

		// Scale zoom to fit content of preferred size into canvas size.


		RectSPX canvas = _canvas;
		if (!m_preferredSize.isEmpty() )
		{
			SizeSPX preferred = align(ptsToSpx(m_preferredSize, scale));
			if (canvas.size() != preferred)
			{
				float xScale = float(_canvas.w) / float(preferred.w);
				float yScale = float(_canvas.h) / float(preferred.h);
				float scaleSrc = min(xScale, yScale);

				spx w = preferred.w * scaleSrc;
				spx h = preferred.h * scaleSrc;

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
		pDevice->rotScaleBlit(_canvas, degrees, zoom, m_srcCenter, m_dstCenter);
	}

	//____ _preferredSize() ______________________________________________________________

	SizeSPX SpinMeterSkin::_preferredSize(int scale) const
	{
		return align(ptsToSpx(m_preferredSize, scale));
	}

	//____ _markTest() _________________________________________________________

	bool SpinMeterSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
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
