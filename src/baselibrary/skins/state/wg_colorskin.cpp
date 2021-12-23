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

#include <wg_colorskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	using namespace Util;

	const TypeInfo ColorSkin::TYPEINFO = { "ColorSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	ColorSkin_p ColorSkin::create( const Blueprint& blueprint )
	{
		return ColorSkin_p(new ColorSkin(blueprint));
	}

	ColorSkin_p ColorSkin::create(HiColor color, Border contentPadding )
	{
		Blueprint bp;
		bp.color = color;
		bp.contentPadding = contentPadding;

		return ColorSkin_p(new ColorSkin(bp));
	}

	//____ constructor ____________________________________________________________

	ColorSkin::ColorSkin(const Blueprint& blueprint)
	{
		m_blendMode = blueprint.blendMode;
		m_contentPadding = blueprint.contentPadding;
		m_layer = blueprint.layer;

		m_color[0] = blueprint.color;


		for (auto& stateInfo : blueprint.states)
		{
			if( stateInfo.state != StateEnum::Normal )
			{
				int index = _stateToIndex(stateInfo.state);

				if (stateInfo.data.contentShift.x != 0 || stateInfo.data.contentShift.y != 0)
				{
					m_contentShiftStateMask.setBit(index);
					m_contentShift[index] = stateInfo.data.contentShift;
					m_bContentShifting = true;
				}

				if( stateInfo.data.color != HiColor::Undefined )
				{
					m_stateColorMask.setBit(index);
					m_color[index] = stateInfo.data.color;
				}
			}
		}

		_updateContentShift();
		_updateOpaqueFlag();
		_updateUnsetColors();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ColorSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _isOpaque() _____________________________________________________________

	bool ColorSkin::_isOpaque(State state) const
	{
		return (m_color[_stateToIndex(state)].a == 4096);
	}

	bool ColorSkin::_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		return (m_color[_stateToIndex(state)].a == 4096);
	}

	//____ _render() _______________________________________________________________

	void ColorSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		int i = _stateToIndex(state);
		pDevice->fill( canvas, m_color[i] );
	}

	//____ _markTest() _____________________________________________________________

	bool ColorSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		if( !canvas.contains(ofs) )
			return false;

		return ( m_color[_stateToIndex(state)].a >= opacityTreshold);
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX ColorSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, 
		float newValue, float oldValue, float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return RectSPX();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if (m_color[i1] != m_color[i2])
			return canvas;

		return StateSkin::_dirtyRect(canvas, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}

	//____ _updateOpaqueFlag() ____________________________________________________

	void ColorSkin::_updateOpaqueFlag()
	{
		switch (m_blendMode)
		{
			case BlendMode::Replace:
				m_bOpaque = true;
				break;

			case BlendMode::Blend:
			{
				int alpha = 0;

				for (int i = 0; i < StateEnum_Nb; i++)
					alpha += (int)m_color[i].a;

				m_bOpaque = (alpha == 4096 * StateEnum_Nb);
				break;
			}

			default:
				m_bOpaque = false;
		}
	}

	//____ _updateUnsetColors() _______________________________________________

	void ColorSkin::_updateUnsetColors()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateColorMask);
				m_color[i] = m_color[bestAlternative];
			}
		}
	}


} // namespace wg
