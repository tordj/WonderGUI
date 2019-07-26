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


namespace wg
{
	const char SimpleVolumeMeter::CLASSNAME[] = {"SimpleVolumeMeter"};

	//____ Constructor ____________________________________________________________

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

		_updateValueDisplays( SizeI(0,0) );
	}

	//____ Destructor _____________________________________________________________

	SimpleVolumeMeter::~SimpleVolumeMeter()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool SimpleVolumeMeter::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * SimpleVolumeMeter::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	SimpleVolumeMeter_p SimpleVolumeMeter::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SimpleVolumeMeter_p( static_cast<SimpleVolumeMeter*>(pObject) );

		return 0;
	}

	//____ setColors() ___________________________________________________________

	void SimpleVolumeMeter::setColors( Color bottom, Color middle, Color top )
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

			SizeI canvasSize = m_pSkin ? SizeI() - m_pSkin->_contentPadding() : SizeI();

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

			SizeI size = m_pSkin ? m_size - m_pSkin->_contentPadding() : m_size;
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

		RectI canvas = m_pSkin ? m_pSkin->_contentRect( m_size, m_state ) : RectI(m_size);

		int length = (m_direction == Direction::Left || m_direction == Direction::Right) ? canvas.w : canvas.h;
		int  iPeak = (int) (peak * length);

		int iHold = _calcIHold(hold, canvas.size());


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

		RectI canvas = m_pSkin ? m_pSkin->_contentRect( m_size, m_state ) : RectI(m_size);

		int length = (m_direction == Direction::Left || m_direction == Direction::Right) ? canvas.w : canvas.h;

		int	iPeakL = (int) leftPeak * length;
		int	iPeakR = (int) rightPeak * length;
		int iHoldL = _calcIHold(leftHold, canvas);
		int iHoldR = _calcIHold(rightHold, canvas);

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

			SizeI size = m_pSkin ? m_size - m_pSkin->_contentPadding() : m_size;
			_updateValueDisplays(size);
			_requestResize();
			_requestRender();
		}
	}


	//____ _requestRenderPartial() _________________________________________________

	void SimpleVolumeMeter::_requestRenderPartial( const RectI& canvas, int newLeftPeak, int newLeftHold, int newRightPeak, int newRightHold )
	{
		int	beg = INT_MAX, end = INT_MIN;

		if( newLeftPeak != m_iPeak[0] )
		{
			beg = min(newLeftPeak,m_iPeak[0]);
			end = max(newLeftPeak,m_iPeak[0]);
		}

		if( m_bStereo && newRightPeak != m_iPeak[1] )
		{
			int b = min(newRightPeak,m_iPeak[1]);
			if(beg>b)
				beg = b;

			int e = max(newRightPeak,m_iPeak[1]);
			if(end<e)
				end = e;
		}

		if( m_iHoldHeight != 0 )
		{
			if( newLeftHold != m_iHold[0] )
			{
				int b = min(newLeftHold,m_iHold[0]) - m_iHoldHeight;
				if(beg>b)
					beg = b;

				int e = max(newLeftHold,m_iHold[0]);
				if(end<e)
					end = e;
			}

			if( m_bStereo && newRightHold != m_iHold[1] )
			{
				int b = min(newRightHold,m_iHold[1]) - m_iHoldHeight;
				if(beg>b)
					beg = b;

				int e = max(newRightHold,m_iHold[1]);
				if(end<e)
					end = e;
			}
		}

		switch (m_direction)
		{
		case Direction::Up:
			_requestRender(RectI(canvas.x, canvas.y + canvas.h - end, canvas.w, end - beg));
			break;
		case Direction::Down:
			_requestRender(RectI(canvas.x, canvas.y + beg, canvas.w, end - beg));
			break;
		case Direction::Left:
			_requestRender(RectI(canvas.x + canvas.w - end, canvas.y, end - beg, canvas.h));
			break;
		case Direction::Right:
			_requestRender(RectI(canvas.x + beg, canvas.y, end - beg, canvas.h));
			break;
		}
	}


	//____ _calcIHold() ____________________________________________________________

	int SimpleVolumeMeter::_calcIHold( float holdValue, SizeI canvas )
	{
		if( m_iHoldHeight == 0 )
			return 0;					// Should not be visible.

		int height = m_iHoldHeight;

		int canvasLength = m_direction == Direction::Up || m_direction == Direction::Down ? canvas.h : canvas.w;
		int ofs = (int)(holdValue * canvasLength);

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

	//____ _preferredSize() ________________________________________________________________

	SizeI SimpleVolumeMeter::_preferredSize() const
	{
		return (m_direction == Direction::Up || m_direction == Direction::Down ? SizeI(9, 20) : SizeI(20, 9)) * Base::pixelQuartersPerPoint();
	}

	//____ _setSize() ____________________________________________________________________

	void SimpleVolumeMeter::_setSize( const SizeI& size )
	{
		Widget::_setSize( size );

		SizeI canvasSize = m_pSkin ? size - m_pSkin->_contentPadding() : size;

		_updateValueDisplays( canvasSize );
	}

	//____ _setSkin() ________________________________________________________________

	void SimpleVolumeMeter::_setSkin( Skin * pSkin )
	{
		SizeI sz = SizeI();

		RectI oldCanvas = m_pSkin ? m_pSkin->_contentRect(sz,m_state) : RectI(sz);
		RectI newCanvas = pSkin ? pSkin->_contentRect(sz,m_state) : RectI(sz);

		Widget::_setSkin( pSkin );

		if( oldCanvas != newCanvas )
			_updateValueDisplays( newCanvas );
	}

	//____ _render() _____________________________________________________________________

	void SimpleVolumeMeter::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice, _canvas, _window);

		if( !m_state.isEnabled() )
			return;

		RectI canvas;
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		if( m_bStereo )
		{
			RectI r = canvas;

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
			RectI r = canvas;

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

	void SimpleVolumeMeter::_renderBar( GfxDevice * pDevice, int nb, const RectI& _rect )
	{
		int peakHeight 	= m_iPeak[nb];
		int holdOfs 	= m_iHold[nb];

		// Possibly render Hold

		if( m_iHoldHeight > 0 )
		{
			if( holdOfs - m_iHoldHeight > peakHeight )				// Render Hold separately if separated from Peak
			{
				Color c;

				if( holdOfs <= m_iSectionHeight[0] )
					c = m_sectionColors[0];
				else if( holdOfs > m_iSectionHeight[0] + m_iSectionHeight[1] )
					c = m_sectionColors[2];
				else
					c = m_sectionColors[1];

				RectI r = _rect;
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

				pDevice->fill( r, c );
			}
			else if( holdOfs > peakHeight )
				peakHeight = m_iHold[nb];							// Hold and Peak are connected, so we let Hold extend the peakHeight.
		}

		// Render Peak

		int ofs = 0;

		for( int i = 0 ; i < 3 ; i++ )
		{
			if( peakHeight <= 0 )
				break;

			int sectionHeight = m_iSectionHeight[i];
			if( sectionHeight > peakHeight )
				sectionHeight = peakHeight;

			RectI r = _rect;

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

			pDevice->fill( r, m_sectionColors[i] );

			ofs += sectionHeight;
			peakHeight -= sectionHeight;
		}

	}

	//____ _updateValueDisplays() ______________________________________________________

	void SimpleVolumeMeter::_updateValueDisplays( SizeI sz )
	{
		int length = sz.h;
		int width = sz.w;

		if (m_direction == Direction::Left || m_direction == Direction::Right)
			std::swap(length, width);

		m_iGap = (int)(width * m_fGap);
		if( m_iGap == 0 && m_fGap > 0.f )
			m_iGap = 1;

		m_iSidePadding = (int) (width * m_fSidePadding);
		if( m_iSidePadding == 0 && m_fSidePadding > 0.f )
			m_iSidePadding = 1;

		m_iHoldHeight = (int) m_fHoldHeight * length;
		if( m_iHoldHeight == 0 && m_fHoldHeight > 0.f )
			m_iHoldHeight = 1;

		m_iSectionHeight[0] = (int) (m_fSectionHeight[0] * length + 0.5f);
		m_iSectionHeight[1] =  ((int)((m_fSectionHeight[0] + m_fSectionHeight[1]) * length + 0.5f)) - m_iSectionHeight[0];
		m_iSectionHeight[2] = length - m_iSectionHeight[1] - m_iSectionHeight[0];

		m_iPeak[0] = (int) m_fPeak[0] * length;
		m_iPeak[1] = (int) m_fPeak[1] * length;

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

		SizeI canvasSize = m_pSkin ? m_size - m_pSkin->_contentPadding() : m_size;
		_updateValueDisplays( canvasSize );
	}

	//____ _alphaTest() ____________________________________________________________________

	bool SimpleVolumeMeter::_alphaTest( const CoordI& ofs )
	{
		//TODO: Should we detect on the bars?

		return Widget::_alphaTest(ofs);
	}
} // namespace wg
