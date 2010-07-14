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

#ifndef	WG_ITEM_DOT_H
#define	WG_ITEM_DOT_H

#include <wg_types.h>
#include <wg_chain.h>
#include <wg_input.h>
#include <wg_geo.h>
#include <wg_string.h>


class Wg_Interface_ItemHolder;
class WgEmitter;
class WgChar;

class WgItem : public WgLink
{
	//TODO: Just have ItemHolder as friend and then provide inline methods?

	friend class Wg_Interface_ItemHolder;
	friend class Wdg_TableView;
	friend class Wdg_ListView;
	friend class Wdg_GridView;
	friend class WgGizmoTablist;
	friend class WgItemRow;
	friend class WgItemStack;
	friend class Wdg_TreeView;

public:

	WgItem();
	WgItem( Uint32 id );
	virtual ~WgItem();

	virtual	Uint32	Width() const		{ return m_width; };
	virtual	Uint32	Height() const		{ return m_height; };
	virtual	Uint32	Id() const			{ return m_id; };
	virtual	void	SetId(Uint32 id)	{ m_id = id; }

	inline	WgItem * GetNext() const	{ return (WgItem *) getNext(); };
	inline	WgItem * GetPrev() const	{ return (WgItem *) getPrev(); };
	inline	Wg_Interface_ItemHolder * GetHolder() const	{ return m_pMyHolder; };

	// Only works for rows in a Wdg_TableView atm.
	void			SetVisible(bool bVisible);
	inline	void	Show() { SetVisible(true); }
	inline	void	Hide() { SetVisible(false); }
	inline	bool	IsVisible() const { return m_bVisible; }
	inline	bool	IsHidden() const { return !m_bVisible; }

	bool Select();
	void Unselect();
	inline bool IsSelected() const { if( m_mode == WG_MODE_SELECTED ) return true; return false; }

	virtual void Enable();
	virtual void Disable();
	virtual bool IsDisabled() const { if( m_mode == WG_MODE_DISABLED ) return true; return false; }
	virtual WgMode GetMode() const { return m_mode; }

	// Deprecated methods, inconsistent naming causing confusion...

	//inline void Mark() { Select(); }
	//inline void Unmark() { Unselect(); }
	//inline bool IsMarked() const { return IsSelected(); }

	virtual WgString	GetTooltipString() const	{ return 0; }

	virtual const char *Type( void ) const = 0;

	bool			GrabInputFocus();
	bool			RemoveInputFocus();


protected:	

	LINK_METHODS( WgItem );

	virtual void 	AdaptToWidth( Uint32 displayed_width );
	virtual void 	AdaptToHeight( Uint32 displayed_height );

	virtual void	ActionRespond( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
 	virtual void	Render( const WgRect& _window, const WgRect& _clip );
	virtual WgItem* Clone( WgItem * _pClone );

	virtual void	OnGotInputFocus() {}
	virtual void	OnLostInputFocus() {}

	virtual void	Refresh( void );
//	virtual void	Update( const WgUpdateInfo& _updateInfo );
	virtual	WgItem*	GetMarkedItem( Uint32 _x, Uint32 _y );
	virtual void	Enable( bool bEnable ); 

	Uint32			m_width;		// Width needed to properly display content.
	Uint32			m_height;		// Height needed to properly display content.

	Uint32			m_id;
	WgMode			m_mode;

	bool			m_bVisible;

	void			Modified( Sint32 widthDiff , Sint32 heightDiff );

	Wg_Interface_ItemHolder * m_pMyHolder;
};


#endif //WG_ITEM_DOT_H
