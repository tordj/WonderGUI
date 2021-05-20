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

#include <wg_framemeterskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo FrameMeterSkin::TYPEINFO = { "FrameMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	FrameMeterSkin_p FrameMeterSkin::create()
	{
		return FrameMeterSkin_p(new FrameMeterSkin());
	}

	//____ constructor ____________________________________________________________

	FrameMeterSkin::FrameMeterSkin() : frames(this)
	{
		m_bIgnoresValue = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FrameMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setColor() _____________________________________________________

	void FrameMeterSkin::setColor(HiColor color)
	{
		m_color = color;
		_updateOpacityFlag();
	}

	//____ setGradient() ______________________________________________________

	void FrameMeterSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_updateOpacityFlag();
	}


	//____ setBlendMode() _____________________________________________________

	void FrameMeterSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpacityFlag();
	}

	//____ setGfxPadding() ____________________________________________________

	void FrameMeterSkin::setGfxPadding(Border padding)
	{
		m_gfxPadding = padding;
	}

	//____ _preferredSize() ______________________________________________________________

	SizeSPX FrameMeterSkin::_preferredSize(int scale) const
	{
		if (!frames.isEmpty())
			return SizeSPX::max(align(ptsToSpx(frames.frameSize(),scale)), align(ptsToSpx(m_contentPadding,scale)));
		else
			return Skin::_minSize(scale);
	}

	//____ _minSize() ______________________________________________________________

	SizeSPX FrameMeterSkin::_minSize(int scale) const
	{
		if (!frames.isEmpty())
			return SizeSPX::max(SizeSPX(align(ptsToSpx(m_gfxPadding,scale))), SizeSPX(align(ptsToSpx(m_contentPadding,scale))));
		else
			return Skin::_minSize(scale);
	}


	//____ _render() ______________________________________________________________

	void FrameMeterSkin::_render(GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		//TODO: Support flip!

		auto pFrame = _valueToFrame(value);
		if (pFrame)
		{
			RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_color, canvas, m_gradient, m_bGradient);

			pDevice->setBlitSource(frames.surface());

			NinePatch patch;
			patch.block = Rect(pFrame->source(), frames.frameSize());
			patch.frame = m_gfxPadding;
			pDevice->blitNinePatch(canvas, align(ptsToSpx(m_gfxPadding,scale)), patch,scale);
		}
	}

	//____ _markTest() _________________________________________________________

	bool FrameMeterSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Support flip!
		//TODO: Support tint!

		if (!canvas.contains(ofs))
			return false;

		auto pFrame = _valueToFrame(value);
		if (pFrame)
		{
			NinePatch patch;
			patch.block = Rect(pFrame->source(), frames.frameSize());
			patch.frame = m_gfxPadding;
			return Util::markTestNinePatch(ofs, frames._surface(), patch, canvas, scale, opacityTreshold);
		}

		return false;
	}

	//____ _dirtyRect() ________________________________________________________

	RectSPX FrameMeterSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newValue == oldValue)
			return RectSPX();

		auto pOldFrame = _valueToFrame(oldValue);
		auto pNewFrame = _valueToFrame(newValue);

		if (pOldFrame == pNewFrame)
			return RectSPX();
		else
			return canvas;
	}

	//____ _updateOpacityFlag() _______________________________________________

	void FrameMeterSkin::_updateOpacityFlag()
	{
		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
		{
			if ((m_bGradient && !m_gradient.isOpaque()) || m_color.a != 4096)
				m_bOpaque = false;
			else
				m_bOpaque = frames._surface() ? frames._surface()->isOpaque() : false;
		}
		else
			m_bOpaque = false;
	}

	//____ _valueToFrame() _________________________________________________

	const AnimFrame* FrameMeterSkin::_valueToFrame(float fraction) const
	{
		if (frames.isEmpty())
			return nullptr;

		return frames.find(int(fraction * frames.duration()));
	}

	//____ _didAddEntries() ___________________________________________________

	void FrameMeterSkin::_didAddEntries(AnimFrame* pEntry, int nb)
	{
	}

	//____ _didMoveEntries() __________________________________________________

	void FrameMeterSkin::_didMoveEntries(AnimFrame* pFrom, AnimFrame* pTo, int nb)
	{
	}

	//____ _willEraseEntries() ________________________________________________

	void FrameMeterSkin::_willEraseEntries(AnimFrame* pEntry, int nb)
	{
	}

	//____ _didSetAnimFrameSize() _____________________________________________

	void FrameMeterSkin::_didSetAnimFrameSize(CAnimFrames* pComponent)
	{
	}

	//____ _didSetAnimSurface() _______________________________________________

	void FrameMeterSkin::_didSetAnimSurface(CAnimFrames* pComponent)
	{
		_updateOpacityFlag();
	}

	//____ _object() __________________________________________________________

	Object* FrameMeterSkin::_object()
	{
		return this;
	}

} // namespace wg
