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

namespace wg
{

	using namespace Util;

	const char ColorSkin::CLASSNAME[] = {"ColorSkin"};

	//____ create() _______________________________________________________________

	ColorSkin_p ColorSkin::create()
	{
		return ColorSkin_p(new ColorSkin());
	}

	ColorSkin_p ColorSkin::create(Color color)
	{
		return ColorSkin_p(new ColorSkin(color));
	}

	ColorSkin_p ColorSkin::create(std::initializer_list< std::tuple<State, Color> > stateColors )
	{
		ColorSkin_p p = new ColorSkin();
		p->setColor(stateColors);

		return p;
	}


	//____ Constructor ____________________________________________________________

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

	//____ isInstanceOf() _________________________________________________________

	bool ColorSkin::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return ExtendedSkin::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * ColorSkin::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	ColorSkin_p ColorSkin::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ColorSkin_p( static_cast<ColorSkin*>(pObject) );

		return 0;
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

	//____ render() _______________________________________________________________

	void ColorSkin::render( GfxDevice * pDevice, const RectI& _canvas, State state ) const
	{
		BlendMode	oldBlendMode = pDevice->blendMode();

		if (m_blendMode != oldBlendMode )
			pDevice->setBlendMode(m_blendMode);

		int i = _stateToIndex(state);

		pDevice->fill( _canvas, m_color[i] );

		if (m_blendMode != oldBlendMode)
			pDevice->setBlendMode(oldBlendMode);
	}

	//____ markTest() _____________________________________________________________

	bool ColorSkin::markTest( const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold ) const
	{
		if( !canvas.contains(ofs) )
			return false;

		return ( m_color[_stateToIndex(state)].a >= opacityTreshold);
	}

	//____ isOpaque() _____________________________________________________________

	bool ColorSkin::isOpaque() const
	{
		return m_bOpaque;
	}

	bool ColorSkin::isOpaque( State state ) const
	{
		return (m_color[_stateToIndex(state)].a == 255);
	}

	bool ColorSkin::isOpaque( const RectI& rect, const SizeI& canvasSize, State state ) const
	{
		return (m_color[_stateToIndex(state)].a == 255);
	}

	//____ isStateIdentical() ____________________________________________________

	bool ColorSkin::isStateIdentical( State state, State comparedTo ) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		return (m_color[i1] == m_color[i2] && ExtendedSkin::isStateIdentical(state, comparedTo));
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
