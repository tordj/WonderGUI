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
#include	<wg_base.h>
#include	<wg_standardpresenter.h>

namespace wg 
{
	
	const char FpsDisplay::CLASSNAME[] = {"FpsDisplay"};
	
	
	
	//____ FpsDisplay() _________________________________________________________________
	
	FpsDisplay::FpsDisplay( void ) : m_labelsText(this), m_valuesText(this), labels(&m_labelsText), values(&m_valuesText)
	{
		m_pTickBuffer		= new int[c_tickBuffer];
		for( int i = 0 ; i < c_tickBuffer ; i++ )
			m_pTickBuffer[i] = 1;
	
		m_tickBufferOfs		= 0;
	
		m_labelsText.set( "Now:\nMin:\nAvg:\nMax:" );

		
		// We set our own default text presenters since we rely on alignment
		// to present the information in a readable way.

		m_labelsText.setPresenter( StandardPresenter::create() );

		StandardPresenter_p pValuePresenter = StandardPresenter::create();
		pValuePresenter->setAlignment( Origo::NorthEast );
		m_valuesText.setPresenter(pValuePresenter);

	
		m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
	}
	
	//____ ~FpsDisplay() __________________________________________________________
	
	FpsDisplay::~FpsDisplay( void )
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
		if( m_pTickBuffer )
		{
			delete [] m_pTickBuffer;
			m_pTickBuffer = 0;
		}
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool FpsDisplay::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * FpsDisplay::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	FpsDisplay_p FpsDisplay::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return FpsDisplay_p( static_cast<FpsDisplay*>(pObject.rawPtr()) );
	
		return 0;
	}
	
		
	//____ preferredSize() __________________________________________________________
	
	Size FpsDisplay::preferredSize() const
	{
		Size contentSize = m_labelsText.preferredSize();
	
		TextAttr attr;
		m_valuesText._style()->exportAttr( State(StateEnum::Normal), &attr );

		int prevSize = attr.pFont->size();
		attr.pFont->setSize(attr.size);
		contentSize.w += attr.pFont->maxAdvance() * 7;			// Reserve space for: ' 999.99' after longest label.
		attr.pFont->setSize(prevSize);
	
		int valueHeight = m_valuesText.preferredSize().h;
		if( valueHeight > contentSize.h )
			contentSize.h = valueHeight;
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	
	//____ _onRender() ________________________________________________________
	
	void FpsDisplay::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		Rect content;
		if( m_pSkin )
			content = m_pSkin->contentRect( _canvas, m_state );
		else
			content = _canvas;

		m_labelsText.onRender( pDevice, content, _clip );
		m_valuesText.onRender( pDevice, content, _clip );	
	}
	
	
	//____ _onMsg() _____________________________________________________________
	
	void FpsDisplay::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg(pMsg);
	
		switch( pMsg->type() )
		{
			case MsgType::Tick:
			{
				// Update tick buffer
	
				m_tickBufferOfs = (++m_tickBufferOfs) % c_tickBuffer;
	
				int msDiff = TickMsg::cast(pMsg)->timediff();
				if( msDiff > 0 )
					m_pTickBuffer[m_tickBufferOfs] = msDiff;
				else
					m_pTickBuffer[m_tickBufferOfs] = 1;
	
				// Update valueTexts from tick-buffer
	
				const int	cCurrentFrames = 10;
				int currOfs = ((int)m_tickBufferOfs) - cCurrentFrames;
				if( currOfs < 0 )
					currOfs += c_tickBuffer;
	
				int	currTotal = 0;
				for( int i = 0 ; i < cCurrentFrames ; i++ )
				{
					currTotal += m_pTickBuffer[currOfs++];
					currOfs %= c_tickBuffer;
				}
				float	fpsCurrent = 1000.f / (currTotal / (float) cCurrentFrames);
	
				//____
	
				int	avg = 0;
				for( int i = 0 ; i < c_tickBuffer ; i++ )
					avg += m_pTickBuffer[i];
				float fpsAvg = 1000.f / (((float)avg)/c_tickBuffer);
	
				//____
	
				int	min = 1000000000;
				for( int i = 0 ; i < c_tickBuffer ; i++ )
					if( min > m_pTickBuffer[i] )
						min = m_pTickBuffer[i];
				float fpsMax = 1000.f / min;
	
				//____
	
				int	max = 0;
				for( int i = 0 ; i < c_tickBuffer ; i++ )
					if( max < m_pTickBuffer[i] )
						max = m_pTickBuffer[i];
				float fpsMin = 1000.f / max;
	
				//____
		
				char	temp[100];
				snprintf( temp, 100, "%.2f\n%.2f\n%.2f\n%.2f", fpsCurrent, fpsMin, fpsAvg, fpsMax );
				m_valuesText.set(temp);
	
				_requestRender();
			}
			default:
				break;
		}
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void FpsDisplay::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		m_labelsText.setState(m_state);
		m_valuesText.setState(m_state);
		_requestRender();							//TODO: Check if there has been changes to text appearance.
	
		if( m_state.isEnabled() && !oldState.isEnabled() )
			m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
	
		if( !m_state.isEnabled() && oldState.isEnabled() )
		{
			Base::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
		}
	}
		
	//____ _onCloneContent() _______________________________________________________
	
	void FpsDisplay::_onCloneContent( const Widget * _pOrg )
	{
		FpsDisplay * pOrg		= (FpsDisplay *) _pOrg;
	
//		m_labelsText.clone( &pOrg->m_labelsText );
//		m_valuesText.clone( &pOrg->m_valuesText );
	
		m_tickBufferOfs	= pOrg->m_tickBufferOfs;
	
		for( int i = 0 ; i < c_tickBuffer ; i++ )
			m_pTickBuffer[i] = pOrg->m_pTickBuffer[i];
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void FpsDisplay::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}

	void FpsDisplay::_onFieldDirty( Field * pField, const Rect& rect )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void FpsDisplay::_onFieldResize( Field * pField )
	{
		_requestResize();
		_requestRender();
	}
	

} // namespace wg
