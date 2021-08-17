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

#include <wg_staticgradientskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo StaticGradientSkin::TYPEINFO = { "StaticGradientSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticGradientSkin_p StaticGradientSkin::create(const Gradient& gradient)
	{
		if (!gradient.isValid)
			return nullptr;

		Blueprint blueprint;
		blueprint.gradient = gradient;
		return StaticGradientSkin_p(new StaticGradientSkin(blueprint));

	}

	StaticGradientSkin_p StaticGradientSkin::create( const Blueprint& blueprint )
	{
		if (!blueprint.gradient.isValid)
			return nullptr;

		return StaticGradientSkin_p(new StaticGradientSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	StaticGradientSkin::StaticGradientSkin( const Blueprint& blueprint )
	{
		m_gradient			= blueprint.gradient;
		m_bOpaque			= m_gradient.isOpaque();
		m_blendMode			= blueprint.blendMode;
		m_contentPadding	= blueprint.contentPadding;
		m_layer				= blueprint.layer;

		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
			m_bOpaque = m_gradient.isOpaque();
		else
			m_bOpaque = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticGradientSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void StaticGradientSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, Color::White, canvas, m_gradient);

		pDevice->fill(canvas, Color::White);
	}

	//____ _markTest() _________________________________________________________

	bool StaticGradientSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		if (!canvas.contains(ofs) )
			return false;

		int xFrac = (ofs.x - canvas.x) * 4096 / canvas.w;
		int yFrac = (ofs.y - canvas.y) * 4096 / canvas.h;

		int xVal1 = m_gradient.topLeft.a + (m_gradient.topRight.a - m_gradient.topLeft.a) * xFrac / 4096;
		int xVal2 = m_gradient.bottomLeft.a + (m_gradient.bottomRight.a - m_gradient.bottomLeft.a) * xFrac / 4096;

		int val = xVal1 + (xVal2 - xVal1) * yFrac / 4096;

		return (val >= opacityTreshold);
	}


} // namespace wg
