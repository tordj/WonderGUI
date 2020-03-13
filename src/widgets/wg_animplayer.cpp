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
#include <wg_util.h>
#include <wg_msgrouter.h>
#include <wg_gfxdevice.h>

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

	//____ setAnimation() ____________________________________________________________

	bool AnimPlayer::setAnimation( GfxAnim * pAnim )
	{
		m_pAnim			= pAnim;
		m_playPos		= 0.0;

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
		if( m_bPlaying )
			return true;

		if( !m_pAnim )
			return false;

		m_bPlaying = true;
		m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
		return true;
	}

	//____ stop() _________________________________________________________________

	bool AnimPlayer::stop()
	{
		if( !m_bPlaying )
			return true;

		m_bPlaying = false;
		Base::msgRouter()->deleteRoute( m_tickRouteId );
		m_tickRouteId = 0;
		return true;
	}

	//____ _preferredSize() ___________________________________________________________

	SizeI AnimPlayer::_preferredSize() const
	{
		SizeI	sz;

		if( m_pAnim )
			sz = m_pAnim->size();

		if( m_pSkin )
			sz = m_pSkin->_sizeForContent(sz);

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

			Base::msgRouter()->post( ValueUpdateMsg::create(this, (int)m_playPos, (float) (m_playPos/(m_pAnim->duration()-1)),true));
		}
	}


	//____ _receive() ______________________________________________________________

	void AnimPlayer::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );

		switch( pMsg->type() )
		{
			case MsgType::Tick:
			{
				if( !m_pAnim || !m_state.isEnabled() )
					return;

				m_playPos += static_cast<TickMsg*>(pMsg)->timediff() * m_speed;
				_playPosUpdated();

			}
			break;

			default:
				break;
		}
	}


	//____ _render() ________________________________________________________

	void AnimPlayer::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render( pDevice, _canvas, _window );

		if (m_pAnim && m_state.isEnabled())
		{
			pDevice->setBlitSource(m_pAnimFrame->pSurf);
			pDevice->stretchBlit(_canvas, m_pAnimFrame->rect );
		}
	}

	//____ _refresh() _______________________________________________________

	void AnimPlayer::_refresh( void )
	{
		Widget::_refresh();
	}

	//____ _cloneContent() _______________________________________________________

	void AnimPlayer::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		AnimPlayer * pOrg = (AnimPlayer *) _pOrg;

		m_pAnim				= pOrg->m_pAnim;
		m_pAnimFrame		= pOrg->m_pAnimFrame;
		m_pSkin				= pOrg->m_pSkin;

		m_bPlaying			= pOrg->m_bPlaying;
		m_playPos			= pOrg->m_playPos;
		m_speed				= pOrg->m_speed;
	}

	//____ _alphaTest() ______________________________________________________

	bool AnimPlayer::_alphaTest( const CoordI& ofs )
	{
		if( m_pAnim && m_state.isEnabled() && Util::markTestStretchRect( ofs, m_pAnimFrame->pSurf, m_pAnimFrame->rect, RectI(0,0,m_size), m_markOpacity ) )
			return true;

		return Widget::_alphaTest(ofs);
	}

	//____ _setState() ______________________________________________________

	void AnimPlayer::_setState( State state )
	{
		if( state.isEnabled() != m_state.isEnabled() && m_bPlaying )
		{
			if( state.isEnabled() )
				m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
			else
			{
				Base::msgRouter()->deleteRoute( m_tickRouteId );
				m_tickRouteId = 0;
			}
			_requestRender();
		}

		Widget::_setState(state);
	}


} // namespace wg
