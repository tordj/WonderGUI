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

#ifndef	WG_GIZMO_MENU_DOT_H
#define	WG_GIZMO_MENU_DOT_H


#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef	WG_MENUITEM_DOT_H
#	include <wg_menuitem.h>
#endif

#ifndef	WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_TILEHOLDER_DOT_H
#	include	<wg_tileholder.h>
#endif

#ifndef WG_GIZMO_SLIDER_DOT_H
#	include <wg_gizmo_slider.h>
#endif

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif

#ifndef WG_SLIDERTARGET_DOT_H
#	include <wg_slidertarget.h>
#endif

class	WgSurface;
class	WgFont;
class	WgGfxAnim;
class	WgText;


class WgGizmoMenu:public WgGizmoContainer, public WgTileHolder, private WgSliderTarget
{
	friend class WgMenuitem;
	friend class WgMenuSubMenu;

public:
	WgGizmoMenu();
	virtual ~WgGizmoMenu();

	virtual const char * Type() const;
	static const char * GetClass();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoMenu(); };

	//____ Methods ___________________________________________

	bool			SetBgSource( const WgBlocksetPtr pBgGfx, Uint8 iconFieldWidth, Uint8 arrowFieldWidth );
	WgBlocksetPtr	GetBgSource() const				{ return m_pBgGfx; }
	Uint8			GetIconFieldWidth() const		{ return m_iconFieldWidth; }
	Uint8			GetArrowFieldWidth() const		{ return m_arrowFieldWidth; }

	bool			SetSeparatorSource( const WgBlocksetPtr pGfx, const WgBorders& sepBorders = WgBorders() );
	WgBlocksetPtr	GetSeparatorSource() const { return m_pSepGfx; }
	WgBorders		GetSeparatorBorders() const { return m_sepBorders; }

	bool			SetCheckBoxSource( const WgBlocksetPtr pUnchecked, const WgBlocksetPtr pChecked );
	WgBlocksetPtr	GetCheckedSource() const { return m_pCbGfxChecked; }
	WgBlocksetPtr	GetUncheckedSource() const { return m_pCbGfxUnchecked; }

	bool			SetRadioButtonSource( const WgBlocksetPtr pUnchecked, const WgBlocksetPtr pChecked );
	WgBlocksetPtr 	GetRadioCheckedSource() const { return m_pRbGfxSelected; }
	WgBlocksetPtr 	GetRadioUncheckedSource() const { return m_pRbGfxUnselected; }

	bool			SetArrowSource( WgGfxAnim * pAnim );

	bool			SetTextProperties( const WgTextpropPtr& pEntryProperties, const WgTextpropPtr& pKeyAccelProperties );
	WgTextpropPtr	GetTextEntryProperties() const { return m_pEntryProp; }
	WgTextpropPtr	GetKeyAccelProperties() const { return m_pKeyAccelProp; }

	bool			SetSliderSource(  WgBlocksetPtr pBgGfx, WgBlocksetPtr pBarGfx, WgBlocksetPtr pBtnBwdGfx, WgBlocksetPtr pBtnFwdGfx );
	bool			SetSliderButtonLayout(  WgGizmoSlider::ButtonLayout layout );
	WgGizmoSlider::ButtonLayout GetSliderButtonLayout() const { return m_sliderBtnLayout; }
	WgBlocksetPtr 	GetSliderBgSource() const { return m_pSliderBgGfx; }
	WgBlocksetPtr 	GetSliderBarSource() const { return m_pSliderBarGfx; }
	WgBlocksetPtr 	GetSliderBwdSource() const { return m_pSliderBtnBwdGfx; }
	WgBlocksetPtr 	GetSliderFwdSource() const { return m_pSliderBtnFwdGfx; }

	int			GetEntryHeight() const;

	int			AddItem( WgMenuitem * pEntry );
	int			InsertItem( WgMenuitem * pEntry, int pos );

	bool		RemoveItem( WgMenuitem * pEntry );
	WgMenuitem*	RemoveItem( int pos );
	void		RemoveAllItems();

	bool		DeleteItem( WgMenuitem * pEntry );
	bool		DeleteItem( int pos );
	void		DeleteAllItems();

	int			GetItemPos( WgMenuitem* pEntry );
	WgMenuitem*	GetItem( int pos );
	int			GetItemCount() const { return (int)m_items.Size(); }


	WgMenuitem *FindItem( int id );
	WgMenuitem *GetFirstItem() const { return m_items.First(); }
	WgMenuitem *GetLastItem() const { return m_items.Last(); }
	WgMenuitem *GetSelectedItem() const { return m_pSelectedItem; }

	void		SelectItem(WgMenuitem* pItem);

	WgGizmo * 	FindGizmo( const WgCoord& ofs, WgSearchMode mode );


	//____ Overloaded from WgGizmo & WgGizmoContainer ___________________________

	bool		DeleteChild(WgGizmo* pChild ) { return false; }
	WgGizmo*	ReleaseChild(WgGizmo* pChild ) { return 0; }
	bool		DeleteAllChildren() { return false; }
	bool		ReleaseAllChildren() { return false; }

	int			WidthForHeight( int height ) const;

	WgSize		DefaultSize() const;

	//


private:

	class SliderHook : public WgHook
	{
		friend class WgGizmoMenu;

	public:
		const char *Type( void ) const;
		static const char * ClassType();

		// Standard Hook methods

		WgCoord		Pos() const;
		WgSize		Size() const;
		WgRect		Geo() const;

		WgCoord		ScreenPos() const;
		WgRect		ScreenGeo() const;

		bool		SetHidden( bool bHidden ) { return false; }
		WgGizmoMenu* Parent() const { return m_pParent; }

		WgGizmoVSlider * Slider() { return m_pGizmo?static_cast<WgGizmoVSlider*>(m_pGizmo):0; }


	protected:
		SliderHook() : m_pParent(0) {}

		void		_requestRender();
		void		_requestRender( const WgRect& rect );
		void		_requestResize();

		WgHook *	_prevHook() const { return 0; }
		WgHook *	_nextHook() const { return 0; }
		WgGizmoParent * _parent() const { return m_pParent; }

		WgGizmoMenu * 	m_pParent;
		WgSize			m_size;

	};

	//

	void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer );
	void		_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void		_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void		_onCloneContent( const WgGizmo * _pOrg );
	void		_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void		_onNewSize( const WgSize& size );
	void		_onRefresh();

	void		_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	bool		_onAlphaTest( const WgCoord& ofs );

	void		_onEnable();			// We are a container, but we also have our own graphics.
	void		_onDisable();			// " -

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
	void		_tilesModified();
	WgRect		_sliderGeo( const WgRect& menuGeo ) const;

	// Overriden for WgSliderTarget

	float		_stepFwd();
	float		_stepBwd();
	float		_jumpFwd();
	float		_jumpBwd();
	float		_wheelRolled( int distance );

	float		_setPosition( float fraction );

	WgGizmo*	_getGizmo();
	float		_getSliderPosition();
	float		_getSliderSize();

	//

	WgMenuitem *_getItemAtPos( int posX, int posY );
	void		_calcEntryMinWidth( WgMenuEntry * pEntry );
	void		_openSubMenu( WgMenuSubMenu * pItem );

	void		_itemSelected();
	void		_openSubMenu( WgGizmoMenu * pMenu, const WgRect& laucherGeo, WgOrientation orientation );
	void		_closeSubMenu();

	WgBorders	_getPadding() const;
	void		_refreshEntryHeight();

	void		_scrollItemIntoView( WgMenuitem * pItem, bool bForceAtTop = false );
	void		_markFirstFilteredEntry();

	int			_getViewSize();
	void		_setViewOfs(int pos);

	//____ Members ___________________________________________

	SliderHook				m_sliderHook;
	WgSize					m_defaultSize;

	// Members defining items.

	WgChain<WgMenuitem>		m_items;
	Uint32					m_nItems;
	Uint32					m_markedItem;		// 0 = no item is selected.
	WgMenuitem*				m_pSelectedItem;

	// Members holding data for open menu

	int						m_contentHeight;	// Total height of content in pixels.
	int						m_contentOfs;		// Offset in pixels of content displayed.

	const static int		c_maxSelectorKeys = 20;
	const static int		c_selectorCountdownStart = 1000;

	Uint16					m_selectorKeys[c_maxSelectorKeys];	// Filter for marking entry on keyboard input.
	int						m_nSelectorKeys;					// Number of characters in the filter.
	int						m_selectorCountdown;				// Countdown in ms for clearing filter due to no keyboard input.

	// Members defining background

	WgBlocksetPtr			m_pBgGfx;

	// Members defining content layout

	Uint8					m_iconFieldWidth;
	Uint8					m_arrowFieldWidth;

	// Members defining separators

	WgBlocksetPtr			m_pSepGfx;
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

	WgBlocksetPtr			m_pCbGfxChecked;
	WgBlocksetPtr			m_pCbGfxUnchecked;

	// Members defining radiobutton entries.

	WgBlocksetPtr			m_pRbGfxSelected;
	WgBlocksetPtr			m_pRbGfxUnselected;

	// Members defining dragbar.

	WgBlocksetPtr				m_pSliderBgGfx;
	WgBlocksetPtr				m_pSliderBarGfx;
	WgBlocksetPtr				m_pSliderBtnFwdGfx;
	WgBlocksetPtr				m_pSliderBtnBwdGfx;
	WgGizmoSlider::ButtonLayout	m_sliderBtnLayout;
};


#endif //	WDG_MENU_DOT_H


