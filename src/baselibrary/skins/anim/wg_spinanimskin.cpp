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

#include <wg_spinanimskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_skin.impl.h>

#include <cmath>

namespace wg
{

	const TypeInfo SpinAnimSkin::TYPEINFO = { "SpinAnimSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	SpinAnimSkin_p SpinAnimSkin::create(	Surface * pSurface, Size preferredSize, int cycleDuration, CoordF srcCenter,	CoordF dstCenter, 
											float fromDegrees, float toDegrees, float zoom, const Border& gfxPadding, 
											const Border& contentPadding)
	{
		return SpinAnimSkin_p(new SpinAnimSkin(pSurface, preferredSize, cycleDuration, srcCenter, dstCenter, fromDegrees, toDegrees, zoom, gfxPadding, contentPadding));
	}

	//____ constructor ____________________________________________________________

	SpinAnimSkin::SpinAnimSkin(	Surface * pSurface, Size preferredSize, int cycleDuration, CoordF srcCenter, CoordF dstCenter, float fromDegrees, 
									float toDegrees, float zoom, const Border& gfxPadding, const Border& contentPadding) : 
		m_pSurface(pSurface),
		m_preferredSize(preferredSize),
		m_srcCenter(srcCenter),
		m_dstCenter(dstCenter),
		m_fromDegrees(fromDegrees),
		m_toDegrees(toDegrees),
		m_zoom(zoom),
		m_gfxPadding(gfxPadding),
		m_cycleDuration(cycleDuration)
	{
		//TODO: Also take frame opacity into account.

		m_bOpaque = pSurface->isOpaque();
		m_contentPadding = contentPadding;

		for (int i = 0; i < StateBits_Nb; i++)
			m_animationCycles[i] = cycleDuration;
	}

	//____ destructor _________________________________________________________

	SpinAnimSkin::~SpinAnimSkin()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SpinAnimSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setCycleDuration() _________________________________________________

	void SpinAnimSkin::setCycleDuration(int millisec)
	{
		m_cycleDuration = millisec;

		for (int i = 0; i < StateBits_Nb; i++)
			m_animationCycles[i] = millisec;
	}

	//____ setColor() _____________________________________________________

	void SpinAnimSkin::setColor(HiColor color)
	{
		m_color = color;
		_updateOpacityFlag();
	}

	//____ setGradient() ______________________________________________________

	void SpinAnimSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		_updateOpacityFlag();
	}


	//____ setBlendMode() _____________________________________________________

	void SpinAnimSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpacityFlag();
	}

	//____ _render() ______________________________________________________________

	void SpinAnimSkin::_render(GfxDevice* pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		float	zoom = m_zoom * scale / 64.f;

		// Scale zoom to fit content of preferred size into canvas size.

		RectSPX canvas = _canvas;
		if (!m_preferredSize.isEmpty())
		{
			SizeSPX prefSize = align(ptsToSpx(m_preferredSize, scale));
			if (canvas.size() != prefSize)
			{
				float xScale = float(_canvas.w) / float(prefSize.w);
				float yScale = float(_canvas.h) / float(prefSize.h);
				float scaleSrc = min(xScale, yScale);

				spx w = prefSize.w * scaleSrc;
				spx h = prefSize.h * scaleSrc;

				canvas.x += (w - canvas.w) / 2;
				canvas.y += (h - canvas.h) / 2;
				canvas.w = w;
				canvas.h = h;

				zoom *= scaleSrc;
			}
		}

		//

		canvas -= align(ptsToSpx(m_gfxPadding, scale));

		float	degrees = m_fromDegrees + (m_toDegrees - m_fromDegrees) * animPos / (float)m_cycleDuration;

		if (degrees < 0.f)
			degrees = 360.f + (float)fmod(degrees, 360.f);
		else if (degrees >= 360.f)
			degrees = (float)fmod(degrees, 360.f);

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient);

		pDevice->setBlitSource(m_pSurface);
		pDevice->rotScaleBlit(_canvas, degrees, zoom, m_srcCenter, m_dstCenter);
	}

	//____ _preferredSize() ______________________________________________________________

	SizeSPX SpinAnimSkin::_preferredSize(int scale) const
	{
		return align(ptsToSpx(m_preferredSize, scale));
	}

	//____ _markTest() _________________________________________________________

	bool SpinAnimSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		if (m_bOpaque)
			return true;

		//TODO: Implement!

		return true;
	}


	//____ _dirtyRect() ________________________________________________________

	RectSPX SpinAnimSkin::_dirtyRect(const RectSPX& _canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newAnimPos == oldAnimPos)
			return RectSPX();

		return _canvas;
	}

	//____ _animationLength() __________________________________________________

	int SpinAnimSkin::_animationLength(State state) const
	{
		return m_cycleDuration;
	}

	//____ _updateOpacityFlag() _______________________________________________

	void SpinAnimSkin::_updateOpacityFlag()
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
