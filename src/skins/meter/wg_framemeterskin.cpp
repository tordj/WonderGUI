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

	//____ preferredSize() ______________________________________________________________

	Size FrameMeterSkin::preferredSize() const
	{
		if (!frames.isEmpty())
			return Size::max( frames.frameSize(), Size(Border(m_contentPadding).aligned()) );
		else
			return Skin::minSize();
	}

	//____ minSize() ______________________________________________________________

	Size FrameMeterSkin::minSize() const
	{
		if (!frames.isEmpty())
			return Size::max(Size(Border(m_gfxPadding).aligned()), Size(Border(m_contentPadding).aligned()));
		else
			return Skin::minSize();
	}

	//____ setGfxPadding() ____________________________________________________

	void FrameMeterSkin::setGfxPadding(BorderI padding)
	{
		m_gfxPadding = padding;
	}

	//____ render() ______________________________________________________________

	void FrameMeterSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		//TODO: Support flip!

		auto pFrame = _valueToFrame(value);
		if (pFrame)
		{
			pDevice->setBlitSource(frames.surface());
			pDevice->blitNinePatch(_canvas.px(), pointsToPixels(m_gfxPadding ), RectI(pFrame->source(), frames.frameSize()), m_gfxPadding);
		}
	}

	//____ markTest() _________________________________________________________

	bool FrameMeterSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Support flip!

		if (!canvas.contains(ofs))
			return false;

		auto pFrame = _valueToFrame(value);
		if (pFrame)
			return Util::markTestNinePatch(ofs, frames._surface(), RectI(pFrame->source(), frames.frameSize()), canvas, opacityTreshold, m_gfxPadding);

		return false;
	}

	//____ dirtyRect() ________________________________________________________

	Rect FrameMeterSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newValue == oldValue)
			return Rect();

		auto pOldFrame = _valueToFrame(oldValue);
		auto pNewFrame = _valueToFrame(newValue);

		if (pOldFrame == pNewFrame)
			return Rect();
		else
			return canvas;
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
		auto pSurf = pComponent->_surface();

		m_bOpaque = pSurf ? pSurf->isOpaque() : false;
	}

	//____ _object() __________________________________________________________

	Object* FrameMeterSkin::_object()
	{
		return this;
	}

} // namespace wg
