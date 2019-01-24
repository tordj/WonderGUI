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

#ifndef	WG_ANIMPLAYER_DOT_H
#define	WG_ANIMPLAYER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_gfxanim.h>

namespace wg 
{
	
	class AnimPlayer;
	typedef	StrongPtr<AnimPlayer>		AnimPlayer_p;
	typedef	WeakPtr<AnimPlayer>	AnimPlayer_wp;
	
	//____ AnimPlayer ____________________________________________________________
	/**
	* @brief A widget for playing simple animations.
	*
	**/

	class AnimPlayer:public Widget
	{
	public:

		//.____ Creation __________________________________________

		static AnimPlayer_p	create() { return AnimPlayer_p(new AnimPlayer()); }
	
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static AnimPlayer_p	cast( Object * pObject );
	
		//.____ Control __________________________________________
	
		bool			setAnimation( GfxAnim * pAnim );
		GfxAnim_p		animation() const { return m_pAnim; }
			
		int				playPos();										/// Returns play position in ticks.
		bool			setPlayPos( int ticks );						/// Position in ticks for next update.
		bool			setPlayPosFractional( float fraction );			/// Position in fractions of duration.
		
		bool			rewind( int ticks );
		bool			fastForward( int ticks );
	
		int				duration();										/// Returns duration of animation (one-shot-through, no looping).
		int				durationScaled();								/// Returns duration of animation, scaled by speed.
	
		float			speed();
		bool			setSpeed( float speed );
	
		bool			play();
		bool			stop();
		bool			isPlaying() { return m_bPlaying; };
	
		//.____ Geometry ____________________________________________
		
		Size			preferredSize() const;
	
	protected:
		AnimPlayer();
		virtual ~AnimPlayer();
		virtual Widget* _newOfMyType() const { return new AnimPlayer(); };
	
		void			_cloneContent( const Widget * _pOrg );
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window );
		void			_refresh();
		void			_receive( Msg * pMsg );
		bool			_alphaTest( const Coord& ofs );
		void			_setState( State state );
	
		void			_playPosUpdated();
	
	private:
	
		GfxAnim_p	m_pAnim;
		GfxFrame *	m_pAnimFrame;			// Frame currently used by animation.
		RouteId		m_tickRouteId;
	
		bool			m_bPlaying;
		double			m_playPos;
		float			m_speed;
	};
	
	

} // namespace wg
#endif //	WG_ANIMPLAYER_DOT_H
