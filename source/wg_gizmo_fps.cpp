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
#include	<stdio.h>
#include	<wg_gizmo_fps.h>
#include	<wg_texttool.h>
#include	<wg_gfxdevice.h>
#include	<wg_color.h>
#include	<wg_char.h>
#include	<wg_pen.h>
#include	<wg_base.h>

static const char	c_gizmoType[] = {"Fps"};


#define		TICK_BUFFER			64



//____ WgGizmoFps() _________________________________________________________________

WgGizmoFps::WgGizmoFps( void )
{
	m_pTickBuffer		= new Uint32[TICK_BUFFER];
	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		m_pTickBuffer[i] = 1;

	m_tickBufferOfs		= 0;
	m_bOpaque			= false;
}

//____ ~WgGizmoFps() __________________________________________________________

WgGizmoFps::~WgGizmoFps( void )
{
	if( m_pTickBuffer )
	{
		delete [] m_pTickBuffer;
		m_pTickBuffer = 0;
	}
}


//____ Type() _________________________________________________________________

const char * WgGizmoFps::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoFps::GetMyType( void )
{
	return c_gizmoType;
}


//____ SetTextProperties() ____________________________________________________

void WgGizmoFps::SetTextProperties( const WgTextPropPtr& pProp )
{
	m_pProp = pProp;
	_requestRender();
}

//____ DefaultSize() __________________________________________________________

WgSize WgGizmoFps::DefaultSize() const
{
	//TODO: Implement!

	return WgSize(1,1);
}


//____ _onRender() ________________________________________________________

void WgGizmoFps::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{

	const int	cCurrentFrames = 10;
	int currOfs = ((int)m_tickBufferOfs) - cCurrentFrames;
	if( currOfs < 0 )
		currOfs += TICK_BUFFER;

	Uint32	currTotal = 0;
	for( int i = 0 ; i < cCurrentFrames ; i++ )
	{
		currTotal += m_pTickBuffer[currOfs++];
		currOfs %= TICK_BUFFER;
	}
	float	fpsCurrent = 1000.f / (currTotal / (float) cCurrentFrames);

	//____

	Uint32	avg = 0;
	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		avg += m_pTickBuffer[i];
	float fpsAvg = 1000.f / (((float)avg)/TICK_BUFFER);

	//____

	Uint32	min = 1000000000;
	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		if( min > m_pTickBuffer[i] )
			min = m_pTickBuffer[i];
	float fpsMax = 1000.f / min;

	//____

	Uint32	max = 0;
	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		if( max < m_pTickBuffer[i] )
			max = m_pTickBuffer[i];
	float fpsMin = 1000.f / max;

	//____

	char	temp[40];
	WgChar	temp2[40];
	const char *	pTemp;

	WgPen	pen( pDevice, _canvas, _clip );
	WgTextAttr attr;

	WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextProp());
	WgTextTool::AddPropAttributes(attr, m_pProp);

	pen.SetAttributes( attr );
	pen.SetPos( WgCoord(_canvas.x, _canvas.y + pen.GetBaseline()) );

	int height = pen.GetLineSpacing();

	pTemp = temp;
	sprintf( temp, "Now: %.2f", fpsCurrent );
	WgTextTool::readString( pTemp, temp2, 39 );

	pDevice->PrintLine( pen, attr, temp2 );

	pTemp = temp;
	sprintf( temp, "Min: %.2f", fpsMin );
	WgTextTool::readString( pTemp, temp2, 39 );
	pen.SetPosX( _canvas.x );
	pen.MoveY( height );
	pDevice->PrintLine( pen, attr, temp2 );

	pTemp = temp;
	sprintf( temp, "Avg: %.2f", fpsAvg );
	WgTextTool::readString( pTemp, temp2, 39 );
	pen.SetPosX( _canvas.x );
	pen.MoveY( height );
	pDevice->PrintLine( pen, attr, temp2 );

	pTemp = temp;
	sprintf( temp, "Max: %.2f", fpsMax );
	WgTextTool::readString( pTemp, temp2, 39 );
	pen.SetPosX( _canvas.x );
	pen.MoveY( height );
	pDevice->PrintLine( pen, attr, temp2 );
}

//____ _onUpdate() ________________________________________________________

void WgGizmoFps::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	m_tickBufferOfs = (++m_tickBufferOfs) % TICK_BUFFER;

	if( _updateInfo.msDiff > 0 )
		m_pTickBuffer[m_tickBufferOfs] = _updateInfo.msDiff;
	else
		m_pTickBuffer[m_tickBufferOfs] = 1;
	_requestRender();
}


//____ _onEvent() _____________________________________________________________

#ifdef WG_TNG
void WgGizmoFps::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			m_tickBufferOfs = (++m_tickBufferOfs) % TICK_BUFFER;

			int msDiff = ((WgEvent::Tick*)pEvent)->Millisec();
			if( msDiff > 0 )
				m_pTickBuffer[m_tickBufferOfs] = msDiff;
			else
				m_pTickBuffer[m_tickBufferOfs] = 1;
			_requestRender();
		}
		default:
			break;
	}
}
#endif

//____ DoMyOwnCloning() _______________________________________________________

void WgGizmoFps::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoFps * pOrg		= (WgGizmoFps *) _pOrg;

	m_pProp			= pOrg->m_pProp;
	m_tickBufferOfs	= pOrg->m_tickBufferOfs;

	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		m_pTickBuffer[i] = pOrg->m_pTickBuffer[i];
}


