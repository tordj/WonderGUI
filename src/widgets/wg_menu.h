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

#ifndef	WG_MENU_DOT_H
#define	WG_MENU_DOT_H


#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_MENUITEM_DOT_H
#	include <wg_menuitem.h>
#endif

#ifndef	WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_SCROLLBAR_DOT_H
#	include <wg_scrollbar.h>
#endif

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef WG_SCROLLBARTARGET_DOT_H
#	include <wg_scrollbartarget.h>
#endif

namespace wg 
{
	
	class	WgSurface;
	class	WgFont;
	class	WgGfxAnim;
	
	class WgMenu;
	typedef	WgStrongPtr<WgMenu,WgWidget_p>		WgMenu_p;
	typedef	WgWeakPtr<WgMenu,WgWidget_wp>	WgMenu_wp;
	
	class WgMenu:public WgPanel, private WgScrollbarTarget
	{
		friend class WgMenuItem;
		friend class WgMenuSubMenu;
	
	public:
		static WgMenu_p	create() { return WgMenu_p(new WgMenu()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgMenu_p	cast( const WgObject_p& pObject );
	
		//____ Methods ___________________________________________
	
		bool			setSkin( const WgSkin_p& pSkin, int iconFieldWidth, int arrowFieldWidth );
		int				iconFieldWidth() const			{ return m_iconFieldWidth; }
		int				arrowFieldWidth() const			{ return m_arrowFieldWidth; }
	
		void			setEntrySkin( const WgSkin_p& pSkin );
		WgSkin_p		entrySkin() const { return m_pEntrySkin; }
	
		bool			setSeparatorSkin( const WgSkin_p& pSkin, const WgBorder& sepBorder = WgBorder() );
		WgSkin_p		separatorSkin() const { return m_pSeparatorSkin; }
		WgBorder		separatorBorder() const { return m_sepBorder; }
	
		bool			setCheckBoxSkin( const WgSkin_p& pSkin );
		WgSkin_p		checkBoxSkin() const { return m_pCheckBoxSkin; }
	
		bool			setRadioButtonSkin( const WgSkin_p& pSkin );
		WgSkin_p 		radioButtonSkin() const { return m_pRadioButtonSkin; }
	
		bool			setArrowSource( const WgGfxAnim_p& pAnim );
	
		bool			setTextProperties( const WgTextprop_p& pEntryProperties, const WgTextprop_p& pKeyAccelProperties );
		WgTextprop_p	getTextEntryProperties() const { return m_pEntryProp; }
		WgTextprop_p	getKeyAccelProperties() const { return m_pKeyAccelProp; }
	
		bool			setScrollbarSkins(  const WgSkin_p& pBackgroundSkin, const WgSkin_p& pHandleSkin, const WgSkin_p& pBwdButtonSkin, const WgSkin_p& pFwdButtonSkin );
		bool			setScrollbarButtonLayout(  WgScrollbar::BtnLayout layout );
		WgScrollbar::BtnLayout scrollbarButtonLayout() const { return m_scrollbarBtnLayout; }
		WgSkin_p 		scrollbarBackgroundSkin() const { return m_pScrollbarBgSkin; }
		WgSkin_p 		scrollbarHandleSkin() const { return m_pScrollbarHandleSkin; }
		WgSkin_p 		scrollbarBwdButtonSkin() const { return m_pScrollbarBtnBwdSkin; }
		WgSkin_p 		scrollbarFwdButtonSkin() const { return m_pScrollbarBtnFwdSkin; }
	
		int			getEntryHeight() const;
	
		int			addItem( WgMenuItem * pEntry );
		int			insertItem( WgMenuItem * pEntry, int pos );
	
		bool		removeItem( WgMenuItem * pEntry );
		WgMenuItem*	removeItem( int pos );
		void		removeAllItems();
	
		bool		deleteItem( WgMenuItem * pEntry );
		bool		deleteItem( int pos );
		void		deleteAllItems();
	
		int			getItemPos( WgMenuItem* pEntry );
		WgMenuItem*	getItem( int pos );
		int			getItemCount() const { return (int)m_items.size(); }
	
	
		WgMenuItem *FindItem( int id );
		WgMenuItem *GetFirstItem() const { return m_items.first(); }
		WgMenuItem *GetLastItem() const { return m_items.last(); }
		WgMenuItem *GetSelectedItem() const { return m_pSelectedItem; }
	
		void		selectItem(WgMenuItem* pItem);
	
	
	
		//____ Overloaded from WgWidget & WgPanel ___________________________
	
		bool		removeWidget(const WgWidget_p& pWidget ) { return false; }
		bool		clear() { return false; }
	
		int			matchingWidth( int height ) const;
	
		WgSize		preferredSize() const;
	
		//
	
	protected:
		WgMenu();
		virtual ~WgMenu();
		virtual WgWidget* _newOfMyType() const { return new WgMenu(); };
	
	private:
	
		class ScrollbarHook : public WgHook
		{
			friend class WgMenu;
	
		public:
			const char *type( void ) const;
			static const char * classType();
	
			// Standard Hook methods
	
			WgCoord		pos() const;
			WgSize		size() const;
			WgRect		geo() const;
	
			WgCoord		globalPos() const;
			WgRect		globalGeo() const;
	
			bool		setVisible( bool bVisible ) { return false; }
			WgMenu* parent() const { return m_pParent; }
	
			WgScrollbar * Scrollbar() { return m_pWidget?static_cast<WgScrollbar*>(m_pWidget):0; }
	
	
		protected:
			ScrollbarHook() : m_pParent(0) {}
	
			void			_requestRender();
			void			_requestRender( const WgRect& rect );
			void			_requestResize();
	
			WgHook *		_prevHook() const { return 0; }
			WgHook *		_nextHook() const { return 0; }
			WgContainer *	_parent() const { return m_pParent; }
	
			WgMenu * 		m_pParent;
			WgSize			m_size;
	
		};
	
		//
		WgWidget * 	_findWidget( const WgCoord& ofs, WgSearchMode mode );
	
		void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
		void		_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
		void		_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
		void		_onCloneContent( const WgWidget * _pOrg );
		void		_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void		_onNewSize( const WgSize& size );
		void		_onRefresh();
	
		void		_onMsg( const WgMsg_p& pMsg );
		void		_onStateChanged( WgState oldState );
		bool		_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
	
		//
	
		WgHook*		_firstHook() const;
		WgHook*		_lastHook() const { return _firstHook(); }
	
		WgHook *	_firstHookWithGeo( WgRect& writeGeo ) const;
		WgHook *	_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const { return 0; }
	
		WgHook *	_lastHookWithGeo( WgRect& writeGeo ) const { return _firstHookWithGeo(writeGeo); }
		WgHook *	_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const { return 0; }
	
		//
	
		void		_itemModified();
		void		_adjustSize();
		WgRect		_scrollbarGeo( const WgRect& menuGeo ) const;
	
		// Overriden for WgScrollbarTarget
	
		float		_stepFwd();
		float		_stepBwd();
		float		_jumpFwd();
		float		_jumpBwd();
		float		_wheelRolled( int distance );
	
		float		_setPosition( float fraction );
	
		WgWidget*	_getWidget();
		float		_getHandlePosition();
		float		_getHandleSize();
	
		//
	
		WgMenuItem *_getItemAtPos( int posX, int posY );
		void		_calcEntryMinWidth( WgMenuEntry * pEntry );
		void		_itemSelected();
	
		void		_openSubMenu( WgMenuSubMenu * pItem );
		void		_closeSubMenu( WgMenuSubMenu * pItem );
	
		WgBorder	_getPadding() const;
		void		_refreshEntryHeight();
	
		void		_scrollItemIntoView( WgMenuItem * pItem, bool bForceAtTop = false );
		void		_markFirstFilteredEntry();
	
		int			_getViewSize();
		void		_setViewOfs(int pos);
	
		//____ Members ___________________________________________
	
		ScrollbarHook			m_scrollbarHook;
		WgSize					m_defaultSize;
	
		// Members defining items.
	
		WgChain<WgMenuItem>		m_items;
		Uint32					m_nItems;
		Uint32					m_markedItem;		// 0 = no item is selected.
		WgMenuItem*				m_pSelectedItem;
		WgMenuSubMenu*			m_pOpenSubMenu;		// Pointer at open submenu (if any).
	
		// Members holding data for open menu
	
		int						m_contentHeight;	// Total height of content in pixels.
		int						m_contentOfs;		// Offset in pixels of content displayed.
	
		const static int		c_maxSelectorKeys = 20;
		const static int		c_selectorCountdownStart = 1000;
	
		Uint16					m_selectorKeys[c_maxSelectorKeys];	// Filter for marking entry on keyboard input.
		int						m_nSelectorKeys;					// Number of characters in the filter.
		int						m_selectorCountdown;				// Countdown in ms for clearing filter due to no keyboard input.
	
		// Members defining background
	
		WgSkin_p				m_pEntrySkin;
	
		// Members defining content layout
	
		int						m_iconFieldWidth;
		int						m_arrowFieldWidth;
	
		// Members defining separators
	
		WgSkin_p				m_pSeparatorSkin;
		WgBorder				m_sepBorder;
	
		// Members defining the arrow for submenus
	
		WgGfxAnim_p			m_pArrowAnim;
		Uint32					m_arrowAnimCount;	// Animation position in milliseconds.
	
		//
	
		Uint8					m_entryHeight;		// Height of a menu entry >= font height.
		Uint8					m_sepHeight;		// Height of a separator menu entry.
	
		WgTextprop_p			m_pEntryProp;		// Default text properties for entry.
		WgTextprop_p			m_pKeyAccelProp;	// Default text properties for keyboard shortcuts.
	
		// Members defining checkbox entries
	
		WgSkin_p				m_pCheckBoxSkin;
	
		// Members defining radiobutton entries.
	
		WgSkin_p				m_pRadioButtonSkin;
	
		// Members defining dragbar.
	
		WgSkin_p					m_pScrollbarBgSkin;
		WgSkin_p					m_pScrollbarHandleSkin;
		WgSkin_p					m_pScrollbarBtnFwdSkin;
		WgSkin_p					m_pScrollbarBtnBwdSkin;
		WgScrollbar::BtnLayout		m_scrollbarBtnLayout;
	};
	
	

} // namespace wg
#endif //	WG_MENU_DOT_H


