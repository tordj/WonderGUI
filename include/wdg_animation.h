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

#ifndef	WDG_ANIMATION_DOT_H
#define	WDG_ANIMATION_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_ANIMATION_DOT_H
#	include <wg_gizmo_animation.h>
#endif

class Wdg_Animation : public Wdg_Widget<WgGizmoAnimation>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_Animation,WgGizmoAnimation);
	virtual ~Wdg_Animation() {};

	//____ Callbacks ________________________________________

	static void cbSetPlayPos( void * pWdg, int ticks )				{ ((Wdg_Animation*)pWdg)->SetPlayPos(ticks); }
	static void cbSetPlayPosFractional( void * pWdg, float frac )	{ ((Wdg_Animation*)pWdg)->SetPlayPosFractional(frac); }


};



/*
#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

class Wdg_Animation:public WgWidget
{
public:
	WIDGET_CONSTRUCTORS(Wdg_Animation,WgWidget);
	~Wdg_Animation();
	virtual const char * Type() const;
	static const char * GetMyType();


	//____ Callbacks ________________________________________

	static void cbSetPlayPos( void * pWdg, int ticks )				{ ((Wdg_Animation*)pWdg)->SetPlayPos(ticks); }
	static void cbSetPlayPosFractional( void * pWdg, float frac )	{ ((Wdg_Animation*)pWdg)->SetPlayPosFractional(frac); }

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

protected:
	WgWidget *	NewWidgetOfMyType() const;
	void		DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 _layer );	
private:
	void		Init();

	void		DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
	void		DoMyOwnRefresh( void );
	void		DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
	bool 		DoMyOwnMarkTest( int _x, int _y );
	void 		DoMyOwnDisOrEnable( void );

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
*/

#endif //	WDG_ANIMATION_DOT_H
