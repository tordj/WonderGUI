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

#ifndef	WG_GIZMO_MENUBAR_DOT_H
#define	WG_GIZMO_MENUBAR_DOT_H


#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
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
class	Wdg_Menu;
class	WgFont;

class	WgChar;


class	WgGizmoMenubar;
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

	void	Enable();
	void	Disable();
	void	Show();
	void	Hide();


	WgChar *		m_pText;
	Uint16			m_navKey;
	Wdg_Menu *		m_pMenu;
	Uint16			m_width;		// Width of this item.
	bool			m_bEnabled;
	bool			m_bVisible;
	WgGizmoMenubar *	m_pMenuBar;		// Pointer at the menubar, needed for some callbacks.
};


//____ WgGizmoMenubar ____________________________________________________________

class WgGizmoMenubar:public WgGizmo
{
	friend class WgMenuBarItem;

public:
	WgGizmoMenubar();
	~WgGizmoMenubar();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	bool			SetBgSource( const WgBlockSetPtr& pBlocks );
	WgBlockSetPtr	GetBgSource() const { return m_pBgGfx; }

	bool			SetEntrySource( const WgBlockSetPtr& pBlocks, const WgTextPropPtr& pTextProperties );

	WgBlockSetPtr	GetEntrySource() const { return m_pEntryGfx; }
	WgTextPropPtr	GetTextProp() const { return m_pTextProp; }

	bool			AddMenu( const char * pTitle, Wdg_Menu * pMenu, Uint16 navKey = 0 );
	bool			RemoveMenu( Wdg_Menu * pMenu );

	WgMenuBarItem*	GetFirstMenuBarItem() { return m_items.First(); }

	WgChar *		GetMenuTitle(Wdg_Menu * pMenu) const;

	bool			ShowMenu(Wdg_Menu * pMenu);
	bool			HideMenu(Wdg_Menu * pMenu);

protected:
	//TODO: Should handle disable/enable (close open menu?) and cloning.

	void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onRefresh();
	void	_onCloneContent( const WgGizmo * _pOrg );


private:

	WgBorders	GetEntryBorders() const;

	Uint32		GetItemAtAbsPos( int x, int y );
	bool		OpenMenu( Uint32 nb );
	bool		CloseMenu( Uint32 nb );

	static void	cbMoveOutsideModal( void * pWdg, int x, int y ) { ((WgGizmoMenubar*)pWdg)->MoveOutsideModal(x,y);}
	static void	cbMenuClosed( void * pItem ) { ((WgMenuBarItem*)pItem)->m_pMenuBar->MenuClosed((WgMenuBarItem*)pItem);}
	static void	cbMenuOpened( void * pItem ) { ((WgMenuBarItem*)pItem)->m_pMenuBar->MenuOpened((WgMenuBarItem*)pItem);}

	void		MoveOutsideModal( int x, int y );
	void		MenuOpened( WgMenuBarItem * pItem );
	void		MenuClosed( WgMenuBarItem * pItem );

	WgChain<WgMenuBarItem>	m_items;
	Uint32					m_selectedItem;		// 0 = no item is selected.
	Uint32					m_markedItem;		// 0 = no item is marked:

	WgBlockSetPtr			m_pBgGfx;
	WgBlockSetPtr			m_pEntryGfx;
	WgTextPropPtr			m_pTextProp;
};


#endif //	WG_GIZMO_MENUBAR_DOT_H
