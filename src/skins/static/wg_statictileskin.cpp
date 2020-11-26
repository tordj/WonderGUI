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

		return StaticTileSkin_p(new StaticTileSkin(pSurface));
	}

	//____ constructor ____________________________________________________________

	StaticTileSkin::StaticTileSkin(Surface* pSurface)
	{
		m_pSurface = pSurface;
		m_bOpaque = m_pSurface->isOpaque();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticTileSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() ______________________________________________________________

	Size StaticTileSkin::preferredSize() const
	{
		return m_pSurface ? Size(m_pSurface->size()) : Size();
	}

	//____ setBlendMode() _____________________________________________________

	void StaticTileSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpacityFlag();
	}

	//____ setTint() __________________________________________________________

	void StaticTileSkin::setColor(HiColor tintColor)
	{
		m_color = tintColor;
		_updateOpacityFlag();
	}

	void StaticTileSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_updateOpacityFlag();
	}

	//____ render() ______________________________________________________________

	void StaticTileSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient, m_bGradient );

		pDevice->setBlitSource(m_pSurface);
		pDevice->scaleTile(canvas.px(),MU::scale());
	}

	//____ markTest() _________________________________________________________

	bool StaticTileSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Take tint into account.

		return markTestTileRect(ofs, m_pSurface, canvas, opacityTreshold);
	}

	//____ _updateOpacityFlag() _______________________________________________

	void StaticTileSkin::_updateOpacityFlag()
	{
		if (m_blendMode == BlendMode::Replace)
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
