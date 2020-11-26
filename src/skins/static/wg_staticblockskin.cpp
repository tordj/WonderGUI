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

#include <wg_staticblockskin.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo StaticBlockSkin::TYPEINFO = { "StaticBlockSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const BorderI& frame)
	{
		return StaticBlockSkin_p(new StaticBlockSkin(pSurface,pSurface->size(),frame));
	}

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const RectI& block, const BorderI& frame)
	{
		return StaticBlockSkin_p(new StaticBlockSkin(pSurface, block, frame));
	}

	//____ constructor ____________________________________________________________

	StaticBlockSkin::StaticBlockSkin(Surface* pSurface, const RectI& block, const BorderI& frame)
	{
		m_pSurface = pSurface;
		m_block = block;
		m_frame = frame;
		m_bOpaque = m_pSurface->isOpaque();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticBlockSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() ______________________________________________________________

	Size StaticBlockSkin::preferredSize() const
	{
		return sizeForContent(m_block.size());
	}

	//____ setBlendMode() _____________________________________________________

	void StaticBlockSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpacityFlag();
	}

	//____ setColor() __________________________________________________________

	void StaticBlockSkin::setColor(HiColor color)
	{
		m_color = color;
		_updateOpacityFlag();
	}

	//____ setGradient() ______________________________________________________

	void StaticBlockSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_updateOpacityFlag();
	}

	//____ render() ______________________________________________________________

	void StaticBlockSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, 
								  float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient, m_bGradient);

		pDevice->setBlitSource(m_pSurface);
		pDevice->blitNinePatch(canvas.px(), pointsToPixels(m_frame * 4 / m_pSurface->qpixPerPoint()), m_block, m_frame);
	}

	//____ markTest() _________________________________________________________

	bool StaticBlockSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		return markTestNinePatch(ofs, m_pSurface, m_block, canvas, opacityTreshold, m_frame);
	}

	//____ _updateOpacityFlag() _______________________________________________

	void StaticBlockSkin::_updateOpacityFlag()
	{
		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
		{
			if ((m_bGradient && !m_gradient.isOpaque()) || m_color.a != 4096)
				m_bOpaque = false;
			else
				m_bOpaque = m_pSurface->isOpaque();
		}
		else
			m_bOpaque = false;
	}


} // namespace wg
