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

#ifndef WG_GIZMO_DOT_H
#define WG_GIZMO_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

#ifndef WG_INPUT_DOT_H
#	include <wg_input.h>
#endif

#ifndef WG_GIZMO_HOOK_DOT_H
#	include <wg_gizmo_hook.h>
#endif

#ifndef WG_EMITTER_DOT_H
#	include <wg_emitter.h>
#endif

#ifndef WG_SIGNALS_DOT_H
#	include <wg_signals.h>
#endif



class WgUpdateInfo;
class WgEmitter;
class WgGfxDevice;
class WgChildManager;


class WgGizmo
{
public:
	WgGizmo();
	virtual ~WgGizmo();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	inline Sint32		Id() const { return m_id; }
	inline void			SetId( Sint32 id ) { m_id = id; }

	inline WgString		GetTooltipString() const { return m_tooltip; }
	inline void			SetTooltipString( const WgString& str ) { m_tooltip = str; }


	void				Enable();
	void				Disable();
	inline bool			IsEnabled() const { return m_bEnabled; }

	void				CloneContent( const WgGizmo * _pOrg );

	inline void				SetCursorStyle( WgCursorStyle style )	{ m_cursorStyle = style; }
	inline WgCursorStyle	GetCursorStyle() const					{ return m_cursorStyle; }


	// Convenient calls to hook

	inline WgCord		Pos() const { if( m_pHook ) return m_pHook->Pos(); return WgCord(0,0); }
	inline WgSize		Size() const { if( m_pHook ) return m_pHook->Size(); return WgSize(256,256); }
	inline WgCord		ScreenPos() const { if( m_pHook ) return m_pHook->ScreenPos(); return WgCord(0,0); }
	inline WgRect		ScreenGeometry() const { if( m_pHook ) return m_pHook->ScreenGeo(); return WgRect(0,0,256,256); }
	inline bool			GrabFocus() { if( m_pHook ) return m_pHook->RequestFocus(); return false; }
	inline bool			ReleaseFocus() { if( m_pHook ) return m_pHook->ReleaseFocus(); return false; }

	inline WgGizmo *	NextSibling() const { if( m_pHook ) {WgGizmoHook * p = m_pHook->NextHook(); if( p ) return p->Gizmo(); } return 0; }
	inline WgGizmo *	PrevSibling() const { if( m_pHook ) {WgGizmoHook * p = m_pHook->PrevHook(); if( p ) return p->Gizmo(); } return 0; }

	WgCord				Local2abs( const WgCord& cord ) const;		// Cordinate from local cordsys to global
	WgCord				Abs2local( const WgCord& cord ) const; 		// Cordinate from global to local cordsys


	// To be overloaded by Gizmo

	virtual Uint32	HeightForWidth( Uint32 width ) const;
	virtual Uint32	WidthForHeight( Uint32 height ) const;

	virtual WgSize	MinSize() const;
	virtual WgSize	BestSize() const;
	virtual WgSize	MaxSize() const;

	virtual bool	IsView() const { return false; }
	virtual bool	IsContainer() const { return false; }

//	virtual WgGizmoManager*	GetView() const { return 0; }
//	virtual WgGizmoManager*	GetContainer() const { return 0; }

protected:

	void			SetHook( WgGizmoHook * pHook );

	// Convenient calls to hook

	inline void		RequestRender() { if( m_pHook ) m_pHook->RequestRender(); }
	inline void		RequestRender( const WgRect& rect ) { if( m_pHook ) m_pHook->RequestRender( rect ); }
	inline void		RequestResize() { if( m_pHook ) m_pHook->RequestResize(); }

	// To be overloaded by Gizmo

	virtual void	OnCloneContent( const WgGizmo * _pOrg ) = 0;
	virtual void	OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer ) = 0;
	virtual void	OnNewSize( const WgSize& size );
	virtual void	OnRefresh();
	virtual void	OnUpdate( const WgUpdateInfo& _updateInfo );
	virtual void	OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	virtual	bool	OnMarkTest( const WgCord& ofs );
	virtual void	OnEnable();
	virtual void	OnDisable();
	virtual void	OnGotInputFocus();
	virtual void	OnLostInputFocus();

	//

	WgGizmoHook *	m_pHook;

	WgChildManager * m_pChildManager;

	WgCursorStyle	m_cursorStyle;

	Uint32			m_id;
	WgString		m_tooltip;

	bool			m_bEnabled;
	bool			m_bOpaque;

	bool			m_bRendersAll;
	bool			m_bRenderOne;
};



#endif

