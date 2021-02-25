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

#include <wg_circleskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{
	using namespace Util;

	const TypeInfo CircleSkin::TYPEINFO = { "CircleSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	CircleSkin_p CircleSkin::create()
	{
		return CircleSkin_p(new CircleSkin());
	}

	CircleSkin_p CircleSkin::create(HiColor color, float size, float thickness, float outlineThickness, HiColor outlineColor, BorderI contentPadding)
	{
		return  new CircleSkin(color, size, thickness, outlineThickness, outlineColor, contentPadding);
	}

	//____ constructor ____________________________________________________________

	CircleSkin::CircleSkin()
	{
		m_stateInfoMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_stateInfo[i].color = Color::White;
			m_stateInfo[i].size = 1.f;
			m_stateInfo[i].thickness = 1.f;
			m_stateInfo[i].outlineThickness = 0.f;
			m_stateInfo[i].outlineColor = Color::Black;
		}

		m_bOpaque = false;
	}

	CircleSkin::CircleSkin(HiColor color, float size, float thickness, float outlineThickness, HiColor outlineColor, BorderI contentPadding)
	{
		m_stateInfoMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_stateInfo[i].color = color;
			m_stateInfo[i].size = size;
			m_stateInfo[i].thickness = thickness;
			m_stateInfo[i].outlineThickness = outlineThickness;
			m_stateInfo[i].outlineColor = outlineColor;
		}

		m_contentPadding = contentPadding;
		m_bOpaque = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CircleSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void CircleSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
	}

	//____ setAppearance() ________________________________________________________

	void CircleSkin::setAppearance(State state, HiColor color, float size, float thickness, float outlineThickness, HiColor outlineColor)
	{
		int i = _stateToIndex(state);

		m_stateInfoMask.setBit(i);

		m_stateInfo[i].color = color;
		m_stateInfo[i].size = size;
		m_stateInfo[i].thickness = thickness;
		m_stateInfo[i].outlineThickness = outlineThickness;
		m_stateInfo[i].outlineColor = outlineColor;

		_updateUnsetStates();
	}


	//____ isOpaque() _____________________________________________________________

	bool CircleSkin::isOpaque(State state) const
	{
		return false;
	}

	bool CircleSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return false;
	}

	//____ render() _______________________________________________________________

	void CircleSkin::render(GfxDevice* pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		int i = _stateToIndex(state);

		RectF canvas = _canvas.fpx();
		if (canvas.w != canvas.h || m_stateInfo[i].size != 1.f )
		{
			float sideLength = std::min(canvas.w, canvas.h) * m_stateInfo[i].size;
			canvas.x += (canvas.w - sideLength) / 2;
			canvas.y += (canvas.h - sideLength) / 2;
			canvas.w = sideLength;
			canvas.h = sideLength;
		}


		float thickness = m_stateInfo[i].thickness * MU::scale();
		float outlineThickness = m_stateInfo[i].outlineThickness * MU::scale();


		pDevice->drawElipse(canvas, thickness, m_stateInfo[i].color, outlineThickness, m_stateInfo[i].outlineColor);
	}

	//____ markTest() _____________________________________________________________

	bool CircleSkin::markTest(const Coord& _ofs, const Rect& _canvas, State state, int opacityTreshold, float value, float value2) const
	{
		int i = _stateToIndex(state);

		RectF	canvas = _canvas;
		CoordF	ofs = _ofs;

		float sideLength = std::min(canvas.w, canvas.h) * m_stateInfo[i].size;

		if (canvas.w != canvas.h || m_stateInfo[i].size != 1.f)
		{
			canvas.x += (canvas.w - sideLength) / 2;
			canvas.y += (canvas.h - sideLength) / 2;
			canvas.w = sideLength;
			canvas.h = sideLength;
		}

		CoordF center = canvas.center();

		float distanceSquared = (float) (pow((ofs.x - center.x), 2) + pow((ofs.y - center.y), 2));

		float radius = sideLength / 2;

		float thickness = m_stateInfo[i].thickness;
		float outlineThickness = m_stateInfo[i].outlineThickness;


		if (distanceSquared > radius * radius)
			return false;

		float fillRadius = radius - outlineThickness;

		if( distanceSquared > fillRadius * fillRadius )
			return (m_stateInfo[i].outlineColor.a >= opacityTreshold);

		float innerOutlineRadius = fillRadius - thickness;

		if( innerOutlineRadius < 0 || distanceSquared > innerOutlineRadius * innerOutlineRadius )
			return (m_stateInfo[i].color.a >= opacityTreshold);

		float holeRadius = innerOutlineRadius - outlineThickness;

		if( holeRadius < 0 || distanceSquared > holeRadius*holeRadius )
			return (m_stateInfo[i].outlineColor.a >= opacityTreshold);

		return false;
	}

	//____ dirtyRect() ______________________________________________________

	Rect CircleSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return Rect();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);


		if ( m_stateInfo[i1].color != m_stateInfo[i2].color || m_stateInfo[i1].size != m_stateInfo[i2].size ||
			m_stateInfo[i1].thickness != m_stateInfo[i2].thickness || m_stateInfo[i1].outlineThickness != m_stateInfo[i2].outlineThickness ||
			m_stateInfo[i1].outlineColor != m_stateInfo[i2].outlineColor)
			return canvas;

		return StateSkin::dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}


	//____ _updateUnsetStates() _______________________________________________

	void CircleSkin::_updateUnsetStates()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateInfoMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateInfoMask);
				m_stateInfo[i] = m_stateInfo[bestAlternative];
			}
		}
	}


} // namespace wg
