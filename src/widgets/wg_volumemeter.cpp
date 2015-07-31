
#include <wg_volumemeter.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char WgVolumeMeter::CLASSNAME[] = {"VolumeMeter"};
	
	
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
		m_LEDSpacing = 0.33f;
		m_direction = WG_UP;
		
		m_peak = 0.f;
		m_hold = 0.f;	
	}
	
	//____ Destructor _____________________________________________________________
	
	WgVolumeMeter::~WgVolumeMeter()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgVolumeMeter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgVolumeMeter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgVolumeMeter_p WgVolumeMeter::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgVolumeMeter_p( static_cast<WgVolumeMeter*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setDirection() ________________________________________________________
	
	void WgVolumeMeter::setDirection( WgDirection direction )
	{
		if( direction != m_direction )
		{
			m_direction = direction;
			_requestResize();
			_requestRender();
		}
	}
	
	
	//____ setLEDColors() ___________________________________________________________
	
	void WgVolumeMeter::setLEDColors(	WgColor bottomOn, WgColor middleOn, WgColor topOn, 
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
	
	//____ setNbLEDs() _________________________________________________________
	
	void WgVolumeMeter::setNbLEDs( int bottomSection, int middleSection, int topSection )
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
	}
	
	//____ setLEDSpacing() ___________________________________________________________
	
	void WgVolumeMeter::setLEDSpacing( float spacing )
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
	
	void WgVolumeMeter::setValue( float peak, float hold )
	{
		LIMIT( peak, 0.f, 1.f );
		LIMIT( hold, 0.f, 1.f );
		
		if( m_peak != peak || m_hold != hold )
		{
			m_peak = peak;
			m_hold = hold;
			_requestRender();
		}
	}
	
	
	//____ preferredSize() ________________________________________________________________
	
	WgSize WgVolumeMeter::preferredSize() const
	{
		WgSize	content;
	
		if( m_direction == WG_UP || m_direction == WG_DOWN )
			content = WgSize(10,5*m_nLEDs);
		else
			content = WgSize(5*m_nLEDs,10);
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(content);
		else
			return content;
	}
	
	
	//____ _onRender() _____________________________________________________________________
	
	void WgVolumeMeter::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		WgWidget::_onRender(pDevice,_canvas,_window,_clip);
	
		WgRect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;
	
	
		int	peak = 0;
		int hold = 0;
		
		if( m_state.isEnabled() )
		{
			peak = (int) (m_nLEDs * m_peak);
			hold = (int) (m_nLEDs * m_hold);
		}
	
			
		float ledSize = ((m_direction == WG_UP || m_direction == WG_DOWN)?canvas.h:canvas.w) / (float)(m_nLEDs + (m_nLEDs-1)*m_LEDSpacing);
		float stepSize = ledSize * (1.f+m_LEDSpacing);
	
		WgRectF ledRect;
		float	stepX;
		float	stepY;
		
		switch( m_direction )
		{
			case WG_UP:
				ledRect = WgRectF( (float) canvas.x, (float) (canvas.y + canvas.h - ledSize), (float) canvas.w, (float) ledSize );
				stepX = 0.f;
				stepY = -stepSize;
				break;
			case WG_DOWN:
				ledRect = WgRectF( (float) canvas.x, (float) canvas.y, (float) canvas.w, (float) ledSize );
				stepX = 0.f;
				stepY = stepSize;
				break;
			case WG_LEFT:
				ledRect = WgRectF( (float) (canvas.x + canvas.w - ledSize), (float) canvas.y, (float) ledSize, (float) canvas.h );
				stepX = -stepSize;
				stepY = 0.f;
				break;
			case WG_RIGHT:
				ledRect = WgRectF( (float) canvas.x, (float) canvas.y, (float) ledSize, (float) canvas.h );
				stepX = stepSize;
				stepY = 0.f;
				break;
		}
		
		WgRectF	clip( _clip );
			
		for( int i = 0 ; i < m_nLEDs ; i++ )
		{
			int section = 0;
			int onoff = 0;
	
			if( i < m_nSectionLEDs[0] )
				section = 0;
			else if( i < m_nSectionLEDs[0] + m_nSectionLEDs[1] )
				section = 1;
			else
				section = 2;
			
			if( i < peak || i+1 == hold )
				onoff = 1;
			
			pDevice->fillSubPixel( WgRectF(ledRect, clip), m_LEDColors[section][onoff] );
			
			ledRect.x += stepX;
			ledRect.y += stepY;
		}
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void  WgVolumeMeter::_onStateChanged( WgState oldState )
	{
		WgWidget::_onStateChanged(oldState);
	
		if( oldState.isEnabled() != m_state.isEnabled() )
			_requestRender();
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
		m_peak = pOrg->m_peak;
		m_hold = pOrg->m_hold;	
	}

} // namespace wg
