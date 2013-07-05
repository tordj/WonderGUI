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

#ifndef WG_WIDGET_DOT_H
#define WG_WIDGET_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

#ifndef WG_HOOK_DOT_H
#	include <wg_hook.h>
#endif

#ifndef WG_EVENT_DOT_H
#	include <wg_event.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

class WgGfxDevice;
class Wg_Interface_TextHolder;
class WgContainer;
class WgPanel;
class WgList;
class WgCapsule;
class WgLayer;
class WgEventHandler;
class WgPatches;


class WgWidget : public WgWeakPtrTarget
{
friend class WgInput;
friend class WgEventHandler;

friend class WgHook;
friend class WgFlexHook;
friend class WgModalHook;
friend class WgListHook;
friend class WgStackHook;

friend class WgRootPanel;
friend class WgFlexPanel;
friend class WgModalLayer;
friend class WgTablePanel;
friend class WgScrollPanel;
friend class WgListPanel;
friend class WgCapsule;
friend class WgLayer;
friend class WgStackPanel;
friend class WgContainer;
friend class WgPanel;
friend class WgPackPanel;
friend class WgShaderCapsule;
friend class WgMenuLayer;

friend class WgTableRow;

public:
	WgWidget();
	virtual ~WgWidget();

	virtual const char *Type( void ) const = 0;

	inline int			Id() const { return m_id; }
	inline void			SetId( int id ) { m_id = id; }

	virtual WgString	TooltipString() const { return m_tooltip; }
	WgString			RealTooltipString() const { return m_tooltip; }
	inline void			SetTooltipString( const WgString& str ) { m_tooltip = str; }

	inline void			Refresh() { _onRefresh(); }
	void				SetEnabled(bool bEnabled);
	inline bool			IsEnabled() const { return m_state.IsEnabled(); }

	inline WgState		State() const { return m_state; }

	bool				CloneContent( const WgWidget * _pOrg );

	void				SetPointerStyle( WgPointerStyle style )	{ m_pointerStyle = style; }
	virtual WgPointerStyle	PointerStyle() const;

	void				SetTabLock( bool bLock ) { m_bTabLock = bLock; }
	bool				IsTabLocked() const { return m_bTabLock; }

	void				SetMarkOpacity( int opacity ) { m_markOpacity = opacity; }
	int					GetMarkOpacity() const { return m_markOpacity; }
	bool				MarkTest( const WgCoord& ofs );

	virtual void		SetSkin( const WgSkinPtr& pSkin );
	WgSkinPtr			Skin( ) const	{ return m_pSkin; }


	WgHook*			Hook() const { return m_pHook; }

	virtual WgWidget * NewOfMyType() const = 0;

	// Convenient calls to hook

	WgCoord			Pos() const { if( m_pHook ) return m_pHook->Pos(); return WgCoord(0,0); }
	WgSize			Size() const { if( m_pHook ) return m_pHook->Size(); return WgSize(256,256); }
	WgRect			Geo() const { if( m_pHook ) return m_pHook->Geo(); return WgRect(0,0,256,256); }
	WgCoord			ScreenPos() const { if( m_pHook ) return m_pHook->ScreenPos(); return WgCoord(0,0); }
	WgRect			ScreenGeo() const { if( m_pHook ) return m_pHook->ScreenGeo(); return WgRect(0,0,256,256); }
	bool			GrabFocus() { if( m_pHook ) return m_pHook->_requestFocus(); return false; }
	bool			ReleaseFocus() { if( m_pHook ) return m_pHook->_releaseFocus(); return false; }
	bool			IsFocused() { return m_state.IsFocused(); }
	WgContainer *	Parent() const { if( m_pHook ) return m_pHook->_parent(); return 0; }
	WgWidgetHolder* Holder() const { if( m_pHook ) return m_pHook->_holder(); return 0; }

	WgWidget *		NextSibling() const { if( m_pHook ) {WgHook * p = m_pHook->Next(); if( p ) return p->Widget(); } return 0; }
	WgWidget *		PrevSibling() const { if( m_pHook ) {WgHook * p = m_pHook->Prev(); if( p ) return p->Widget(); } return 0; }

	WgCoord			Local2abs( const WgCoord& cord ) const;		// Cordinate from local cordsys to global
	WgCoord			Abs2local( const WgCoord& cord ) const; 		// Cordinate from global to local cordsys

	// To be overloaded by Widget

	virtual int		HeightForWidth( int width ) const;
	virtual int		WidthForHeight( int height ) const;

	virtual WgSize	PreferredSize() const;
	virtual WgSize	MinSize() const;
	virtual WgSize	MaxSize() const;

	virtual bool	IsContainer() const { return false; }
	virtual bool	IsPanel() const { return false; }
	virtual bool	IsList() const { return false; }
	virtual bool	IsCapsule() const { return false; }
	virtual bool	IsLayer() const { return false; }
	virtual WgContainer * CastToContainer() { return 0; }
	virtual const WgContainer * CastToContainer() const { return 0; }
	virtual WgPanel * CastToPanel() { return 0; }
	virtual const WgPanel * CastToPanel() const { return 0; }
	virtual WgList * CastToList() { return 0; }
	virtual const WgList * CastToList() const { return 0; }
	virtual WgCapsule * CastToCapsule() { return 0; }
	virtual const WgCapsule * CastToCapsule() const { return 0; }
	virtual WgLayer * CastToLayer() { return 0; }
	virtual const WgLayer * CastToLayer() const { return 0; }

protected:

	void			_onNewHook( WgHook * pHook );

	void			_onNewRoot( WgRootPanel * pRoot );
	void			_startReceiveTicks();
	void			_stopReceiveTicks();
	virtual WgBlendMode	_getBlendMode() const;

	WgEventHandler* _eventHandler() const;
	void			_queueEvent( WgEvent::Event * pEvent );


	// Convenient calls to hook

	void			_requestRender() { if( m_pHook ) m_pHook->_requestRender(); }
	void			_requestRender( const WgRect& rect ) { if( m_pHook ) m_pHook->_requestRender( rect ); }
	void			_requestResize() { if( m_pHook ) m_pHook->_requestResize(); }

	// To be overloaded by Widget

	virtual void	_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	virtual void	_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	virtual void	_onCloneContent( const WgWidget * _pOrg ) = 0;
	virtual void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );

	virtual void	_onRefresh();
	virtual void	_onNewSize( const WgSize& size );
	virtual void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	virtual void	_onStateChanged( WgState oldState, WgState newState );

	virtual void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	virtual	bool	_onAlphaTest( const WgCoord& ofs );


	// rename when widgets are done
	virtual bool	IsInputField() const;
	virtual Wg_Interface_TextHolder*	TextInterface();

	//

	int				m_id;
	WgHook *		m_pHook;

	WgSkinPtr		m_pSkin;
	WgPointerStyle	m_pointerStyle;

	WgString		m_tooltip;
	int				m_markOpacity;

	bool			m_bOpaque;
	bool			m_bTabLock;		// If set, the widget prevents focus shifting away from it with tab.
	bool			m_bReceiveTick;	// Set if Widget should reveive periodic Tick() events.

	WgState			m_state;
private:
	bool			m_bPressed;		// Keeps track of pressed button when mouse leaves/re-enters widget.

};

typedef class WgWeakPtr<WgWidget> WgWidgetWeakPtr;

typedef	int(*WgWidgetSortFunc)(const WgWidget *,const WgWidget *);

template<typename T> T* WgCast(WgWidget * pWidget)
{
	if(pWidget)
	{
		if(T::GetClass() == pWidget->Type())
			return static_cast<T*>(pWidget);
	}
	return 0;
}

template<typename T> const T* WgCast(const WgWidget * pWidget)
{
	if(pWidget)
	{
		if(T::GetClass() == pWidget->Type())
			return static_cast<const T*>(pWidget);
	}
	return 0;
}


#endif

