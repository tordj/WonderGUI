
#include <wg_simplevolumemeter.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char SimpleVolumeMeter::CLASSNAME[] = {"SimpleVolumeMeter"};
	
	
	//____ Constructor ____________________________________________________________
	
	SimpleVolumeMeter::SimpleVolumeMeter()
	{
		m_sectionColors[0] = Color::Green;
		m_sectionColors[1] = Color::Yellow;
		m_sectionColors[2] = Color::Red;
		
		m_sectionHeight[0] = 0.75f;
		m_sectionHeight[1] = 0.18f;
		m_sectionHeight[2] = 0.07f;
		
		m_holdHeight = 0.10f;
		m_bStereo = false;
		m_peak[0] = 0.f;
		m_peak[1] = 0.f;
		m_hold[0] = 0.f;
		m_hold[1] = 0.f;
		
	//	_updateSectionPixelHeight();
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
	
	SimpleVolumeMeter_p SimpleVolumeMeter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SimpleVolumeMeter_p( static_cast<SimpleVolumeMeter*>(pObject.rawPtr()) );
	
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
		if( bottomFraction < 0.f )
			bottomFraction = 0.f;
		if( bottomFraction > 1.f )
			bottomFraction = 1.f;
			
		if( topFraction < 0.f )
			topFraction = 0.f;
	
		float middleFraction = 1.f - bottomFraction - topFraction;
		if( middleFraction < 0.f )
			middleFraction = 0.f;
		
		topFraction = 1.f - bottomFraction - middleFraction;
		
		if( bottomFraction != m_sectionHeight[0] || topFraction != m_sectionHeight[2] )
		{
			m_sectionHeight[0] = bottomFraction;
			m_sectionHeight[1] = middleFraction;
			m_sectionHeight[2] = topFraction;
			_updateSectionPixelHeight();
			_requestRender();
		}
	}
	
	//____ setHoldHeight() ________________________________________________________
	
	void SimpleVolumeMeter::setHoldHeight( float fraction )
	{
		limit( fraction, 0.f, 0.25f );
		
		if( m_holdHeight != fraction )
		{
			m_holdHeight = fraction;
			_requestRender();
		}
	}
	
	//____ setValue() ______________________________________________________________
	
	void SimpleVolumeMeter::setValue( float peak, float hold )
	{
		limit( peak, 0.f, 1.f );
		limit( hold, 0.f, 1.f );
	
		if( m_bStereo || m_peak[0] != peak || m_hold[0] != hold )
		{
			m_bStereo = false;
			m_peak[0] = peak;
			m_hold[0] = hold;
			_requestRender();
		}
	}
	
	void SimpleVolumeMeter::setValue( float leftPeak, float leftHold, float rightPeak, float rightHold )
	{
		limit( leftPeak, 0.f, 1.f );
		limit( leftHold, 0.f, 1.f );
		limit( rightPeak, 0.f, 1.f );
		limit( rightHold, 0.f, 1.f );
		
		if( !m_bStereo || m_peak[0] != leftPeak || m_hold[0] != leftHold || m_peak[1] != rightPeak || m_hold[1] != rightHold )
		{
			m_bStereo = true;
			m_peak[0] = leftPeak;
			m_peak[1] = rightPeak;
			m_hold[0] = leftHold;
			m_hold[1] = rightHold;
			_requestRender();
		}
	}
	
	//____ preferredSize() ________________________________________________________________
	
	Size SimpleVolumeMeter::preferredSize() const
	{
		Size content(9,20);
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(content);
		else
			return content;
	}
	
	//____ _onNewSize() ____________________________________________________________________
	
	void SimpleVolumeMeter::_onNewSize( const Size& size )
	{
		_updateSectionPixelHeight();
		_requestRender();
	}
	
	
	
	//____ _onRender() _____________________________________________________________________
	
	void SimpleVolumeMeter::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		if( !m_state.isEnabled() )
			return;
		
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->sizeForContent(_canvas);
		else
			canvas = _canvas;
	
		if( m_bStereo )
		{
			Rect r = canvas;
			r.w = (r.w - 1) / 2;
			_renderHold( pDevice, 0, r, _clip );
			_renderPeak( pDevice, 0, r, _clip );
	
			r.x += r.w + 1;
			_renderHold( pDevice, 1, r, _clip );
			_renderPeak( pDevice, 1, r, _clip );
			
		}
		else 
		{
			_renderHold( pDevice, 0, canvas, _clip );
			_renderPeak( pDevice, 0, canvas, _clip );			
		}
	}
	
	//____ _renderPeak() ___________________________________________________________________
	
	void SimpleVolumeMeter::_renderPeak( GfxDevice * pDevice, int nb, const Rect& _rect, const Rect& _clip )
	{
		int height = (int) (m_peak[nb] * _rect.h);
		int ofs = 0;
		
		for( int i = 0 ; i < 3 ; i++ )
		{
			if( height <= 0 )
				break;
			
			int sectionHeight = m_sectionPixelHeight[i];
			if( sectionHeight > height )
				sectionHeight = height;
			
			Rect r( _rect.x, _rect.y + _rect.h - ofs - sectionHeight, _rect.w, sectionHeight );
			pDevice->fill( Rect( r, _clip ), m_sectionColors[i] );
			
			ofs += sectionHeight;
			height -= sectionHeight;
		}
	}
	
	//____ _renderHold() ___________________________________________________________________
	
	void SimpleVolumeMeter::_renderHold( GfxDevice * pDevice, int nb, const Rect& _rect, const Rect& _clip )
	{
		if( m_holdHeight == 0.f )
			return;						// Hold should not be displayed.
		
		int height = (int) (m_holdHeight * _rect.h);
		if( height < 1 )
			height = 1;
	
		Color c;
		
		int ofs = (int) ((1.f - m_hold[nb]) * _rect.h);
	
		if( ofs < m_sectionPixelHeight[2] )
		{
			c = m_sectionColors[2];
			if( ofs + height > m_sectionPixelHeight[2] )
				ofs = m_sectionPixelHeight[2] - height;
			
		}
		else if( ofs < m_sectionPixelHeight[2] + m_sectionPixelHeight[1] )
		{
			c = m_sectionColors[1];
			if( ofs + height > m_sectionPixelHeight[2] + m_sectionPixelHeight[1] )
				ofs = m_sectionPixelHeight[2] + m_sectionPixelHeight[1] - height;
		}
		else 
		{
			c = m_sectionColors[0];
		}
	
		Rect r( _rect.x, _rect.y + ofs, _rect.w, height );
		pDevice->fill( Rect( r, _clip ), c );
	}
	
	//____ _updateSectionPixelHeight() ______________________________________________________
	
	void SimpleVolumeMeter::_updateSectionPixelHeight()
	{
		int totalHeight = geo().h;
		if( m_pSkin )
			totalHeight -= m_pSkin->contentPadding().h;
	
		m_sectionPixelHeight[0] = (int) (m_sectionHeight[0] * totalHeight + 0.5f);
		m_sectionPixelHeight[1] =  ((int)((m_sectionHeight[0] + m_sectionHeight[1]) * totalHeight + 0.5f)) - m_sectionPixelHeight[0];
		m_sectionPixelHeight[2] = totalHeight - m_sectionPixelHeight[1] - m_sectionPixelHeight[0];
	}
	
	//____ _onCloneContent() _________________________________________________________________ 
	
	void SimpleVolumeMeter::_onCloneContent( const Widget * _pOrg )
	{
		const SimpleVolumeMeter * pOrg = static_cast<const SimpleVolumeMeter*>(_pOrg);
	
		for( int i = 0 ; i < 3 ; i++ )
		{
			m_sectionColors[i] 	= pOrg->m_sectionColors[i];
			m_sectionHeight[i] = pOrg->m_sectionHeight[i];
		}
		
		m_holdHeight = pOrg->m_holdHeight;
		m_bStereo = pOrg->m_bStereo;
		m_peak[0] = pOrg->m_peak[0];
		m_peak[1] = pOrg->m_peak[1];
		m_hold[0] = pOrg->m_hold[0];
		m_hold[1] = pOrg->m_hold[1];
		
		_updateSectionPixelHeight();
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void  SimpleVolumeMeter::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		if( oldState.isEnabled() != m_state.isEnabled() )
			_requestRender();
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void  SimpleVolumeMeter::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
		_updateSectionPixelHeight();
	}
	

} // namespace wg
