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

#include <wg_staticboxskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo StaticBoxSkin::TYPEINFO = { "StaticBoxSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticBoxSkin_p StaticBoxSkin::create(BorderI frame, HiColor fillColor, HiColor frameColor)
	{
		return StaticBoxSkin_p(new StaticBoxSkin(frame, fillColor, frameColor));
	}

	//____ constructor ____________________________________________________________

	StaticBoxSkin::StaticBoxSkin(BorderI frame, HiColor fillColor, HiColor frameColor)
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

	bool StaticBoxSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		int opacity;

		if (m_bOpaque)
			opacity = 4096;
		else
		{
			Rect center = canvas - Border(m_frame).aligned();
			opacity =  center.contains(ofs) ? m_fillColor.a : m_frameColor.a;
		}

		return (opacity/16 >= opacityTreshold);
	}

	//____ render() ______________________________________________________________

	void StaticBoxSkin::render(GfxDevice* pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		//TODO: Optimize! Clip patches against canvas first.

		RenderSettings settings(pDevice, m_layer, m_blendMode);

		RectI canvas = _canvas.px();

		if (m_frame.isEmpty() || m_frameColor == m_fillColor)
		{
			pDevice->fill(canvas, m_fillColor);
		}
		else
		{
			BorderI frame = pointsToPixels(m_frame);

			if (frame.width() >= canvas.w || frame.height() >= canvas.h)
			{
				pDevice->fill(canvas, m_frameColor);
			}
			else
			{
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
		}
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
			m_bOpaque = (m_fillColor.a == 4096 && (m_frame.isEmpty() || m_frameColor.a == 4096));
			break;
		}

		default:
			m_bOpaque = false;
		}
	}


} // namespace wg
