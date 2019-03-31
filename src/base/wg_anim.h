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

#ifndef WG_ANIM_DOT_H
#define WG_ANIM_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_chain.h>
#include <wg_pointers.h>

namespace wg 
{
	
	class		KeyFrame;
	struct	AnimPlayPos;
	
	
	//____ Class KeyFrame _______________________________________________________
	
	class KeyFrame : public Link
	{
		friend class Anim;
	public:
	
		//.____ Misc ___________________________________________________________
	
		bool	setDuration( int ticks );
		int		duration( void ) { return m_duration; };
		int		timestamp( void ) { return m_timestamp; };
	
	protected:
		int		m_timestamp;
		int		m_duration;
		LINK_METHODS( KeyFrame );
	};
	
	class Anim;
	typedef	StrongPtr<Anim>		Anim_p;
	typedef	WeakPtr<Anim>	Anim_wp;
	
	//____ Class Anim ______________________________________________________
	
	class Anim : public Object
	{
		friend class KeyFrame;
	
	public:

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Anim_p	cast( Object * pObject );
	
		//.____ Misc ________________________________________________

		bool				setPlayMode( AnimMode mode );
		bool				setTimeScaler( float scale );
	
		bool				deleteKeyFrame( int pos );
		bool				deleteKeyFrame( KeyFrame * pKeyFrame );
		void				clear( void ) { m_keyframes.clear(); };
	
		int					duration( void ) { return m_duration; };
		AnimMode			playMode( void ) { return m_playMode; };
		float				timeScaler( void ) { return m_scale; };
		int					durationScaled( void ) { return (int) (m_duration * m_scale); };
		int					timeToOfs( int64_t ticks );			/// Convert play-time to offset in animation by scaling with timeScaler and unwinding loops.
	
	protected:
		Anim();
		virtual ~Anim();
	
		// Meant to be overloaded with methods by the same name that builds up their 
		// KeyFrame-derived class.
	
		bool				_insertKeyFrame( int pos, KeyFrame * pFrame, int duration );
		bool				_insertKeyFrame( KeyFrame * pBefore, KeyFrame * pFrame, int duration );
		bool				_addKeyFrame( KeyFrame * pFrame, int duration );
	
		// Meant to be overloaded with public methods returning right type.
	
		KeyFrame *		_firstKeyFrame( void ) {return m_keyframes.first();};
		KeyFrame *		_lastKeyFrame( void ) {return m_keyframes.last();};
	
		//
	
		AnimPlayPos		_playPos( int64_t ticks, KeyFrame * pProximity = 0 ) const;		// ticks gets scaled.
	
		KeyFrame *		_keyFrame( int64_t ticks, KeyFrame * pProximity = 0 ) const;
	
	private:
		
		float				m_scale;			// Only used for getKeyFrame
		int					m_duration;
		AnimMode			m_playMode;	
		Chain<KeyFrame>	m_keyframes;
	};
	
	//____ Struct AnimPlayPos ___________________________________________________
	
	struct AnimPlayPos
	{
		//.____ Properties _____________________________________________________
		
		KeyFrame *	pKeyFrame1;					// KeyFrame we are transitioning from.
		KeyFrame *	pKeyFrame2;					// KeyFrame we are transitioning towards.
		float			transition;					// 0 -> 1.0, current position. (1.0 only when anim has ended).
		int				animOffset;					// Offset in ticks from start of animation (unwinding loops etc).
	};
	
	

} // namespace wg
#endif //WG_ANIM_DOT_H
