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

#include <wg_simplevolumemeter.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>


namespace wg
{
	using namespace Util;

	const TypeInfo SimpleVolumeMeter::TYPEINFO = { "SimpleVolumeMeter", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	SimpleVolumeMeter::SimpleVolumeMeter()
	{
		m_direction = Direction::Up;

		m_sectionColors[0] = Color::Green;
		m_sectionColors[1] = Color::Yellow;
		m_sectionColors[2] = Color::Red;

		m_fSectionHeight[0] = 0.75f;
		m_fSectionHeight[1] = 0.18f;
		m_fSectionHeight[2] = 0.07f;

		m_fHoldHeight = 0.10f;
		m_bStereo = false;
		m_fPeak[0] = 0.f;
		m_fPeak[1] = 0.f;
		m_fHold[0] = 0.f;
		m_fHold[1] = 0.f;

		m_fGap = 0.1f;
		m_fSidePadding = 0.1f;

		_updateValueDisplays( Size(0,0) );
	}

	//____ Destructor _____________________________________________________________

	SimpleVolumeMeter::~SimpleVolumeMeter()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SimpleVolumeMeter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setColors() ___________________________________________________________

	void SimpleVolumeMeter::setColors( HiColor bottom, HiColor middle, HiColor top )
	{
		if( bottom != m_sectionColors[0] || middle != m_sectionColors[1] || top != m_sectionColors[2] )
		{
			m_sectionColors[0] = bottom;
			m_sectionColors[1] = middle;
			m_sectionColors[2] = top;
			_requestRender();
		}
	}

	//____ setSections() _________________________________________________________

	void SimpleVolumeMeter::setSections( float bottomFraction, float topFraction )
	{
		limit( bottomFraction, 0.f, 1.f );
		limit( topFraction, 0.f, 1.f - bottomFraction );

		float middleFraction = 1.f - bottomFraction - topFraction;

		if( bottomFraction != m_fSectionHeight[0] || topFraction != m_fSectionHeight[2] )
		{
			m_fSectionHeight[0] = bottomFraction;
			m_fSectionHeight[1] = middleFraction;
			m_fSectionHeight[2] = topFraction;

			Size canvasSize = Size() - OO(skin)._contentPaddingSize();

			_updateValueDisplays( canvasSize );
			_requestRender();
		}
	}

	//____ setHoldHeight() ________________________________________________________

	void SimpleVolumeMeter::setHoldHeight( float fraction )
	{
		limit( fraction, 0.f, 0.25f );

		if( m_fHoldHeight != fraction )
		{
			m_fHoldHeight = fraction;

			Size size = m_size - OO(skin)._contentPaddingSize();
			_updateValueDisplays( size );
			_requestRender();
		}
	}

	//____ setValue() ______________________________________________________________

	void SimpleVolumeMeter::setValue( float peak, float hold )
	{
		limit( peak, 0.f, 1.f );
		limit( hold, 0.f, 1.f );

		m_fPeak[0] = peak;
		m_fHold[0] = hold;

		Rect canvas = OO(skin)._contentRect( m_size, m_state );

		MU length = (m_direction == Direction::Left || m_direction == Direction::Right) ? canvas.w : canvas.h;
		MU  iPeak = (peak * length).aligned();

		MU iHold = _calcIHold(hold, canvas.size());


		if( m_bStereo )
		{
			m_bStereo = false;
			_requestRender();
		}
		else if( m_iPeak[0] != iPeak || m_iHold[0] != iHold )
		{
			_requestRenderPartial( canvas, iPeak, iHold, 0, 0 );
		}

		m_iPeak[0] = iPeak;
		m_iHold[0] = iHold;
	}

	void SimpleVolumeMeter::setValue( float leftPeak, float leftHold, float rightPeak, float rightHold )
	{
		limit( leftPeak, 0.f, 1.f );
		limit( leftHold, 0.f, 1.f );
		limit( rightPeak, 0.f, 1.f );
		limit( rightHold, 0.f, 1.f );

		m_fPeak[0] = leftPeak;
		m_fPeak[1] = rightPeak;
		m_fHold[0] = leftHold;
		m_fHold[1] = rightHold;

		Rect canvas = OO(skin)._contentRect( m_size, m_state );

		MU length = (m_direction == Direction::Left || m_direction == Direction::Right) ? canvas.w : canvas.h;

		MU	iPeakL = (leftPeak * length).aligned();
		MU	iPeakR = (rightPeak * length).aligned();
		MU	iHoldL = _calcIHold(leftHold, canvas);
		MU	iHoldR = _calcIHold(rightHold, canvas);


		if( !m_bStereo )
		{
			m_bStereo = true;
			_requestRender();
		}
		else if( m_iPeak[0] != iPeakL || m_iHold[0] != iHoldL || m_iPeak[1] != iPeakR || m_iHold[1] != iHoldR )
		{
			_requestRenderPartial( canvas, iPeakL, iHoldL, iPeakR, iHoldR );
		}

		m_iPeak[0] = iPeakL;
		m_iPeak[1] = iPeakR;
		m_iHold[0] = iHoldL;
		m_iHold[1] = iHoldR;

	}

	//____ setDirection() _________________________________________________________

	void SimpleVolumeMeter::setDirection(Direction direction)
	{
		if (direction != m_direction)
		{
			m_direction = direction;

			Size size = m_size - OO(skin)._contentPaddingSize();
			_updateValueDisplays(size);
			_requestResize();
			_requestRender();
		}
	}


	//____ _requestRenderPartial() _________________________________________________

	void SimpleVolumeMeter::_requestRenderPartial( const Rect& canvas, MU newLeftPeak, MU newLeftHold, MU newRightPeak, MU newRightHold )
	{
		MU	beg = MU::Max, end = MU::Max;

		if( newLeftPeak != m_iPeak[0] )
		{
			beg = min(newLeftPeak,m_iPeak[0]);
			end = max(newLeftPeak,m_iPeak[0]);
		}

		if( m_bStereo && newRightPeak != m_iPeak[1] )
		{
			MU b = min(newRightPeak,m_iPeak[1]);
			if(beg>b)
				beg = b;

			MU e = max(newRightPeak,m_iPeak[1]);
			if(end<e)
				end = e;
		}

		if( m_iHoldHeight != 0 )
		{
			if( newLeftHold != m_iHold[0] )
			{
				MU b = min(newLeftHold,m_iHold[0]) - m_iHoldHeight;
				if(beg>b)
					beg = b;

				MU e = max(newLeftHold,m_iHold[0]);
				if(end<e)
					end = e;
			}

			if( m_bStereo && newRightHold != m_iHold[1] )
			{
				MU b = min(newRightHold,m_iHold[1]) - m_iHoldHeight;
				if(beg>b)
					beg = b;

				MU e = max(newRightHold,m_iHold[1]);
				if(end<e)
					end = e;
			}
		}

		switch (m_direction)
		{
		case Direction::Up:
			_requestRender(Rect(canvas.x, canvas.y + canvas.h - end, canvas.w, end - beg));
			break;
		case Direction::Down:
			_requestRender(Rect(canvas.x, canvas.y + beg, canvas.w, end - beg));
			break;
		case Direction::Left:
			_requestRender(Rect(canvas.x + canvas.w - end, canvas.y, end - beg, canvas.h));
			break;
		case Direction::Right:
			_requestRender(Rect(canvas.x + beg, canvas.y, end - beg, canvas.h));
			break;
		}
	}


	//____ _calcIHold() ____________________________________________________________

	MU SimpleVolumeMeter::_calcIHold( float holdValue, Size canvas )
	{
		if( m_iHoldHeight == 0 )
			return 0;					// Should not be visible.

		MU height = m_iHoldHeight;

		MU canvasLength = m_direction == Direction::Up || m_direction == Direction::Down ? canvas.h : canvas.w;
		MU ofs = (holdValue * canvasLength).aligned();

		if( ofs > m_iSectionHeight[0] )
		{
			if( ofs - height < m_iSectionHeight[0] )
				ofs = m_iSectionHeight[0] + height;
			else if( ofs > m_iSectionHeight[0] + m_iSectionHeight[1] )
			{
				if( ofs - height < m_iSectionHeight[0] + m_iSectionHeight[1] )
					ofs = m_iSectionHeight[0] + m_iSectionHeight[1] + height;
			}
		}

		return ofs;
	}

	//____ preferredSize() ________________________________________________________________

	Size SimpleVolumeMeter::preferredSize() const
	{
		return m_direction == Direction::Up || m_direction == Direction::Down ? Size(9, 20) : Size(20, 9);
	}

	//____ _resize() ____________________________________________________________________

	void SimpleVolumeMeter::_resize( const Size& size )
	{
		Widget::_resize( size );

		Size canvasSize = size - OO(skin)._contentPaddingSize();

		_updateValueDisplays( canvasSize );
	}

	//____ _render() _____________________________________________________________________

	void SimpleVolumeMeter::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice, _canvas, _window);

		if( !m_state.isEnabled() )
			return;

		Rect canvas = OO(skin)._contentRect(_canvas, m_state);

		if( m_bStereo )
		{
			Rect r = canvas;

			if (m_direction == Direction::Up || m_direction == Direction::Down)
			{
				r.w = (r.w - m_iGap) / 2 - m_iSidePadding;
				r.x += m_iSidePadding;
			}
			else
			{
				r.h = (r.h - m_iGap) / 2 - m_iSidePadding;
				r.y += m_iSidePadding;
			}

			_renderBar( pDevice, 0, r );

			if (m_direction == Direction::Up || m_direction == Direction::Down)
				r.x += r.w + m_iGap;
			else
				r.y += r.h + m_iGap;

			_renderBar( pDevice, 1, r );

		}
		else
		{
			Rect r = canvas;

			if (m_direction == Direction::Up || m_direction == Direction::Down)
			{
				r.w = r.w - 2 * m_iSidePadding;
				r.x += m_iSidePadding;
			}
			else
			{
				r.h = r.h - 2 * m_iSidePadding;
				r.y += m_iSidePadding;
			}
			_renderBar( pDevice, 0, r );
		}
	}

	//____ _renderBar()_____________________________________________________________

	void SimpleVolumeMeter::_renderBar( GfxDevice * pDevice, int nb, const Rect& _rect )
	{
		MU peakHeight 	= m_iPeak[nb];
		MU holdOfs 		= m_iHold[nb];

		// Possibly render Hold

		if( m_iHoldHeight > 0 )
		{
			if( holdOfs - m_iHoldHeight > peakHeight )				// Render Hold separately if separated from Peak
			{
				HiColor c;

				if( holdOfs <= m_iSectionHeight[0] )
					c = m_sectionColors[0];
				else if( holdOfs > m_iSectionHeight[0] + m_iSectionHeight[1] )
					c = m_sectionColors[2];
				else
					c = m_sectionColors[1];

				Rect r = _rect;
				switch (m_direction)
				{
				case Direction::Up:
					r.y += _rect.h - holdOfs;
					r.h = m_iHoldHeight;
					break;
				case Direction::Down:
					r.y += holdOfs - m_iHoldHeight;
					r.h = m_iHoldHeight;
					break;
				case Direction::Left:
					r.x += _rect.w - holdOfs;
					r.w = m_iHoldHeight;
					break;
				case Direction::Right:
					r.x += holdOfs - m_iHoldHeight;
					r.w = m_iHoldHeight;
					break;
				}

				pDevice->fill( r.px(), c );
			}
			else if( holdOfs > peakHeight )
				peakHeight = m_iHold[nb];							// Hold and Peak are connected, so we let Hold extend the peakHeight.
		}

		// Render Peak

		MU ofs = 0;

		for( int i = 0 ; i < 3 ; i++ )
		{
			if( peakHeight <= 0 )
				break;

			MU sectionHeight = m_iSectionHeight[i];
			if( sectionHeight > peakHeight )
				sectionHeight = peakHeight;

			Rect r = _rect;

			switch (m_direction)
			{
			case Direction::Up:
				r.y += _rect.h - ofs - sectionHeight;
				r.h = sectionHeight;
				break;

			case Direction::Down:
				r.y += ofs;
				r.h = sectionHeight;
				break;

			case Direction::Left:
				r.x += _rect.w - ofs - sectionHeight;
				r.w = sectionHeight;
				break;

			case Direction::Right:
				r.x += ofs;
				r.w = sectionHeight;
				break;
			}

			pDevice->fill( r.px(), m_sectionColors[i] );

			ofs += sectionHeight;
			peakHeight -= sectionHeight;
		}

	}

	//____ _updateValueDisplays() ______________________________________________________

	void SimpleVolumeMeter::_updateValueDisplays( Size sz )
	{
		MU length = sz.h;
		MU width = sz.w;

		if (m_direction == Direction::Left || m_direction == Direction::Right)
			std::swap(length, width);

		m_iGap = (width * m_fGap).aligned();
		if( m_iGap == 0 && m_fGap > 0.f )
			m_iGap.qpix = 4;

		m_iSidePadding = (width * m_fSidePadding).aligned();
		if( m_iSidePadding == 0 && m_fSidePadding > 0.f )
			m_iSidePadding = 4;

		m_iHoldHeight = (m_fHoldHeight * length).aligned();
		if( m_iHoldHeight == 0 && m_fHoldHeight > 0.f )
			m_iHoldHeight = 4;

		m_iSectionHeight[0] = (m_fSectionHeight[0] * length + 0.5f).aligned();
		m_iSectionHeight[1] = (((m_fSectionHeight[0] + m_fSectionHeight[1]) * length + 0.5f)).aligned() - m_iSectionHeight[0];
		m_iSectionHeight[2] = length - m_iSectionHeight[1] - m_iSectionHeight[0];

		m_iPeak[0] = (m_fPeak[0] * length).aligned();
		m_iPeak[1] = (m_fPeak[1] * length).aligned();

		m_iHold[0] = _calcIHold(m_fHold[0], sz);
		m_iHold[1] = _calcIHold(m_fHold[1], sz);
	}

	//____ _cloneContent() _________________________________________________________________

	void SimpleVolumeMeter::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const SimpleVolumeMeter * pOrg = static_cast<const SimpleVolumeMeter*>(_pOrg);

		for( int i = 0 ; i < 3 ; i++ )
		{
			m_sectionColors[i] 	= pOrg->m_sectionColors[i];
			m_fSectionHeight[i] = pOrg->m_fSectionHeight[i];
		}

		m_direction = pOrg->m_direction;
		m_fHoldHeight = pOrg->m_fHoldHeight;
		m_fGap = pOrg->m_fGap;
		m_fSidePadding = pOrg->m_fSidePadding;
		m_bStereo = pOrg->m_bStereo;
		m_fPeak[0] = pOrg->m_fPeak[0];
		m_fPeak[1] = pOrg->m_fPeak[1];
		m_fHold[0] = pOrg->m_fHold[0];
		m_fHold[1] = pOrg->m_fHold[1];

		Size canvasSize = m_size - OO(skin)._contentPaddingSize();
		_updateValueDisplays( canvasSize );
	}

	//____ _alphaTest() ____________________________________________________________________

	bool SimpleVolumeMeter::_alphaTest( const Coord& ofs )
	{
		//TODO: Should we detect on the bars?

		return Widget::_alphaTest(ofs);
	}
} // namespace wg
