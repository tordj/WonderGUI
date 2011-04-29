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

#ifndef WG_ITEM_ROW_DOT_H
#define WG_ITEM_ROW_DOT_H

#include <wg_item.h>
#include <wg_interface_itemholder.h>
#include <wg_interface_textholder.h>

#include <wg_tooltip.h>


class WgItemRow : public WgItem, public Wg_Interface_ItemHolder
{
public:
	WgItemRow();
	WgItemRow( Sint64 id );
	virtual ~WgItemRow();

	DECLARE_TOOLTIP_SUPPORT();

	virtual const char *Type( void ) const;
	static const char * GetMyType();


	void	SetHeightModify( int pixels );
	void	SetMarkChildren( bool bMarkChildren );
	void	SetStretchLastItem( bool bStretch );
	void	SetUseAllHeight( bool bUseAllHeight );
	void	SetWidthExpandUsage( float usage );		// 0.f -> 1.f, how much of extra width the items should grow into.

	inline bool	GrabInputFocus() { return WgItem::GrabInputFocus(); }
	inline bool	RemoveInputFocus() { return WgItem::GrabInputFocus(); }

protected:

	void	AdaptToHeight( Uint32 displayed_height );
	void	AdaptToWidth( Uint32 displayed_width );

	float	WidthExpandFactor( int screen_width ); // Factor to expand each items width with.

	void	ActionRespond( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
 	void	Render( const WgRect& _window, const WgRect& _clip );
	WgItem* Clone( WgItem * _pClone = 0 );

//	void	Refresh( void );
//	void	Update( const WgUpdateInfo& _updateInfo );
	void	Enable( bool bEnable ); 
//	Sint32	CompareTo( WgItem * pOther );

	// Callbacks for interfaces;

	void refreshItems();
	void ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff );
	void ItemVisibilityModified( WgItem * pItem, bool bVisible );
	WgItem* GetMarkedItem( Uint32 x, Uint32 y );
	virtual WgRect	RequestItemGeo( WgItem * pItem );
	float	WidthExpandPercentage( int screen_width );


	int		m_heightModify;
	bool	m_bMarkChildren;

	bool	m_bStretchLastItem;
	bool	m_bUseAllHeight;
	float	m_widthExpandUsage;
};



#endif // WG_ITEM_ROW_DOT_H
