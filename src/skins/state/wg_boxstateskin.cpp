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

#include <wg_boxstateskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const TypeInfo BoxStateSkin::TYPEINFO = { "BoxStateSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	BoxStateSkin_p BoxStateSkin::create()
	{
		return BoxStateSkin_p(new BoxStateSkin());
	}

	BoxStateSkin_p BoxStateSkin::create(BorderI frame, Color fillColor, Color frameColor )
	{
		return BoxStateSkin_p(new BoxStateSkin(frame, fillColor, frameColor));
	}

	BoxStateSkin_p BoxStateSkin::create(BorderI frame, std::initializer_list< std::tuple<State, Color, Color> > stateColors )
	{
		BoxStateSkin_p p = new BoxStateSkin();

		p->setFrame(frame);
		p->setColors(stateColors);

		return p;
	}


	//____ constructor ____________________________________________________________

	BoxStateSkin::BoxStateSkin()
	{
		m_stateColorMask = 1;

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_fillColor[i] = Color::White;
			m_frameColor[i] = Color::White;
		}

		m_bOpaque = true;
	}

	BoxStateSkin::BoxStateSkin(BorderI frame, Color fillColor, Color frameColor )
	{
		m_frame = frame;
		setColors(fillColor, frameColor);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BoxStateSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void BoxStateSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlag();
	}

	//____ setFrame() ____________________________________________________

	void BoxStateSkin::setFrame(BorderI frame)
	{
		bool hadFrame = (m_frame.width() + m_frame.height() > 0);
		bool hasFrame = (frame.width() + frame.height() > 0);

		m_frame = frame;

		if (hadFrame != hasFrame)
			_updateOpaqueFlag();
	}

	//____ setColors() ________________________________________________________

	void BoxStateSkin::setColors(Color fill, Color frame)
	{
		m_stateColorMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_fillColor[i] = fill;
			m_frameColor[i] = frame;
		}

		bool hasFrame = (m_frame.width() + m_frame.height() > 0);
		if (fill.a == 255 && (!hasFrame || frame.a == 255))
			m_bOpaque = true;
		else
			m_bOpaque = false;
	}

	void BoxStateSkin::setColors(State state, Color fill, Color frame)
	{
		int i = _stateToIndex(state);

		m_stateColorMask.setBit(i);

		m_fillColor[i] = fill;
		m_frameColor[i] = frame;

		_updateUnsetColors();
		_updateOpaqueFlag();
	}

	void BoxStateSkin::setColors(std::initializer_list< std::tuple<State, Color, Color> > stateColors)
	{
		for (auto& state : stateColors)
		{
			int i = _stateToIndex(std::get<0>(state));
			m_stateColorMask.setBit(i);
			m_fillColor[i] = std::get<1>(state);
			m_frameColor[i] = std::get<2>(state);
		}

		_updateUnsetColors();
		_updateOpaqueFlag();
	}

	//____ colors() ______________________________________________________

	std::tuple<Color, Color> BoxStateSkin::colors(State state) const
	{
		int i = _stateToIndex(state);
		return std::make_tuple(m_fillColor[i], m_frameColor[i]);
	}

	//____ _render() _______________________________________________________________

	void BoxStateSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
	{
		//TODO: Optimize! Clip patches against canvas first.

		BlendMode	oldBlendMode = pDevice->blendMode();

		RectI canvas = _canvas.px();

		if (m_blendMode != oldBlendMode )
			pDevice->setBlendMode(m_blendMode);

		int i = _stateToIndex(state);
		if( m_frame.width() + m_frame.height() == 0 || m_frameColor[i] == m_fillColor[i] )
		{
			pDevice->fill( canvas, m_fillColor[i] );
		}
		else
		{
			BorderI frame = pointsToPixels(m_frame);

			RectI top( canvas.x, canvas.y, canvas.w, frame.top );
			RectI left( canvas.x, canvas.y+frame.top, frame.left, canvas.h - frame.height() );
			RectI right( canvas.x + canvas.w - frame.right, canvas.y+frame.top, frame.right, canvas.h - frame.height() );
			RectI bottom( canvas.x, canvas.y + canvas.h - frame.bottom, canvas.w, frame.bottom );
			RectI center( canvas - frame );

			pDevice->fill( top, m_frameColor[i] );
			pDevice->fill( left, m_frameColor[i] );
			pDevice->fill( right, m_frameColor[i] );
			pDevice->fill( bottom, m_frameColor[i] );

			if( center.w > 0 || center.h > 0 )
				pDevice->fill( center, m_fillColor[i] );
		}

		if (m_blendMode != oldBlendMode)
			pDevice->setBlendMode(oldBlendMode);
	}

	//____ minSize() ______________________________________________________________

	Size BoxStateSkin::minSize() const
	{
		Size content = StateSkin::minSize();
		Size frame = Border(m_frame).aligned();

		return Size::max(content,frame);
	}

	//____ preferredSize() ________________________________________________________

	Size BoxStateSkin::preferredSize() const
	{
		Size content = StateSkin::minSize();
		Size frame = Border(m_frame).aligned();

		return Size::max(content, frame);
	}

	//____ sizeForContent() _______________________________________________________

	Size BoxStateSkin::sizeForContent( const Size& contentSize ) const
	{
		Size content = StateSkin::sizeForContent(contentSize);
		Size frame = Border(m_frame).aligned();

		return Size::max(content, frame);
	}

	//____ markTest() _____________________________________________________________

	bool BoxStateSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if( !canvas.contains(ofs) )
			return false;

		int opacity;

		if( m_bOpaque )
			opacity = 255;
		else
		{
			int i = _stateToIndex(state);

			Rect center = canvas - Border(m_frame).aligned();
			if( center.contains(ofs) )
				opacity = m_fillColor[i].a;
			else
				opacity = m_frameColor[i].a;
		}

		return ( opacity >= opacityTreshold );
	}

	//____ isOpaque() _____________________________________________________________

	bool BoxStateSkin::isOpaque( State state ) const
	{
		int i = _stateToIndex(state);
		if( m_bOpaque || (m_fillColor[i].a == 255 && (m_frameColor[i] == 255 || (m_frame.width() + m_frame.height() == 0))) )
			return true;

		return false;
	}

	bool BoxStateSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		if( m_bOpaque )
			return true;

		Rect center = Rect(canvasSize) - Border(m_frame).aligned();
		int i = _stateToIndex(state);
		if( center.contains(rect) )
			return m_fillColor[i].a == 255;
		else if( !center.intersectsWith(rect) )
			return m_frameColor[i].a == 255;

		return m_fillColor[i].a == 255 && m_frameColor[i].a == 255;
	}

	//____ isStateIdentical() ____________________________________________________

	bool BoxStateSkin::isStateIdentical( State state, State comparedTo, float fraction) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		if( m_fillColor[i1] == m_fillColor[i2] && (m_frame.isEmpty() || m_frameColor[i1] == m_frameColor[i2]) &&
			StateSkin::isStateIdentical(state,comparedTo) )
			return true;
		else
			return false;
	}

	//____ _updateOpaqueFlag() ____________________________________________________

	void BoxStateSkin::_updateOpaqueFlag()
	{
		switch (m_blendMode)
		{
			case BlendMode::Replace:
				m_bOpaque = true;
				break;

			case BlendMode::Blend:
			{
				int alpha = 0;
				int frameAlpha = 0;

				for (int i = 0; i < StateEnum_Nb; i++)
				{
					alpha += (int)m_fillColor[i].a;
					frameAlpha += (int)m_frameColor[i].a;
				}

				bool hasFrame = (m_frame.width() + m_frame.height() > 0);

				if (alpha == 255 * StateEnum_Nb && (!hasFrame || frameAlpha == 255 * StateEnum_Nb))
					m_bOpaque = true;
				else
					m_bOpaque = false;

				break;
			}

			default:
				m_bOpaque = false;
		}
	}

	//____ _updateUnsetColors() _______________________________________________

	void BoxStateSkin::_updateUnsetColors()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateColorMask);
				m_fillColor[i] = m_fillColor[bestAlternative];
				m_frameColor[i] = m_frameColor[bestAlternative];
			}
		}
	}


} // namespace wg
