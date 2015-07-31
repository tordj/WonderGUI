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

#include <wg_base.h>
#include <wg_animplayer.h>

#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

#include <wg_msgrouter.h>
#include <math.h>

namespace wg 
{
	
	const char WgAnimPlayer::CLASSNAME[] = {"AnimPlayer"};
	
	
	//____ Constructor _________________________________________________________________
	
	WgAnimPlayer::WgAnimPlayer()
	{
		m_pAnim			= 0;
		m_pAnimFrame	= 0;
		m_playPos		= 0.0;
	
		m_bPlaying		= false;
		m_speed			= 1.f;
		m_tickRouteId	= 0;
	}
	
	//____ ~WgAnimPlayer() _______________________________________________________
	
	WgAnimPlayer::~WgAnimPlayer()
	{
		if( m_tickRouteId )
			WgBase::msgRouter()->deleteRoute( m_tickRouteId );		
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgAnimPlayer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgAnimPlayer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgAnimPlayer_p WgAnimPlayer::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgAnimPlayer_p( static_cast<WgAnimPlayer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setAnimation() ____________________________________________________________
	
	bool WgAnimPlayer::setAnimation( const WgGfxAnim_p& pAnim )
	{
		m_pAnim			= pAnim;
		m_playPos		= 0.0;
	
		WgSize	currSize = size();
		WgSize	wantedSize;
	
		_requestResize();
		_requestRender();
		return true;
	}
	
	//____ playPos() ______________________________________________________________
	
	int WgAnimPlayer::playPos()
	{
		return (int) m_playPos;
	}
	
	//____ setPlayPosFractional() _________________________________________________
	
	bool WgAnimPlayer::setPlayPosFractional( float _fraction )
	{
		if( !m_pAnim )
			return false;
	
		_fraction *= m_pAnim->duration();
	
		m_playPos		= _fraction;
		_playPosUpdated();
		return true;
	}
	
	
	//____ setPlayPos() ___________________________________________________________
	
	bool WgAnimPlayer::setPlayPos( int _ticks )
	{
		if( !m_pAnim )
			return false;
	
		m_playPos		= _ticks;
		_playPosUpdated();
		return true;
	}
	
	
	
	//____ rewind() _______________________________________________________________
	
	bool WgAnimPlayer::rewind( int _ticks )
	{
		if( !m_pAnim )
			return false;
	
		if( m_playPos - _ticks < 0 )
			m_playPos = 0;
		else
			m_playPos -= _ticks;
	
		_playPosUpdated();
		return true;
	}
	
	//____ fastForward() __________________________________________________________
	
	bool WgAnimPlayer::fastForward( int _ticks )
	{
		if( !m_pAnim )
			return false;
	
	 	m_playPos += _ticks;
	
		_playPosUpdated();
		return true;
	}
	
	//____ duration() _____________________________________________________________
	
	int WgAnimPlayer::duration()
	{
		if( !m_pAnim )
			return 0;
	
		return	m_pAnim->durationScaled();
	}
	
	//____ durationScaled() _______________________________________________________
	
	int WgAnimPlayer::durationScaled()
	{
		if( !m_pAnim )
			return 0;
	
		return	(int) (m_pAnim->durationScaled() / m_speed);
	}
	
	//____ speed() ________________________________________________________________
	
	float WgAnimPlayer::speed()
	{
		return m_speed;
	}
	
	//____ setSpeed() _____________________________________________________________
	
	bool WgAnimPlayer::setSpeed( float _speed )
	{
		if( _speed <= 0 || _speed > 1000 )
			return false;
	
		m_speed = _speed;
		return true;
	}
	
	//____ play() _________________________________________________________________
	
	bool WgAnimPlayer::play()
	{
		if( !m_pAnim )
			return false;
	
		m_bPlaying = true;
		m_tickRouteId = WgBase::msgRouter()->addRoute( WG_MSG_TICK, this );
		return true;
	}
	
	//____ stop() _________________________________________________________________
	
	bool WgAnimPlayer::stop()
	{
		m_bPlaying = false;
		WgBase::msgRouter()->deleteRoute( m_tickRouteId );
		m_tickRouteId = 0;
		return true;
	}
	
	//____ preferredSize() ___________________________________________________________
	
	WgSize WgAnimPlayer::preferredSize() const
	{
		WgSize	sz;
	
		if( m_pAnim )
			sz = m_pAnim->size();
	
		if( m_pSkin )
			sz = m_pSkin->sizeForContent(sz);
	
		return sz;
	}
	
	//_____ _playPosUpdated() ______________________________________________________
	
	void WgAnimPlayer::_playPosUpdated()
	{
		if( !m_pAnim )
			return;
	
		WgGfxFrame * pAnimFrame = m_pAnim->getFrame( (int64_t) m_playPos );
	
		if( pAnimFrame != m_pAnimFrame )
		{
			m_pAnimFrame = pAnimFrame;
			_requestRender();
	
			WgBase::msgRouter()->post( new WgValueUpdateMsg(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->duration()-1)),true));
		}
	}
	
	
	//____ _onMsg() ______________________________________________________________
	
	void WgAnimPlayer::_onMsg( const WgMsg_p& pMsg )
	{
		WgWidget::_onMsg( pMsg );
	
		switch( pMsg->type() )
		{
			case WG_MSG_TICK:
			{
				if( !m_pAnim || !m_state.isEnabled() )
					return;
	
				m_playPos += WgTickMsg::cast(pMsg)->millisec() * m_speed;
				_playPosUpdated();
	
			}
			break;
		}
	}
	
	
	//____ _onRender() ________________________________________________________
	
	void WgAnimPlayer::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		WgWidget::_onRender( pDevice, _canvas, _window, _clip );
	
		if( m_pAnim && m_state.isEnabled() )
			pDevice->clipStretchBlit( _clip, m_pAnimFrame->pSurf, m_pAnimFrame->rect, _canvas );
	}
	
	//____ _onRefresh() _______________________________________________________
	
	void WgAnimPlayer::_onRefresh( void )
	{
		WgWidget::_onRefresh();
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void WgAnimPlayer::_onCloneContent( const WgWidget * _pOrg )
	{
		WgAnimPlayer * pOrg = (WgAnimPlayer *) _pOrg;
	
		m_pAnim				= pOrg->m_pAnim;
		m_pAnimFrame		= pOrg->m_pAnimFrame;
		m_pSkin				= pOrg->m_pSkin;
	
		m_bPlaying			= pOrg->m_bPlaying;
		m_playPos			= pOrg->m_playPos;
		m_speed				= pOrg->m_speed;
	}
	
	//____ _onAlphaTest() ______________________________________________________
	
	bool WgAnimPlayer::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
	{
		if( m_pAnim && m_state.isEnabled() && WgUtil::markTestStretchRect( ofs, m_pAnimFrame->pSurf, m_pAnimFrame->rect, WgRect(0,0,sz), m_markOpacity ) )
			return true;
	
		return WgWidget::_onAlphaTest(ofs,sz);
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void WgAnimPlayer::_onStateChanged( WgState oldState )
	{
		WgWidget::_onStateChanged(oldState);
	
		if( oldState.isEnabled() != m_state.isEnabled() && m_bPlaying )
		{
			if( m_state.isEnabled() )
				m_tickRouteId = WgBase::msgRouter()->addRoute( WG_MSG_TICK, this );
			else
			{	
				WgBase::msgRouter()->deleteRoute( m_tickRouteId );
				m_tickRouteId = 0;
			}
			_requestRender();
		}
	}
	

} // namespace wg
