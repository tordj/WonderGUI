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

#include <wg_eventhandler.h>
#include <math.h>

const char WgAnimPlayer::CLASSNAME[] = {"AnimPlayer"};


//____ Constructor _________________________________________________________________

WgAnimPlayer::WgAnimPlayer()
{
	m_pAnim			= 0;
	m_pAnimFrame	= 0;
	m_playPos		= 0.0;

	m_bPlaying		= false;
	m_speed			= 1.f;
}

//____ ~WgAnimPlayer() _______________________________________________________

WgAnimPlayer::~WgAnimPlayer()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgAnimPlayer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgAnimPlayer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgAnimPlayerPtr WgAnimPlayer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgAnimPlayerPtr( static_cast<WgAnimPlayer*>(pObject.RawPtr()) );

	return 0;
}


//____ SetAnimation() ____________________________________________________________

bool WgAnimPlayer::SetAnimation( const WgGfxAnimPtr& pAnim )
{
	m_pAnim			= pAnim;
	m_playPos		= 0.0;

	WgSize	currSize = Size();
	WgSize	wantedSize;

	_requestResize();
	_requestRender();
	return true;
}

//____ PlayPos() ______________________________________________________________

int WgAnimPlayer::PlayPos()
{
	return (int) m_playPos;
}

//____ SetPlayPosFractional() _________________________________________________

bool WgAnimPlayer::SetPlayPosFractional( float _fraction )
{
	if( !m_pAnim )
		return false;

	_fraction *= m_pAnim->Duration();

	m_playPos		= _fraction;
	_playPosUpdated();
	return true;
}


//____ SetPlayPos() ___________________________________________________________

bool WgAnimPlayer::SetPlayPos( int _ticks )
{
	if( !m_pAnim )
		return false;

	m_playPos		= _ticks;
	_playPosUpdated();
	return true;
}



//____ Rewind() _______________________________________________________________

bool WgAnimPlayer::Rewind( int _ticks )
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

//____ FastForward() __________________________________________________________

bool WgAnimPlayer::FastForward( int _ticks )
{
	if( !m_pAnim )
		return false;

 	m_playPos += _ticks;

	_playPosUpdated();
	return true;
}

//____ Duration() _____________________________________________________________

int WgAnimPlayer::Duration()
{
	if( !m_pAnim )
		return 0;

	return	m_pAnim->DurationScaled();
}

//____ DurationScaled() _______________________________________________________

int WgAnimPlayer::DurationScaled()
{
	if( !m_pAnim )
		return 0;

	return	(int) (m_pAnim->DurationScaled() / m_speed);
}

//____ Speed() ________________________________________________________________

float WgAnimPlayer::Speed()
{
	return m_speed;
}

//____ SetSpeed() _____________________________________________________________

bool WgAnimPlayer::SetSpeed( float _speed )
{
	if( _speed <= 0 || _speed > 1000 )
		return false;

	m_speed = _speed;
	return true;
}

//____ Play() _________________________________________________________________

bool WgAnimPlayer::Play()
{
	if( !m_pAnim )
		return false;

	m_bPlaying = true;
	_startReceiveTicks();
	return true;
}

//____ Stop() _________________________________________________________________

bool WgAnimPlayer::Stop()
{
	m_bPlaying = false;
	_stopReceiveTicks();
	return true;
}

//____ PreferredSize() ___________________________________________________________

WgSize WgAnimPlayer::PreferredSize() const
{
	WgSize	sz;

	if( m_pAnim )
		sz = m_pAnim->Size();

	if( m_pSkin )
		sz = m_pSkin->SizeForContent(sz);

	return sz;
}

//_____ _playPosUpdated() ______________________________________________________

void WgAnimPlayer::_playPosUpdated()
{
	if( !m_pAnim )
		return;

	WgGfxFrame * pAnimFrame = m_pAnim->GetFrame( (int64_t) m_playPos );

	if( pAnimFrame != m_pAnimFrame )
	{
		m_pAnimFrame = pAnimFrame;
		_requestRender();

		WgBase::MsgRouter()->QueueEvent( new WgValueUpdateEvent(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->Duration()-1)),true));
	}
}


//____ _onEvent() ______________________________________________________________

void WgAnimPlayer::_onEvent( const WgEventPtr& pEvent )
{
	WgWidget::_onEvent( pEvent );

	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( !m_pAnim || !m_state.IsEnabled() )
				return;

			m_playPos += WgTickEvent::Cast(pEvent)->Millisec() * m_speed;
			_playPosUpdated();

		}
		break;
	}
}


//____ _onRender() ________________________________________________________

void WgAnimPlayer::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender( pDevice, _canvas, _window, _clip );

	if( m_pAnim && m_state.IsEnabled() )
		pDevice->ClipStretchBlit( _clip, m_pAnimFrame->pSurf, m_pAnimFrame->rect, _canvas );
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
	if( m_pAnim && m_state.IsEnabled() && WgUtil::MarkTestStretchRect( ofs, m_pAnimFrame->pSurf, m_pAnimFrame->rect, WgRect(0,0,sz), m_markOpacity ) )
		return true;

	return WgWidget::_onAlphaTest(ofs,sz);
}

//____ _onStateChanged() ______________________________________________________

void WgAnimPlayer::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	if( oldState.IsEnabled() != m_state.IsEnabled() && m_bPlaying )
	{
		if( m_state.IsEnabled() )
			_startReceiveTicks();
		else
			_stopReceiveTicks();

		_requestRender();
	}
}

