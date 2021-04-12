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

	ColorSkin_p ColorSkin::create()
	{
		return ColorSkin_p(new ColorSkin());
	}

	ColorSkin_p ColorSkin::create(HiColor color, Border contentPadding )
	{
		auto p = new ColorSkin(color);
		p->setContentPadding(contentPadding);
		return p;
	}

	ColorSkin_p ColorSkin::create(std::initializer_list< std::tuple<State, HiColor> > stateColors, Border contentPadding )
	{
		auto p = new ColorSkin();
		p->setColor(stateColors);
		p->setContentPadding(contentPadding);
		return p;
	}


	//____ constructor ____________________________________________________________

	ColorSkin::ColorSkin()
	{
		m_stateColorMask = 1;

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
			m_color[i] = Color::White;

		m_bOpaque = true;
	}

	ColorSkin::ColorSkin(HiColor color )
	{
		setColor(color);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ColorSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void ColorSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlag();
	}

	//____ setColor() ________________________________________________________

	void ColorSkin::setColor(HiColor color)
	{
		m_stateColorMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
			m_color[i] = color;

		m_bOpaque = (color.a == 4096);
	}

	void ColorSkin::setColor(State state, HiColor color)
	{
		int i = _stateToIndex(state);

		m_stateColorMask.setBit(i);

		m_color[i] = color;

		_updateOpaqueFlag();
		_updateUnsetColors();
	}

	void ColorSkin::setColor(std::initializer_list< std::tuple<State, HiColor> > stateColors)
	{
		for (auto& state : stateColors)
		{
			int i = _stateToIndex(std::get<0>(state));
			m_stateColorMask.setBit(i);
			m_color[i] = std::get<1>(state);
		}

		_updateOpaqueFlag();
		_updateUnsetColors();
	}

	//____ color() ______________________________________________________

	HiColor ColorSkin::color(State state) const
	{
		int i = _stateToIndex(state);
		return m_color[i];
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
