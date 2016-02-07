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
	
	const char AnimPlayer::CLASSNAME[] = {"AnimPlayer"};
	
	
	//____ Constructor _________________________________________________________________
	
	AnimPlayer::AnimPlayer()
	{
		m_pAnim			= 0;
		m_pAnimFrame	= 0;
		m_playPos		= 0.0;
	
		m_bPlaying		= false;
		m_speed			= 1.f;
		m_tickRouteId	= 0;
	}
	
	//____ ~AnimPlayer() _______________________________________________________
	
	AnimPlayer::~AnimPlayer()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );		
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool AnimPlayer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * AnimPlayer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	AnimPlayer_p AnimPlayer::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return AnimPlayer_p( static_cast<AnimPlayer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setAnimation() ____________________________________________________________
	
	bool AnimPlayer::setAnimation( const GfxAnim_p& pAnim )
	{
		m_pAnim			= pAnim;
		m_playPos		= 0.0;
	
		Size	currSize = size();
		Size	wantedSize;
	
		_requestResize();
		_requestRender();
		return true;
	}
	
	//____ playPos() ______________________________________________________________
	
	int AnimPlayer::playPos()
	{
		return (int) m_playPos;
	}
	
	//____ setPlayPosFractional() _________________________________________________
	
	bool AnimPlayer::setPlayPosFractional( float _fraction )
	{
		if( !m_pAnim )
			return false;
	
		_fraction *= m_pAnim->duration();
	
		m_playPos		= _fraction;
		_playPosUpdated();
		return true;
	}
	
	
	//____ setPlayPos() ___________________________________________________________
	
	bool AnimPlayer::setPlayPos( int _ticks )
	{
		if( !m_pAnim )
			return false;
	
		m_playPos		= _ticks;
		_playPosUpdated();
		return true;
	}
	
	
	
	//____ rewind() _______________________________________________________________
	
	bool AnimPlayer::rewind( int _ticks )
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
	
	bool AnimPlayer::fastForward( int _ticks )
	{
		if( !m_pAnim )
			return false;
	
	 	m_playPos += _ticks;
	
		_playPosUpdated();
		return true;
	}
	
	//____ duration() _____________________________________________________________
	
	int AnimPlayer::duration()
	{
		if( !m_pAnim )
			return 0;
	
		return	m_pAnim->durationScaled();
	}
	
	//____ durationScaled() _______________________________________________________
	
	int AnimPlayer::durationScaled()
	{
		if( !m_pAnim )
			return 0;
	
		return	(int) (m_pAnim->durationScaled() / m_speed);
	}
	
	//____ speed() ________________________________________________________________
	
	float AnimPlayer::speed()
	{
		return m_speed;
	}
	
	//____ setSpeed() _____________________________________________________________
	
	bool AnimPlayer::setSpeed( float _speed )
	{
		if( _speed <= 0 || _speed > 1000 )
			return false;
	
		m_speed = _speed;
		return true;
	}
	
	//____ play() _________________________________________________________________
	
	bool AnimPlayer::play()
	{
		if( !m_pAnim )
			return false;
	
		m_bPlaying = true;
		m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
		return true;
	}
	
	//____ stop() _________________________________________________________________
	
	bool AnimPlayer::stop()
	{
		m_bPlaying = false;
		Base::msgRouter()->deleteRoute( m_tickRouteId );
		m_tickRouteId = 0;
		return true;
	}
	
	//____ preferredSize() ___________________________________________________________
	
	Size AnimPlayer::preferredSize() const
	{
		Size	sz;
	
		if( m_pAnim )
			sz = m_pAnim->size();
	
		if( m_pSkin )
			sz = m_pSkin->sizeForContent(sz);
	
		return sz;
	}
	
	//_____ _playPosUpdated() ______________________________________________________
	
	void AnimPlayer::_playPosUpdated()
	{
		if( !m_pAnim )
			return;
	
		GfxFrame * pAnimFrame = m_pAnim->getFrame( (int64_t) m_playPos );
	
		if( pAnimFrame != m_pAnimFrame )
		{
			m_pAnimFrame = pAnimFrame;
			_requestRender();
	
			Base::msgRouter()->post( new ValueUpdateMsg(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->duration()-1)),true));
		}
	}
	
	
	//____ _onMsg() ______________________________________________________________
	
	void AnimPlayer::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg( pMsg );
	
		switch( pMsg->type() )
		{
			case MsgType::Tick:
			{
				if( !m_pAnim || !m_state.isEnabled() )
					return;
	
				m_playPos += TickMsg::cast(pMsg)->timediff() * m_speed;
				_playPosUpdated();
	
			}
			break;
		}
	}
	
	
	//____ _onRender() ________________________________________________________
	
	void AnimPlayer::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender( pDevice, _canvas, _window, _clip );
	
		if( m_pAnim && m_state.isEnabled() )
			pDevice->clipStretchBlit( _clip, m_pAnimFrame->pSurf, m_pAnimFrame->rect, _canvas );
	}
	
	//____ _onRefresh() _______________________________________________________
	
	void AnimPlayer::_onRefresh( void )
	{
		Widget::_onRefresh();
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void AnimPlayer::_onCloneContent( const Widget * _pOrg )
	{
		AnimPlayer * pOrg = (AnimPlayer *) _pOrg;
	
		m_pAnim				= pOrg->m_pAnim;
		m_pAnimFrame		= pOrg->m_pAnimFrame;
		m_pSkin				= pOrg->m_pSkin;
	
		m_bPlaying			= pOrg->m_bPlaying;
		m_playPos			= pOrg->m_playPos;
		m_speed				= pOrg->m_speed;
	}
	
	//____ _onAlphaTest() ______________________________________________________
	
	bool AnimPlayer::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		if( m_pAnim && m_state.isEnabled() && Util::markTestStretchRect( ofs, m_pAnimFrame->pSurf, m_pAnimFrame->rect, Rect(0,0,sz), m_markOpacity ) )
			return true;
	
		return Widget::_onAlphaTest(ofs,sz);
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void AnimPlayer::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		if( oldState.isEnabled() != m_state.isEnabled() && m_bPlaying )
		{
			if( m_state.isEnabled() )
				m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
			else
			{	
				Base::msgRouter()->deleteRoute( m_tickRouteId );
				m_tickRouteId = 0;
			}
			_requestRender();
		}
	}
	

} // namespace wg
