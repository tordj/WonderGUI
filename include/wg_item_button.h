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

#ifndef WG_ITEM_BUTTON_DOT_H
#define WG_ITEM_BUTTON_DOT_H

#include <wg_item.h>
#include <wg_text.h>
#include <wg_interface_textholder.h>
#include <wg_gfx.h>

class WgItemButton : public WgItem, public Wg_Interface_TextHolder
{
public:
	WgItemButton( );
	WgItemButton( Uint32 id );
	WgItemButton( Uint32 id, WgBlockSetPtr pGfx );
	WgItemButton( Uint32 id, WgBlockSetPtr pGfx, const char * pStr, const WgTextPropPtr& pProp );
	WgItemButton( Uint32 id, WgBlockSetPtr pGfx, const WgChar * pStr, const WgTextPropPtr& pProp );
	~WgItemButton();

	void			SetSource( WgBlockSetPtr pGfx )	{ m_pGfx = pGfx; TextModified(); }
	WgBlockSetPtr	GetSource() const				{ return m_pGfx; }

	void			SetText( const char * pText );
	const WgChar *	GetText() const { return m_text.getText(); }

	virtual const char *Type( void ) const;
	static const char * GetMyType();

protected:
	void	Init();
	void	TextModified();		// Callback for Wg_Interface_TextHolder


 	void	Render( const WgRect& _window, const WgRect& _clip );
	WgItem* Clone( WgItem * _pClone = 0 );
	void	Enable( bool bEnable ); 
	Sint32	CompareTo( WgItem * pOther );
	void	ActionRespond( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );

private:

	WgText			m_text;
	bool			m_bEnabled;
	bool			m_bMouseOver;

	WgBlockSetPtr	m_pGfx;
};


#endif // WG_ITEM_BUTTON_DOT_H

