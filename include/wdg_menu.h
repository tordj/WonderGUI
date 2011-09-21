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

#ifndef	WDG_MENU_DOT_H
#define	WDG_MENU_DOT_H


#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_MENUITEM_DOT_H
#	include <wg_menuitem.h>
#endif

#ifndef	WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WDG_DRAGBARS_DOT_H
#	include <wdg_dragbars.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

class	WgSurface;
class	WgFont;
class	WgGfxAnim;
class	WgText;
class	Wdg_VDrag;



class Wdg_Menu:public WgWidget, public WgEmitter
{
	friend class WgMenuItem;
	friend class WgMenuSubMenu;

public:
	WIDGET_CONSTRUCTORS(Wdg_Menu,WgWidget)
	~Wdg_Menu();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Callbacks _________________________________________

	static void cbOpen(void * pWdg) { ((Wdg_Menu*)pWdg)->Open(); }

	//____ Methods ___________________________________________

	bool			SetBgSource( const WgBlockSetPtr pBgGfx, Uint8 iconFieldWidth, Uint8 arrowFieldWidth );

	WgBlockSetPtr	GetBgSource() const				{ return m_pBgGfx; }
	Uint8			GetIconFieldWidth() const		{ return m_iconFieldWidth; }
	Uint8			GetArrowFieldWidth() const		{ return m_arrowFieldWidth; }

	bool			SetSeparatorSource( const WgBlockSetPtr pGfx, const WgBorders& sepBorders = WgBorders() );
	WgBlockSetPtr	GetSeparatorSource() const { return m_pSepGfx; }
	WgBorders		GetSeparatorBorders() const { return m_sepBorders; }

	bool			SetMarkSource( const WgBlockSetPtr pGfx, const WgBorders& markBorders = WgBorders() );
	WgBlockSetPtr	GetMarkSource() const { return m_pMarkGfx; }
	WgBorders		GetMarkBorders() const { return m_markBorders; }

	bool			SetCheckBoxSource( const WgBlockSetPtr pUnchecked, const WgBlockSetPtr pChecked );
	WgBlockSetPtr	GetCheckedSource() const { return m_pCbGfxChecked; }
	WgBlockSetPtr	GetUncheckedSource() const { return m_pCbGfxUnchecked; }

	bool			SetRadioButtonSource( const WgBlockSetPtr pUnchecked, const WgBlockSetPtr pChecked );
	WgBlockSetPtr 	GetRadioCheckedSource() const { return m_pRbGfxSelected; }
	WgBlockSetPtr 	GetRadioUncheckedSource() const { return m_pRbGfxUnselected; }

	bool			SetArrowSource( WgGfxAnim * pAnim );

	bool			SetTextProperties( const WgTextPropPtr& pEntryProperties, const WgTextPropPtr& pKeyAccelProperties );
	WgTextPropPtr	GetTextEntryProperties() const { return m_pEntryProp; }
	WgTextPropPtr	GetKeyAccelProperties() const { return m_pKeyAccelProp; }

	bool	SetSliderSource(  WgBlockSetPtr pBgGfx, WgBlockSetPtr pBarGfx, WgBlockSetPtr pBtnBwdGfx, WgBlockSetPtr pBtnFwdGfx );
	bool	SetSliderButtonLayout(  WgGizmoDragbar::ButtonLayout layout );
	WgGizmoDragbar::ButtonLayout GetSliderButtonLayout() const { return m_sliderBtnLayout; }
	WgBlockSetPtr GetSliderBgSource() const { return m_pSliderBgGfx; }
	WgBlockSetPtr GetSliderBarSource() const { return m_pSliderBarGfx; }
	WgBlockSetPtr GetSliderBwdSource() const { return m_pSliderBtnBwdGfx; }
	WgBlockSetPtr GetSliderFwdSource() const { return m_pSliderBtnFwdGfx; }

	void	SetOpenParams( Wdg_Root * pRoot, Uint32 x, Uint32 y, Uint32 minW = 0, const WgRect *releaseArea = 0, Wdg_Menu * pParentMenu = 0 );

	int		GetEntryHeight() const;
	bool	SetEntryHeight( Uint8 height );


	int			AddItem( WgMenuItem * pEntry );
	int			InsertItem( WgMenuItem * pEntry, int pos );

	bool		RemoveItem( WgMenuItem * pEntry );
	WgMenuItem*	RemoveItem( int pos );
	void		RemoveAllItems();

	bool		DeleteItem( WgMenuItem * pEntry );
	bool		DeleteItem( int pos );
	void		DeleteAllItems();

	int			GetItemPos( WgMenuItem* pEntry );
	WgMenuItem*	GetItem( int pos );
	int			GetItemCount() const { return (int)m_items.Size(); }


	WgMenuItem *FindItem( int id );
	WgMenuItem *GetFirstItem() const { return m_items.First(); }
	WgMenuItem *GetSelectedItem() const { return m_pSelectedItem; }

	void	SelectItem(WgMenuItem* pItem);

	void	Open( );
	void	Open( Wdg_Root * pRoot, Uint32 x, Uint32 y, Uint32 minW = 0, WgMenuItem * pMarkedItem = 0, WgRect * pBtnReleaseArea = 0, Wdg_Menu * pParentMenu = 0 );
	void	Close();

	//____ Methods __________________________________________

	// This widget controls its own size and modalness...

	void	Modal() {}
	void	UnModal() {}
	bool	SetSize( int _w, int _h, bool bAllowMoveChildren = false ) { return false; }
	bool	SetWidth( int _w, bool bAllowMoveChildren = false ) { return false; }
	bool	SetHeight( int _h, bool bAllowMoveChildren = false ) { return false; }
	bool	Resize( int _w, int _h, bool bAllowMoveChildren = false ) { return false; }
	bool	ResizeWidth( int _w, bool bAllowMoveChildren = false ) { return false; }
	bool	ResizeHeight( int _h, bool bAllowMoveChildren = false ) { return false; }
	bool	SetGeometry(WgOrigo _origo, const WgRect &_geometry, bool bEmit = true ) { return false; }
	bool	SetGeometry( WgOrigo _topLeft, Sint32 x1, Sint32 y1, WgOrigo _bottomRight, Sint32 x2, Sint32 y2, bool bEmit = true ) { return false; }

	//

	void	AdjustSize();


protected:
	WgWidget * NewOfMyType() const;
	WgEmitter * GetEmitter() { return this; }
private:
	void		Init();
	void		DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 _layer );
	void		DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
	void		DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void		DoMyOwnInputFocusChange( bool _bFocus );
	void		DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo );

/*
	void		DoMyOwnRefresh( void );
	void		DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
    void 		DoMyOwnDisOrEnable( void );
*/
	bool		DoMyOwnMarkTest( int _x, int _y );

	WgMenuItem *GetItemAtAbsPos( int absX, int absY );
	void		CalcEntryMinWidth( WgMenuEntry * pEntry );
	void		OpenSubMenu( WgMenuSubMenu * pItem );
	void		CloseSubMenu( Uint32 item );


	static void	cbMoveOutsideModal( void * pWdg, int x, int y ) { ((Wdg_Menu*)pWdg)->MoveOutsideModal(x,y);}
//	static void	cbSubMenuClosed( void * pItem ) { ((WgMenuSubMenu*)pItem)->m_pParentMenu->SubMenuClosed((WgMenuSubMenu*)pItem);}
//	static void	cbSubMenuOpened( void * pItem ) { ((WgMenuSubMenu*)pItem)->m_pParentMenu->SubMenuOpened((WgMenuSubMenu*)pItem);}

	void		MoveOutsideModal( int x, int y );
//	void		SubMenuOpened( WgMenuSubMenu * pItem );
//	void		SubMenuClosed( WgMenuSubMenu * pItem );

	WgBorders	GetContentBorders() const;

	void		ScrollItemIntoView( WgMenuItem * pItem, bool bForceAtTop = false );
	void		MarkFirstFilteredEntry();

	float		GetViewOfs();
	float		GetViewSize();
	int			GetViewSizePixels();

	void		SetView(float pos);
	void		SetViewPixels(int pos);

	void		StepWheelRoll(int distance);
	void		StepViewDown();
	void		StepViewUp();
	void		StepViewPageDown();
	void		StepViewPageUp();


	static void cbSliderPos( void * pMenu, float pos ) { ((Wdg_Menu*)pMenu)->SetView(pos); }
	static void cbSliderFwd( void * pMenu ) { ((Wdg_Menu*)pMenu)->StepViewDown(); }
	static void cbSliderBwd( void * pMenu ) { ((Wdg_Menu*)pMenu)->StepViewUp(); }
	static void cbSliderNextPage( void * pMenu ) { ((Wdg_Menu*)pMenu)->StepViewPageDown(); }
	static void cbSliderPrevPage( void * pMenu ) { ((Wdg_Menu*)pMenu)->StepViewPageUp(); }

	static void cbPressOnSlider( void * pMenu ) { ((Wdg_Menu*)pMenu)->m_bPressOnSlider = true; }
	static void cbReleasedSlider( void * pMenu ) { ((Wdg_Menu*)pMenu)->m_bPressOnSlider = false; }

	static void cbWheelRoll( void * pMenu, int distance ) { ((Wdg_Menu*)pMenu)->StepWheelRoll(distance); }


	//____ Members ___________________________________________

	bool	m_bIsOpeningSubMenu;				// Lets us know if we lose focus due to submenu opening or not.


	// Members defining items.

	WgChain<WgMenuItem>		m_items;
	Uint32					m_nItems;
	Uint32					m_markedItem;		// 0 = no item is selected.
	WgMenuItem*				m_pSelectedItem;

	// Members defining menu opening

	Wdg_Root *				m_pMenuRoot;
	Wdg_Menu *				m_pParentMenu;
	Uint16					m_menuPosX;
	Uint16					m_menuPosY;
	Uint16					m_menuMinW;			// Minimum width requested on open, overriden by size-limits.
	WgRect					m_btnReleaseArea;	// Mousebutton can be released in this area (abs pos) without
												// closing the menu.

	// Members holding data for open menu

	Wdg_VDrag *				m_pSlider;
	Uint32					m_contentHeight;	// Total height of content in pixels.
	Uint32					m_contentOfs;		// Offset in pixels of content displayed.
	bool					m_bPressOnSlider;	// Set if button 1 is down and was pressed on slider.
	WgCoord					m_savedMousePos;	// Holds absolute mouse position so we know when it has moved (only highlight item under pointer when it's moved).

	const static int		c_maxSelectorKeys = 20;
	const static int		c_selectorCountdownStart = 1000;

	Uint16					m_selectorKeys[c_maxSelectorKeys];	// Filter for marking entry on keyboard input.
	int						m_nSelectorKeys;					// Number of characters in the filter.
	int						m_selectorCountdown;				// Countdown in ms for clearing filter due to no keyboard input.

	// Members defining background

	WgBlockSetPtr			m_pBgGfx;

	// Members defining content layout

	Uint8					m_iconFieldWidth;
	Uint8					m_arrowFieldWidth;

	// Members defining separators

	WgBlockSetPtr			m_pSepGfx;
	WgBorders				m_sepBorders;

	// Members defining the mark-rectangle

	WgBlockSetPtr			m_pMarkGfx;
	WgBorders				m_markBorders;

	// Members defining the arrow for submenus

	WgGfxAnim *				m_pArrowAnim;
	Uint32					m_arrowAnimCount;	// Animation position in milliseconds.

	//

	Uint8					m_entryHeight;		// Height of a menu entry >= font height.
	Uint8					m_sepHeight;		// Height of a separator menu entry.

	WgTextPropPtr			m_pEntryProp;		// Default text properties for entry.
	WgTextPropPtr			m_pKeyAccelProp;	// Default text properties for keyboard shortcuts.

	// Members defining checkbox entries

	WgBlockSetPtr			m_pCbGfxChecked;	// Normal, Marked, Disabled.
	WgBlockSetPtr			m_pCbGfxUnchecked;	// Normal, Marked, Disabled.

	// Members defining radiobutton entries.

	WgBlockSetPtr			m_pRbGfxSelected;	// Normal, Marked, Disabled.
	WgBlockSetPtr			m_pRbGfxUnselected;	// Normal, Marked, Disabled.

	// Members defining dragbar.

	WgBlockSetPtr				m_pSliderBgGfx;
	WgBlockSetPtr				m_pSliderBarGfx;
	WgBlockSetPtr				m_pSliderBtnFwdGfx;
	WgBlockSetPtr				m_pSliderBtnBwdGfx;
	WgGizmoDragbar::ButtonLayout	m_sliderBtnLayout;
};


#endif //	WDG_MENU_DOT_H


