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

#ifndef WG_LEGACYEDITTEXT_DOT_H
#	include <wg_legacyedittext.h>
#endif

namespace wg 
{
	
	class MenuItem;
	
	class Combobox;
	typedef	StrongPtr<Combobox,Widget_p>		Combobox_p;
	typedef	WeakPtr<Combobox,Widget_wp>	Combobox_wp;
	
	class Combobox : public Widget, protected LegacyTextHolder
	{
	public:
		static Combobox_p	create() { return Combobox_p(new Combobox()); }
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static Combobox_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		LegacyEditText		text;
	
		//____ Methods _________________________________________
	
		void			setMenu( const Menu_p& pMenu );
		Menu_p		menu( ) const;
	
		MenuItem*		selectedItem() const { return m_pSelectedItem; }
	
		void			setTextFormat( const CharSeq& str );
		String		textFormat() const { return m_textFormat; }
	
		void			setPlaceholderText( const CharSeq& str );
		String		placeholderText() const { return m_placeholderText; }
	
		Size			preferredSize() const;
		bool			isAutoEllipsisDefault() const { return false; };
	
		void			setEditMode(TextEditMode mode);
		TextEditMode	editMode() const { return m_text.editMode(); }
	
		int				insertTextAtCaret( const CharSeq& str );
		bool			insertCharAtCaret( uint16_t c );
	
		// Press in textfield:
		//		Editable - Grab input focus.
		//		Non-editable - open menu.
	
		//	+ Option för att fälla ut meny när man börjar editera?
		//	+ Option för att öppna menyn över/under/over? (hur gör vi med button om vi tillåter over?).
		//	+ Sätta bredd och maxhöjd på meny? Eller funktion för detta i menyn?
		//	+ Funktion för att sätta default text (antingen entry eller meddelande eller specificerad text?)
		//	+ Pil upp/ner för att stega igenom menyalternativen, både på öppen och stängd meny.
	
	
	protected:
		Combobox();
		virtual ~Combobox();
		virtual Widget* _newOfMyType() const { return new Combobox(); };
	
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_onRefresh();
		void	_onMsg( const Msg_p& pMsg );
		bool	_onAlphaTest( const Coord& ofs, const Size& sz );
		void	_onStateChanged( State oldState );
		void	_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
	
	private:
		bool	_isEditable() const { return m_text.isEditable(); }
		bool	_isSelectable() const { return m_text.isSelectable(); }
	
		Object * 		_object() { return this; };
		void			_onFieldDirty( Field * pField );
		void			_onFieldDirty( Field * pField, const Rect& rect );
		void 			_onFieldResize( Field * pField );
	
		void	_adjustViewOfs();
		void	_closeMenu();
	
		void	_entrySelected(int itemId);
		static void cbEntrySelected( const Msg_p& pMsg, const Object_p& pWdg) { Combobox::cast(pWdg)->_entrySelected(ItemsSelectMsg::cast(pMsg)->items()->id); }
	
		String		m_textFormat;
		String		m_placeholderText;		// Text displayed when field is empty and has no caret.
	
		Menu_p		m_pMenu;
		MenuItem*		m_pSelectedItem;
		LegacyTextField		m_text;
		bool			m_bResetCaretOnFocus;
		bool			m_bPressInInputRect;
		bool			m_bFocusPress;
		int				m_maxCharacters;
		int				m_viewOfs;
	
		RouteId	m_tickRouteId;
		RouteId	m_routeId;
	};
	

} // namespace wg
#endif // WG_COMBOBOX_DOT_H
