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
#include <wg_cdynamicvector.impl.h>

#include <math.h>


namespace wg
{
	template class CDynamicVector<AnimFrame>;

	const TypeInfo AnimPlayer::TYPEINFO = { "AnimPlayer", &Widget::TYPEINFO };


	//____ constructor _________________________________________________________________

	AnimPlayer::AnimPlayer() : frames(this)
	{
	}

	//____ ~AnimPlayer() _______________________________________________________

	AnimPlayer::~AnimPlayer()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& AnimPlayer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPlayMode() ____________________________________________________________

	void AnimPlayer::setPlayMode(PlayMode mode)
	{
		bool bWasPingPong = (m_playMode == PlayMode::PingPong || m_playMode == PlayMode::BackwardPingPong);
		bool bIsPingPong = (mode == PlayMode::PingPong || mode == PlayMode::BackwardPingPong);

		if (bWasPingPong && !bIsPingPong)
		{
			m_cycleDuration = frames.duration();
			if (m_playPos >= m_cycleDuration)
				m_playPos = m_cycleDuration * 2 - m_playPos - 1;
		}
		else if (!bWasPingPong && bIsPingPong)
			m_cycleDuration = frames.duration() * 2;

		m_playMode = mode;
		_requestRender();
	}

	//____ setPlayPosFraction() _________________________________________________

	bool AnimPlayer::setPlayPosFraction( float _fraction )
	{
		m_playPos		= int(_fraction * m_cycleDuration);
		return _playPosUpdated();
	}

	//____ setPlayPos() ___________________________________________________________

	bool AnimPlayer::setPlayPos( int _ticks )
	{
		m_playPos		= _ticks;
		return _playPosUpdated();
	}

	//____ rewind() _______________________________________________________________

	bool AnimPlayer::rewind( int _ticks )
	{
		if( m_playPos - _ticks < 0 )
			m_playPos = 0;
		else
			m_playPos -= _ticks;

		return _playPosUpdated();
	}

	//____ fastForward() __________________________________________________________

	bool AnimPlayer::fastForward( int _ticks )
	{
	 	m_playPos += _ticks;
		return _playPosUpdated();
	}

	//____ setSpeed() _____________________________________________________________

	void AnimPlayer::setSpeed( float _speed )
	{
		m_speed = _speed;
	}

	//____ play() _________________________________________________________________

	bool AnimPlayer::play()
	{
		if( m_bPlaying )
			return true;

		if( m_cycleDuration == 0 )
			return false;

		m_bPlaying = true;
		m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
		return true;
	}

	//____ stop() _________________________________________________________________

	void AnimPlayer::stop()
	{
		if( !m_bPlaying )
			return;

		m_bPlaying = false;
		Base::msgRouter()->deleteRoute( m_tickRouteId );
		m_tickRouteId = 0;
	}

	//____ preferredSize() ___________________________________________________________

	Size AnimPlayer::preferredSize() const
	{
		return m_pSkin ? m_pSkin->sizeForContent(frames.frameSize()) : Size(frames.frameSize());
	}

	//____ _receive() ______________________________________________________________

	void AnimPlayer::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );

		switch( pMsg->type() )
		{
			case MsgType::Tick:
			{
				if( m_cycleDuration == 0 || !m_state.isEnabled() )
					return;

				m_playPos += int(static_cast<TickMsg*>(pMsg)->timediff() * m_speed);
				_playPosUpdated();
			}
			break;

			default:
				break;
		}
	}

	//____ _render() ________________________________________________________

	void AnimPlayer::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render( pDevice, _canvas, _window );

		if (m_cycleDuration > 0 && m_state.isEnabled())
		{
			Rect canv = _contentRect();
			pDevice->setBlitSource(frames._surface());
			auto * pFrame = frames.find(_playPosToTimestamp(m_playPos));
			pDevice->stretchFlipBlit(canv, RectI(pFrame->source(), frames.frameSize()), pFrame->flip());
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
	}

	//____ _alphaTest() ______________________________________________________

	bool AnimPlayer::_alphaTest( const Coord& ofs )
	{
		//TODO: Take flip into account!

		if (Widget::_alphaTest(ofs))
			return true;

		if (m_cycleDuration > 0 && m_state.isEnabled())
			return Util::markTestStretchRect(ofs, frames._surface(), RectI(frames.find(_playPosToTimestamp(m_playPos))->source(), frames.frameSize()), Rect(m_size), m_markOpacity);

		return false;
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

	//_____ _playPosUpdated() ______________________________________________________

	bool AnimPlayer::_playPosUpdated()
	{
		if (m_cycleDuration == 0)
		{
			m_playPos = 0;
			return false;
		}

		if (m_playPos >= m_cycleDuration)
		{
			if (m_playMode == PlayMode::Forward || m_playMode == PlayMode::Backward)
			{
				m_playPos = 0;
				stop();
				return false;
			}
			else
				m_playPos %= m_cycleDuration;
		}

		auto pNewFrame = frames.find(_playPosToTimestamp(m_playPos));
		if (pNewFrame->timestamp() != m_frameTimestamp)
		{
			m_frameTimestamp = pNewFrame->timestamp();
			_requestRender();
//			Base::msgRouter()->post(ValueUpdateMsg::create(this, (int)m_playPos, (float)(m_playPos / (m_cycleDuration)), true));
		}
		return true;
	}

	//____ _playPosToTimestamp() __________________________________________________________

	int AnimPlayer::_playPosToTimestamp(int playPos) const
	{
		switch (m_playMode)
		{
		case PlayMode::PingPong:
			if (playPos >= frames.duration())
				playPos = frames.duration() * 2 - (playPos + 1);
			break;

		case PlayMode::BackwardPingPong:
			if (playPos >= frames.duration())
				playPos = frames.duration() * 2 - (playPos + 1);
			// fall through on purpose.
		case PlayMode::Backward:
		case PlayMode::BackwardLooping:
			playPos = frames.duration() - (playPos + 1);
			break;
		}

		return playPos;
	}

	//____ _didSetAnimFrameSize() _____________________________________________

	void AnimPlayer::_didSetAnimFrameSize(CAnimFrames* pComponent)
	{
		_requestResize();
	}

	//____ _didSetAnimSurface() _______________________________________________

	void AnimPlayer::_didSetAnimSurface(CAnimFrames* pComponent)
	{
		_requestRender();
	}

	//____ _didAddEntries() ___________________________________________________

	void AnimPlayer::_didAddEntries(AnimFrame* pEntry, int nb)
	{
		int total = 0;

		for (int i = 0; i < nb; i++)
			total += pEntry[i].duration();

		//TODO: Compensate playPos for change

		if (m_playMode == PlayMode::PingPong || m_playMode == PlayMode::BackwardPingPong)
			total *= 2;
		m_cycleDuration += total;
	}

	//____ _didMoveEntries() ___________________________________________________

	void AnimPlayer::_didMoveEntries(AnimFrame* pFrom, AnimFrame* pTo, int nb)
	{
		//TODO: Compensate playPos for change
	}

	//____ _willEraseEntries() _________________________________________________

	void AnimPlayer::_willEraseEntries(AnimFrame* pEntry, int nb)
	{
		int total = 0;

		for (int i = 0; i < nb; i++)
			total += pEntry[i].duration();

		//TODO: Compensate playPos for change

		if (m_playMode == PlayMode::PingPong || m_playMode == PlayMode::BackwardPingPong)
			total *= 2;
		m_cycleDuration -= total;

	}

	//____ _object() __________________________________________________________

	Object* AnimPlayer::_object()
	{
		return this;
	}


} // namespace wg
