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

#ifndef WG_ITEM_TEXT_DOT_H
#define WG_ITEM_TEXT_DOT_H

#include <wg_item.h>
#include <wg_text.h>
#include <wg_interface_textholder.h>
#include <wg_blockset.h>

class WgCharSeq;

class WgItemText : public WgItem, public Wg_Interface_TextHolder
{
public:
	WgItemText( );
	WgItemText( Sint64 id, const WgCharSeq& seq, const WgTextPropPtr& pProp, Uint8 marginLeft=0, Uint8 marginRight=0, Uint8 marginTop=0, Uint8 marginBottom=0 );
	WgItemText( Sint64 id, const WgText * pText, const WgTextPropPtr& pProp, Uint8 marginLeft=0, Uint8 marginRight=0, Uint8 marginTop=0, Uint8 marginBottom=0 );
	~WgItemText();

	void SetTextProp(const WgTextPropPtr& pProp);
	void SetText( const WgCharSeq& seq );
	void SetText(const WgText * pText);
	void SetMargin(WgBorders margin);

	virtual const char *Type( void ) const;
	static const char * GetMyType();
protected:
	void	Init();
	void	TextModified();		// Callback for Wg_Interface_TextHolder


 	virtual void	Render( const WgRect& _window, const WgRect& _clip );
	virtual WgItem* Clone( WgItem * _pClone = 0 );
	virtual void	Enable( bool bEnable ); 

private:

	WgText			m_text;
	WgBorders		m_margin;
	bool			m_bEnabled;
};


#endif // WG_ITEM_TEXT_DOT_H

