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

#ifndef	WG_GIZMO_ANIMATION_DOT_H
#define	WG_GIZMO_ANIMATION_DOT_H


#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef	WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

class WgGizmoAnimation:public WgGizmo
{
public:
	WgGizmoAnimation();
	~WgGizmoAnimation();
	virtual const char * Type() const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoAnimation(); };


	//____ Methods __________________________________________

	bool			SetAnimation( WgGfxAnim * pAnim );
	WgGfxAnim *		Animation() const { return m_pAnim; }

	bool			SetSource( const WgBlockSetPtr& pStaticBlock );
	WgBlockSetPtr	Source() const { return m_pStaticBlock; }
		
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

	WgSize			DefaultSize() const;

protected:
	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			_onRefresh();
	void			_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	bool			_onAlphaTest( const WgCoord& ofs );
	void			_onEnable();
	void			_onDisable();

	void			_playPosUpdated();

private:

	WgGfxAnim *		m_pAnim;
	WgBlock			m_animFrame;			// Frame currently used by animation.
	WgBlockSetPtr	m_pStaticBlock;			// Blockset used when no animation is displayed (not set or gizmo disabled).

	bool			m_bPlaying;
	double			m_playPos;
	float			m_speed;
};


#endif //	WG_GIZMO_ANIMATION_DOT_H
