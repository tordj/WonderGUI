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


//____ Constructor ____________________________________________________________

WgGfxAnim::WgGfxAnim()
{
	m_blockFlags = 0;
}


WgGfxAnim::WgGfxAnim( WgSize size, WgBorders gfxBorders, Uint32 blockFlags )
{
	m_size = size;
	m_borders = gfxBorders;
	m_blockFlags = blockFlags;
}

//____ SetSize() ______________________________________________________________

void WgGfxAnim::SetSize( WgSize size )
{
	m_size = size;
}

//____ SetGfxBorders() ________________________________________________________

void WgGfxAnim::SetGfxBorders( WgBorders borders )
{
	m_borders = borders;
}

//____ SetBlockFlags() ________________________________________________________

void WgGfxAnim::SetBlockFlags( Uint32 flags )
{
	m_blockFlags = flags;
}



//____ InsertFrame() __________________________________________________________

bool WgGfxAnim::InsertFrame( int pos, WgSurface * pSurf, WgCoord ofs, int duration )
{
	if( pSurf->Width() < (int)(ofs.x + m_size.w) || pSurf->Height() < (int)(ofs.y + m_size.h) )
		return false;


	WgGfxFrame * pFrame = new WgGfxFrame();

	pFrame->pSurf = pSurf;
	pFrame->ofs = ofs;

	bool bOk = WgAnim::_insertKeyFrame( pos, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

bool WgGfxAnim::InsertFrame( WgGfxFrame * pBefore, WgSurface * pSurf, WgCoord ofs, int duration )
{
	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->ofs = ofs;

	bool bOk = WgAnim::_insertKeyFrame( pBefore, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

//____ AddFrame() _____________________________________________________________

bool WgGfxAnim::AddFrame( WgSurface * pSurf, WgCoord ofs, int duration )
{
	if( pSurf->Width() < (int)(ofs.x + m_size.w) || pSurf->Height() < (int)(ofs.y + m_size.h) )
		return false;

	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->ofs = ofs;

	bool bOk = WgAnim::_addKeyFrame( pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;	
}


//____ AddFrames() ____________________________________________________________

int WgGfxAnim::AddFrames( WgSurface * pSurf, int duration, int nFrames, WgSize spacing )
{
	if( pSurf == 0 )
		return 0;

	WgSize arraySize;

	arraySize.w = (pSurf->Width()+spacing.w) / (m_size.w+spacing.w);
	arraySize.h = (pSurf->Height()+spacing.h) / (m_size.h+spacing.h);

	return AddFrames( pSurf, WgCoord(0,0), arraySize, duration, nFrames, spacing );
}


int WgGfxAnim::AddFrames( WgSurface * pSurf, WgCoord arrayOfs, WgSize arraySize, int duration, int nFrames, WgSize spacing )
{
	int nFramesX = arraySize.w;
	int nFramesY = arraySize.h;

	if( pSurf == 0 || nFrames < 0 || nFrames > nFramesX*nFramesY || nFramesX <= 0 || nFramesY <= 0 ||
		((nFramesX*m_size.w+(nFramesX-1)*spacing.w ) > pSurf->Width() - arrayOfs.x ) ||
		((nFramesY*m_size.h+(nFramesY-1)*spacing.h ) > pSurf->Height() - arrayOfs.y ) )
		return 0;

	if( nFrames == 0 )
		nFrames = nFramesX*nFramesY;

	int framesAdded = 0;
	for( int y = 0 ; y < nFramesY ; y++ )
	{
		for( int x = 0 ; x < nFramesX ; x++ )
		{
			if( framesAdded == nFrames )
				break;

			WgGfxFrame * pFrame = new WgGfxFrame;

			pFrame->pSurf = pSurf;
			pFrame->ofs.x = arrayOfs.x + x*(m_size.w+spacing.w);
			pFrame->ofs.y = arrayOfs.y + y*(m_size.h+spacing.h);

			bool bOk = WgAnim::_addKeyFrame( pFrame, duration );
				
			if( !bOk )
			{
				delete pFrame;
				return framesAdded;
			}

			framesAdded++;
		}
	}
	return framesAdded;
}


//____ GetFrame() _____________________________________________________________

WgGfxFrame * WgGfxAnim::GetFrame( int64_t ticks, WgGfxFrame * pProximity ) const
{
	WgAnimPlayPos playPos = _playPos( ticks, pProximity );

	return (WgGfxFrame *) playPos.pKeyFrame1;
}

//____ GetBlock() _____________________________________________________________

WgBlock WgGfxAnim::GetBlock( int64_t ticks, WgGfxFrame * pProximity ) const
{
	WgAnimPlayPos playPos = _playPos( ticks, pProximity );
	WgGfxFrame * pFrame = (WgGfxFrame*) playPos.pKeyFrame1;

	return WgBlock( pFrame->pSurf, WgRect(pFrame->ofs,m_size), m_borders, WgBorders(), WgCoord(), m_blockFlags );
}
