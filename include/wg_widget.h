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

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

class WgGfxDevice;
class WgContainer;
class WgPanel;
class WgList;
class WgCapsule;
class WgLayer;
class WgEventHandler;
class WgPatches;

class WgWidget;
typedef	WgStrongPtr<WgWidget,WgObjectPtr>		WgWidgetPtr;
typedef	WgWeakPtr<WgWidget,WgObjectWeakPtr>	WgWidgetWeakPtr;

class WgContainer;
typedef	WgStrongPtr<WgContainer,WgWidgetPtr>			WgContainerPtr;
typedef	WgWeakPtr<WgContainer,WgWidgetWeakPtr>		WgContainerWeakPtr;


/**
 * @brief Base class for widgets.
 * 
 * WgWidget is the base class for all widgets, providing common functionality.
 */

class WgWidget : public WgObject
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
friend class WgList;
friend class WgPackList;
friend class WgCapsule;
friend class WgLayer;
friend class WgStackPanel;
friend class WgContainer;
friend class WgPanel;
friend class WgVectorPanel;
friend class WgPackPanel;
friend class WgShaderCapsule;
friend class WgPopupLayer;
friend class WgRadioButton;

friend class WgTableRow;

public:
	WgWidget();
	virtual ~WgWidget();

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgWidgetPtr	Cast( const WgObjectPtr& pObject );

	inline int			Id() const { return m_id; }
	inline void			SetId( int id ) { m_id = id; }

	virtual WgString	Tooltip() const { return m_tooltip; }
	inline void			SetTooltip( const WgString& str ) { m_tooltip = str; }

	inline void			Refresh() { _onRefresh(); }
	void				SetEnabled(bool bEnabled);
	inline bool			IsEnabled() const { return m_state.IsEnabled(); }

	inline WgState		State() const { return m_state; }

	bool				CloneContent( const WgWidgetPtr& _pOrg );

	void				SetPointerStyle( WgPointerStyle style )	{ m_pointerStyle = style; }
	virtual WgPointerStyle	PointerStyle() const;

	void				SetTabLock( bool bLock ) { m_bTabLock = bLock; }
	bool				IsTabLocked() const { return m_bTabLock; }

	void				SetMarkOpacity( int opacity ) { m_markOpacity = opacity; }
	int					MarkOpacity() const { return m_markOpacity; }
	bool				MarkTest( const WgCoord& ofs );

	virtual void		SetSkin( const WgSkinPtr& pSkin );
	WgSkinPtr			Skin( ) const	{ return m_pSkin; }


	WgHookPtr			Hook() const { return m_pHook; }

	WgWidgetPtr		NewOfMyType() const { return WgWidgetPtr(_newOfMyType() ); } ///< @brief Create and return a new widget of the same type.

	// Convenient calls to hook

	inline WgCoord		Pos() const;
	inline WgSize		Size() const;
	inline WgRect		Geo() const;
	inline WgCoord		GlobalPos() const;
	inline WgRect		GlobalGeo() const;
	inline bool			GrabFocus();
	inline bool			ReleaseFocus();
	inline bool			IsFocused();
	WgContainerPtr		Parent() const;

	inline WgWidgetPtr	NextSibling() const;
	inline WgWidgetPtr	PrevSibling() const;

	WgCoord				ToGlobal( const WgCoord& coord ) const;
	WgCoord				ToLocal( const WgCoord& coord ) const; 
	
	// To be overloaded by Widget

	virtual int		HeightForWidth( int width ) const;
	virtual int		WidthForHeight( int height ) const;

	virtual WgSize	PreferredSize() const;	
	virtual WgSize	MinSize() const;
	virtual WgSize	MaxSize() const;

	virtual bool	IsContainer() const { return false; }		///< @brief Check if this widget is a container.
																///< This method is just a quicker way to check if the widget
																///< is a container (thus might have children) than calling 
																///< IsInstanceOf(WgContainer::CLASS).
																///< @return True if the widget is a subclass of WgContainer.

protected:
	void			_onNewHook( WgHook * pHook );

	void			_onNewRoot( WgRootPanel * pRoot );
	void			_startReceiveTicks();
	void			_stopReceiveTicks();
	virtual WgBlendMode	_getBlendMode() const;

	WgEventHandler* _eventHandler() const;
	void			_queueEvent( const WgEventPtr& pEvent );

	virtual WgWidget* _newOfMyType() const = 0;


	// Convenient calls to hook

	void			_requestRender() { if( m_pHook ) m_pHook->_requestRender(); }
	void			_requestRender( const WgRect& rect ) { if( m_pHook ) m_pHook->_requestRender( rect ); }
	void			_requestResize() { if( m_pHook ) m_pHook->_requestResize(); }

	WgWidget *		_nextSibling() const { if( m_pHook ) {WgHook * p = m_pHook->_nextHook(); if( p ) return p->m_pWidget; } return 0; }
	WgWidget *		_prevSibling() const { if( m_pHook ) {WgHook * p = m_pHook->_prevHook(); if( p ) return p->m_pWidget; } return 0; }
	inline WgHook *	_hook() const { return m_pHook; }
	WgContainer *	_parent() const { if( m_pHook ) return m_pHook->_parent(); return 0; }

	WgRect			_windowSection() const { if( m_pHook ) return m_pHook->_windowSection(); return WgRect(); }

	// To be overloaded by Widget

	virtual void	_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	virtual void	_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	virtual void	_onCloneContent( const WgWidget * _pOrg ) = 0;
	virtual void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );

	virtual void	_onRefresh();
	virtual void	_onNewSize( const WgSize& size );
	virtual void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	virtual void	_onStateChanged( WgState oldState );

	virtual void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	virtual	bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );

	virtual WgSize	_windowPadding() const;	// Padding of window before we get to (scrollable) content.

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

typedef	int(*WgWidgetSortFunc)(const WgWidget *,const WgWidget *);



//____ Inline methods __________________________________________________________

/**
 * @brief	Return the local position of widget.
 * 
 * Local position is the position relative to parent.
 * 
 * @return Local position in pixels. 
 */
WgCoord WgWidget::Pos() const 
{ 
	if( m_pHook ) 
		return m_pHook->Pos(); 
	return WgCoord(0,0); 
}


/**
 * @brief	Return the width and height of widget.
 * 
 * @return Width and height in pixels.
 */
WgSize WgWidget::Size() const 
{ 
	if( m_pHook ) 
		return m_pHook->Size(); 
	return WgSize(256,256); 
}

/**
 * @brief	Return the local geometry of widget.
 * 
 * Return a rectangle with the size and local position, e.g position relative to parent, of the widget.
 * 
 * @return Local geometry in pixels. 
 */
WgRect WgWidget::Geo() const 
{ 
	if( m_pHook ) 
		return m_pHook->Geo(); 
	return WgRect(0,0,256,256); 
}

/**
 * @brief	Return the global position of widget.
 * 
 * Global position is the position relative to RootPanel.
 * 
 * @return Global position in pixels. 
 */
WgCoord WgWidget::GlobalPos() const 
{ 
	if( m_pHook ) 
		return m_pHook->GlobalPos(); 
	return WgCoord(0,0); 
}

/**
 * @brief	Return the global geometry of widget.
 * 
 * Return a rectangle with the size and global position, e.g position relative to RootPanel, of the widget.
 * 
 * @return Global geometry in pixels. 
 */
WgRect WgWidget::GlobalGeo() const 
{ 
	if( m_pHook ) 
		return m_pHook->GlobalGeo(); 
	return WgRect(0,0,256,256); 
}


bool WgWidget::GrabFocus() 
{ 
	if( m_pHook ) 
		return m_pHook->_requestFocus(); 
	return false; 
}

bool WgWidget::ReleaseFocus() 
{ 
	if( m_pHook ) 
		return m_pHook->_releaseFocus(); 
	return false; 
}

/** @brief Check if widget has keyboard focus.
 * 
 * @return True if widget has focus, otherwise false.
 */

bool WgWidget::IsFocused() 
{ 
	return m_state.IsFocused(); 
}

/** @brief Get next sibling.
 * 
 * Next sibling is next widget with same parent. Order of siblings is determined by parent
 * and generally left to right or top to bottom for containers where children can't overlap.
 * For containers where children can overlap the order is generally background to foreground.
 * 
 * @return Pointer to next sibling or null if none.
 */

WgWidgetPtr WgWidget::NextSibling() const 
{ 
	if( m_pHook ) 
	{
		WgHook * p = m_pHook->_nextHook(); 
		if( p ) 
			return p->m_pWidget; 
	} 
	return 0; 
}

/** @brief Get previous sibling.
 * 
 * Previous sibling is previous widget with same parent. Order of siblings is determined by parent
 * and generally left to right or top to bottom for containers where children can't overlap.
 * For containers where children can overlap the order is generally background to foreground.
 * 
 * @return Pointer to previous sibling or null if none.
 */

WgWidgetPtr WgWidget::PrevSibling() const 
{ 
	if( m_pHook ) 
	{
		WgHook * p = m_pHook->_prevHook();
		if( p ) 
			return p->m_pWidget; 
	} 
	return 0; 
}


#endif
