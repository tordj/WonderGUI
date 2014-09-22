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
#include	<wg_fpsdisplay.h>
#include	<wg_texttool.h>
#include	<wg_gfxdevice.h>
#include	<wg_color.h>
#include	<wg_char.h>
#include	<wg_pen.h>
#include	<wg_base.h>

const char WgFpsDisplay::CLASSNAME[] = {"FpsDisplay"};


#define		TICK_BUFFER			64



//____ WgFpsDisplay() _________________________________________________________________

WgFpsDisplay::WgFpsDisplay( void )
{
	m_pTickBuffer		= new int[TICK_BUFFER];
	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		m_pTickBuffer[i] = 1;

	m_tickBufferOfs		= 0;

	labels.Set( "Now:/nMin:/nAvg:/nMax:/n" );
	values.SetAlignment( WG_NORTHEAST );

	m_bReceiveTick = true;

	labels._setHolder(this);
	values._setHolder(this);
}

//____ ~WgFpsDisplay() __________________________________________________________

WgFpsDisplay::~WgFpsDisplay( void )
{
	if( m_pTickBuffer )
	{
		delete [] m_pTickBuffer;
		m_pTickBuffer = 0;
	}
}


//____ IsInstanceOf() _________________________________________________________

bool WgFpsDisplay::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgFpsDisplay::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgFpsDisplayPtr WgFpsDisplay::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFpsDisplayPtr( static_cast<WgFpsDisplay*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetTextProperties() ____________________________________________________

void WgFpsDisplay::SetTextProperties( const WgTextpropPtr& pProp )
{
	labels.SetProperties(pProp);
	values.SetProperties(pProp);
	_requestResize();
	_requestRender();
}

//____ PreferredSize() __________________________________________________________

WgSize WgFpsDisplay::PreferredSize() const
{
	WgSize contentSize = labels.unwrappedSize();

	WgTextAttr attr;
	values.GetBaseAttr( attr );
	contentSize.w += WgTextTool::lineWidth( values.getNode(), attr, " 1000.00" );

	int valueHeight = values.unwrappedSize().h;
	if( valueHeight > contentSize.h )
		contentSize.h = valueHeight;

	if( m_pSkin )
		return m_pSkin->SizeForContent(contentSize);
	else
		return contentSize;
}


//____ _onRender() ________________________________________________________

void WgFpsDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect content;
	if( m_pSkin )
		content = m_pSkin->ContentRect( _canvas, m_state );
	else
		content = _canvas;

	pDevice->PrintText( _clip, &labels, _canvas );
	pDevice->PrintText( _clip, &values, _canvas );
}


//____ _onEvent() _____________________________________________________________

void WgFpsDisplay::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	WgWidget::_onEvent(pEvent,pHandler);

	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			// Update tick buffer

			m_tickBufferOfs = (++m_tickBufferOfs) % TICK_BUFFER;

			int msDiff = WgTickEvent::Cast(pEvent)->Millisec();
			if( msDiff > 0 )
				m_pTickBuffer[m_tickBufferOfs] = msDiff;
			else
				m_pTickBuffer[m_tickBufferOfs] = 1;

			// Update valueTexts from tick-buffer

			const int	cCurrentFrames = 10;
			int currOfs = ((int)m_tickBufferOfs) - cCurrentFrames;
			if( currOfs < 0 )
				currOfs += TICK_BUFFER;

			int	currTotal = 0;
			for( int i = 0 ; i < cCurrentFrames ; i++ )
			{
				currTotal += m_pTickBuffer[currOfs++];
				currOfs %= TICK_BUFFER;
			}
			float	fpsCurrent = 1000.f / (currTotal / (float) cCurrentFrames);

			//____

			int	avg = 0;
			for( int i = 0 ; i < TICK_BUFFER ; i++ )
				avg += m_pTickBuffer[i];
			float fpsAvg = 1000.f / (((float)avg)/TICK_BUFFER);

			//____

			int	min = 1000000000;
			for( int i = 0 ; i < TICK_BUFFER ; i++ )
				if( min > m_pTickBuffer[i] )
					min = m_pTickBuffer[i];
			float fpsMax = 1000.f / min;

			//____

			int	max = 0;
			for( int i = 0 ; i < TICK_BUFFER ; i++ )
				if( max < m_pTickBuffer[i] )
					max = m_pTickBuffer[i];
			float fpsMin = 1000.f / max;

			//____
	
			char	temp[100];
			sprintf( temp, "%.2f/n%.2f/n%.2f/n%.2f/n", fpsCurrent, fpsMin, fpsAvg, fpsMax );
			values.Set(temp);

			_requestRender();
		}
		default:
			break;
	}
}

//____ _onStateChanged() ______________________________________________________

void WgFpsDisplay::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	labels.setState(m_state);
	values.setState(m_state);
	_requestRender();							//TODO: Check if there has been changes to text appearance.

	if( m_state.IsEnabled() && !oldState.IsEnabled() )
		_startReceiveTicks();

	if( !m_state.IsEnabled() && oldState.IsEnabled() )
		_stopReceiveTicks();
}

//____ _onSkinChanged() _______________________________________________________

void WgFpsDisplay::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	labels.SetColorSkin(pNewSkin);
	values.SetColorSkin(pNewSkin);
}

//____ _fieldModified() ________________________________________________________

void WgFpsDisplay::_fieldModified( WgTextField * pField )
{
	_requestRender();
	_requestResize();
}

//____ _onCloneContent() _______________________________________________________

void WgFpsDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	WgFpsDisplay * pOrg		= (WgFpsDisplay *) _pOrg;

	labels.clone( &pOrg->labels );
	values.clone( &pOrg->values );

	m_tickBufferOfs	= pOrg->m_tickBufferOfs;

	for( int i = 0 ; i < TICK_BUFFER ; i++ )
		m_pTickBuffer[i] = pOrg->m_pTickBuffer[i];
}


