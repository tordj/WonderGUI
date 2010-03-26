#include <wg_cursor.h>


//____ WgCursor() _____________________________________________________________

WgCursor::WgCursor()
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]	= 0;
		m_advance[i] = 0;
	}

	m_scaleMode = FIXED_SIZE;
	m_sizeRatio	= 1.f;
}

WgCursor::WgCursor(	WgCursor& in )
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]		= in.m_pAnim[i];
		m_bearing[i]	= in.m_bearing[i];
		m_advance[i]	= in.m_advance[i];
	}

	m_scaleMode = in.m_scaleMode;
	m_sizeRatio	= in.m_sizeRatio;
}


//____ setStretchBorders() ____________________________________________________

void WgCursor::setStretchBorders( WgBorders borders )
{
	m_stretchBorders = borders;
}

//____ setScaleMode() _________________________________________________________

void WgCursor::setScaleMode( ScaleMode mode )
{
	m_scaleMode = mode;
}

//____ setSizeRatio() _________________________________________________________

void WgCursor::setSizeRatio( float ratio )
{
	m_sizeRatio = ratio;
}



//____ setMode() ______________________________________________________________

bool WgCursor::setMode( Mode m, WgGfxAnim * pAnim, WgCord bearing, int advance )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return false;

	m_pAnim[m]		= pAnim;
	m_bearing[m]	= bearing;
	m_advance[m]	= advance;
	
	return true;
}

//____ setBearing() ___________________________________________________________

void WgCursor::setBearing( Mode m, WgCord bearing )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_bearing[m] = bearing;
}


//____ setAdvance() ___________________________________________________________

void WgCursor::setAdvance( Mode m, int advance )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_advance[m] = advance;
}


//____ setAnim() ______________________________________________________________

void WgCursor::setAnim( Mode m, WgGfxAnim * pAnim )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_pAnim[m] = pAnim;
}

