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

#include <wg_anim.h>

const char WgAnim::CLASSNAME[] = {"Anim"};

//____ WgAnim() _______________________________________________________________

WgAnim::WgAnim()
{ 
	m_scale			= 1.f;
	m_duration		= 0;
	m_playMode	= WG_FORWARD_ONCE;
}

//____ ~WgAnim() ______________________________________________________________

WgAnim::~WgAnim()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgAnim::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgAnim::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgAnimPtr WgAnim::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgAnimPtr( static_cast<WgAnim*>(pObject.rawPtr()) );

	return 0;
}


//____ setPlayMode() __________________________________________________________

bool WgAnim::setPlayMode( WgAnimMode mode )
{
	m_playMode = mode;
	return true;
}

//____ setTimeScaler() ________________________________________________________

bool WgAnim::setTimeScaler( float scale )
{
	if( scale <= 0.f )
		return false;
	
	m_scale = scale;
	return true;
}


//____ deleteKeyFrame() _______________________________________________________

bool WgAnim::deleteKeyFrame( int pos )
{
	return false;
}

//____ deleteKeyFrame() _______________________________________________________

bool WgAnim::deleteKeyFrame( WgKeyFrame * pKeyFrame )
{
	delete pKeyFrame;
	return true;
}


//____ _insertKeyFrame() _______________________________________________________

bool WgAnim::_insertKeyFrame( int pos, WgKeyFrame * pFrame, int duration )
{
	WgKeyFrame * pPos = m_keyframes.first();

	for( int i = 0 ; i < pos ; i++ )
	{
		pPos = pPos->next();
		if( pPos == 0 )
			return false;
	}

	return _insertKeyFrame( pPos, pFrame, duration );
}


bool WgAnim::_insertKeyFrame( WgKeyFrame * pPos, WgKeyFrame * pFrame, int duration )
{
	if( pFrame->m_duration == 0 || pPos->chain() != &m_keyframes || m_duration + pFrame->m_duration < m_duration )
		return false;
	
	pFrame->moveBefore( pPos );
	pFrame->m_timestamp = pPos->m_timestamp;
	pFrame->m_duration = duration;

	while( pPos != 0 )
	{
		pPos->m_timestamp += duration;
		pPos = pPos->next();
	}
	m_duration += duration; 
	return true;
}


//____ _addKeyFrame() __________________________________________________________

bool WgAnim::_addKeyFrame( WgKeyFrame * pFrame, int duration )
{
	if( duration == 0 || m_duration + duration < m_duration )
		return false;

	pFrame->m_duration = duration;
	pFrame->moveLast( &m_keyframes );
	pFrame->m_timestamp = m_duration;
	m_duration += pFrame->m_duration;
	return true;
}

//____ _playPos() ___________________________________________________________

WgAnimPlayPos WgAnim::_playPos( int64_t ticks, WgKeyFrame * pProximity ) const
{
	ticks = (int64_t)(ticks * ((double)m_scale));
	
	WgAnimPlayPos	pos;

	switch( m_playMode )
	{
		case WG_FORWARD_ONCE:
		{
			if( ticks >= m_duration )
			{
				pos.pKeyFrame1		= m_keyframes.last();
				pos.pKeyFrame2		= m_keyframes.last();
				pos.transition		= 1.f;													// 1.f only when it has stopped.
				pos.animOffset		= m_duration;
			}
			else
			{
				WgKeyFrame * pFrame = _keyFrame( ticks, pProximity );
				pos.pKeyFrame1			= pFrame;
				pos.transition			= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= (int) ticks;
				pos.pKeyFrame2			= pFrame->next();

				if( pos.pKeyFrame2 == 0 )
					pos.pKeyFrame2		= m_keyframes.last();
			}
			break;
		}

		case WG_BACKWARD_ONCE:
		{
			if( ticks >= m_duration )
			{
				pos.pKeyFrame1			= m_keyframes.first();
				pos.pKeyFrame2			= m_keyframes.first();
				pos.transition			= 1.f;											// 1.f only when it has stopped.
				pos.animOffset			= 0;
			}
			else
			{
				WgKeyFrame * pFrame = _keyFrame( m_duration - ticks, pProximity );
				pos.pKeyFrame2			= pFrame;
				pos.transition			= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= m_duration - (int) ticks;
				pos.pKeyFrame1			= pFrame->next();

				if( pos.pKeyFrame1 == 0 )
					pos.pKeyFrame1		= m_keyframes.last();
			}
			break;
		}
	
		case WG_FORWARD_LOOPING:
		{
			ticks = ticks % m_duration;

			WgKeyFrame * pFrame		= _keyFrame( ticks, pProximity );
			pos.pKeyFrame1			= pFrame;
			pos.transition			= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
			pos.animOffset			= (int) ticks;
			pos.pKeyFrame2			= pFrame->next();

			if( pos.pKeyFrame2 == 0 )
				pos.pKeyFrame2		= m_keyframes.first();
			break;
		}

		case WG_BACKWARD_LOOPING:
		{
			ticks = ticks % m_duration;

			WgKeyFrame * pFrame =	_keyFrame( m_duration - ticks, pProximity );
			pos.pKeyFrame2			= pFrame;
			pos.transition			= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
			pos.animOffset			= m_duration - (int) ticks;
			pos.pKeyFrame1			= pFrame->next();

			if( pos.pKeyFrame1 == 0 )
				pos.pKeyFrame1		= m_keyframes.first();
			break;
		}


		case WG_FORWARD_PINGPONG:
		{
			int64_t	ts;

			if( ((ticks/m_duration) % 2) == 0 )
			{
				ts = ticks % m_duration;
				WgKeyFrame * pFrame		= _keyFrame( ts, pProximity );
				pos.pKeyFrame1			= pFrame;
				pos.transition	= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= (int) ts;
				pos.pKeyFrame2			= pFrame->next();

				if( pos.pKeyFrame2 == 0 )
					pos.pKeyFrame2		= m_keyframes.last();
			}
			else
			{
				ts = m_duration - (ticks % m_duration);

				WgKeyFrame * pFrame		= _keyFrame( ts, pProximity );
				pos.pKeyFrame2			= pFrame;
				pos.transition			= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= (int) ts;
				pos.pKeyFrame1			= pFrame->next();

				if( pos.pKeyFrame1 == 0 )
					pos.pKeyFrame1		= m_keyframes.last();
			
			}
			break;
		}

		case WG_BACKWARD_PINGPONG:
		{
			int64_t		ts;

			if( ((ticks/m_duration) % 2) == 1 )
			{
				ts = ticks % m_duration;
				WgKeyFrame * pFrame		= _keyFrame( ts, pProximity );
				pos.pKeyFrame1			= pFrame;
				pos.transition	= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= (int) ts;
				pos.pKeyFrame2			= pFrame->next();

				if( pos.pKeyFrame2 == 0 )
					pos.pKeyFrame2		= m_keyframes.last();
			}
			else
			{
				ts = m_duration - (ticks % m_duration);

				WgKeyFrame * pFrame		= _keyFrame( ts, pProximity );
				pos.pKeyFrame2			= pFrame;
				pos.transition	= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= (int) ts;
				pos.pKeyFrame1			= pFrame->next();

				if( pos.pKeyFrame1 == 0 )
					pos.pKeyFrame1		= m_keyframes.last();			
			}
			break;
		}
	}
	return pos;
}

//____ _keyFrame() __________________________________________________________

WgKeyFrame * WgAnim::_keyFrame( int64_t ticks, WgKeyFrame * pProximity ) const
{
	// If proximity isn't set, we'll start from first or last depending
	// on which we belive is closest.


	if( !pProximity )
	{
		if( ticks < m_duration - ticks )
			pProximity = m_keyframes.first();
		else
			pProximity = m_keyframes.last();
	}


	// Step forward or backward until we find the right keyframe

	if( pProximity->m_timestamp > ticks )
	{
		pProximity = pProximity->prev();
		while( pProximity->m_timestamp > ticks )
			pProximity = pProximity->prev();

	}
	else
	{
		while( pProximity->m_timestamp + pProximity->m_duration <= ticks )
			pProximity = pProximity->next();
	}
	
	return pProximity;
}



//____ timeToOfs() _____________________________________________________________

int WgAnim::timeToOfs( int64_t ticks )
{
	ticks = (int64_t)(ticks * ((double)m_scale));
	
	switch( m_playMode )
	{
		case WG_FORWARD_ONCE:
			if( ticks >= m_duration )
				return m_duration;

			return (int) ticks;

		case WG_BACKWARD_ONCE:
			if( ticks >= m_duration )
				return 0;

			return m_duration - (int)ticks;
	
		case WG_FORWARD_LOOPING:
			return (int) (ticks % m_duration);

		case WG_BACKWARD_LOOPING:
			return m_duration - (int)(ticks % m_duration);


		case WG_FORWARD_PINGPONG:
			if( ((ticks/m_duration) % 2) == 0 )
				return (int) (ticks % m_duration);
			else
				return m_duration - (int)(ticks % m_duration);

		case WG_BACKWARD_PINGPONG:
			if( ((ticks/m_duration) % 2) == 1 )
				return (int) (ticks % m_duration);
			else
				return m_duration - (int)(ticks % m_duration);

	
		default:
			return 0;
	}
}




//____ WgKeyFrame::setDuration() ______________________________________________

bool WgKeyFrame::setDuration( int ticks )
{
	
	// Ugly hack to get a pointer to our WgAnim object.

	WgChain<WgKeyFrame> * pChain = chain();
	WgAnim * pAnim = 0;
	char * ofs = (char *) &(pAnim->m_keyframes);
	pAnim = (WgAnim *)(((char *)pChain) - ofs);

	//

	int	change = ticks - m_duration;

	if( pAnim->m_duration + change < pAnim->m_duration )
		return false;
	
	pAnim->m_duration += change;

	m_duration = ticks;

	WgKeyFrame * pFrame = next();
	while( pFrame )
	{
		pFrame->m_duration += change;
		pFrame = pFrame->next();
	}
	return true;
}
