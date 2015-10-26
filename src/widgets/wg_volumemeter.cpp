
#include <wg_volumemeter.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char VolumeMeter::CLASSNAME[] = {"VolumeMeter"};
	
	
	//____ Constructor ____________________________________________________________
	
	VolumeMeter::VolumeMeter()
	{
		m_LEDColors[0][0] = Color::black;
		m_LEDColors[1][0] = Color::black;
		m_LEDColors[2][0] = Color::black;
		m_LEDColors[0][1] = Color::green;
		m_LEDColors[1][1] = Color::yellow;
		m_LEDColors[2][1] = Color::red;
		
		m_nSectionLEDs[0] = 8;
		m_nSectionLEDs[1] = 2;
		m_nSectionLEDs[2] = 1;
		
		m_nLEDs = m_nSectionLEDs[0] + m_nSectionLEDs[1] + m_nSectionLEDs[2];
		m_LEDSpacing = 0.33f;
		m_direction = Direction::Up;
		
		m_peak = 0.f;
		m_hold = 0.f;	
	}
	
	//____ Destructor _____________________________________________________________
	
	VolumeMeter::~VolumeMeter()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool VolumeMeter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * VolumeMeter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	VolumeMeter_p VolumeMeter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return VolumeMeter_p( static_cast<VolumeMeter*>(pObject.rawPtr()) );
	
		return 0;
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
	
	void VolumeMeter::setLEDColors(	Color bottomOn, Color middleOn, Color topOn, 
										Color bottomOff, Color middleOff, Color topOff )
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
	
	void VolumeMeter::setValue( float peak, float hold )
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
	
	Size VolumeMeter::preferredSize() const
	{
		Size	content;
	
		if( m_direction == Direction::Up || m_direction == Direction::Down )
			content = Size(10,5*m_nLEDs);
		else
			content = Size(5*m_nLEDs,10);
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(content);
		else
			return content;
	}
	
	
	//____ _onRender() _____________________________________________________________________
	
	void VolumeMeter::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
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
	
			
		float ledSize = ((m_direction == Direction::Up || m_direction == Direction::Down)?canvas.h:canvas.w) / (float)(m_nLEDs + (m_nLEDs-1)*m_LEDSpacing);
		float stepSize = ledSize * (1.f+m_LEDSpacing);
	
		RectF ledRect;
		float	stepX;
		float	stepY;
		
		switch( m_direction )
		{
			case Direction::Up:
				ledRect = RectF( (float) canvas.x, (float) (canvas.y + canvas.h - ledSize), (float) canvas.w, (float) ledSize );
				stepX = 0.f;
				stepY = -stepSize;
				break;
			case Direction::Down:
				ledRect = RectF( (float) canvas.x, (float) canvas.y, (float) canvas.w, (float) ledSize );
				stepX = 0.f;
				stepY = stepSize;
				break;
			case Direction::Left:
				ledRect = RectF( (float) (canvas.x + canvas.w - ledSize), (float) canvas.y, (float) ledSize, (float) canvas.h );
				stepX = -stepSize;
				stepY = 0.f;
				break;
			case Direction::Right:
				ledRect = RectF( (float) canvas.x, (float) canvas.y, (float) ledSize, (float) canvas.h );
				stepX = stepSize;
				stepY = 0.f;
				break;
		}
		
		RectF	clip( _clip );
			
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
			
			pDevice->fillSubPixel( RectF(ledRect, clip), m_LEDColors[section][onoff] );
			
			ledRect.x += stepX;
			ledRect.y += stepY;
		}
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void  VolumeMeter::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		if( oldState.isEnabled() != m_state.isEnabled() )
			_requestRender();
	}
	
	//____ _onCloneContent() _________________________________________________________________ 
	
	void VolumeMeter::_onCloneContent( const Widget * _pOrg )
	{
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
		m_peak = pOrg->m_peak;
		m_hold = pOrg->m_hold;	
	}

} // namespace wg
