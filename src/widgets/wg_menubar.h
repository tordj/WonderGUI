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

#ifndef	WG_MENUBAR_DOT_H
#define	WG_MENUBAR_DOT_H


#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_MENU_DOT_H
#	include <wg_menu.h>
#endif

namespace wg 
{
	
	
	class	WgSurface;
	class	WgFont;
	
	class	WgChar;
	
	
	class	WgMenubar;
	/*
		Att stödja:
	
		- Ikon/checkbox till vänster.
		- Accelerator till höger (innan pilen).
		- Separator.
		- Cascading menus med pil till höger.
		- Disablade entries.
		- Navigera menyerna med tangenter + keybindings.
	
		- Drop down/Pull down.
	
		- "End of menu"-section with info.
		- Baloon help.
		- Automatiskt öppna uppåt eller nedåt beroende på plats.
	
		* Press on head drar ner menyn, release på entry aktiverar entry (så fungerar alla fallen).
		* Click på öppen header samt Release/press utanför menyn stänger menyn.
		* Mouse over på annan header byter meny.
	
	
	*/
	
	//____ WgMenuBarItem __________________________________________________________
	
	class WgMenuBarItem:public WgLink
	{
	public:
		WgMenuBarItem();
		virtual ~WgMenuBarItem();
		LINK_METHODS( WgMenuBarItem );
	
		inline bool	isEnabled() { return m_bEnabled; }
		inline bool isVisible() { return m_bVisible; }
	
		void	setEnabled( bool bEnabled );
		void	setVisible( bool bVisible );
	
		WgChar *		m_pText;
		Uint16			m_navKey;
		WgMenu_p		m_pMenu;
		Uint16			m_width;		// Width of this item.
		bool			m_bEnabled;
		bool			m_bVisible;
		WgMenubar *	m_pMenuBar;		// Pointer at the menubar, needed for some callbacks.
	};
	
	class WgMenubar;
	typedef	WgStrongPtr<WgMenubar,WgWidget_p>		WgMenubar_p;
	typedef	WgWeakPtr<WgMenubar,WgWidget_wp>	WgMenubar_wp;
	
	//____ WgMenubar ____________________________________________________________
	
	class WgMenubar:public WgWidget
	{
		friend class WgMenuBarItem;
	
	public:
		static WgMenubar_p	create() { return WgMenubar_p(new WgMenubar()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgMenubar_p	cast( const WgObject_p& pObject );
	
		//____ Methods __________________________________________
	
		bool			setEntrySkin( const WgSkin_p& pSkin, const WgTextprop_p& pTextProperties );
		WgSkin_p		entrySkin() const { return m_pEntrySkin; }
		WgTextprop_p	textProp() const { return m_pTextProp; }
	
		bool			addMenu( const char * pTitle, const WgMenu_p& pMenu, Uint16 navKey = 0 );
		bool			removeMenu( const WgMenu_p& pMenu );
	
		WgMenuBarItem*	firstMenuBarItem() { return m_items.first(); }
	
		WgChar *		menuTitle(const WgMenu_p& pMenu) const;
	
		bool			showMenu(const WgMenu_p& pMenu);
		bool			hideMenu(const WgMenu_p& pMenu);
	
		WgSize			preferredSize() const;
	
	
	protected:
		WgMenubar();
		virtual ~WgMenubar();
		virtual WgWidget* _newOfMyType() const { return new WgMenubar(); };
	
		//TODO: Should handle disable/enable (close open menu?) and cloning.
	
		void	_onMsg( const WgMsg_p& pMsg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
		void	_onRefresh();
		void	_onCloneContent( const WgWidget * _pOrg );
	
	
	private:
	
		WgBorder	_getEntryBorder() const;
	
		Uint32		_getItemAtAbsPos( int x, int y );
		bool		_openMenu( int nb );
		bool		_closeMenu( int nb );
	
		static void	_cbMoveOutsideModal( void * pWdg, int x, int y ) { ((WgMenubar*)pWdg)->_moveOutsideModal(x,y);}
		static void	_cbMenuClosed( void * pItem ) { ((WgMenuBarItem*)pItem)->m_pMenuBar->_menuClosed((WgMenuBarItem*)pItem);}
		static void	_cbMenuOpened( void * pItem ) { ((WgMenuBarItem*)pItem)->m_pMenuBar->_menuOpened((WgMenuBarItem*)pItem);}
	
		void		_moveOutsideModal( int x, int y );
		void		_menuOpened( WgMenuBarItem * pItem );
		void		_menuClosed( WgMenuBarItem * pItem );
	
		WgChain<WgMenuBarItem>	m_items;
		Uint32					m_selectedItem;		// 0 = no item is selected.
		Uint32					m_markedItem;		// 0 = no item is marked:
	
		WgSkin_p				m_pEntrySkin;
		WgTextprop_p			m_pTextProp;
	};
	
	

} // namespace wg
#endif //	WG_MENUBAR_DOT_H
