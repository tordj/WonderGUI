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

//____ IsInstanceOf() _________________________________________________________

bool WgGfxAnim::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgAnim::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgGfxAnim::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgGfxAnimPtr WgGfxAnim::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgGfxAnimPtr( static_cast<WgGfxAnim*>(pObject.RawPtr()) );

	return 0;
}

//____ SetSize() ______________________________________________________________

void WgGfxAnim::SetSize( WgSize size )
{
	m_size = size;
}

//____ InsertFrame() __________________________________________________________

bool WgGfxAnim::InsertFrame( int pos, const WgSurfacePtr& pSurf, WgCoord ofs, int duration )
{
	if( !pSurf || pSurf->Width() < (int)(ofs.x + m_size.w) || pSurf->Height() < (int)(ofs.y + m_size.h) )
		return false;


	WgGfxFrame * pFrame = new WgGfxFrame();

	pFrame->pSurf = pSurf;
	pFrame->rect = WgRect(ofs,m_size);

	bool bOk = WgAnim::_insertKeyFrame( pos, pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;
}

bool WgGfxAnim::InsertFrame( WgGfxFrame * pBefore, const WgSurfacePtr& pSurf, WgCoord ofs, int duration )
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

//____ AddFrame() _____________________________________________________________

bool WgGfxAnim::AddFrame( const WgSurfacePtr& pSurf, WgCoord ofs, int duration )
{
	if( !pSurf || pSurf->Width() < (int)(ofs.x + m_size.w) || pSurf->Height() < (int)(ofs.y + m_size.h) )
		return false;

	WgGfxFrame * pFrame = new WgGfxFrame;

	pFrame->pSurf = pSurf;
	pFrame->rect = WgRect(ofs,m_size);

	bool bOk = WgAnim::_addKeyFrame( pFrame, duration );

	if( !bOk )
		delete pFrame;

	return bOk;	
}


//____ AddFrames() ____________________________________________________________

int WgGfxAnim::AddFrames( const WgSurfacePtr& pSurf, int duration, int nFrames, WgSize spacing )
{
	if( !pSurf )
		return 0;

	WgSize arraySize;

	arraySize.w = (pSurf->Width()+spacing.w) / (m_size.w+spacing.w);
	arraySize.h = (pSurf->Height()+spacing.h) / (m_size.h+spacing.h);

	return AddFrames( pSurf, WgCoord(0,0), arraySize, duration, nFrames, spacing );
}


int WgGfxAnim::AddFrames( const WgSurfacePtr& pSurf, WgCoord arrayOfs, WgSize arraySize, int duration, int nFrames, WgSize spacing )
{
	int nFramesX = arraySize.w;
	int nFramesY = arraySize.h;

	if( !pSurf || nFrames < 0 || nFrames > nFramesX*nFramesY || nFramesX <= 0 || nFramesY <= 0 ||
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


//____ GetFrame() _____________________________________________________________

WgGfxFrame * WgGfxAnim::GetFrame( int64_t ticks, WgGfxFrame * pProximity ) const
{
	WgAnimPlayPos playPos = _playPos( ticks, pProximity );

	return (WgGfxFrame *) playPos.pKeyFrame1;
}
