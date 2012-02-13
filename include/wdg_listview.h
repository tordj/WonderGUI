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

#ifndef WDG_LISTVIEW_DOT_H
#define WDG_LISTVIEW_DOT_H

#ifndef WDG_BASECLASS_VIEW_DOT_H
#	include <wdg_baseclass_view.h>
#endif

#include <wg_interface_itemholder.h>


class Wdg_ListView : public Wdg_Baseclass_View, public Wg_Interface_ItemHolder
{
public:
	WIDGET_CONSTRUCTORS(Wdg_ListView,Wdg_Baseclass_View)
	virtual ~Wdg_ListView();
	virtual const char * Type() const;
	static const char * GetMyType();

	void	SetAutoScrollMarked( bool bAutoScroll )			{ m_bAutoScrollMarked = bAutoScroll; }
	bool	GetAutoScrollMarked()							{ return m_bAutoScrollMarked; }

	void	ScrollIntoView( WgItem* pItem );
	bool	GetItemPixelPos( WgItem * pItem, WgCoord& pos );

	WgItem*		GetFirstSelectedItem() { return 0; }

	inline bool	GrabInputFocus() { return WgWidget::GrabInputFocus(); }
	inline bool	RemoveInputFocus() { return WgWidget::GrabInputFocus(); }

	WgPointerStyle		GetPointerStyle() const;


protected:
	WgWidget * NewWidgetOfMyType() const;
private:
	void		Init();

	void		refreshItems();
	void		ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff );
	void		ItemVisibilityModified( WgItem * pItem, bool bVisible );

	void		ItemAdded( WgItem * pItem );
	void		ItemMarkChanged( WgItem * pItem, bool bMarked );
	WgItem* 	GetMarkedItem( Uint32 x, Uint32 y );
	inline void	SetLineMarkColor( WgColor c ) { SetItemMarkColor(c); }
	void		SetLineMarkSource(WgSurface* pSurf, const WgRect& rect, Uint8 tileOfs, Uint8 tileLen, bool bStretch );
	bool		HasLineMarkSource() const { return m_pLineMarkSurf != 0; }

	void		DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo );
	void		DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void		DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );	
//	void		DoMyOwnRefresh( void );
	void		DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
	bool		DoMyOwnMarkTest( int _x, int _y );
//	void		DoMyOwnDisOrEnable( void );

	void		UpdateContentSize();
	WgRect		RequestItemGeo( WgItem * pItem );
	bool		DoWeHaveItem( const WgItem * pItem );

	WgItem *	m_pLastMarkedItem;

	bool		m_bAutoScrollMarked;
                
	WgSurface *	m_pLineMarkSurf;
	WgRect		m_lineMarkSrc;
	Uint16		m_lineMarkTileOfs;
	Uint16		m_lineMarkTileLen;
	bool		m_bLineMarkStretch;
};


#endif // WDG_TEXTVIEW_DOT_H
