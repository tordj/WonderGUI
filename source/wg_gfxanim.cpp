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


#include <wg_gfxanim.h>
#include <wg_surface.h>


//____ WgGfxAnim() ____________________________________________________________

WgGfxAnim::WgGfxAnim()
{
	m_width = 0;
	m_height = 0;
}

//____ WgGfxAnim() ____________________________________________________________

WgGfxAnim::WgGfxAnim( Uint32 width, Uint32 height )
{
	m_width = width;
	m_height = height;
}

//____ insertKeyFrame() _______________________________________________________

bool WgGfxAnim::insertFrame( Uint32 pos, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration )
{
	if( pSurf->width() < xOfs + m_width || pSurf->height() < yOfs + m_height )
		return false;


	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->ofs.x = xOfs;
	pFrame->ofs.y = yOfs;

	bool bOk = WgAnim::insertKeyFrame( pos, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

bool WgGfxAnim::insertFrame( WgGfxFrame * pBefore, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration )
{
	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->ofs.x = xOfs;
	pFrame->ofs.y = yOfs;

	bool bOk = WgAnim::insertKeyFrame( pBefore, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

//____ addFrame() _____________________________________________________________

bool WgGfxAnim::addFrame( WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration )
{
	if( pSurf->width() < xOfs + m_width || pSurf->height() < yOfs + m_height )
		return false;

	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->ofs.x = xOfs;
	pFrame->ofs.y = yOfs;

	bool bOk = WgAnim::addKeyFrame( pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;	
}


//____ addHorrTiledFrames() ___________________________________________________

Uint32 WgGfxAnim::addHorrTiledFrames( Uint32 nFrames, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration, int spacing )
{
	if( nFrames == 0 )
		nFrames = (pSurf->width() - xOfs) / m_width;				// Put in as many as fits along surface width.

	if( pSurf->width() < xOfs + (m_width+spacing) * nFrames || pSurf->height() < yOfs + m_height )
		return 0;

	for( Uint32 i = 0 ; i < nFrames ; i++ )
	{
		WgGfxFrame * pFrame = new WgGfxFrame;

		pFrame->pSurf = pSurf;
		pFrame->ofs.x = xOfs + i*(m_width+spacing);
		pFrame->ofs.y = yOfs;

		bool bOk = WgAnim::addKeyFrame( pFrame, duration );
			
		if( !bOk )
		{
			delete pFrame;
			return i;
		}
	}

	return nFrames;
}


//____ addVertTiledFrames() ___________________________________________________

Uint32 WgGfxAnim::addVertTiledFrames( Uint32 nFrames, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration, int spacing )
{
	if( nFrames == 0 )
		nFrames = (pSurf->height() - yOfs) / m_height;			// Put in as many as fits along surface width.

	if( pSurf->width() < xOfs + m_width || pSurf->height() < yOfs + (m_height+spacing)*nFrames )
		return 0;

	for( Uint32 i = 0 ; i < nFrames ; i++ )
	{
		WgGfxFrame * pFrame = new WgGfxFrame;

		pFrame->pSurf = pSurf;
		pFrame->ofs.x = xOfs;
		pFrame->ofs.y = yOfs + i*(m_height+spacing);

		bool bOk = WgAnim::addKeyFrame( pFrame, duration );
			
		if( !bOk )
		{
			delete pFrame;
			return i;
		}
	}

	return nFrames;
}


//____ getFrame() _____________________________________________________________

WgGfxFrame * WgGfxAnim::getFrame( Uint32 ticks, WgGfxFrame * pProximity ) const
{
	WgAnimPlayPos playPos = getPlayPos( ticks, pProximity );

	return (WgGfxFrame *) playPos.pKeyFrame1;
}
