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


#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

class WgAnimPlayer;
typedef	WgSmartPtr<WgAnimPlayer,WgWidgetPtr>		WgAnimPlayerPtr;
typedef	WgWeakPtr<WgAnimPlayer,WgWidgetWeakPtr>	WgAnimPlayerWeakPtr;


class WgAnimPlayer:public WgWidget
{
public:
	static WgAnimPlayerPtr	Create() { return WgAnimPlayerPtr(new WgAnimPlayer()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgAnimPlayerPtr	Cast( const WgObjectPtr& pObject );

	//____ Methods __________________________________________

	bool			SetAnimation( WgGfxAnim * pAnim );
	WgGfxAnim *		Animation() const { return m_pAnim; }
		
	int				PlayPos();										/// Returns play position in ticks.
	bool			SetPlayPos( int ticks );						/// Position in ticks for next update.
	bool			SetPlayPosFractional( float fraction );			/// Position in fractions of duration.
	
	bool			Rewind( int ticks );
	bool			FastForward( int ticks );

	int				Duration();										/// Returns duration of animation (one-shot-through, no looping).
	int				DurationScaled();								/// Returns duration of animation, scaled by speed.

	float			Speed();
	bool			SetSpeed( float speed );

	bool			Play();
	bool			Stop();
	bool			IsPlaying() { return m_bPlaying; };

	WgSize			PreferredSize() const;

protected:
	WgAnimPlayer();
	virtual ~WgAnimPlayer();
	virtual WgWidget* _newOfMyType() const { return new WgAnimPlayer(); };

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onRefresh();
	void			_onEvent( WgEvent::Event * pEvent, WgEventHandler * pHandler );
	bool			_onAlphaTest( const WgCoord& ofs );
	void			_onStateChanged( WgState oldState, WgState newState );

	void			_playPosUpdated();

private:

	WgGfxAnim *		m_pAnim;
	WgGfxFrame *	m_pAnimFrame;			// Frame currently used by animation.

	bool			m_bPlaying;
	double			m_playPos;
	float			m_speed;
};


#endif //	WG_ANIMPLAYER_DOT_H
