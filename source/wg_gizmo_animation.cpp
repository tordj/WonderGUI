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

#include <wg_gizmo_animation.h>

#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

#ifdef WG_TNG
#include <wg_eventhandler.h>
#endif

#include <math.h>

static const char	c_gizmoType[] = {"Animation"};


//____ Constructor _________________________________________________________________

WgGizmoAnimation::WgGizmoAnimation()
{
	m_pAnim			= 0;
	m_playPos		= 0.0;

	m_bPlaying		= false;
	m_speed			= 1.f;
}

//____ ~WgGizmoAnimation() _______________________________________________________

WgGizmoAnimation::~WgGizmoAnimation()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoAnimation::Type() const
{
	return GetMyType();
}

const char * WgGizmoAnimation::GetMyType( void )
{
	return c_gizmoType;
}

//____ SetAnimation() ____________________________________________________________

bool WgGizmoAnimation::SetAnimation( WgGfxAnim * pAnim )
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

bool WgGizmoAnimation::SetSource( const WgBlockSetPtr& pBlocks )
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

int WgGizmoAnimation::PlayPos()
{
	return (int) m_playPos;
}

//____ SetPlayPosFractional() _________________________________________________

bool WgGizmoAnimation::SetPlayPosFractional( float _fraction )
{
	if( !m_pAnim )
		return false;

	_fraction *= m_pAnim->Duration();

	m_playPos		= _fraction;
	_playPosUpdated();
	return true;
}


//____ SetPlayPos() ___________________________________________________________

bool WgGizmoAnimation::SetPlayPos( int _ticks )
{
	if( !m_pAnim )
		return false;

	m_playPos		= _ticks;
	_playPosUpdated();
	return true;
}



//____ Rewind() _______________________________________________________________

bool WgGizmoAnimation::Rewind( int _ticks )
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

bool WgGizmoAnimation::FastForward( int _ticks )
{
	if( !m_pAnim )
		return false;

 	m_playPos += _ticks;

	_playPosUpdated();
	return true;
}

//____ Duration() _____________________________________________________________

int WgGizmoAnimation::Duration()
{
	if( !m_pAnim )
		return 0;

	return	m_pAnim->DurationScaled();
}

//____ DurationScaled() _______________________________________________________

int WgGizmoAnimation::DurationScaled()
{
	if( !m_pAnim )
		return 0;

	return	(int) (m_pAnim->DurationScaled() / m_speed);
}

//____ Speed() ________________________________________________________________

float WgGizmoAnimation::Speed()
{
	return m_speed;
}

//____ SetSpeed() _____________________________________________________________

bool WgGizmoAnimation::SetSpeed( float _speed )
{
	if( _speed <= 0 || _speed > 1000 )
		return false;

	m_speed = _speed;
	return true;
}

//____ Play() _________________________________________________________________

bool WgGizmoAnimation::Play()
{
	if( !m_pAnim )
		return false;

	m_bPlaying = true;
#ifdef WG_TNG
	_startReceiveTicks();
#endif
	return true;
}

//____ Stop() _________________________________________________________________

bool WgGizmoAnimation::Stop()
{
	m_bPlaying = false;
#ifdef WG_TNG
	_stopReceiveTicks();
#endif
	return true;
}

//____ DefaultSize() ___________________________________________________________

WgSize WgGizmoAnimation::DefaultSize() const
{
	if( m_pAnim )
		return m_pAnim->Size();
	else if( m_pStaticBlock )
		return m_pStaticBlock->Size();
	else
		return WgSize(0,0);
}

//_____ _playPosUpdated() ______________________________________________________

void WgGizmoAnimation::_playPosUpdated()
{
	if( !m_pAnim )
		return;

	WgBlock block = m_pAnim->GetBlock( (int64_t) m_playPos );

	if( block != m_animFrame )
	{
		m_animFrame = block;
		_requestRender();

		Emit( WgSignal::IntegerChanged(), static_cast<int>(m_playPos) );
		Emit( WgSignal::Fraction(), (float)m_playPos/(float)m_pAnim->Duration()-1);
#ifdef WG_TGN
		EventHandler()->QueueEvent( new WgEvent::AnimationUpdate(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->Duration()-1))));
#endif
	}
}


//____ _onEvent() ______________________________________________________________

#ifdef WG_TNG
void WgGizmoAnimation::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
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
#endif

//____ _onUpdate() ________________________________________________________

void WgGizmoAnimation::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( !m_pAnim && !m_bEnabled )
		return;

	if( m_bPlaying )
	{
		m_playPos += _updateInfo.msDiff * m_speed;
		_playPosUpdated();
	}
}


//____ _onRender() ________________________________________________________

void WgGizmoAnimation::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
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

void WgGizmoAnimation::_onRefresh( void )
{
	_requestRender();
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoAnimation::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoAnimation * pOrg = (WgGizmoAnimation *) _pOrg;

	m_pAnim				= pOrg->m_pAnim;
	m_animFrame			= pOrg->m_animFrame;
	m_pStaticBlock		= pOrg->m_pStaticBlock;

	m_bPlaying			= pOrg->m_bPlaying;
	m_playPos			= pOrg->m_playPos;
	m_speed				= pOrg->m_speed;
}

//____ _onAlphaTest() ______________________________________________________

bool WgGizmoAnimation::_onAlphaTest( const WgCoord& ofs )
{
	WgSize sz = Size();

	if( m_pAnim && m_bEnabled )
		return WgUtil::MarkTestBlock( ofs, m_animFrame, WgRect(0,0,sz) );
	else if( m_pStaticBlock )
	{
		WgMode mode = WG_MODE_NORMAL;
		if( !m_bEnabled )
			mode = WG_MODE_DISABLED;

		return WgUtil::MarkTestBlock( ofs, m_pStaticBlock->GetBlock(mode,sz), WgRect(0,0,sz) );
	}
	return false;
}

//____ _onEnable() ___________________________________________________

void WgGizmoAnimation::_onEnable( void )
{
	if( m_bPlaying )
	{
#ifdef WG_TNG
		_startReceiveTicks();
#endif
	}
	_requestRender();
}

//____ _onDisable() ___________________________________________________

void WgGizmoAnimation::_onDisable( void )
{
	if( m_bPlaying )
	{
#ifdef WG_TNG
		_stopReceiveTicks();
#endif
	}
	_requestRender();
}
