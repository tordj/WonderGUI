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

#ifndef	WG_COMBOBOX_DOT_H
#define WG_COMBOBOX_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_MENU_DOT_H
#	include <wg_menu.h>
#endif

#ifndef WG_EVENTHANDLER_DOT_H
#	include <wg_eventhandler.h>
#endif

#include <wg_textfield.h>

class WgMenuItem;

class WgCombobox;
typedef	WgStrongPtr<WgCombobox,WgWidgetPtr>		WgComboboxPtr;
typedef	WgWeakPtr<WgCombobox,WgWidgetWeakPtr>	WgComboboxWeakPtr;

class WgCombobox : public WgWidget, public WgTextHolder
{
public:
	static WgComboboxPtr	Create() { return WgComboboxPtr(new WgCombobox()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgComboboxPtr	Cast( const WgObjectPtr& pObject );

	void			SetMenu( const WgMenuPtr& pMenu );
	WgMenuPtr		Menu( ) const;

	WgMenuItem*		SelectedItem() const { return m_pSelectedItem; }

	void			SetTextFormat( const WgCharSeq& str );
	WgString		TextFormat() const { return m_textFormat; }

	void			SetPlaceholderText( const WgCharSeq& str );
	WgString		PlaceholderText() const { return m_placeholderText; }

	WgSize			PreferredSize() const;
	bool			IsAutoEllipsisDefault() const { return false; };

	void			SetEditMode(WgTextEditMode mode);
	WgTextEditMode	EditMode() const { return m_text.EditMode(); }

	int				InsertTextAtCursor( const WgCharSeq& str );
	bool			InsertCharAtCursor( Uint16 c );

	inline WgIStaticTextPtr	Text() { return WgIStaticTextPtr(this,&m_text); } 

	// Press in textfield:
	//		Editable - Grab input focus.
	//		Non-editable - open menu.

	//	+ Option för att fälla ut meny när man börjar editera?
	//	+ Option för att öppna menyn över/under/over? (hur gör vi med button om vi tillåter over?).
	//	+ Sätta bredd och maxhöjd på meny? Eller funktion för detta i menyn?
	//	+ Funktion för att sätta default text (antingen entry eller meddelande eller specificerad text?)
	//	+ Pil upp/ner för att stega igenom menyalternativen, både på öppen och stängd meny.


protected:
	WgCombobox();
	virtual ~WgCombobox();
	virtual WgWidget* _newOfMyType() const { return new WgCombobox(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onRefresh();
	void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
	void	_onStateChanged( WgState oldState );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

private:
	bool	_isEditable() const { return m_text.IsEditable(); }
	bool	_isSelectable() const { return m_text.IsSelectable(); }

	void	_fieldModified( WgTextField * pField );
	void	_adjustViewOfs();
	void	_closeMenu();

	void	_entrySelected(int itemId);
	static void cbEntrySelected( const WgEventPtr& pEvent, const WgObjectPtr& pWdg) { WgCombobox::Cast(pWdg)->_entrySelected(WgItemsSelectEvent::Cast(pEvent)->Items()->id); }

	WgString		m_textFormat;
	WgString		m_placeholderText;		// Text displayed when field is empty and has no cursor.

	WgMenuPtr		m_pMenu;
	WgMenuItem*		m_pSelectedItem;
	WgTextField		m_text;
	bool			m_bResetCursorOnFocus;
	bool			m_bPressInInputRect;
	bool			m_bFocusPress;
	int				m_maxCharacters;
	int				m_viewOfs;

	WgCallbackHandle	m_cbHandler;
};

#endif // WG_COMBOBOX_DOT_H
