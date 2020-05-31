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

#include <wg_colorstateskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const TypeInfo ColorStateSkin::TYPEINFO = { "ColorStateSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	ColorStateSkin_p ColorStateSkin::create()
	{
		return ColorStateSkin_p(new ColorStateSkin());
	}

	ColorStateSkin_p ColorStateSkin::create(Color color, BorderI contentPadding )
	{
		auto p = new ColorStateSkin(color);
		p->setContentPadding(contentPadding);
		return p;
	}

	ColorStateSkin_p ColorStateSkin::create(std::initializer_list< std::tuple<State, Color> > stateColors, BorderI contentPadding )
	{
		auto p = new ColorStateSkin();
		p->setColor(stateColors);
		p->setContentPadding(contentPadding);
		return p;
	}


	//____ constructor ____________________________________________________________

	ColorStateSkin::ColorStateSkin()
	{
		m_stateColorMask = 1;

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
			m_color[i] = Color::White;

		m_bOpaque = true;
	}

	ColorStateSkin::ColorStateSkin(Color color )
	{
		setColor(color);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ColorStateSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void ColorStateSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlag();
	}

	//____ setColor() ________________________________________________________

	void ColorStateSkin::setColor(Color color)
	{
		m_stateColorMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
			m_color[i] = color;

		m_bOpaque = (color.a == 255);
	}

	void ColorStateSkin::setColor(State state, Color color)
	{
		int i = _stateToIndex(state);

		m_stateColorMask.setBit(i);

		m_color[i] = color;

		_updateOpaqueFlag();
		_updateUnsetColors();
	}

	void ColorStateSkin::setColor(std::initializer_list< std::tuple<State, Color> > stateColors)
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

	Color ColorStateSkin::color(State state) const
	{
		int i = _stateToIndex(state);
		return m_color[i];
	}

	//____ isStateIdentical() ____________________________________________________

	bool ColorStateSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		return (m_color[i1] == m_color[i2] && StateSkin::isStateIdentical(state, comparedTo));
	}

	//____ isOpaque() _____________________________________________________________

	bool ColorStateSkin::isOpaque() const
	{
		return m_bOpaque;
	}

	bool ColorStateSkin::isOpaque(State state) const
	{
		return (m_color[_stateToIndex(state)].a == 255);
	}

	bool ColorStateSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return (m_color[_stateToIndex(state)].a == 255);
	}

	//____ render() _______________________________________________________________

	void ColorStateSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float fraction) const
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

	bool ColorStateSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if( !canvas.contains(ofs) )
			return false;

		return ( m_color[_stateToIndex(state)].a >= opacityTreshold);
	}

	//____ _updateOpaqueFlag() ____________________________________________________

	void ColorStateSkin::_updateOpaqueFlag()
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

	void ColorStateSkin::_updateUnsetColors()
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
