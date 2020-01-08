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

#ifdef SOFTUBE_USE_PACE_FUSION
#include "PaceFusion.h"
#endif

//____ Constructor _________________________________________________________________

WgAnimPlayer::WgAnimPlayer()
{
	m_pAnim			= 0;
	m_playPos		= 0.0;
	m_destinationPos = 0;

	m_bPlaying		= false;
	m_speed			= 1.f;
    
    m_kTintColor    = WgColor( 0xff, 0xff, 0xff, 0xff );
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

#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
bool WgAnimPlayer::SetAnimation( WgGfxAnim * pAnim )
{
	m_pAnim			= pAnim;
	m_playPos		= 0.0;
	_playPosUpdated();

	WgSize	currSize = PixelSize();
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

//____ SetSkin() ________________________________________________________

void WgAnimPlayer::SetSkin( const WgSkinPtr& pSkin )
{
    m_pSkin = pSkin;
    
    if( !m_pAnim || !m_bEnabled )
    {
        _requestResize();
        _requestRender();
    }
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

	m_playPos = _fraction * m_pAnim->Duration();
	_playPosUpdated();
	return true;
}


//____ SetPlayPos() ___________________________________________________________

#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
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

	m_destinationPos = -1;

	m_bPlaying = true;
	_startReceiveTicks();
	return true;
}

//____ PlayToFractional() _________________________________________________________________

bool WgAnimPlayer::PlayToFractional(float fraction)
{
	if (!m_pAnim || fraction < 0.f )
		return false;

	m_destinationPos = (int) (fraction * m_pAnim->Duration() + 0.5f);
	
	m_bPlaying = true;
	_startReceiveTicks();
	return true;
}


//____ PlayTo() _________________________________________________________________

bool WgAnimPlayer::PlayTo( int pos )
{
	if (!m_pAnim || pos < 0 )
		return false;

	m_destinationPos = pos;

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

//____ PreferredPixelSize() ___________________________________________________________

WgSize WgAnimPlayer::PreferredPixelSize() const
{
	if( m_pAnim )
		return m_pAnim->Size(m_scale);
    else if( m_pSkin )
        return m_pSkin->PreferredSize(m_scale);
	else if( m_pStaticBlock )
		return m_pStaticBlock->Size(m_scale);
	else
		return WgSize(0,0);
}

void WgAnimPlayer::SetTint(WgColor kColor)
{
    if((kColor.r == m_kTintColor.r) && (kColor.g == m_kTintColor.g) && (kColor.b == m_kTintColor.b) && (kColor.a == m_kTintColor.a))
        return;
    
    
    m_kTintColor = kColor;
    
    _requestRender();
}
void WgAnimPlayer::RemoveTint() { SetTint(WgColor(0,0,0,0)); }

//_____ _playPosUpdated() ______________________________________________________

void WgAnimPlayer::_playPosUpdated()
{
	if( !m_pAnim )
		return;

	WgBlock block = m_pAnim->GetBlock( (int64_t) m_playPos, m_scale );

	if( block != m_animFrame )
	{
		m_animFrame = block;
		_requestRender();

		_queueEvent( new WgEvent::AnimationUpdate(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->Duration()-1))));
	}
}


//____ _onEvent() ______________________________________________________________

#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
void WgAnimPlayer::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( !m_pAnim || !m_bEnabled )
				return;

			const WgEvent::Tick * pTick = static_cast<const WgEvent::Tick*>(pEvent);

			double posChange = pTick->Millisec() * m_speed;

			if (m_destinationPos >= 0)
			{
				if (m_playPos > m_destinationPos)
				{
					m_playPos -= posChange;
					if (m_playPos < m_destinationPos)
					{
						m_playPos = m_destinationPos;
						Stop();
					}
				}
				else
				{
					m_playPos += posChange;
					if (m_playPos > m_destinationPos)
					{
						m_playPos = m_destinationPos;
						Stop();
					}
				}
			}
			else
			{
				m_playPos += posChange;
			}
			_playPosUpdated();

		}
		break;

		default:
            WgWidget::_onEvent(pEvent, pHandler);
		break;
	}
}


//____ _onRender() ________________________________________________________

#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
void WgAnimPlayer::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
    if( m_kTintColor.argb != 0)
    {
        pDevice->setTintColor( m_kTintColor );
    }
    
	if( m_pAnim && m_bEnabled )
        WgGfxDevice::BlitBlock( pDevice, m_animFrame, _canvas );
    else if( m_pSkin )
    {
        m_pSkin->Render( pDevice, WgStateEnum::Normal, _canvas, m_scale );
    }
    else if( m_pStaticBlock )
	{
		WgMode mode = WG_MODE_NORMAL;
		if( !m_bEnabled )
			mode = WG_MODE_DISABLED;

        WgGfxDevice::BlitBlock( pDevice, m_pStaticBlock->GetBlock(mode,m_scale), _canvas );
	}
    
    // Reset tint color
    pDevice->setTintColor( WgColor( 0xff, 0xff, 0xff, 0xff ) );
}

//____ _onRefresh() _______________________________________________________

#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
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
	WgSize sz = PixelSize();

	if( m_pAnim && m_bEnabled && m_animFrame.IsValid() )
		return WgUtil::MarkTestBlock( ofs, m_animFrame, WgRect(0,0,sz), m_markOpacity );
	else if( m_pStaticBlock )
	{
		WgMode mode = WG_MODE_NORMAL;
		if( !m_bEnabled )
			mode = WG_MODE_DISABLED;

		return WgUtil::MarkTestBlock( ofs, m_pStaticBlock->GetBlock(mode,m_scale), WgRect(0,0,sz), m_markOpacity );
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
