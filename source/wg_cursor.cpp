#include <wg_cursor.h>


//____ WgCursor() _____________________________________________________________

WgCursor::WgCursor()
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]	= 0;
		m_advance[i] = 0;
		m_scaleMode[i] = FIXED_SIZE;
		m_sizeRatio[i]	= 1.f;
	}

}

WgCursor::WgCursor(	WgCursor& in )
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]		= in.m_pAnim[i];
		m_bearing[i]	= in.m_bearing[i];
		m_advance[i]	= in.m_advance[i];
		m_scaleMode[i]	= in.m_scaleMode[i];
		m_sizeRatio[i]	= in.m_sizeRatio[i];
	}
}


//____ setStretchBorders() ____________________________________________________

void WgCursor::setStretchBorders( Mode m, WgBorders borders )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_stretchBorders[m] = borders;
}

//____ setScaleMode() _________________________________________________________

void WgCursor::setScaleMode( Mode m, ScaleMode mode )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_scaleMode[m] = mode;
}

//____ setSizeRatio() _________________________________________________________

void WgCursor::setSizeRatio( Mode m, float ratio )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_sizeRatio[m] = ratio;
}



//____ setMode() ______________________________________________________________

bool WgCursor::setMode( Mode m, WgGfxAnim * pAnim, WgCord bearing, int advance, ScaleMode mode, float size_ratio, WgBorders borders )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return false;

	m_pAnim[m]			= pAnim;
	m_bearing[m]		= bearing;
	m_advance[m]		= advance;
	m_scaleMode[m]		= mode;
	m_sizeRatio[m]		= size_ratio;
	m_stretchBorders[m]	= borders;

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

