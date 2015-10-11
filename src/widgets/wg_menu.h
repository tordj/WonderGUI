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
	
	class	Surface;
	class	Font;
	class	GfxAnim;
	
	class Menu;
	typedef	WgStrongPtr<Menu,Widget_p>		Menu_p;
	typedef	WgWeakPtr<Menu,Widget_wp>	Menu_wp;
	
	class Menu:public Panel, private ScrollbarTarget
	{
		friend class MenuItem;
		friend class MenuSubMenu;
	
	public:
		static Menu_p	create() { return Menu_p(new Menu()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Menu_p	cast( const Object_p& pObject );
	
		//____ Methods ___________________________________________
	
		bool			setSkin( const Skin_p& pSkin, int iconFieldWidth, int arrowFieldWidth );
		int				iconFieldWidth() const			{ return m_iconFieldWidth; }
		int				arrowFieldWidth() const			{ return m_arrowFieldWidth; }
	
		void			setEntrySkin( const Skin_p& pSkin );
		Skin_p		entrySkin() const { return m_pEntrySkin; }
	
		bool			setSeparatorSkin( const Skin_p& pSkin, const Border& sepBorder = Border() );
		Skin_p		separatorSkin() const { return m_pSeparatorSkin; }
		Border		separatorBorder() const { return m_sepBorder; }
	
		bool			setCheckBoxSkin( const Skin_p& pSkin );
		Skin_p		checkBoxSkin() const { return m_pCheckBoxSkin; }
	
		bool			setRadioButtonSkin( const Skin_p& pSkin );
		Skin_p 		radioButtonSkin() const { return m_pRadioButtonSkin; }
	
		bool			setArrowSource( const GfxAnim_p& pAnim );
	
		bool			setTextProperties( const Textprop_p& pEntryProperties, const Textprop_p& pKeyAccelProperties );
		Textprop_p	getTextEntryProperties() const { return m_pEntryProp; }
		Textprop_p	getKeyAccelProperties() const { return m_pKeyAccelProp; }
	
		bool			setScrollbarSkins(  const Skin_p& pBackgroundSkin, const Skin_p& pHandleSkin, const Skin_p& pBwdButtonSkin, const Skin_p& pFwdButtonSkin );
		bool			setScrollbarButtonLayout(  Scrollbar::BtnLayout layout );
		Scrollbar::BtnLayout scrollbarButtonLayout() const { return m_scrollbarBtnLayout; }
		Skin_p 		scrollbarBackgroundSkin() const { return m_pScrollbarBgSkin; }
		Skin_p 		scrollbarHandleSkin() const { return m_pScrollbarHandleSkin; }
		Skin_p 		scrollbarBwdButtonSkin() const { return m_pScrollbarBtnBwdSkin; }
		Skin_p 		scrollbarFwdButtonSkin() const { return m_pScrollbarBtnFwdSkin; }
	
		int			getEntryHeight() const;
	
		int			addItem( MenuItem * pEntry );
		int			insertItem( MenuItem * pEntry, int pos );
	
		bool		removeItem( MenuItem * pEntry );
		MenuItem*	removeItem( int pos );
		void		removeAllItems();
	
		bool		deleteItem( MenuItem * pEntry );
		bool		deleteItem( int pos );
		void		deleteAllItems();
	
		int			getItemPos( MenuItem* pEntry );
		MenuItem*	getItem( int pos );
		int			getItemCount() const { return (int)m_items.size(); }
	
	
		MenuItem *FindItem( int id );
		MenuItem *GetFirstItem() const { return m_items.first(); }
		MenuItem *GetLastItem() const { return m_items.last(); }
		MenuItem *GetSelectedItem() const { return m_pSelectedItem; }
	
		void		selectItem(MenuItem* pItem);
	
	
	
		//____ Overloaded from Widget & Panel ___________________________
	
		bool		removeWidget(const Widget_p& pWidget ) { return false; }
		bool		clear() { return false; }
	
		int			matchingWidth( int height ) const;
	
		Size		preferredSize() const;
	
		//
	
	protected:
		Menu();
		virtual ~Menu();
		virtual Widget* _newOfMyType() const { return new Menu(); };
	
	private:
	
		class ScrollbarHook : public Hook
		{
			friend class Menu;
	
		public:
			const char *type( void ) const;
			static const char * classType();
	
			// Standard MyHook methods
	
			Coord		pos() const;
			Size		size() const;
			Rect		geo() const;
	
			Coord		globalPos() const;
			Rect		globalGeo() const;
	
			bool		setVisible( bool bVisible ) { return false; }
			Menu* parent() const { return m_pParent; }
	
			Scrollbar * scrollbar() { return m_pWidget?static_cast<Scrollbar*>(m_pWidget):0; }
	
	
		protected:
			ScrollbarHook() : m_pParent(0) {}
	
			void			_requestRender();
			void			_requestRender( const Rect& rect );
			void			_requestResize();
	
			Hook *		_prevHook() const { return 0; }
			Hook *		_nextHook() const { return 0; }
			Container *	_parent() const { return m_pParent; }
	
			Menu * 		m_pParent;
			Size			m_size;
	
		};
	
		//
		Widget * 	_findWidget( const Coord& ofs, WgSearchMode mode );
	
		void		_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches );
		void		_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip );
		void		_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, WgBlendMode blendMode );
		void		_onCloneContent( const Widget * _pOrg );
		void		_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void		_onNewSize( const Size& size );
		void		_onRefresh();
	
		void		_onMsg( const Msg_p& pMsg );
		void		_onStateChanged( State oldState );
		bool		_onAlphaTest( const Coord& ofs, const Size& sz );
	
		//
	
		Hook*		_firstHook() const;
		Hook*		_lastHook() const { return _firstHook(); }
	
		Hook *	_firstHookWithGeo( Rect& writeGeo ) const;
		Hook *	_nextHookWithGeo( Rect& writeGeo, Hook * pHook ) const { return 0; }
	
		Hook *	_lastHookWithGeo( Rect& writeGeo ) const { return _firstHookWithGeo(writeGeo); }
		Hook *	_prevHookWithGeo( Rect& writeGeo, Hook * pHook ) const { return 0; }
	
		//
	
		void		_itemModified();
		void		_adjustSize();
		Rect		_scrollbarGeo( const Rect& menuGeo ) const;
	
		// Overriden for ScrollbarTarget
	
		float		_stepFwd();
		float		_stepBwd();
		float		_jumpFwd();
		float		_jumpBwd();
		float		_wheelRolled( int distance );
	
		float		_setPosition( float fraction );
	
		Widget*	_getWidget();
		float		_getHandlePosition();
		float		_getHandleSize();
	
		//
	
		MenuItem *_getItemAtPos( int posX, int posY );
		void		_calcEntryMinWidth( MenuEntry * pEntry );
		void		_itemSelected();
	
		void		_openSubMenu( MenuSubMenu * pItem );
		void		_closeSubMenu( MenuSubMenu * pItem );
	
		Border	_getPadding() const;
		void		_refreshEntryHeight();
	
		void		_scrollItemIntoView( MenuItem * pItem, bool bForceAtTop = false );
		void		_markFirstFilteredEntry();
	
		int			_getViewSize();
		void		_setViewOfs(int pos);
	
		//____ Members ___________________________________________
	
		ScrollbarHook			m_scrollbarHook;
		Size					m_defaultSize;
	
		// Members defining items.
	
		Chain<MenuItem>		m_items;
		uint32_t					m_nItems;
		uint32_t					m_markedItem;		// 0 = no item is selected.
		MenuItem*				m_pSelectedItem;
		MenuSubMenu*			m_pOpenSubMenu;		// Pointer at open submenu (if any).
	
		// Members holding data for open menu
	
		int						m_contentHeight;	// Total height of content in pixels.
		int						m_contentOfs;		// Offset in pixels of content displayed.
	
		const static int		c_maxSelectorKeys = 20;
		const static int		c_selectorCountdownStart = 1000;
	
		uint16_t					m_selectorKeys[c_maxSelectorKeys];	// Filter for marking entry on keyboard input.
		int						m_nSelectorKeys;					// Number of characters in the filter.
		int						m_selectorCountdown;				// Countdown in ms for clearing filter due to no keyboard input.
	
		// Members defining background
	
		Skin_p				m_pEntrySkin;
	
		// Members defining content layout
	
		int						m_iconFieldWidth;
		int						m_arrowFieldWidth;
	
		// Members defining separators
	
		Skin_p				m_pSeparatorSkin;
		Border				m_sepBorder;
	
		// Members defining the arrow for submenus
	
		GfxAnim_p			m_pArrowAnim;
		uint32_t					m_arrowAnimCount;	// Animation position in milliseconds.
	
		//
	
		uint8_t					m_entryHeight;		// Height of a menu entry >= font height.
		uint8_t					m_sepHeight;		// Height of a separator menu entry.
	
		Textprop_p			m_pEntryProp;		// Default text properties for entry.
		Textprop_p			m_pKeyAccelProp;	// Default text properties for keyboard shortcuts.
	
		// Members defining checkbox entries
	
		Skin_p				m_pCheckBoxSkin;
	
		// Members defining radiobutton entries.
	
		Skin_p				m_pRadioButtonSkin;
	
		// Members defining dragbar.
	
		Skin_p					m_pScrollbarBgSkin;
		Skin_p					m_pScrollbarHandleSkin;
		Skin_p					m_pScrollbarBtnFwdSkin;
		Skin_p					m_pScrollbarBtnBwdSkin;
		Scrollbar::BtnLayout		m_scrollbarBtnLayout;
	};
	
	

} // namespace wg
#endif //	WG_MENU_DOT_H


