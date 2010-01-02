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

#ifndef WG_ITEM_FOLDER_DOT_H
#define WG_ITEM_FOLDER_DOT_H

#include <wg_item.h>
#include <wg_interface_itemholder.h>
#include <wg_interface_textholder.h>
#include <wg_text.h>
#include <wg_blockset.h>

class WgRect;
class WgSurface;


class WgItemFolder : public WgItem, public Wg_Interface_ItemHolder
{
public:
	WgItemFolder();
	WgItemFolder(Uint32 id );
	WgItemFolder(Uint32 id, WgBlockSetPtr _pOpen, WgBlockSetPtr _pClosed );
	WgItemFolder(Uint32 id, WgBlockSetPtr _pOpen, WgBlockSetPtr _pClosed, Uint8 indentation );
	WgItemFolder(Uint32 id, WgBlockSetPtr _pOpen, WgBlockSetPtr _pClosed, Uint8 indentation, const WgBorders& labelBorders, WgItem * pLabel );
	~WgItemFolder();

	void	OpenFolder();
	void	CloseFolder();
	inline bool	IsOpen() { return m_bIsOpen; };

	void	SetIndentation( Uint8 pixels );
	Uint8	GetIndentation( ) const { return m_indent; }

	void				SetLabel( WgItem * pItem );
	inline WgItem *		GetLabel() const { return m_pLabel; }

	void				SetLabelBorders( const WgBorders& borders );
	inline WgBorders	GetLabelBorders() const { return m_labelBorders; }

	void				SetOpenSource( WgBlockSetPtr _pOpen );
	WgBlockSetPtr 		GetOpenSource() const	{ return m_pSrcOpen; }

	void				SetClosedSource( WgBlockSetPtr _pClosed );
	WgBlockSetPtr 		GetClosedSource() const { return m_pSrcClosed; }

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	inline bool	GrabInputFocus() { return WgItem::GrabInputFocus(); }
	inline bool	RemoveInputFocus() { return WgItem::GrabInputFocus(); }
protected:

	void	Init();
	void	AdaptToWidth( Uint32 displayed_width );
	WgSize	CalcOpenSize();
	WgSize	CalcClosedSize();

	void	ActionRespond( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
 	void	Render( const WgRect& _window, const WgRect& _clip );
	WgItem* Clone( WgItem * _pClone = 0 );

//	void	Refresh( void );
//	void	Update( const WgUpdateInfo& _updateInfo );
	void	Enable( bool bEnable ); 
//	Sint32	CompareTo( WgItem * pOther );


	// Callbacks for interfaces;

	void refreshItems();
	void ItemAdded( WgItem * pItem );
	void ItemModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff );
	WgItem* GetMarkedItem( Uint32 x, Uint32 y );
	void TextModified();

	//

	Uint8			m_indent;		// Indentation of content.

	bool			m_bIsOpen;
	bool			m_bEnabled;
	bool			m_bMouseOver;	// Just for internal use...

	WgItem *		m_pLabel;
	WgBorders		m_labelBorders;

	WgBlockSetPtr	m_pSrcOpen;
	WgBlockSetPtr	m_pSrcClosed;

	Uint32			m_openWidth;
	Uint32			m_openHeight;
	Uint32			m_closedWidth;
	Uint32			m_closedHeight;

	Uint32			m_availableWidth;

};



#endif // WG_ITEM_FOLDER_DOT_H

