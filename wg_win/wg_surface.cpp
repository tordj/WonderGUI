#include "wg_surface.h"
#include "wg_rect.h"


//____ WgSurface() ____________________________________________________________

WgSurface::WgSurface( Uint32 iWidth, Uint32 iHeight, Uint32 iBitdepth )
{
	m_alpha = 0;
 	m_colorKey = 0;
	m_blitMode = BM_ALPHA;
	m_changeCnt = 0;

	m_width = iWidth;
	m_height = iHeight;
	m_bitdepth = iBitdepth;
	m_pData = new Uint8[ m_width*m_height*m_bitdepth/8 ];

}

//____ ~WgSurface() ___________________________________________________________

WgSurface::~WgSurface()
{
//	if( m_pLockedSurf )
//		unlock(false);
	delete m_pData;
}

//____ pixel() ________________________________________________________________

Uint32	WgSurface::pixel( Uint32 x, Uint32 y ) const
{	
	if( m_bitdepth == 32 )
	{
		return * ((Uint32*) &m_pData[ 4*(m_width*y+x) ]);
	}
	else 
	{
		Uint32 k = * ((Uint32*) &m_pData[ 3*(m_width*y+x) ]);
		k |= 0xff000000; // ???
		return k;
	}

}

//____ opacity() ______________________________________________________________

Uint8	WgSurface::opacity( Uint32 x, Uint32 y ) const
{
	if( x >= m_width )
		return 0;
	if( y >= m_height )
		return 0;
	
	if( m_bitdepth == 32 )
		return m_pData[ 4*(m_width*y+x) + 3 ];
	else
		return 0xff;
}

//____ setAlphaValue() ________________________________________________________

void	WgSurface::setAlphaValue( Uint8 alpha )
{
	// TODO: implement
}

//____ setColorKey() __________________________________________________________

void	WgSurface::setColorKey( Uint32 pixel )
{
	// TODO: implement
}

//____ setBlitMode() __________________________________________________________

void WgSurface::setBlitMode( BlitMode mode )
{
	// TODO: implement
}


//____ displayFormat() ________________________________________________________

void WgSurface::displayFormat()
{
//	SDL_Surface * pOldSurf = m_pSurface;
//	m_pSurface = SDL_DisplayFormat( m_pSurface );
//	SDL_FreeSurface( pOldSurf );
//	m_changeCnt++;
}
