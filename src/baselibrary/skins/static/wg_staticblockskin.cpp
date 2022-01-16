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

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const Border& frame)
	{
		Blueprint blueprint;
		blueprint.surface = pSurface;
		blueprint.frame = frame;

		return StaticBlockSkin_p(new StaticBlockSkin(blueprint));
	}

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const Rect& block, const Border& frame)
	{
		Blueprint blueprint;
		blueprint.surface = pSurface;
		blueprint.block = block;
		blueprint.frame = frame;

		return StaticBlockSkin_p(new StaticBlockSkin(blueprint));
	}

	StaticBlockSkin_p StaticBlockSkin::create(const Blueprint& blueprint)
	{
		return StaticBlockSkin_p(new StaticBlockSkin(blueprint));
	}


	//____ constructor ____________________________________________________________

	StaticBlockSkin::StaticBlockSkin( const Blueprint& blueprint )
	{
		m_pSurface = blueprint.surface;

		if (blueprint.block.isEmpty())
			m_ninePatch.block = m_pSurface->pointSize();
		else
			m_ninePatch.block = blueprint.block;

		m_ninePatch.frame	= blueprint.frame;
		m_gfxFrame			= blueprint.frame;

		m_bOpaque			= m_pSurface->isOpaque();

		m_blendMode			= blueprint.blendMode;
		m_color				= blueprint.color;
		m_contentPadding	= blueprint.contentPadding;
		m_gradient			= blueprint.gradient;
		m_layer				= blueprint.layer;
		m_markAlpha			= blueprint.markAlpha;
		m_overflow			= blueprint.overflow;

		_setRigidPartX(blueprint.rigidPartX.ofs, blueprint.rigidPartX.length, blueprint.rigidPartX.sections);
		_setRigidPartY(blueprint.rigidPartY.ofs, blueprint.rigidPartY.length, blueprint.rigidPartY.sections);

		_updateOpacityFlag();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticBlockSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _setRigidPartX() _____________________________________________

	bool StaticBlockSkin::_setRigidPartX(pts _ofs, pts _length, YSections sections)
	{
		spx ofs = align(ptsToSpx(_ofs, m_pSurface->scale()));
		spx length = align(ptsToSpx(_length, m_pSurface->scale()));

		int	midSecLen = m_ninePatch.block.w - m_ninePatch.frame.width();
		ofs -= m_ninePatch.frame.left;

		// Sanity check

		if (length <= 0 || ofs > midSecLen || ofs + length < 0)
		{
			m_ninePatch.rigidPartXOfs = 0;
			m_ninePatch.rigidPartXLength = 0;
			m_ninePatch.rigidPartXSections = YSections::None;
			return false;
		}
		
		//

		if (ofs < 0)
		{
			length += ofs;
			ofs = 0;
		}

		if (ofs + length > midSecLen)
			length = midSecLen - ofs;

		m_ninePatch.rigidPartXOfs = ofs;
		m_ninePatch.rigidPartXLength = length;
		m_ninePatch.rigidPartXSections = sections;

		return true;
	}

	//____ _setRigidPartY() _____________________________________________

	bool StaticBlockSkin::_setRigidPartY(pts _ofs, pts _length, XSections sections)
	{
		spx ofs = align(ptsToSpx(_ofs, m_pSurface->scale()));
		spx length = align(ptsToSpx(_length, m_pSurface->scale()));

		int	midSecLen = m_ninePatch.block.h - m_ninePatch.frame.height();
		ofs -= m_ninePatch.frame.top;

		// Sanity check

		if (length <= 0 || ofs > midSecLen || ofs + length < 0)
		{
			m_ninePatch.rigidPartYOfs = 0;
			m_ninePatch.rigidPartYLength = 0;
			m_ninePatch.rigidPartYSections = XSections::None;
			return false;
		}

		//

		if (ofs < 0)
		{
			length += ofs;
			ofs = 0;
		}

		if (ofs + length > midSecLen)
			length = midSecLen - ofs;

		m_ninePatch.rigidPartYOfs = ofs;
		m_ninePatch.rigidPartYLength = length;
		m_ninePatch.rigidPartYSections = sections;

		return true;
	}

	//____ _preferredSize() ______________________________________________________________

	SizeSPX StaticBlockSkin::_preferredSize(int scale) const
	{
		return SizeSPX::max(align(ptsToSpx(m_ninePatch.block.size(),scale)),_sizeForContent( SizeSPX(), scale));
	}

	//____ _render() ______________________________________________________________

	void StaticBlockSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
								  float value, float value2, int animPos, float* pStateFractions) const
	{
		if (!m_pSurface)
			return;

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient);

		pDevice->setBlitSource(m_pSurface);
		pDevice->blitNinePatch(canvas, align(ptsToSpx(m_gfxFrame,scale)), m_ninePatch, scale);
	}

	//____ _markTest() _________________________________________________________

	bool StaticBlockSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2) const
	{
		return markTestNinePatch(ofs, m_pSurface, m_ninePatch, canvas, scale, m_markAlpha);
	}

	//____ _updateOpacityFlag() _______________________________________________

	void StaticBlockSkin::_updateOpacityFlag()
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
