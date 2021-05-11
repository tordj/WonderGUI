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
		return StaticBlockSkin_p(new StaticBlockSkin(pSurface,pSurface->pointSize(),frame));
	}

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const Rect& block, const Border& frame)
	{
		return StaticBlockSkin_p(new StaticBlockSkin(pSurface, block, frame));
	}

	//____ constructor ____________________________________________________________

	StaticBlockSkin::StaticBlockSkin(Surface* pSurface, const Rect& block, const Border& frame)
	{
		m_pSurface = pSurface;
		m_ninePatch.block = block;
		m_ninePatch.frame = frame;
		m_gfxFrame = frame;
		m_bOpaque = m_pSurface->isOpaque();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticBlockSkin::typeInfo(void) const
	{
		return TYPEINFO;
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

	//____ setRigidPartX() _____________________________________________

	bool StaticBlockSkin::setRigidPartX(pts _ofs, pts _length, YSections sections)
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

	//____ setRigidPartY() _____________________________________________

	bool StaticBlockSkin::setRigidPartY(pts _ofs, pts _length, XSections sections)
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

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient, m_bGradient);

		pDevice->setBlitSource(m_pSurface);
		pDevice->blitNinePatch(canvas, align(ptsToSpx(m_gfxFrame,scale)), m_ninePatch, scale);
	}

	//____ _markTest() _________________________________________________________

	bool StaticBlockSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		BorderSPX destFrame = align(ptsToSpx(m_ninePatch.frame, scale));

		return markTestNinePatch(ofs, m_pSurface, m_ninePatch, canvas, destFrame, scale, opacityTreshold);
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
