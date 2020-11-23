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

#include <wg_boxskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	using namespace Util;

	const TypeInfo BoxSkin::TYPEINFO = { "BoxSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	BoxSkin_p BoxSkin::create()
	{
		return BoxSkin_p(new BoxSkin());
	}

	BoxSkin_p BoxSkin::create(BorderI frame, HiColor fillColor, HiColor frameColor )
	{
		return BoxSkin_p(new BoxSkin(frame, fillColor, frameColor));
	}

	BoxSkin_p BoxSkin::create(BorderI frame, std::initializer_list< std::tuple<State, HiColor, HiColor> > stateColors )
	{
		BoxSkin_p p = new BoxSkin();

		p->setFrame(frame);
		p->setColors(stateColors);

		return p;
	}


	//____ constructor ____________________________________________________________

	BoxSkin::BoxSkin()
	{
		m_stateColorMask = 1;

		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_fillColor[i] = HiColor::White;
			m_frameColor[i] = HiColor::White;
		}

		m_bOpaque = true;
	}

	BoxSkin::BoxSkin(BorderI frame, HiColor fillColor, HiColor frameColor )
	{
		m_frame = frame;
		setColors(fillColor, frameColor);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BoxSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void BoxSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlag();
	}

	//____ setFrame() ____________________________________________________

	void BoxSkin::setFrame(BorderI frame)
	{
		bool hadFrame = (m_frame.width() + m_frame.height() > 0);
		bool hasFrame = (frame.width() + frame.height() > 0);

		m_frame = frame;

		if (hadFrame != hasFrame)
			_updateOpaqueFlag();
	}

	//____ setColors() ________________________________________________________

	void BoxSkin::setColors(HiColor fill, HiColor frame)
	{
		m_stateColorMask = 1;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_fillColor[i] = fill;
			m_frameColor[i] = frame;
		}

		bool hasFrame = (m_frame.width() + m_frame.height() > 0);
		if (fill.a == 4096 && (!hasFrame || frame.a == 4096))
			m_bOpaque = true;
		else
			m_bOpaque = false;
	}

	void BoxSkin::setColors(State state, HiColor fill, HiColor frame)
	{
		int i = _stateToIndex(state);

		m_stateColorMask.setBit(i);

		m_fillColor[i] = fill;
		m_frameColor[i] = frame;

		_updateUnsetColors();
		_updateOpaqueFlag();
	}

	void BoxSkin::setColors(std::initializer_list< std::tuple<State, HiColor, HiColor> > stateColors)
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

	std::tuple<HiColor, HiColor> BoxSkin::colors(State state) const
	{
		int i = _stateToIndex(state);
		return std::make_tuple(m_fillColor[i], m_frameColor[i]);
	}

	//____ _render() _______________________________________________________________

	void BoxSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		//TODO: Optimize! Clip patches against canvas first.

		RenderSettings settings(pDevice, m_layer, m_blendMode);

		RectI canvas = _canvas.px();

		int i = _stateToIndex(state);
		pDevice->fill(canvas, m_fillColor[i]);


		if( m_frame.width() + m_frame.height() == 0 || m_frameColor[i] == m_fillColor[i] )
		{
			pDevice->fill( canvas, m_fillColor[i] );
		}
		else
		{
			BorderI frame = pointsToPixels(m_frame);

			if (frame.width() >= canvas.w || frame.height() >= canvas.h)
			{
				pDevice->fill(canvas, m_frameColor[i]);
			}
			else
			{
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
		}

	}

	//____ minSize() ______________________________________________________________

	Size BoxSkin::minSize() const
	{
		Size content = StateSkin::minSize();
		Size frame = Border(m_frame).aligned();

		return Size::max(content,frame);
	}

	//____ preferredSize() ________________________________________________________

	Size BoxSkin::preferredSize() const
	{
		Size content = StateSkin::minSize();
		Size frame = Border(m_frame).aligned();

		return Size::max(content, frame);
	}

	//____ sizeForContent() _______________________________________________________

	Size BoxSkin::sizeForContent( const Size& contentSize ) const
	{
		Size content = StateSkin::sizeForContent(contentSize);
		Size frame = Border(m_frame).aligned();

		return Size::max(content, frame);
	}

	//____ markTest() _____________________________________________________________

	bool BoxSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
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
				opacity = m_fillColor[i].a * 4096/255;
			else
				opacity = m_frameColor[i].a * 4096/255;
		}

		return ( opacity >= opacityTreshold );
	}

	//____ isOpaque() _____________________________________________________________

	bool BoxSkin::isOpaque( State state ) const
	{
		int i = _stateToIndex(state);
		if( m_bOpaque || (m_fillColor[i].a == 4096 && (m_frameColor[i].a == 4096 || (m_frame.width() + m_frame.height() == 0))) )
			return true;

		return false;
	}

	bool BoxSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		if( m_bOpaque )
			return true;

		Rect center = Rect(canvasSize) - Border(m_frame).aligned();
		int i = _stateToIndex(state);
		if( center.contains(rect) )
			return m_fillColor[i].a == 4096;
		else if( !center.intersectsWith(rect) )
			return m_frameColor[i].a == 4096;

		return m_fillColor[i].a == 4096 && m_frameColor[i].a == 4096;
	}

	//____ dirtyRect() ______________________________________________________

	Rect BoxSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return Rect();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if (m_fillColor[i1] != m_fillColor[i2] || (!m_frame.isEmpty() && m_frameColor[i1] != m_frameColor[i2]))
			return canvas;

		return StateSkin::dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}

	//____ _updateOpaqueFlag() ____________________________________________________

	void BoxSkin::_updateOpaqueFlag()
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

				if (alpha == 4096 * StateEnum_Nb && (!hasFrame || frameAlpha == 4096 * StateEnum_Nb))
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

	void BoxSkin::_updateUnsetColors()
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
