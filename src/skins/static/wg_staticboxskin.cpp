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

#include <wg_staticboxSkin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>

namespace wg
{

	const TypeInfo StaticBoxSkin::TYPEINFO = { "StaticBoxSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticBoxSkin_p StaticBoxSkin::create(BorderI frame, Color fillColor, Color frameColor)
	{
		return StaticBoxSkin_p(new StaticBoxSkin(frame, fillColor, frameColor));
	}

	//____ constructor ____________________________________________________________

	StaticBoxSkin::StaticBoxSkin(BorderI frame, Color fillColor, Color frameColor)
	{
		m_frame = frame;
		m_fillColor = fillColor;
		m_frameColor = frameColor;
		_updateOpaqueFlag();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticBoxSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void StaticBoxSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlag();
	}

	//____ minSize() __________________________________________________________

	Size StaticBoxSkin::minSize() const
	{
		Size content = Skin::minSize();
		Size frame = Border(m_frame).aligned();

		return Size::max(content, frame);
	}


	//____ preferredSize() ____________________________________________________

	Size StaticBoxSkin::preferredSize() const
	{
		Size content = Skin::minSize();
		Size frame = Border(m_frame).aligned();

		return Size::max(content, frame);
	}

	//____ markTest() _________________________________________________________

	bool StaticBoxSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction ) const
	{
		if (!canvas.contains(ofs))
			return false;

		int opacity;

		if (m_bOpaque)
			opacity = 255;
		else
		{
			Rect center = canvas - Border(m_frame).aligned();
			opacity =  center.contains(ofs) ? m_fillColor.a : m_frameColor.a;
		}

		return (opacity >= opacityTreshold);
	}

	//____ render() ______________________________________________________________

	void StaticBoxSkin::render(GfxDevice* pDevice, const Rect& _canvas, State state, float fraction) const
	{
		//TODO: Optimize! Clip patches against canvas first.

		BlendMode	oldBlendMode = pDevice->blendMode();

		RectI canvas = _canvas.px();

		if (m_blendMode != oldBlendMode)
			pDevice->setBlendMode(m_blendMode);

		if (m_frame.isEmpty() == 0 || m_frameColor == m_fillColor)
		{
			pDevice->fill(canvas, m_fillColor);
		}
		else
		{
			BorderI frame = pointsToPixels(m_frame);

			RectI top(canvas.x, canvas.y, canvas.w, frame.top);
			RectI left(canvas.x, canvas.y + frame.top, frame.left, canvas.h - frame.height());
			RectI right(canvas.x + canvas.w - frame.right, canvas.y + frame.top, frame.right, canvas.h - frame.height());
			RectI bottom(canvas.x, canvas.y + canvas.h - frame.bottom, canvas.w, frame.bottom);
			RectI center(canvas - frame);

			pDevice->fill(top, m_frameColor);
			pDevice->fill(left, m_frameColor);
			pDevice->fill(right, m_frameColor);
			pDevice->fill(bottom, m_frameColor);

			if (center.w > 0 || center.h > 0)
				pDevice->fill(center, m_fillColor);
		}

		if (m_blendMode != oldBlendMode)
			pDevice->setBlendMode(oldBlendMode);

	}


	//____ _updateOpaqueFlag() _____________________________________________________

	void StaticBoxSkin::_updateOpaqueFlag()
	{
		switch (m_blendMode)
		{
		case BlendMode::Replace:
			m_bOpaque = true;
			break;

		case BlendMode::Blend:
		{
			m_bOpaque = (m_fillColor.a == 255 && (m_frame.isEmpty() || m_frameColor.a == 255));
			break;
		}

		default:
			m_bOpaque = false;
		}
	}


} // namespace wg
