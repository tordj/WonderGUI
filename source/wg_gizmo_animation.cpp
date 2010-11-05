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

#include <math.h>


static const char	c_gizmoType[] = {"TordJ/Animation"};




//____ Constructor _________________________________________________________________

WgGizmoAnimation::WgGizmoAnimation()
{
	m_pSurf			= 0;
	m_src			= WgRect( 0,0,0,0 );

	m_pDisableSurf	= 0;
	m_dis_src		= WgRect( 0,0,0,0 );

	m_pAnim			= 0;
	m_playPos		= 0.0;

	m_bPlayPosIsNew = true;
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

//____ SetSource() ____________________________________________________________

bool WgGizmoAnimation::SetSource( WgGfxAnim * _pAnim )
{
	m_pAnim			= _pAnim;
	m_playPos		= 0.0;
	m_bPlayPosIsNew = true;

	m_src.w			= _pAnim->width();
	m_src.h			= _pAnim->height();

	return true;
}

//____ DisableSource() ________________________________________________________

bool WgGizmoAnimation::DisableSource( WgSurface * _pSurf, WgRect& _rect )
{
	if( _rect.w == 0 || _rect.h == 0 )
		return false;
	
	m_pDisableSurf	= _pSurf;
	m_dis_src		= _rect;

	if( !m_bEnabled )
	{
		if( _pSurf->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		RequestRender();
	}
	return true;
}

//____ PlayPos() ______________________________________________________________

Uint32 WgGizmoAnimation::PlayPos()
{
	return (Uint32) m_playPos;
}

//____ SetPlayPosFractional() _________________________________________________

bool WgGizmoAnimation::SetPlayPosFractional( float _fraction )
{
	if( !m_pAnim )
		return false;
	
	_fraction *= m_pAnim->Duration();

	
	if( _fraction == m_playPos )
	{
		m_bPlayPosIsNew = true;				// So it doesn't step forward in update before render.
		return true;
	}

	m_playPos		= _fraction;
	m_bPlayPosIsNew	= true;

	return true;
}


//____ SetPlayPos() ___________________________________________________________

bool WgGizmoAnimation::SetPlayPos( Uint32 _ticks )
{
	if( !m_pAnim )
		return false;

	if( _ticks == m_playPos )
	{
		m_bPlayPosIsNew = true;					// So it doesn't step forward in update before render.
		return true;
	}

	m_playPos		= _ticks;
	m_bPlayPosIsNew	= true;

	return true;
}



//____ Rewind() _______________________________________________________________

void WgGizmoAnimation::Rewind( Uint32 _ticks )
{
	if( m_playPos - _ticks < 0 )
		m_playPos = 0;
	else
		m_playPos -= _ticks;

	m_bPlayPosIsNew = true;

	// Do NOT request render here, that is done by next update if necessary.
}

//____ FastForward() __________________________________________________________

void WgGizmoAnimation::FastForward( Uint32 _ticks )
{
	m_playPos += _ticks;
	m_bPlayPosIsNew = true;

	// Do NOT request render here, that is done by next update if necessary.
}

//____ Duration() _____________________________________________________________

Uint32 WgGizmoAnimation::Duration()
{
	if( !m_pAnim )
		return 0;

	return	m_pAnim->DurationScaled();
}

//____ DurationScaled() _______________________________________________________

Uint32 WgGizmoAnimation::DurationScaled()
{
	if( !m_pAnim )
		return 0;

	return	(Uint32) (m_pAnim->DurationScaled() / m_speed);
}

//____ Speed() ________________________________________________________________

float WgGizmoAnimation::Speed()
{
	return m_speed;
}

//____ SetSpeed() _____________________________________________________________

bool WgGizmoAnimation::SetSpeed( float _speed )
{
	if( _speed <= 0 || _speed > 1000000 )
		return false;

	m_speed = _speed;
	return true;
}

//____ Play() _________________________________________________________________

bool WgGizmoAnimation::Play()
{
	if( !m_pAnim || m_bPlaying )
		return false;

	m_bPlaying = true;
	m_bPlayPosIsNew = true;
	return true;
}

//____ Stop() _________________________________________________________________

bool WgGizmoAnimation::Stop()
{
	if( !m_bPlaying )
		return false;

	m_bPlaying = false;
	return true;
}

//____ OnUpdate() ________________________________________________________

void WgGizmoAnimation::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( !m_pAnim || !m_bEnabled )
		return;

	if( !m_bPlayPosIsNew && m_bPlaying )
	{
		m_playPos += _updateInfo.msDiff * m_speed;
	}

	// Emit signals if playPos somehow has changed.

	if( m_bPlayPosIsNew || m_bPlaying )
	{
		Emit( WgSignal::IntegerChanged(), static_cast<int>(m_playPos) );
		Emit( WgSignal::Fraction(), (float)m_playPos/(float)m_pAnim->Duration());
	}


	WgGfxFrame * pFrame = m_pAnim->getFrame( (Uint32) m_playPos );

	if( pFrame->pSurf != m_pSurf )
	{
		m_pSurf = pFrame->pSurf;
		if( m_pSurf && m_pSurf->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;			

		m_src.x = pFrame->ofs.x;
		m_src.y = pFrame->ofs.y;
		RequestRender();
	}
	else if( pFrame->ofs.x != m_src.x || pFrame->ofs.y != m_src.y )
	{
		m_src.x = pFrame->ofs.x;
		m_src.y = pFrame->ofs.y;
		RequestRender();
	}	

	m_bPlayPosIsNew = false;
}


//____ OnRender() ________________________________________________________

void WgGizmoAnimation::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( !m_bEnabled && m_pDisableSurf )
	{
   		pDevice->ClipTileBlit( _clip, m_pDisableSurf, m_dis_src, _canvas );
	}
	else if( m_pSurf != 0 )
	{
		pDevice->ClipTileBlit( _clip, m_pSurf, m_src, _canvas );
	}
}

//____ OnRefresh() _______________________________________________________

void WgGizmoAnimation::OnRefresh( void )
{

	if( m_bEnabled && m_pSurf )
	{
		if( m_pSurf && m_pSurf->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		RequestRender();
	}
	else if( m_pDisableSurf )
	{
		if( m_pDisableSurf && m_pDisableSurf->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		RequestRender();
	}
}

//____ DoMyOwnCloning() _______________________________________________________

void WgGizmoAnimation::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoAnimation * pOrg = (WgGizmoAnimation *) _pOrg;

	m_pSurf				= pOrg->m_pSurf;
	m_src				= pOrg->m_src;
	m_pDisableSurf		= pOrg->m_pDisableSurf;
	m_dis_src			= pOrg->m_dis_src;
	m_pAnim				= pOrg->m_pAnim;
	m_playPos			= pOrg->m_playPos;
	m_bPlayPosIsNew		= pOrg->m_bPlayPosIsNew;
	m_bPlaying			= pOrg->m_bPlaying;
	m_speed				= pOrg->m_speed;

}

//____ OnAlphaTest() ______________________________________________________

bool WgGizmoAnimation::OnAlphaTest( const WgCord& ofs )
{
	if( m_bEnabled && m_pSurf )
	{
  	if( m_pSurf->GetOpacity( m_src.x + (ofs.x % m_src.w), m_src.y + (ofs.y % m_src.h) ) )
			return true;
	}
	else if( m_pDisableSurf )
	{
  	if( m_pDisableSurf->GetOpacity( m_dis_src.x + (ofs.x % m_dis_src.w), m_dis_src.y + (ofs.y % m_dis_src.h) ) )
			return true;
	}

	return false;
}

//____ OnEnable() ___________________________________________________

void WgGizmoAnimation::OnEnable( void )
{
	if( m_pSurf && m_pSurf->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;
		
	RequestRender();
}

//____ OnDisable() ___________________________________________________

void WgGizmoAnimation::OnDisable( void )
{
	if( m_pDisableSurf && m_pDisableSurf->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	RequestRender();
}
