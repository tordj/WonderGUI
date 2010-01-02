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

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

class		WgKeyFrame;
struct	WgAnimPlayPos;


//____ Class WgKeyFrame _______________________________________________________

class WgKeyFrame : public WgLink
{
	friend class WgAnim;
public:
	bool		setDuration( Uint32 ticks );
	Uint32	duration( void ) { return m_duration; };
	Uint32	timestamp( void ) { return m_timestamp; };

protected:
	Uint32	m_timestamp;
	Uint32	m_duration;
	LINK_METHODS( WgKeyFrame );
};


//____ Class WgAnim ______________________________________________________

class WgAnim
{
	friend class WgKeyFrame;

public:
	WgAnim();
	virtual ~WgAnim();

	enum PlayMode
	{
		FORWARD_ONCE,
		BACKWARD_ONCE,
		FORWARD_LOOPING,
		BACKWARD_LOOPING,
		FORWARD_PINGPONG,
		BACKWARD_PINGPONG
	};


	bool				setPlayMode( PlayMode mode );
	bool				setTimeScaler( float scale );

	bool				deleteKeyFrame( Uint32 pos );
	bool				deleteKeyFrame( WgKeyFrame * pKeyFrame );
	inline void			clear( void ) { m_keyframes.clear(); };

	inline Uint32		duration( void ) { return m_duration; };
	inline PlayMode		playMode( void ) { return m_playMode; };
	inline float		timeScaler( void ) { return m_scale; };
	inline Uint32		durationScaled( void ) { return (Uint32) (m_duration * m_scale); };
	inline Uint32		time2ofs( Uint32 ticks );		/// Convert play-time to offset in animation by scaling with timeScaler and unwinding loops.

protected:

	// Meant to be overloaded with methods by the same name that builds up their 
	// WgKeyFrame-derived class.

	bool						insertKeyFrame( Uint32 pos, WgKeyFrame * pFrame, Uint32 duration );
	bool						insertKeyFrame( WgKeyFrame * pBefore, WgKeyFrame * pFrame, Uint32 duration );
	bool						addKeyFrame( WgKeyFrame * pFrame, Uint32 duration );

	// Meant to be overloaded with public methods returning right type.

	inline WgKeyFrame	* getFirstKeyFrame( void ) {return m_keyframes.getFirst();};
	inline WgKeyFrame	* getLastKeyFrame( void ) {return m_keyframes.getLast();};

	//

	WgAnimPlayPos		getPlayPos( Uint32 ticks, WgKeyFrame * pProximity = 0 ) const;		// ticks gets scaled.

	WgKeyFrame		* getKeyFrame( Uint32 ticks, WgKeyFrame * pProximity = 0 ) const;

private:
	
	float								m_scale;			// Only used for getKeyFrame
	Uint32							m_duration;
	PlayMode						m_playMode;	
	WgChain<WgKeyFrame>	m_keyframes;
};

//____ Struct WgAnimPlayPos ___________________________________________________

struct WgAnimPlayPos
{
	WgKeyFrame *	pKeyFrame1;					// KeyFrame we are transitioning from.
	WgKeyFrame *	pKeyFrame2;					// KeyFrame we are transitioning towards.
	float					transition;					// 0 -> 1.0, current position. (1.0 only when anim has ended).
	Uint32				animOffset;					// Offset in ticks from start of animation (unwinding loops etc).
};


#endif //WG_ANIM_DOT_H
