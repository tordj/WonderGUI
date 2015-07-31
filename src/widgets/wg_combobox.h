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

#ifndef WG_EDITTEXT_DOT_H
#	include <wg_edittext.h>
#endif

namespace wg 
{
	
	class WgMenuItem;
	
	class WgCombobox;
	typedef	WgStrongPtr<WgCombobox,WgWidget_p>		WgCombobox_p;
	typedef	WgWeakPtr<WgCombobox,WgWidget_wp>	WgCombobox_wp;
	
	class WgCombobox : public WgWidget, protected WgLegacyTextHolder
	{
	public:
		static WgCombobox_p	create() { return WgCombobox_p(new WgCombobox()); }
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static WgCombobox_p	cast( const WgObject_p& pObject );
	
		//____ Interfaces ______________________________________
	
		WgEditText		text;
	
		//____ Methods _________________________________________
	
		void			SetMenu( const WgMenu_p& pMenu );
		WgMenu_p		Menu( ) const;
	
		WgMenuItem*		SelectedItem() const { return m_pSelectedItem; }
	
		void			SetTextFormat( const WgCharSeq& str );
		WgString		TextFormat() const { return m_textFormat; }
	
		void			SetPlaceholderText( const WgCharSeq& str );
		WgString		PlaceholderText() const { return m_placeholderText; }
	
		WgSize			preferredSize() const;
		bool			IsAutoEllipsisDefault() const { return false; };
	
		void			setEditMode(WgTextEditMode mode);
		WgTextEditMode	editMode() const { return m_text.editMode(); }
	
		int				InsertTextAtCursor( const WgCharSeq& str );
		bool			InsertCharAtCursor( Uint16 c );
	
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
		void	_onMsg( const WgMsg_p& pMsg );
		bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
		void	_onStateChanged( WgState oldState );
		void	_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin );
	
	private:
		bool	_isEditable() const { return m_text.isEditable(); }
		bool	_isSelectable() const { return m_text.isSelectable(); }
	
		WgObject * 		_object() { return this; };
		void			_onFieldDirty( WgField * pField );
		void 			_onFieldResize( WgField * pField );
	
		void	_adjustViewOfs();
		void	_closeMenu();
	
		void	_entrySelected(int itemId);
		static void cbEntrySelected( const WgMsg_p& pMsg, const WgObject_p& pWdg) { WgCombobox::cast(pWdg)->_entrySelected(WgItemsSelectMsg::cast(pMsg)->items()->id); }
	
		WgString		m_textFormat;
		WgString		m_placeholderText;		// Text displayed when field is empty and has no cursor.
	
		WgMenu_p		m_pMenu;
		WgMenuItem*		m_pSelectedItem;
		WgLegacyTextField		m_text;
		bool			m_bResetCursorOnFocus;
		bool			m_bPressInInputRect;
		bool			m_bFocusPress;
		int				m_maxCharacters;
		int				m_viewOfs;
	
		WgRouteId	m_tickRouteId;
		WgRouteId	m_routeId;
	};
	

} // namespace wg
#endif // WG_COMBOBOX_DOT_H
