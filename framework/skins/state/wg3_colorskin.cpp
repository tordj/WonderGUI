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

#include <wg3_colorskin.h>
#include <wg3_gfxdevice.h>
#include <wg3_geo.h>
#include <wg3_util.h>

namespace wg
{

	using namespace Util;

	const TypeInfo ColorSkin::TYPEINFO = { "ColorSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	ColorSkin_p ColorSkin::create()
	{
		return ColorSkin_p(new ColorSkin());
	}

	ColorSkin_p ColorSkin::create(Color color, BorderI contentPadding )
	{
		auto p = new ColorSkin(color);
		p->setContentPadding(contentPadding);
		return p;
	}

	ColorSkin_p ColorSkin::create(std::initializer_list< std::tuple<State, Color> > stateColors, BorderI contentPadding )
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

	ColorSkin::ColorSkin(Color color )
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

	void ColorSkin::setColor(Color color)
	{
		m_stateColorMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
			m_color[i] = color;

		m_bOpaque = (color.a == 255);
	}

	void ColorSkin::setColor(State state, Color color)
	{
		int i = _stateToIndex(state);

		m_stateColorMask.setBit(i);

		m_color[i] = color;

		_updateOpaqueFlag();
		_updateUnsetColors();
	}

	void ColorSkin::setColor(std::initializer_list< std::tuple<State, Color> > stateColors)
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

	Color ColorSkin::color(State state) const
	{
		int i = _stateToIndex(state);
		return m_color[i];
	}

	//____ isStateIdentical() ____________________________________________________

	bool ColorSkin::isStateIdentical(State state, State comparedTo, float fraction, float fraction2) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		return (m_color[i1] == m_color[i2] && StateSkin::isStateIdentical(state, comparedTo));
	}

	//____ isOpaque() _____________________________________________________________

	bool ColorSkin::isOpaque(State state) const
	{
		return (m_color[_stateToIndex(state)].a == 255);
	}

	bool ColorSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return (m_color[_stateToIndex(state)].a == 255);
	}

	//____ render() _______________________________________________________________

	void ColorSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float fraction, float fraction2) const
	{
		BlendMode	oldBlendMode = pDevice->blendMode();

		if (m_blendMode != oldBlendMode )
			pDevice->setBlendMode(m_blendMode);

		int i = _stateToIndex(state);

		pDevice->fill( canvas.px(), m_color[i] );

		if (m_blendMode != oldBlendMode)
			pDevice->setBlendMode(oldBlendMode);
	}

	//____ markTest() _____________________________________________________________

	bool ColorSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction, float fraction2) const
	{
		if( !canvas.contains(ofs) )
			return false;

		return ( m_color[_stateToIndex(state)].a >= opacityTreshold);
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

				m_bOpaque = (alpha == 255 * StateEnum_Nb);
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