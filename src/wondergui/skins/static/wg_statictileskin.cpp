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

#include <wg_statictileskin.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo StaticTileSkin::TYPEINFO = { "StaticTileSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticTileSkin_p StaticTileSkin::create(Surface* pSurface)
	{
		if (pSurface == nullptr || !pSurface->isTiling())
			return nullptr;

		Blueprint blueprint;
		blueprint.surface = pSurface;
		return StaticTileSkin_p(new StaticTileSkin(blueprint));
	}

	StaticTileSkin_p StaticTileSkin::create(const Blueprint& blueprint)
	{
		if (blueprint.surface == nullptr || !blueprint.surface->isTiling())
			return nullptr;

		return StaticTileSkin_p(new StaticTileSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	StaticTileSkin::StaticTileSkin(const Blueprint& blueprint)
	{
		m_pSurface	= blueprint.surface;
		m_bOpaque	= m_pSurface->isOpaque();
		m_blendMode	= blueprint.blendMode;
		m_color		= blueprint.color;
		m_gradient	= blueprint.gradient;
		m_margin	= blueprint.margin;
		m_padding	= blueprint.padding;
		m_layer		= blueprint.layer;
		m_markAlpha	= blueprint.markAlpha;
		m_overflow	= blueprint.overflow;

		_updateOpacityFlag();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticTileSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _defaultSize() ______________________________________________________________

	SizeSPX StaticTileSkin::_defaultSize(int scale) const
	{
		if (!m_pSurface)
			return SizeSPX(align(ptsToSpx(m_margin,scale)));

		return SizeSPX(align(ptsToSpx(m_margin,scale))) + SizeSPX::max(ptsToSpx(m_pSurface->pointSize(), scale),Skin::_defaultSize(scale));
	}

	//____ _render() ______________________________________________________________

	void StaticTileSkin::_render( GfxDevice * pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		RectSPX canvas = _canvas - align(ptsToSpx(m_margin,scale));
		
		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient );

		pDevice->setBlitSource(m_pSurface);
		pDevice->scaleTile(canvas,float(scale)/64);
	}

	//____ _markTest() _________________________________________________________

	bool StaticTileSkin::_markTest( const CoordSPX& ofs, const RectSPX& _canvas, int scale, State state, float value, float value2, int alphaOverride ) const
	{
		//TODO: Take tint into account.

		RectSPX canvas = _canvas - align(ptsToSpx(m_margin,scale));

		int alpha = alphaOverride == -1 ? m_markAlpha : alphaOverride;
		
		return markTestTileRect(ofs, m_pSurface, canvas, scale, alpha);
	}

	//____ _updateOpacityFlag() _______________________________________________

	void StaticTileSkin::_updateOpacityFlag()
	{
		if( !m_margin.isEmpty() )
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
		{
			if ((m_bGradient && !m_gradient.isOpaque()) || m_color.a != 4096 )
				m_bOpaque = false;
			else
				m_bOpaque = m_pSurface->isOpaque();
		}
		else
			m_bOpaque = false;
	}


} // namespace wg
