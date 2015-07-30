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

const char WgGfxAnim::CLASSNAME[] = {"GfxAnim"};

//____ Constructor ____________________________________________________________

WgGfxAnim::WgGfxAnim()
{
}


WgGfxAnim::WgGfxAnim( WgSize size )
{
	m_size = size;
}

//____ isInstanceOf() _________________________________________________________

bool WgGfxAnim::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgAnim::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgGfxAnim::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgGfxAnim_p WgGfxAnim::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgGfxAnim_p( static_cast<WgGfxAnim*>(pObject.rawPtr()) );

	return 0;
}

//____ setSize() ______________________________________________________________

void WgGfxAnim::setSize( WgSize size )
{
	m_size = size;
}

//____ insertFrame() __________________________________________________________

bool WgGfxAnim::insertFrame( int pos, const WgSurface_p& pSurf, WgCoord ofs, int duration )
{
	if( !pSurf || pSurf->width() < (int)(ofs.x + m_size.w) || pSurf->height() < (int)(ofs.y + m_size.h) )
		return false;


	WgGfxFrame * pFrame = new WgGfxFrame();

	pFrame->pSurf = pSurf;
	pFrame->rect = WgRect(ofs,m_size);

	bool bOk = WgAnim::_insertKeyFrame( pos, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

bool WgGfxAnim::insertFrame( WgGfxFrame * pBefore, const WgSurface_p& pSurf, WgCoord ofs, int duration )
{
	if( !pSurf )
		return false;

	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->rect = WgRect(ofs,m_size);

	bool bOk = WgAnim::_insertKeyFrame( pBefore, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

//____ addFrame() _____________________________________________________________

bool WgGfxAnim::addFrame( const WgSurface_p& pSurf, WgCoord ofs, int duration )
{
	if( !pSurf || pSurf->width() < (int)(ofs.x + m_size.w) || pSurf->height() < (int)(ofs.y + m_size.h) )
		return false;

	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->rect = WgRect(ofs,m_size);

	bool bOk = WgAnim::_addKeyFrame( pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;	
}


//____ addFrames() ____________________________________________________________

int WgGfxAnim::addFrames( const WgSurface_p& pSurf, int duration, int nFrames, WgSize spacing )
{
	if( !pSurf )
		return 0;

	WgSize arraySize;

	arraySize.w = (pSurf->width()+spacing.w) / (m_size.w+spacing.w);
	arraySize.h = (pSurf->height()+spacing.h) / (m_size.h+spacing.h);

	return addFrames( pSurf, WgCoord(0,0), arraySize, duration, nFrames, spacing );
}


int WgGfxAnim::addFrames( const WgSurface_p& pSurf, WgCoord arrayOfs, WgSize arraySize, int duration, int nFrames, WgSize spacing )
{
	int nFramesX = arraySize.w;
	int nFramesY = arraySize.h;

	if( !pSurf || nFrames < 0 || nFrames > nFramesX*nFramesY || nFramesX <= 0 || nFramesY <= 0 ||
		((nFramesX*m_size.w+(nFramesX-1)*spacing.w ) > pSurf->width() - arrayOfs.x ) ||
		((nFramesY*m_size.h+(nFramesY-1)*spacing.h ) > pSurf->height() - arrayOfs.y ) )
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
			pFrame->rect = WgRect( arrayOfs.x + x*(m_size.w+spacing.w), arrayOfs.y + y*(m_size.h+spacing.h), m_size );

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


//____ getFrame() _____________________________________________________________

WgGfxFrame * WgGfxAnim::getFrame( int64_t ticks, WgGfxFrame * pProximity ) const
{
	WgAnimPlayPos playPos = _playPos( ticks, pProximity );

	return (WgGfxFrame *) playPos.pKeyFrame1;
}
