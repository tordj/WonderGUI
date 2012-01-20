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


	//____ Methods __________________________________________

	bool		SetSource( WgGfxAnim * pAnim );
	inline WgGfxAnim *	GetSource() const { return m_pAnim; }
	bool		DisableSource( WgSurface * pSurf, WgRect& rect );
		
	Uint32		PlayPos();											/// Returns play position in ticks.
	bool		SetPlayPos( Uint32 ticks );							/// Position in ticks for next update.
	bool		SetPlayPosFractional( float fraction );				/// Position in fractions of duration.
	
	void		Rewind( Uint32 ticks );
	void		FastForward( Uint32 ticks );

	Uint32		Duration();											/// Returns duration of animation (one-shot-through, no looping).
	Uint32		DurationScaled();									/// Returns duration of animation, scaled by speed.

	float		Speed();
	bool		SetSpeed( float speed );

	bool		Play();
	bool		Stop();
	bool		IsPlaying() { return m_bPlaying; };

	WgSize			DefaultSize() const;

protected:
	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	_onRefresh();
#ifdef WG_TNG
	void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
#endif
	void	_onUpdate( const WgUpdateInfo& _updateInfo );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onEnable();
	void	_onDisable();

private:

	WgSurface * m_pSurf;						// Surface currently used by animation.					
	WgRect		m_src;							// Cordinates and width/height of source.


	WgSurface * m_pDisableSurf;
	WgRect      m_dis_src;

	WgGfxAnim *	m_pAnim;

	double		m_playPos;

	bool		m_bPlayPosIsNew;		// Set when we change play-pos, so we don't increase time before displaying.
	bool		m_bPlaying;
	float		m_speed;
};


#endif //	WG_GIZMO_ANIMATION_DOT_H
