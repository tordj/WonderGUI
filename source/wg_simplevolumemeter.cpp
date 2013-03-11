
#include <wg_simplevolumemeter.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"SimpleVolumeMeter"};


//____ Constructor ____________________________________________________________

WgSimpleVolumeMeter::WgSimpleVolumeMeter()
{
	m_sectionColors[0] = WgColor::green;
	m_sectionColors[1] = WgColor::yellow;
	m_sectionColors[2] = WgColor::red;
	
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

WgSimpleVolumeMeter::~WgSimpleVolumeMeter()
{
}

//____ Type() _________________________________________________________________

const char * WgSimpleVolumeMeter::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgSimpleVolumeMeter::GetClass()
{
	return c_widgetType;
}

//____ SetColors() ___________________________________________________________

void WgSimpleVolumeMeter::SetColors( WgColor bottom, WgColor middle, WgColor top )
{
	if( bottom != m_sectionColors[0] || middle != m_sectionColors[1] || top != m_sectionColors[2] )
	{
		m_sectionColors[0] = bottom;
		m_sectionColors[1] = middle;
		m_sectionColors[2] = top;
		_requestRender();
	}
}

//____ SetSections() _________________________________________________________

void WgSimpleVolumeMeter::SetSections( float bottomFraction, float topFraction )
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

//____ SetHoldHeight() ________________________________________________________

void WgSimpleVolumeMeter::SetHoldHeight( float fraction )
{
	WG_LIMIT( fraction, 0.f, 0.25f );
	
	if( m_holdHeight != fraction )
	{
		m_holdHeight = fraction;
		_requestRender();
	}
}

//____ SetValue() ______________________________________________________________

void WgSimpleVolumeMeter::SetValue( float peak, float hold )
{
	WG_LIMIT( peak, 0.f, 1.f );
	WG_LIMIT( hold, 0.f, 1.f );

	if( m_bStereo || m_peak[0] != peak || m_hold[0] != hold )
	{
		m_bStereo = false;
		m_peak[0] = peak;
		m_hold[0] = hold;
		_requestRender();
	}
}

void WgSimpleVolumeMeter::SetValue( float leftPeak, float leftHold, float rightPeak, float rightHold )
{
	WG_LIMIT( leftPeak, 0.f, 1.f );
	WG_LIMIT( leftHold, 0.f, 1.f );
	WG_LIMIT( rightPeak, 0.f, 1.f );
	WG_LIMIT( rightHold, 0.f, 1.f );
	
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

//____ PreferredSize() ________________________________________________________________

WgSize WgSimpleVolumeMeter::PreferredSize() const
{
	return WgSize(9,20);
}

//____ _onNewSize() ____________________________________________________________________

void WgSimpleVolumeMeter::_onNewSize( const WgSize& size )
{
	_updateSectionPixelHeight();
	_requestRender();
}



//____ _onRender() _____________________________________________________________________

void WgSimpleVolumeMeter::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( !m_bEnabled )
		return;
	
	if( m_bStereo )
	{
		WgRect r = _canvas;
		r.w = (r.w - 1) / 2;
		_renderHold( pDevice, 0, r, _clip );
		_renderPeak( pDevice, 0, r, _clip );

		r.x += r.w + 1;
		_renderHold( pDevice, 1, r, _clip );
		_renderPeak( pDevice, 1, r, _clip );
		
	}
	else 
	{
		_renderHold( pDevice, 0, _canvas, _clip );
		_renderPeak( pDevice, 0, _canvas, _clip );			
	}
}

//____ _renderPeak() ___________________________________________________________________

void WgSimpleVolumeMeter::_renderPeak( WgGfxDevice * pDevice, int nb, const WgRect& _rect, const WgRect& _clip )
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
		
		WgRect r( _rect.x, _rect.y + _rect.h - ofs - sectionHeight, _rect.w, sectionHeight );
		pDevice->Fill( WgRect( r, _clip ), m_sectionColors[i] );
		
		ofs += sectionHeight;
		height -= sectionHeight;
	}
}

//____ _renderHold() ___________________________________________________________________

void WgSimpleVolumeMeter::_renderHold( WgGfxDevice * pDevice, int nb, const WgRect& _rect, const WgRect& _clip )
{
	if( m_holdHeight == 0.f )
		return;						// Hold should not be displayed.
	
	int height = (int) (m_holdHeight * _rect.h);
	if( height < 1 )
		height = 1;

	WgColor c;
	
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

	WgRect r( _rect.x, _rect.y + ofs, _rect.w, height );
	pDevice->Fill( WgRect( r, _clip ), c );
}

//____ _updateSectionPixelHeight() ______________________________________________________

void WgSimpleVolumeMeter::_updateSectionPixelHeight()
{
	int totalHeight = Geo().h;

	m_sectionPixelHeight[0] = (int) (m_sectionHeight[0] * totalHeight + 0.5f);
	m_sectionPixelHeight[1] =  ((int)((m_sectionHeight[0] + m_sectionHeight[1]) * totalHeight + 0.5f)) - m_sectionPixelHeight[0];
	m_sectionPixelHeight[2] = totalHeight - m_sectionPixelHeight[1] - m_sectionPixelHeight[0];
}

//____ _onCloneContent() _________________________________________________________________ 

void WgSimpleVolumeMeter::_onCloneContent( const WgWidget * _pOrg )
{
	const WgSimpleVolumeMeter * pOrg = static_cast<const WgSimpleVolumeMeter*>(_pOrg);

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

//____ _onAlphaTest() ____________________________________________________________________

bool WgSimpleVolumeMeter::_onAlphaTest( const WgCoord& ofs )
{
	return false;
}
