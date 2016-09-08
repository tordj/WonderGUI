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
	
	
	class	Surface;
	class	Font;
	
	class	Char;
	
	
	class	Menubar;
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
	
	//____ MenuBarItem __________________________________________________________
	
	class MenuBarItem:public Link
	{
	public:
		MenuBarItem();
		virtual ~MenuBarItem();
		LINK_METHODS( MenuBarItem );
	
		inline bool	isEnabled() { return m_bEnabled; }
		inline bool isVisible() { return m_bVisible; }
	
		void	setEnabled( bool bEnabled );
		void	setVisible( bool bVisible );
	
		Char *		m_pText;
		uint16_t			m_navKey;
		Menu_p		m_pMenu;
		uint16_t			m_width;		// Width of this item.
		bool			m_bEnabled;
		bool			m_bVisible;
		Menubar *	m_pMenuBar;		// Pointer at the menubar, needed for some callbacks.
	};
	
	class Menubar;
	typedef	StrongPtr<Menubar,Widget_p>		Menubar_p;
	typedef	WeakPtr<Menubar,Widget_wp>	Menubar_wp;
	
	//____ Menubar ____________________________________________________________
	
	class Menubar:public Widget
	{
		friend class MenuBarItem;
	
	public:
		static Menubar_p	create() { return Menubar_p(new Menubar()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Menubar_p	cast( const Object_p& pObject );
	
		//____ Methods __________________________________________
	
		bool			setEntrySkin( const Skin_p& pSkin, const Textprop_p& pTextProperties );
		Skin_p		entrySkin() const { return m_pEntrySkin; }
		Textprop_p	textProp() const { return m_pTextProp; }
	
		bool			addMenu( const char * pTitle, const Menu_p& pMenu, uint16_t navKey = 0 );
		bool			removeMenu( const Menu_p& pMenu );
	
		MenuBarItem*	firstMenuBarItem() { return m_items.first(); }
	
		Char *		menuTitle(const Menu_p& pMenu) const;
	
		bool			showMenu(const Menu_p& pMenu);
		bool			hideMenu(const Menu_p& pMenu);
	
		Size			preferredSize() const;
	
	
	protected:
		Menubar();
		virtual ~Menubar();
		virtual Widget* _newOfMyType() const { return new Menubar(); };
	
		//TODO: Should handle disable/enable (close open menu?) and cloning.
	
		void	_receive( const Msg_p& pMsg );
		void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		bool	_alphaTest( const Coord& ofs );
		void	_refresh();
		void	_cloneContent( const Widget * _pOrg );
	
	
	private:
	
		Border	_getEntryBorder() const;
	
		uint32_t		_getItemAtAbsPos( int x, int y );
		bool		_openMenu( int nb );
		bool		_closeMenu( int nb );
	
		static void	_cbMoveOutsideModal( void * pWdg, int x, int y ) { ((Menubar*)pWdg)->_moveOutsideModal(x,y);}
		static void	_cbMenuClosed( void * pItem ) { ((MenuBarItem*)pItem)->m_pMenuBar->_menuClosed((MenuBarItem*)pItem);}
		static void	_cbMenuOpened( void * pItem ) { ((MenuBarItem*)pItem)->m_pMenuBar->_menuOpened((MenuBarItem*)pItem);}
	
		void		_moveOutsideModal( int x, int y );
		void		_menuOpened( MenuBarItem * pItem );
		void		_menuClosed( MenuBarItem * pItem );
	
		Chain<MenuBarItem>	m_items;
		uint32_t					m_selectedItem;		// 0 = no item is selected.
		uint32_t					m_markedItem;		// 0 = no item is marked:
	
		Skin_p				m_pEntrySkin;
		Textprop_p			m_pTextProp;
	};
	
	

} // namespace wg
#endif //	WG_MENUBAR_DOT_H
