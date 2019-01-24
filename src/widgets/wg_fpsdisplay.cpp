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
#include	<wg_stdtextmapper.h>
#include	<wg_msgrouter.h>

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

		
		// We set our own default text textMappers since we rely on alignment
		// to present the information in a readable way.

		m_labelsText.setTextMapper( StdTextMapper::create() );

		StdTextMapper_p pValueTextMapper = StdTextMapper::create();
		pValueTextMapper->setAlignment( Origo::NorthEast );
		m_valuesText.setTextMapper(pValueTextMapper);

	
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
	
	FpsDisplay_p FpsDisplay::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return FpsDisplay_p( static_cast<FpsDisplay*>(pObject) );
	
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
	
	
	//____ _render() ________________________________________________________
	
	void FpsDisplay::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
	
		Rect content;
		if( m_pSkin )
			content = m_pSkin->contentRect( _canvas, m_state );
		else
			content = _canvas;

		m_labelsText.render( pDevice, content );
		m_valuesText.render( pDevice, content );	
	}
	
	
	//____ _receive() _____________________________________________________________
	
	void FpsDisplay::_receive( Msg * pMsg )
	{
		Widget::_receive(pMsg);
	
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
	
	//____ _setState() ______________________________________________________
	
	void FpsDisplay::_setState( State state )
	{
	
		m_labelsText.setState(state);
		m_valuesText.setState(state);
		_requestRender();							//TODO: Check if there has been changes to text appearance.
	
		if( state.isEnabled() && !m_state.isEnabled() )
			m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
	
		if( !state.isEnabled() && m_state.isEnabled() )
		{
			Base::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
		}

		Widget::_setState(state);
	}
		
	//____ _cloneContent() _______________________________________________________
	
	void FpsDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		FpsDisplay * pOrg		= (FpsDisplay *) _pOrg;
	
//		m_labelsText.clone( &pOrg->m_labelsText );
//		m_valuesText.clone( &pOrg->m_valuesText );
	
		m_tickBufferOfs	= pOrg->m_tickBufferOfs;
	
		for( int i = 0 ; i < c_tickBuffer ; i++ )
			m_pTickBuffer[i] = pOrg->m_pTickBuffer[i];
	}


} // namespace wg
