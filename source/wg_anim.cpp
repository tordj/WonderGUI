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

//____ SetPlayMode() __________________________________________________________

bool WgAnim::SetPlayMode( WgAnimMode mode )
{
	m_playMode = mode;
	return true;
}

//____ SetTimeScaler() ________________________________________________________

bool WgAnim::SetTimeScaler( float scale )
{
	if( scale <= 0.f )
		return false;
	
	m_scale = scale;
	return true;
}


//____ DeleteKeyFrame() _______________________________________________________

bool WgAnim::DeleteKeyFrame( Uint32 pos )
{
	return false;
}

//____ DeleteKeyFrame() _______________________________________________________

bool WgAnim::DeleteKeyFrame( WgKeyFrame * pKeyFrame )
{
	delete pKeyFrame;
	return true;
}


//____ InsertKeyFrame() _______________________________________________________

bool WgAnim::InsertKeyFrame( Uint32 pos, WgKeyFrame * pFrame, Uint32 duration )
{
	WgKeyFrame * pPos = m_keyframes.First();

	for( Uint32 i = 0 ; i < pos ; i++ )
	{
		pPos = pPos->Next();
		if( pPos == 0 )
			return false;
	}

	return InsertKeyFrame( pPos, pFrame, duration );
}


bool WgAnim::InsertKeyFrame( WgKeyFrame * pPos, WgKeyFrame * pFrame, Uint32 duration )
{
	if( pFrame->m_duration == 0 || pPos->Chain() != &m_keyframes || m_duration + pFrame->m_duration < m_duration )
		return false;
	
	pFrame->MoveBefore( pPos );
	pFrame->m_timestamp = pPos->m_timestamp;
	pFrame->m_duration = duration;

	while( pPos != 0 )
	{
		pPos->m_timestamp += duration;
		pPos = pPos->Next();
	}
	m_duration += duration; 
	return true;
}


//____ AddKeyFrame() __________________________________________________________

bool WgAnim::AddKeyFrame( WgKeyFrame * pFrame, Uint32 duration )
{
	if( duration == 0 || m_duration + duration < m_duration )
		return false;

	pFrame->m_duration = duration;
	pFrame->MoveLast( &m_keyframes );
	pFrame->m_timestamp = m_duration;
	m_duration += pFrame->m_duration;
	return true;
}

//____ PlayPos() ___________________________________________________________

WgAnimPlayPos WgAnim::PlayPos( Uint32 ticks, WgKeyFrame * pProximity ) const
{
	ticks *= (Uint32) m_scale;
	
	WgAnimPlayPos		pos;

	switch( m_playMode )
	{
		case WG_FORWARD_ONCE:
		{
			if( ticks >= m_duration )
			{
				pos.pKeyFrame1		= m_keyframes.Last();
				pos.pKeyFrame2		= m_keyframes.Last();
				pos.transition		= 1.f;													// 1.f only when it has stopped.
				pos.animOffset		= m_duration;
			}
			else
			{
				WgKeyFrame * pFrame = KeyFrame( ticks, pProximity );
				pos.pKeyFrame1			= pFrame;
				pos.transition			= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= m_duration;
				pos.pKeyFrame2			= pFrame->Next();

				if( pos.pKeyFrame2 == 0 )
					pos.pKeyFrame2		= m_keyframes.Last();
			}
			break;
		}

		case WG_BACKWARD_ONCE:
		{
			if( ticks >= m_duration )
			{
				pos.pKeyFrame1			= m_keyframes.First();
				pos.pKeyFrame2			= m_keyframes.First();
				pos.transition			= 1.f;											// 1.f only when it has stopped.
				pos.animOffset			= 0;
			}
			else
			{
				WgKeyFrame * pFrame = KeyFrame( m_duration - ticks, pProximity );
				pos.pKeyFrame2			= pFrame;
				pos.transition			= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= m_duration - ticks;
				pos.pKeyFrame1			= pFrame->Next();

				if( pos.pKeyFrame1 == 0 )
					pos.pKeyFrame1		= m_keyframes.Last();
			}
			break;
		}
	
		case WG_FORWARD_LOOPING:
		{
			ticks = ticks % m_duration;

			WgKeyFrame * pFrame		= KeyFrame( ticks, pProximity );
			pos.pKeyFrame1			= pFrame;
			pos.transition			= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
			pos.animOffset			= ticks;
			pos.pKeyFrame2			= pFrame->Next();

			if( pos.pKeyFrame2 == 0 )
				pos.pKeyFrame2		= m_keyframes.First();
			break;
		}

		case WG_BACKWARD_LOOPING:
		{
			ticks = ticks % m_duration;

			WgKeyFrame * pFrame =	KeyFrame( m_duration - ticks, pProximity );
			pos.pKeyFrame2			= pFrame;
			pos.transition			= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
			pos.animOffset			= m_duration - ticks;
			pos.pKeyFrame1			= pFrame->Next();

			if( pos.pKeyFrame1 == 0 )
				pos.pKeyFrame1		= m_keyframes.First();
			break;
		}


		case WG_FORWARD_PINGPONG:
		{
			Uint32	ts;

			if( ((ticks/m_duration) % 2) == 0 )
			{
				ts = ticks % m_duration;
				WgKeyFrame * pFrame		= KeyFrame( ts, pProximity );
				pos.pKeyFrame1			= pFrame;
				pos.transition	= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= ts;
				pos.pKeyFrame2			= pFrame->Next();

				if( pos.pKeyFrame2 == 0 )
					pos.pKeyFrame2		= m_keyframes.Last();
			}
			else
			{
				ts = m_duration - (ticks % m_duration);

				WgKeyFrame * pFrame		= KeyFrame( ts, pProximity );
				pos.pKeyFrame2			= pFrame;
				pos.transition			= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= ts;
				pos.pKeyFrame1			= pFrame->Next();

				if( pos.pKeyFrame1 == 0 )
					pos.pKeyFrame1		= m_keyframes.Last();
			
			}
			break;
		}

		case WG_BACKWARD_PINGPONG:
		{
			Uint32	ts;

			if( ((ticks/m_duration) % 2) == 1 )
			{
				ts = ticks % m_duration;
				WgKeyFrame * pFrame		= KeyFrame( ts, pProximity );
				pos.pKeyFrame1			= pFrame;
				pos.transition	= (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= ts;
				pos.pKeyFrame2			= pFrame->Next();

				if( pos.pKeyFrame2 == 0 )
					pos.pKeyFrame2		= m_keyframes.Last();
			}
			else
			{
				ts = m_duration - (ticks % m_duration);

				WgKeyFrame * pFrame		= KeyFrame( ts, pProximity );
				pos.pKeyFrame2			= pFrame;
				pos.transition	= 1.f - (ticks-pFrame->m_timestamp) / (float) pFrame->m_duration;
				pos.animOffset			= ts;
				pos.pKeyFrame1			= pFrame->Next();

				if( pos.pKeyFrame1 == 0 )
					pos.pKeyFrame1		= m_keyframes.Last();			
			}
			break;
		}
	}
	return pos;
}

//____ KeyFrame() __________________________________________________________

WgKeyFrame * WgAnim::KeyFrame( Uint32 ticks, WgKeyFrame * pProximity ) const
{
	// If proximity isn't set, we'll start from first or last depending
	// on which we belive is closest.


	if( !pProximity )
	{
		if( ticks < m_duration - ticks )
			pProximity = m_keyframes.First();
		else
			pProximity = m_keyframes.Last();
	}


	// Step forward or backward until we find the right keyframe

	if( pProximity->m_timestamp > ticks )
	{
		pProximity = pProximity->Prev();
		while( pProximity->m_timestamp > ticks )
			pProximity = pProximity->Prev();

	}
	else
	{
		while( pProximity->m_timestamp + pProximity->m_duration <= ticks )
			pProximity = pProximity->Next();
	}
	
	return pProximity;
}



//____ TimeToOfs() _____________________________________________________________

Uint32 WgAnim::TimeToOfs( Uint32 ticks )
{
	ticks *= (Uint32) m_scale;
	
	switch( m_playMode )
	{
		case WG_FORWARD_ONCE:
			if( ticks >= m_duration )
				return m_duration;

			return ticks;

		case WG_BACKWARD_ONCE:
			if( ticks >= m_duration )
				return 0;

			return m_duration - ticks;
	
		case WG_FORWARD_LOOPING:
			return ticks % m_duration;

		case WG_BACKWARD_LOOPING:
			return m_duration - (ticks % m_duration);


		case WG_FORWARD_PINGPONG:
			if( ((ticks/m_duration) % 2) == 0 )
				return ticks % m_duration;
			else
				return m_duration - (ticks % m_duration);

		case WG_BACKWARD_PINGPONG:
			if( ((ticks/m_duration) % 2) == 1 )
				return ticks % m_duration;
			else
				return m_duration - (ticks % m_duration);

	
		default:
			return 0;
	}
}




//____ WgKeyFrame::SetDuration() ______________________________________________

bool WgKeyFrame::SetDuration( Uint32 ticks )
{
	
	// Ugly hack to get a pointer to our WgAnim object.

	WgChain<WgKeyFrame> * pChain = Chain();
	WgAnim * pAnim = 0;
	char * ofs = (char *) &(pAnim->m_keyframes);
	pAnim = (WgAnim *)(((char *)pChain) - ofs);

	//

	int	change = ticks - m_duration;

	if( pAnim->m_duration + change < pAnim->m_duration )
		return false;
	
	pAnim->m_duration += change;

	m_duration = ticks;

	WgKeyFrame * pFrame = Next();
	while( pFrame )
	{
		pFrame->m_duration += change;
		pFrame = pFrame->Next();
	}
	return true;
}
