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

#include <wg_volumemeter.h>
#include <wg_gfxdevice.h>
#include <wg_userdefines.h>
#include <wg_msg.h>
#include <wg_base.h>
#include <wg_msgrouter.h>

#include <math.h>



namespace wg
{
	const TypeInfo VolumeMeter::TYPEINFO = { "VolumeMeter", &Widget::TYPEINFO };

	static inline const float round(const float x) {return floorf(x+0.5f);}

	//____ constructor ____________________________________________________________

	VolumeMeter::VolumeMeter()
	{
		m_LEDColors[0][0] = Color::Black;
		m_LEDColors[1][0] = Color::Black;
		m_LEDColors[2][0] = Color::Black;
		m_LEDColors[0][1] = Color::Green;
		m_LEDColors[1][1] = Color::Yellow;
		m_LEDColors[2][1] = Color::Red;

		m_nSectionLEDs[0] = 8;
		m_nSectionLEDs[1] = 2;
		m_nSectionLEDs[2] = 1;

		m_nLEDs = m_nSectionLEDs[0] + m_nSectionLEDs[1] + m_nSectionLEDs[2];
		m_LEDSpacing = 0.33f;
		m_direction = Direction::Up;

		m_iPeak = 0;
		m_iHold = 0;
		m_fPeak = 0.0f;

		m_fSidePadding = 0.0f;
		m_iSidePadding = 0;

		m_bZeroInMiddle = false;

		m_LEDStates.resize(m_nLEDs);
		for(int i=0;i<m_nLEDs;i++)
			m_LEDStates[i] = 0.0f;

		m_bUseFades = false;

		_startReceiveUpdates();
	}

	//____ Destructor _____________________________________________________________

	VolumeMeter::~VolumeMeter()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& VolumeMeter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setDirection() ________________________________________________________

	void VolumeMeter::setDirection( Direction direction )
	{
		if( direction != m_direction )
		{
			m_direction = direction;
			_requestResize();
			_requestRender();
		}
	}

	//____ setLEDColors() ___________________________________________________________

	void VolumeMeter::setLEDColors(	HiColor bottomOn, HiColor middleOn, HiColor topOn,
									HiColor bottomOff, HiColor middleOff, HiColor topOff )
	{
		m_LEDColors[0][0] = bottomOff;
		m_LEDColors[0][1] = bottomOn;

		m_LEDColors[1][0] = middleOff;
		m_LEDColors[1][1] = middleOn;

		m_LEDColors[2][0] = topOff;
		m_LEDColors[2][1] = topOn;

		_requestRender();
	}

	//____ setNbLEDs() _________________________________________________________

	void VolumeMeter::setNbLEDs( int bottomSection, int middleSection, int topSection )
	{
		if( bottomSection < 0 )
			bottomSection = 0;
		if( middleSection < 0 )
			middleSection = 0;
		if( topSection < 0 )
			topSection = 0;

		if( bottomSection != m_nSectionLEDs[0] || middleSection != m_nSectionLEDs[1] || topSection != m_nSectionLEDs[2] )
		{
			m_nSectionLEDs[0] = bottomSection;
			m_nSectionLEDs[1] = middleSection;
			m_nSectionLEDs[2] = topSection;
			m_nLEDs = bottomSection + middleSection + topSection;
			_requestResize();
			_requestRender();
		}



		m_LEDStates.resize(m_nLEDs);
		for(int i=0;i<m_nLEDs;i++)
			m_LEDStates[i] = 0.0f;
	}

	//____ setLEDSpacing() ___________________________________________________________

	void VolumeMeter::setLEDSpacing( float spacing )
	{
		if( spacing < 0.f )
			spacing = 0.f;

		if( spacing != m_LEDSpacing )
		{
			m_LEDSpacing = spacing;
			_requestResize();
			_requestRender();
		}
	}


	//____ setValue() ______________________________________________________________

	void VolumeMeter::setValue( float fPeak, float fHold )
	{
		limit( fPeak, 0.f, 1.f );
		limit( fHold, 0.f, 1.f );

		int iPeak = (int)round(fPeak * m_nLEDs);
		int iHold = (int)round(fHold * m_nLEDs);

		if(iPeak != m_iPeak || iHold != m_iHold)
		{
			m_iPeak = iPeak;
			m_iHold = iHold;
		}

		m_fPeak = fPeak;

	}


	//____ preferredSize() ________________________________________________________________

	Size VolumeMeter::preferredSize() const
	{
		Size	sz = m_direction == Direction::Up || m_direction == Direction::Down ? Size(10,5*m_nLEDs) : Size( 5*m_nLEDs,10);

		return OO(skin)._sizeForContent( sz );
	}

	//____ _resize() ____________________________________________________________________

	void VolumeMeter::_resize( const Size& size )
	{
		Widget::_resize( size );

		switch (m_direction)
		{
			case Direction::Left:
			case Direction::Right:
				m_iSidePadding = size.h * m_fSidePadding;
				break;
			case Direction::Up:
			case Direction::Down:
			default:
				m_iSidePadding = size.w * m_fSidePadding;
				break;
		}

		if(m_iSidePadding <= 0)
			m_iSidePadding = 0;
	}

	//____ _update() ______________________________________________________________

	void VolumeMeter::_update(int microPassed, int64_t microsecTimestamp)
	{
		int firstRenderLED = -1;
		int lastRenderLED = -1;

		for( int i = 0 ; i < m_nLEDs ; i++ )
		{
			bool 	on = false;

			// NB: Hold is not implemented for Zero In Middle
			if(m_bZeroInMiddle)
			{
				float d = 1.0f / (float)(m_nLEDs - 1);
				float d2 = 0.5f / (float)(m_nLEDs);


				float id = d*(float)i;

				// This one is tricky...

				if(id < 0.5f)
				{
					if(m_fPeak < id + d2)
						on = true;
				}
				else if(id > 0.5f)
				{
					if(m_fPeak > id - d2)
						on = true;
				}
				else
					on = true;
			}
			else // Normal mode
			{
				if( i < m_iPeak || i+1 == m_iHold )
					on = true;
			}


			float	LEDState;

			if( on )
				LEDState = 1.f;
			else
			{
				if( m_bUseFades )
				{
					LEDState = m_LEDStates[i]*0.7f; // Fade out

					if(LEDState < 0.1f)
						LEDState = 0.f;
				}
				else LEDState =0.f;
			}

			if( LEDState != m_LEDStates[i] )
			{
				m_LEDStates[i] = LEDState;
				if( firstRenderLED == -1 )
					firstRenderLED = i;
				lastRenderLED = i;
			}

		}

		// Possibly calculate a dirty rectangle

		if( firstRenderLED != -1 )
		{
			Size sz = m_size;
			Rect rect;

			MU meterLen = (m_direction == Direction::Up || m_direction == Direction::Down) ? sz.h : sz.w;

			MU dirtBeg = firstRenderLED*meterLen/m_nLEDs;
			MU dirtLen = (lastRenderLED+1)*meterLen/m_nLEDs + 1 - dirtBeg;	// One pixels margin, due to subpixel precision one LED can end on same that the next start

			switch( m_direction )
			{
				case Direction::Up:

					rect.x = 0;
					rect.y = sz.h - (dirtBeg+dirtLen);
					rect.w = sz.w;
					rect.h = dirtLen;
				break;

				case Direction::Down:
					rect.x = 0;
					rect.y = dirtBeg;
					rect.w = sz.w;
					rect.h = dirtLen;
				break;

				case Direction::Left:
					rect.x = sz.w - (dirtBeg+dirtLen);
					rect.y = 0;
					rect.w = dirtLen;
					rect.h = sz.h;
				break;

				case Direction::Right:
					rect.x = dirtBeg;
					rect.y = 0;
					rect.w = dirtLen;
					rect.h = sz.h;
				break;
			}

			_requestRender(rect);
		}
	}


	//____ _render() _____________________________________________________________________

	void VolumeMeter::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice, _canvas, _window);

		RectI canvas = OO(skin)._contentRect(_canvas, m_state).px();

		int p = m_iSidePadding.qpix >> 2;

		float ledSize = ((m_direction == Direction::Up || m_direction == Direction::Down)?canvas.h:canvas.w) / (float)(m_nLEDs + (m_nLEDs-1)*m_LEDSpacing);
		float stepSize = ledSize * (1.f+m_LEDSpacing);

		RectF ledRect;
		float	stepX;
		float	stepY;

		switch( m_direction )
		{
			case Direction::Up:
				ledRect = RectF( (float) canvas.x + p, (float) canvas.y + canvas.h - ledSize, (float) canvas.w - 2*p, ledSize );
				stepX = 0.f;
				stepY = -stepSize;
				break;
			case Direction::Down:
				ledRect = RectF( (float) canvas.x+p, (float) canvas.y, (float) canvas.w - 2*p, ledSize );
				stepX = 0.f;
				stepY = stepSize;
				break;
			case Direction::Left:
				ledRect = RectF((float) canvas.x + canvas.w - ledSize, (float) canvas.y + p, ledSize, (float) canvas.h - 2*p);
				stepX = -stepSize;
				stepY = 0.f;
				break;
			case Direction::Right:
				ledRect = RectF((float) canvas.x, (float) canvas.y + p, ledSize, (float) canvas.h - 2*p);
				stepX = stepSize;
				stepY = 0.f;
				break;
		}

		for( int i = 0 ; i < m_nLEDs ; i++ )
		{
			if( ledRect.w > 0.f && ledRect.h > 0.f )
			{
				int section;

				if( i < m_nSectionLEDs[0] )
					section = 0;
				else if( i < m_nSectionLEDs[0] + m_nSectionLEDs[1] )
					section = 1;
				else
					section = 2;

				HiColor color = m_LEDColors[section][0]*(1.0f-m_LEDStates[i]) + m_LEDColors[section][1]*m_LEDStates[i];

				pDevice->fill( ledRect/4.f, color);
			}

			ledRect.x += stepX;
			ledRect.y += stepY;
		}

	}

	//____ _cloneContent() _________________________________________________________________

	void VolumeMeter::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const VolumeMeter * pOrg = static_cast<const VolumeMeter*>(_pOrg);

		for( int i = 0 ; i < 3 ; i++ )
		{
			m_LEDColors[i][0] 	= pOrg->m_LEDColors[i][0];
			m_LEDColors[i][1] 	= pOrg->m_LEDColors[i][1];
			m_nSectionLEDs[i] = pOrg->m_nSectionLEDs[i];
		}

		m_direction = pOrg->m_direction;
		m_nLEDs = pOrg->m_nLEDs;
		m_LEDSpacing = pOrg->m_LEDSpacing;
		m_iPeak = pOrg->m_iPeak;
		m_iHold = pOrg->m_iHold;
	}

	//____ _alphaTest() ____________________________________________________________________

	bool VolumeMeter::_alphaTest( const Coord& ofs )
	{
		return Widget::_alphaTest(ofs);
	}
} // namespace wg
