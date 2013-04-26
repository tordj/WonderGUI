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

#include <wg_animplayer.h>

#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

#include <wg_eventhandler.h>
#include <math.h>

static const char	c_widgetType[] = {"AnimPlayer"};


//____ Constructor _________________________________________________________________

WgAnimPlayer::WgAnimPlayer()
{
	m_pAnim			= 0;
	m_playPos		= 0.0;

	m_bPlaying		= false;
	m_speed			= 1.f;
}

//____ ~WgAnimPlayer() _______________________________________________________

WgAnimPlayer::~WgAnimPlayer()
{
}

//____ Type() _________________________________________________________________

const char * WgAnimPlayer::Type() const
{
	return GetClass();
}

const char * WgAnimPlayer::GetClass( void )
{
	return c_widgetType;
}

//____ SetAnimation() ____________________________________________________________

bool WgAnimPlayer::SetAnimation( WgGfxAnim * pAnim )
{
	m_pAnim			= pAnim;
	m_playPos		= 0.0;

	WgSize	currSize = Size();
	WgSize	wantedSize;

	_requestResize();
	_requestRender();
	return true;
}

//____ SetSource() ________________________________________________________

bool WgAnimPlayer::SetSource( const WgBlocksetPtr& pBlocks )
{
	m_pStaticBlock = pBlocks;

	if( !m_pAnim || !m_bEnabled )
	{
		_requestResize();
		_requestRender();
	}
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
	if( m_pAnim )
		return m_pAnim->Size();
	else if( m_pStaticBlock )
		return m_pStaticBlock->Size();
	else
		return WgSize(0,0);
}

//_____ _playPosUpdated() ______________________________________________________

void WgAnimPlayer::_playPosUpdated()
{
	if( !m_pAnim )
		return;

	WgBlock block = m_pAnim->GetBlock( (int64_t) m_playPos );

	if( block != m_animFrame )
	{
		m_animFrame = block;
		_requestRender();

		_queueEvent( new WgEvent::AnimationUpdate(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->Duration()-1))));
	}
}


//____ _onEvent() ______________________________________________________________

void WgAnimPlayer::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( !m_pAnim || !m_bEnabled )
				return;

			const WgEvent::Tick * pTick = static_cast<const WgEvent::Tick*>(pEvent);
			m_playPos += pTick->Millisec() * m_speed;
			_playPosUpdated();

		}
		break;

		default:
			pHandler->ForwardEvent( pEvent );
		break;
	}
}


//____ _onRender() ________________________________________________________

void WgAnimPlayer::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	if( m_pAnim && m_bEnabled )
		pDevice->ClipBlitBlock( _clip, m_animFrame, _canvas );
	else if( m_pStaticBlock )
	{
		WgMode mode = WG_MODE_NORMAL;
		if( !m_bEnabled )
			mode = WG_MODE_DISABLED;

		pDevice->ClipBlitBlock( _clip, m_pStaticBlock->GetBlock(mode,_canvas.Size()), _canvas );
	}
}

//____ _onRefresh() _______________________________________________________

void WgAnimPlayer::_onRefresh( void )
{
	_requestRender();
}

//____ _onCloneContent() _______________________________________________________

void WgAnimPlayer::_onCloneContent( const WgWidget * _pOrg )
{
	WgAnimPlayer * pOrg = (WgAnimPlayer *) _pOrg;

	m_pAnim				= pOrg->m_pAnim;
	m_animFrame			= pOrg->m_animFrame;
	m_pStaticBlock		= pOrg->m_pStaticBlock;

	m_bPlaying			= pOrg->m_bPlaying;
	m_playPos			= pOrg->m_playPos;
	m_speed				= pOrg->m_speed;
}

//____ _onAlphaTest() ______________________________________________________

bool WgAnimPlayer::_onAlphaTest( const WgCoord& ofs )
{
	WgSize sz = Size();

	if( m_pAnim && m_bEnabled )
		return WgUtil::MarkTestBlock( ofs, m_animFrame, WgRect(0,0,sz), m_markOpacity );
	else if( m_pStaticBlock )
	{
		WgMode mode = WG_MODE_NORMAL;
		if( !m_bEnabled )
			mode = WG_MODE_DISABLED;

		return WgUtil::MarkTestBlock( ofs, m_pStaticBlock->GetBlock(mode,sz), WgRect(0,0,sz), m_markOpacity );
	}
	return false;
}

//____ _onEnable() ___________________________________________________

void WgAnimPlayer::_onEnable( void )
{
	if( m_bPlaying )
		_startReceiveTicks();

	_requestRender();
}

//____ _onDisable() ___________________________________________________

void WgAnimPlayer::_onDisable( void )
{
	if( m_bPlaying )
		_stopReceiveTicks();

	_requestRender();
}
