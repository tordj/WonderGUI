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

class	WgSurface;
class	WgFont;
class	WgGfxAnim;
class	WgText;

class WgMenu;
typedef	WgSmartChildPtr<WgMenu,WgWidgetPtr>		WgMenuPtr;
typedef	WgWeakChildPtr<WgMenu,WgWidgetWeakPtr>	WgMenuWeakPtr;

class WgMenu:public WgPanel, private WgScrollbarTarget
{
	friend class WgMenuItem;
	friend class WgMenuSubMenu;

public:
	static WgMenuPtr	Create() { return WgMenuPtr(new WgMenu()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgMenuPtr	Cast( const WgObjectPtr& pObject );

	//____ Methods ___________________________________________

	bool			SetSkin( const WgSkinPtr& pSkin, int iconFieldWidth, int arrowFieldWidth );
	int				IconFieldWidth() const			{ return m_iconFieldWidth; }
	int				ArrowFieldWidth() const			{ return m_arrowFieldWidth; }

	void			SetEntrySkin( const WgSkinPtr& pSkin );
	WgSkinPtr		EntrySkin() const { return m_pEntrySkin; }

	bool			SetSeparatorSkin( const WgSkinPtr& pSkin, const WgBorders& sepBorders = WgBorders() );
	WgSkinPtr		SeparatorSkin() const { return m_pSeparatorSkin; }
	WgBorders		SeparatorBorders() const { return m_sepBorders; }

	bool			SetCheckBoxSkin( const WgSkinPtr& pSkin );
	WgSkinPtr		CheckBoxSkin() const { return m_pCheckBoxSkin; }

	bool			SetRadioButtonSkin( const WgSkinPtr& pSkin );
	WgSkinPtr 		RadioButtonSkin() const { return m_pRadioButtonSkin; }

	bool			SetArrowSource( WgGfxAnim * pAnim );

	bool			SetTextProperties( const WgTextpropPtr& pEntryProperties, const WgTextpropPtr& pKeyAccelProperties );
	WgTextpropPtr	GetTextEntryProperties() const { return m_pEntryProp; }
	WgTextpropPtr	GetKeyAccelProperties() const { return m_pKeyAccelProp; }

	bool			SetScrollbarSkins(  const WgSkinPtr& pBackgroundSkin, const WgSkinPtr& pHandleSkin, const WgSkinPtr& pBwdButtonSkin, const WgSkinPtr& pFwdButtonSkin );
	bool			SetScrollbarButtonLayout(  WgScrollbar::ButtonLayout layout );
	WgScrollbar::ButtonLayout GetScrollbarButtonLayout() const { return m_scrollbarBtnLayout; }
	WgSkinPtr 		GetScrollbarBackgroundSkin() const { return m_pScrollbarBgSkin; }
	WgSkinPtr 		GetScrollbarHandleSkin() const { return m_pScrollbarHandleSkin; }
	WgSkinPtr 		GetScrollbarBwdButtonSkin() const { return m_pScrollbarBtnBwdSkin; }
	WgSkinPtr 		GetScrollbarFwdButtonSkin() const { return m_pScrollbarBtnFwdSkin; }

	int			GetEntryHeight() const;

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
	WgMenuItem *GetLastItem() const { return m_items.Last(); }
	WgMenuItem *GetSelectedItem() const { return m_pSelectedItem; }

	void		SelectItem(WgMenuItem* pItem);



	//____ Overloaded from WgWidget & WgPanel ___________________________

	bool		RemoveChild(const WgWidgetPtr& pChild ) { return false; }
	bool		Clear() { return false; }

	int			WidthForHeight( int height ) const;

	WgSize		PreferredSize() const;

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
		const char *Type( void ) const;
		static const char * ClassType();

		// Standard Hook methods

		WgCoord		Pos() const;
		WgSize		Size() const;
		WgRect		Geo() const;

		WgCoord		ScreenPos() const;
		WgRect		ScreenGeo() const;

		bool		SetVisible( bool bVisible ) { return false; }
		WgMenu* Parent() const { return m_pParent; }

		WgVScrollbar * Scrollbar() { return m_pWidget?static_cast<WgVScrollbar*>(m_pWidget):0; }


	protected:
		ScrollbarHook() : m_pParent(0) {}

		void		_requestRender();
		void		_requestRender( const WgRect& rect );
		void		_requestResize();

		WgHook *	_prevHook() const { return 0; }
		WgHook *	_nextHook() const { return 0; }
		WgWidgetHolder * _holder() const { return m_pParent; }
		WgContainer * _parent() const { return m_pParent; }

		WgMenu * 	m_pParent;
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

	void		_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	void		_onStateChanged( WgState oldState, WgState newState );
	bool		_onAlphaTest( const WgCoord& ofs );

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

	WgBorders	_getPadding() const;
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

	WgSkinPtr				m_pEntrySkin;

	// Members defining content layout

	int						m_iconFieldWidth;
	int						m_arrowFieldWidth;

	// Members defining separators

	WgSkinPtr				m_pSeparatorSkin;
	WgBorders				m_sepBorders;

	// Members defining the arrow for submenus

	WgGfxAnim *				m_pArrowAnim;
	Uint32					m_arrowAnimCount;	// Animation position in milliseconds.

	//

	Uint8					m_entryHeight;		// Height of a menu entry >= font height.
	Uint8					m_sepHeight;		// Height of a separator menu entry.

	WgTextpropPtr			m_pEntryProp;		// Default text properties for entry.
	WgTextpropPtr			m_pKeyAccelProp;	// Default text properties for keyboard shortcuts.

	// Members defining checkbox entries

	WgSkinPtr				m_pCheckBoxSkin;

	// Members defining radiobutton entries.

	WgSkinPtr				m_pRadioButtonSkin;

	// Members defining dragbar.

	WgSkinPtr					m_pScrollbarBgSkin;
	WgSkinPtr					m_pScrollbarHandleSkin;
	WgSkinPtr					m_pScrollbarBtnFwdSkin;
	WgSkinPtr					m_pScrollbarBtnBwdSkin;
	WgScrollbar::ButtonLayout	m_scrollbarBtnLayout;
};


#endif //	WG_MENU_DOT_H


