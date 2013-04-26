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

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif


class	WgSurface;
class	WgMenu;
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

	inline bool	IsEnabled() { return m_bEnabled; }
	inline bool IsVisible() { return m_bVisible; }

	void	SetEnabled( bool bEnabled );
	void	SetVisible( bool bVisible );

	WgChar *		m_pText;
	Uint16			m_navKey;
	WgMenu *		m_pMenu;
	Uint16			m_width;		// Width of this item.
	bool			m_bEnabled;
	bool			m_bVisible;
	WgMenubar *	m_pMenuBar;		// Pointer at the menubar, needed for some callbacks.
};


//____ WgMenubar ____________________________________________________________

class WgMenubar:public WgWidget
{
	friend class WgMenuBarItem;

public:
	WgMenubar();
	~WgMenubar();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgMenubar(); };	

	//____ Methods __________________________________________

	bool			SetBgSource( const WgBlocksetPtr& pBlocks );
	WgBlocksetPtr	GetBgSource() const { return m_pBgGfx; }

	bool			SetEntrySource( const WgBlocksetPtr& pBlocks, const WgTextpropPtr& pTextProperties );

	WgBlocksetPtr	GetEntrySource() const { return m_pEntryGfx; }
	WgTextpropPtr	GetTextProp() const { return m_pTextProp; }

	bool			AddMenu( const char * pTitle, WgMenu * pMenu, Uint16 navKey = 0 );
	bool			RemoveMenu( WgMenu * pMenu );

	WgMenuBarItem*	GetFirstMenuBarItem() { return m_items.First(); }

	WgChar *		GetMenuTitle(WgMenu * pMenu) const;

	bool			ShowMenu(WgMenu * pMenu);
	bool			HideMenu(WgMenu * pMenu);

	WgSize			PreferredSize() const;


protected:
	//TODO: Should handle disable/enable (close open menu?) and cloning.

	void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
//	void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onRefresh();
	void	_onCloneContent( const WgWidget * _pOrg );


private:

	WgBorders	GetEntryBorders() const;

	Uint32		GetItemAtAbsPos( int x, int y );
	bool		OpenMenu( Uint32 nb );
	bool		CloseMenu( Uint32 nb );

	static void	cbMoveOutsideModal( void * pWdg, int x, int y ) { ((WgMenubar*)pWdg)->MoveOutsideModal(x,y);}
	static void	cbMenuClosed( void * pItem ) { ((WgMenuBarItem*)pItem)->m_pMenuBar->MenuClosed((WgMenuBarItem*)pItem);}
	static void	cbMenuOpened( void * pItem ) { ((WgMenuBarItem*)pItem)->m_pMenuBar->MenuOpened((WgMenuBarItem*)pItem);}

	void		MoveOutsideModal( int x, int y );
	void		MenuOpened( WgMenuBarItem * pItem );
	void		MenuClosed( WgMenuBarItem * pItem );

	WgChain<WgMenuBarItem>	m_items;
	Uint32					m_selectedItem;		// 0 = no item is selected.
	Uint32					m_markedItem;		// 0 = no item is marked:

	WgBlocksetPtr			m_pBgGfx;
	WgBlocksetPtr			m_pEntryGfx;
	WgTextpropPtr			m_pTextProp;
};


#endif //	WG_MENUBAR_DOT_H
