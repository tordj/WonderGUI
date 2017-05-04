
#include <wg_volumemeter.h>
#include <wg_gfxdevice.h>
#include <wg_eventhandler.h>
#include <math.h>
static const char	c_widgetType[] = {"VolumeMeter"};

#ifdef WIN32
#include <wg_userdefines.h>
#endif

//____ Constructor ____________________________________________________________

WgVolumeMeter::WgVolumeMeter()
{
	m_LEDColors[0][0] = WgColor::black;
	m_LEDColors[1][0] = WgColor::black;
	m_LEDColors[2][0] = WgColor::black;
	m_LEDColors[0][1] = WgColor::green;
	m_LEDColors[1][1] = WgColor::yellow;
	m_LEDColors[2][1] = WgColor::red;
	
	m_nSectionLEDs[0] = 8;
	m_nSectionLEDs[1] = 2;
	m_nSectionLEDs[2] = 1;
	
	m_nLEDs = m_nSectionLEDs[0] + m_nSectionLEDs[1] + m_nSectionLEDs[2];
	m_LEDSpacing = 0.33;
	m_direction = WG_UP;
	
	m_iPeak = 0;
	m_iHold = 0; 
    m_fPeak = 0.0f;
    
    m_fSidePadding = 0.05f;
    m_iSidePadding = 1;
    
    m_bZeroInMiddle = false;
    d = 1.0f/(float)(m_nLEDs-1);
    d2 = 0.5f/(float)(m_nLEDs);
   
    m_LEDStates.resize(m_nLEDs);
    for(int i=0;i<m_nLEDs;i++) 
        m_LEDStates[i] = 0.0f;
	
    m_bUseFades = false;
	
	_startReceiveTicks();
}

//____ Destructor _____________________________________________________________

WgVolumeMeter::~WgVolumeMeter()
{
}

//____ Type() _________________________________________________________________

const char * WgVolumeMeter::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgVolumeMeter::GetClass()
{
	return c_widgetType;
}

//____ SetDirection() ________________________________________________________

void WgVolumeMeter::SetDirection( WgDirection direction )
{
	if( direction != m_direction )
	{
		m_direction = direction;
		_requestResize();
		_requestRender();
	}
}


//____ SetLEDColors() ___________________________________________________________

void WgVolumeMeter::SetLEDColors(	WgColor bottomOn, WgColor middleOn, WgColor topOn, 
									WgColor bottomOff, WgColor middleOff, WgColor topOff )
{
	m_LEDColors[0][0] = bottomOff;
	m_LEDColors[0][1] = bottomOn;

	m_LEDColors[1][0] = middleOff;
	m_LEDColors[1][1] = middleOn;

	m_LEDColors[2][0] = topOff;
	m_LEDColors[2][1] = topOn;
	
	_requestRender();
}

//____ SetNbLEDs() _________________________________________________________

void WgVolumeMeter::SetNbLEDs( int bottomSection, int middleSection, int topSection )
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

    
    d = 1.0f/(float)(m_nLEDs-1);
    d2 = 0.5f/(float)(m_nLEDs);
    
    m_LEDStates.resize(m_nLEDs);
    for(int i=0;i<m_nLEDs;i++) 
        m_LEDStates[i] = 0.0f;
}

//____ SetLEDSpacing() ___________________________________________________________

void WgVolumeMeter::SetLEDSpacing( float spacing )
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


//____ SetValue() ______________________________________________________________

void WgVolumeMeter::SetValue( float fPeak, float fHold )
{
	WG_LIMIT( fPeak, 0.f, 1.f );
	WG_LIMIT( fHold, 0.f, 1.f );

    int iPeak = (int)wg_round(fPeak * m_nLEDs);
    int iHold = (int)wg_round(fHold * m_nLEDs);
    
    if(iPeak != m_iPeak || iHold != m_iHold)
	{
		m_iPeak = iPeak;
		m_iHold = iHold;
 	}
    
    m_fPeak = fPeak;

}


//____ PreferredSize() ________________________________________________________________

WgSize WgVolumeMeter::PreferredSize() const
{
	if( m_direction == WG_UP || m_direction == WG_DOWN )
		return WgSize(10,5*m_nLEDs);
	else
		return WgSize(5*m_nLEDs,10);
}

//____ _onNewSize() ____________________________________________________________________

void WgVolumeMeter::_onNewSize( const WgSize& size )
{
    switch (m_direction)
    {
        case WG_LEFT:
        case WG_RIGHT:
            m_iSidePadding = (int) ((float)size.h * m_fSidePadding);
            break;
        case WG_UP:
        case WG_DOWN:
        default:
            m_iSidePadding = (int) ((float)size.w * m_fSidePadding);
            break;
    }
    
    if(m_iSidePadding <= 0)
        m_iSidePadding = 0;
    
	_requestRender();
}

//____ _onEvent() ______________________________________________________________

void WgVolumeMeter::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
//			const WgEvent::Tick * pTick = static_cast<const WgEvent::Tick*>(pEvent);
						
			int firstRenderLED = -1;
			int lastRenderLED = -1;
						
			for( int i = 0 ; i < m_nLEDs ; i++ )
			{
				bool 	on = false;
				
				// NB: Hold is not implemented for Zero In Middle
				if(m_bZeroInMiddle)
				{
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
				WgSize sz = Size();
				WgRect rect;

				int meterLen = (m_direction == WG_UP || m_direction == WG_DOWN) ? sz.h : sz.w; 

				int dirtBeg = firstRenderLED*meterLen/m_nLEDs;
				int dirtLen = (lastRenderLED+1)*meterLen/m_nLEDs + 1 - dirtBeg;	// One pixels margin, due to subpixel precision one LED can end on same that the next start
								
				switch( m_direction )
				{
					case WG_UP:

						rect.x = 0;
						rect.y = sz.h - (dirtBeg+dirtLen);
						rect.w = sz.w;
						rect.h = dirtLen;
					break;
					
					case WG_DOWN:
						rect.x = 0;
						rect.y = dirtBeg;
						rect.w = sz.w;
						rect.h = dirtLen;
					break;
					
					case WG_LEFT:
						rect.x = sz.w - (dirtBeg+dirtLen);
						rect.y = 0;
						rect.w = dirtLen;
						rect.h = sz.h;
					break;
					
					case WG_RIGHT:
						rect.x = dirtBeg;
						rect.y = 0;
						rect.w = dirtLen;
						rect.h = sz.h;
					break;
				}
				
				_requestRender(rect);				
			}
		}
		break;

		default:
			pHandler->ForwardEvent( pEvent );
		break;
	}
}


//____ _onRender() _____________________________________________________________________

void WgVolumeMeter::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	int p = m_iSidePadding;
	
	float ledSize = ((m_direction == WG_UP || m_direction == WG_DOWN)?_canvas.h:_canvas.w) / (float)(m_nLEDs + (m_nLEDs-1)*m_LEDSpacing);
	float stepSize = ledSize * (1.f+m_LEDSpacing);

	WgRectF ledRect;
	float	stepX;
	float	stepY;
	
	switch( m_direction )
	{
		case WG_UP:
			ledRect = WgRectF( _canvas.x + p, _canvas.y + _canvas.h - ledSize, _canvas.w - 2*p, ledSize );
			stepX = 0.f;
			stepY = -stepSize;
			break;
		case WG_DOWN:
			ledRect = WgRectF( _canvas.x+p, _canvas.y, _canvas.w - 2*p, ledSize );
			stepX = 0.f;
			stepY = stepSize;
			break;
		case WG_LEFT:
			ledRect = WgRectF( _canvas.x + _canvas.w - ledSize, _canvas.y + p, ledSize, _canvas.h - 2*p);
			stepX = -stepSize;
			stepY = 0.f;
			break;
		case WG_RIGHT:
			ledRect = WgRectF( _canvas.x, _canvas.y + p, ledSize, _canvas.h - 2*p);
			stepX = stepSize;
			stepY = 0.f;
			break;
	}
	
	WgRectF	clip( _clip );
    
	for( int i = 0 ; i < m_nLEDs ; i++ )
	{
   
		WgRectF rect(ledRect, clip);
		
		if( rect.w > 0.f && rect.h > 0.f )
		{
			int section;

			if( i < m_nSectionLEDs[0] )
				section = 0;
			else if( i < m_nSectionLEDs[0] + m_nSectionLEDs[1] )
				section = 1;
			else
				section = 2;
					
			WgColor color = m_LEDColors[section][0]*(1.0f-m_LEDStates[i]) + m_LEDColors[section][1]*m_LEDStates[i];

			pDevice->FillSubPixel( rect, color);
        }

		ledRect.x += stepX;
		ledRect.y += stepY;
	}
    
}

//____ _onCloneContent() _________________________________________________________________

void WgVolumeMeter::_onCloneContent( const WgWidget * _pOrg )
{
	const WgVolumeMeter * pOrg = static_cast<const WgVolumeMeter*>(_pOrg);
	
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

//____ _onAlphaTest() ____________________________________________________________________

bool WgVolumeMeter::_onAlphaTest( const WgCoord& ofs )
{
	return false;
}