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
#include <wg_skin.impl.h>

#include <cmath>

namespace wg
{

	const TypeInfo SpinAnimSkin::TYPEINFO = { "SpinAnimSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	SpinAnimSkin_p SpinAnimSkin::create( const Blueprint& blueprint )	
	{
		return SpinAnimSkin_p(new SpinAnimSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	SpinAnimSkin::SpinAnimSkin(	const Blueprint& blueprint) : 
		m_pSurface(blueprint.surface),
		m_defaultSize(blueprint.defaultSize),
		m_pivot(blueprint.pivot),
		m_placement(blueprint.placement),
		m_fromDegrees(blueprint.angleBegin),
		m_toDegrees(blueprint.angleEnd),
		m_zoom(blueprint.zoom),
		m_gfxPadding(blueprint.destPadding),
		m_cycleDuration(blueprint.cycleDuration),
		m_color(blueprint.color),
		m_gradient(blueprint.gradient),
		m_blendMode(blueprint.blendMode)
	{
		//TODO: Also take frame opacity into account.

		m_layer = blueprint.layer;
		m_bOpaque = m_pSurface->isOpaque();
		m_contentPadding = blueprint.padding;
		m_markAlpha = blueprint.markAlpha;
		m_overflow = blueprint.overflow;

		for (int i = 0; i < StateBits_Nb; i++)
			m_animationCycles[i] = blueprint.cycleDuration;

		_updateOpacityFlag();
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

	//____ _render() ______________________________________________________________

	void SpinAnimSkin::_render(GfxDevice* pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		float	zoom = m_zoom * scale / 64.f;

		// Scale zoom to fit content of default size into canvas size.

		RectSPX canvas = _canvas;
		if (!m_defaultSize.isEmpty())
		{
			SizeSPX prefSize = align(ptsToSpx(m_defaultSize, scale));
			if (canvas.size() != prefSize)
			{
				float xScale = float(_canvas.w) / float(prefSize.w);
				float yScale = float(_canvas.h) / float(prefSize.h);
				float scaleSrc = std::min(xScale, yScale);

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
		pDevice->rotScaleBlit(_canvas, degrees, zoom, m_pivot, m_placement);
	}

	//____ _defaultSize() ______________________________________________________________

	SizeSPX SpinAnimSkin::_defaultSize(int scale) const
	{
		return align(ptsToSpx(m_defaultSize, scale));
	}

	//____ _markTest() _________________________________________________________

	bool SpinAnimSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2, int alphaOverride ) const
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
