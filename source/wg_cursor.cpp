#include <wg_cursor.h>


//____ WgCursor::WgCursor() ___________________________________________________

WgCursor::WgCursor()
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]	= 0;
		m_ofsX[i]	= 0;
		m_ofsY[i]	= 0;
		m_spacing[i] = 0;
	}
}

WgCursor::WgCursor(	WgCursor& in )
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]	= in.m_pAnim[i];
		m_ofsX[i]	= in.m_ofsX[i];
		m_ofsY[i]	= in.m_ofsY[i];
		m_spacing[i] = in.m_spacing[i];
	}
}


//____ WgCursor::setMode() ____________________________________________________

bool WgCursor::setMode( Mode _m, WgGfxAnim * _pAnim, Sint8 _ofsX, Sint8 _ofsY, Uint8 _spacing )
{
	if( _m < 0 || _m >= (Mode) N_MODES )
		return false;

	m_pAnim[_m]		= _pAnim;
	m_ofsX[_m]		= _ofsX;
	m_ofsY[_m]		= _ofsY;
	m_spacing[_m]	= _spacing;
	
	return true;
}

//____ WgCursor::setOfs() _____________________________________________________

void WgCursor::setOfs( Mode _m, Sint8 _ofsX, Sint8 _ofsY )
{
	if( _m < 0 || _m >= (Mode) N_MODES )
		return;

	m_ofsX[_m] = _ofsX;
	m_ofsY[_m] = _ofsY;
}


//____ WgCursor::setSpacing() __________________________________________________

void WgCursor::setSpacing( Mode _m, Uint8 _spacing )
{
	if( _m < 0 || _m >= (Mode) N_MODES )
		return;

	m_spacing[_m] = _spacing;
}


//____ WgCursor::setAnim() ____________________________________________________

void WgCursor::setAnim( Mode _m, WgGfxAnim * _pAnim )
{
	if( _m < 0 || _m >= (Mode) N_MODES )
		return;

	m_pAnim[_m] = _pAnim;
}

