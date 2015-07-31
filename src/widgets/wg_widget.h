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

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_RECEIVER_DOT_H
#	include <wg_receiver.h>
#endif

namespace wg 
{
	
	class WgGfxDevice;
	class WgContainer;
	class WgPanel;
	class WgList;
	class WgCapsule;
	class WgLayer;
	class WgMsgRouter;
	class WgPatches;
	
	class WgWidget;
	typedef	WgStrongPtr<WgWidget,WgReceiver_p>	WgWidget_p;
	typedef	WgWeakPtr<WgWidget,WgReceiver_wp>	WgWidget_wp;
	
	class WgContainer;
	typedef	WgStrongPtr<WgContainer,WgWidget_p>		WgContainer_p;
	typedef	WgWeakPtr<WgContainer,WgWidget_wp>		WgContainer_wp;
	
	class WgMsg;
	typedef	WgStrongPtr<WgMsg,WgObject_p>			WgMsg_p;
	typedef	WgWeakPtr<WgMsg,WgObject_wp>			WgMsg_wp;
	
	
	/**
	 * @brief Base class for widgets.
	 * 
	 * WgWidget is the base class for all widgets, providing common functionality.
	 */
	
	class WgWidget : public WgReceiver
	{
	friend class WgMsgRouter;
	friend class WgInputHandler;
	
	friend class WgHook;
	friend class WgFlexHook;
	friend class WgModalHook;
	friend class WgListHook;
	friend class WgStackHook;
	
	friend class WgRootPanel;
	friend class WgFlexPanel;
	friend class WgModalLayer;
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
	
	friend class WgTableRow;
	
	public:
		WgWidget();
		virtual ~WgWidget();
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgWidget_p	cast( const WgObject_p& pObject );
	
		inline int			id() const { return m_id; }
		inline void			setId( int id ) { m_id = id; }
	
		virtual WgString	tooltip() const { return m_tooltip; }
		inline void			setTooltip( const WgString& str ) { m_tooltip = str; }
	
		inline void			refresh() { _onRefresh(); }
		void				setEnabled(bool bEnabled);
		inline bool			isEnabled() const { return m_state.isEnabled(); }
	
		inline WgState		state() const { return m_state; }
	
		bool				cloneContent( const WgWidget_p& _pOrg );
	
		void				setPointerStyle( WgPointerStyle style )	{ m_pointerStyle = style; }
		virtual WgPointerStyle	pointerStyle() const;
	
		void				setTabLock( bool bLock ) { m_bTabLock = bLock; }
		bool				isTabLocked() const { return m_bTabLock; }
	
		void				setMarkOpacity( int opacity ) { m_markOpacity = opacity; }
		int					markOpacity() const { return m_markOpacity; }
		bool				markTest( const WgCoord& ofs );
	
		virtual void		setSkin( const WgSkin_p& pSkin );
		WgSkin_p			skin( ) const	{ return m_pSkin; }
	
	
		WgHook_p			hook() const { return m_pHook; }
	
		WgWidget_p		newOfMyType() const { return WgWidget_p(_newOfMyType() ); } ///< @brief Create and return a new widget of the same type.
	
		void 				onMsg( const WgMsg_p& pMsg );
	
	
		// Convenient calls to hook
	
		inline WgCoord		pos() const;
		inline WgSize		size() const;
		inline WgRect		geo() const;
		inline WgCoord		globalPos() const;
		inline WgRect		globalGeo() const;
		inline bool			grabFocus();
		inline bool			releaseFocus();
		inline bool			isFocused();
		WgContainer_p		parent() const;
	
		inline WgWidget_p	nextSibling() const;
		inline WgWidget_p	prevSibling() const;
	
		WgCoord				toGlobal( const WgCoord& coord ) const;
		WgCoord				toLocal( const WgCoord& coord ) const; 
		
		// To be overloaded by Widget
	
		virtual int		matchingHeight( int width ) const;
		virtual int		matchingWidth( int height ) const;
	
		virtual WgSize	preferredSize() const;	
		virtual WgSize	minSize() const;
		virtual WgSize	maxSize() const;
	
		virtual bool	isContainer() const { return false; }		///< @brief Check if this widget is a container.
																	///<
																	///< Check if widget is a container.
																	///< This method is a quicker way to check if the widget
																	///< is a container than calling isInstanceOf(WgContainer::CLASS).
																	///< @return True if the widget is a subclass of WgContainer.
	
	protected:
		void			_onNewHook( WgHook * pHook );
	
		void			_onNewRoot( WgRootPanel * pRoot );
		virtual WgBlendMode	_getBlendMode() const;
	
	
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
		virtual void	_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin );
		virtual void	_onStateChanged( WgState oldState );
	
		virtual void	_onMsg( const WgMsg_p& pMsg );
		virtual	bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
	
		virtual WgSize	_windowPadding() const;	// Padding of window before we get to (scrollable) content.
	
		//
	
		int				m_id;
		WgHook *		m_pHook;
	
		WgSkin_p		m_pSkin;
		WgPointerStyle	m_pointerStyle;
	
		WgString		m_tooltip;
		int				m_markOpacity;
	
		bool			m_bOpaque;
		bool			m_bTabLock;		// If set, the widget prevents focus shifting away from it with tab.
	
		WgState			m_state;
	private:
		bool			m_bPressed;		// Keeps track of pressed button when mouse leaves/re-enters widget.
	
	};
	
	typedef	int(*WgWidgetSortFunc)(const WgWidget *,const WgWidget *);
	
	
	
	//____ Inline methods __________________________________________________________
	
	/**
	 * @brief	Get the local position of widget.
	 * 
	 * Get the local position of the widget, which is the position relative to parent.
	 * 
	 * @return Local position in pixels. 
	 */
	WgCoord WgWidget::pos() const 
	{ 
		if( m_pHook ) 
			return m_pHook->pos(); 
		return WgCoord(0,0); 
	}
	
	
	/**
	 * @brief	Get the width and height of widget.
	 *
	 * Get the size (width and height) of widget.
	 * 
	 * @return Width and height in pixels.
	 */
	WgSize WgWidget::size() const 
	{ 
		if( m_pHook ) 
			return m_pHook->size(); 
		return WgSize(256,256); 
	}
	
	/**
	 * @brief	Get the local geometry of widget.
	 *
	 * Get the local geometry of the widget, eg a rectangle with the size of the widget and
	 * positon relative to the parent. If the widget doesn't have a parent, it's position
	 * defaults to (0,0).
	 * 
	 * @return Local geometry in pixels. 
	 */
	WgRect WgWidget::geo() const 
	{ 
		if( m_pHook ) 
			return m_pHook->geo(); 
		return WgRect(0,0,256,256); 
	}
	
	/**
	 * @brief	Get the global position of widget.
	 * 
	 * Get the global position of widet, which is the position relative to RootPanel.
	 * 
	 * @return Global position in pixels. 
	 */
	WgCoord WgWidget::globalPos() const 
	{ 
		if( m_pHook ) 
			return m_pHook->globalPos(); 
		return WgCoord(0,0); 
	}
	
	/**
	 * @brief	Get the global geometry of widget.
	 * 
	 * Get the global geometry of the widget, eg a rectangle with the size of the widget and
	 * positon relative to RootPanel. If the widget doesn't have a parent, it's position
	 * defaults to (0,0).
	 *
	 * @return Global geometry in pixels. 
	 */
	WgRect WgWidget::globalGeo() const 
	{ 
		if( m_pHook ) 
			return m_pHook->globalGeo(); 
		return WgRect(0,0,256,256); 
	}
	
	
	bool WgWidget::grabFocus() 
	{ 
		if( m_pHook ) 
			return m_pHook->_requestFocus(); 
		return false; 
	}
	
	bool WgWidget::releaseFocus() 
	{ 
		if( m_pHook ) 
			return m_pHook->_releaseFocus(); 
		return false; 
	}
	
	/** @brief Check if widget has keyboard focus.
	 *
	 * Check if widget has keyboard focus.
	 *
	 * @return True if widget has focus, otherwise false.
	 */
	
	bool WgWidget::isFocused() 
	{ 
		return m_state.isFocused(); 
	}
	
	/** @brief Get next sibling.
	 * 
	 * Get the widgets next sibling. Order of siblings is determined by parent
	 * and generally left to right or top to bottom for containers where children can't overlap.
	 * For containers where children can overlap the order is generally background to foreground.
	 * 
	 * @return Pointer to next sibling or null if none.
	 */
	
	WgWidget_p WgWidget::nextSibling() const 
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
	 * Get the widgets previous sibling. Order of siblings is determined by parent
	 * and generally left to right or top to bottom for containers where children can't overlap.
	 * For containers where children can overlap the order is generally background to foreground.
	 * 
	 * @return Pointer to previous sibling or null if none.
	 */
	
	WgWidget_p WgWidget::prevSibling() const 
	{ 
		if( m_pHook ) 
		{
			WgHook * p = m_pHook->_prevHook();
			if( p ) 
				return p->m_pWidget; 
		} 
		return 0; 
	}
	
	

} // namespace wg
#endif
