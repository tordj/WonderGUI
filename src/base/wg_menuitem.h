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
#ifndef	WG_MENUITEM_DOT_H
#define	WG_MENUITEM_DOT_H
#ifndef	WG_CHAIN_DOT_H
#	include "wg_chain.h"
#endif

#ifndef	WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

namespace wg 
{
	
	class Char;
	class GfxAnim;
	
	class Menu;
	//typedef class Menu Menu;
	
	typedef	WgStrongPtr<Menu,Widget_p>		Menu_p;
	typedef	WgWeakPtr<Menu,Widget_wp>	Menu_wp;
	
	enum MenuItemType
	{
			ENTRY,
			CHECKBOX,
			RADIOBUTTON,
			SUBMENU,
			SEPARATOR
	};
	
	
	//____ MenuItem _____________________________________________________________
	
	class MenuItem : public Link
	{
		friend class Wdg_Menu;
		friend class Menu;
	public:
		virtual ~MenuItem() {};
		LINK_METHODS( MenuItem );
	
		inline MenuItemType getType() const { return m_type; }
		inline void setId(int id) { m_id = id; }
		inline int getId() const { return m_id; }
	
		inline bool isVisible() const { return m_bVisible; }
	
		void setVisible(bool bVisible);
		void modified();
	
	protected:
		MenuItem(MenuItemType _type, int id) {m_type = _type; m_id = id; m_pMyMenu = 0; m_bVisible = true; }
	
		virtual void setMyMenu( Menu * pMenu ) { m_pMyMenu = pMenu; }
	
		MenuItemType	m_type;
	    int				m_id;
		bool			m_bVisible;
	
		Menu *	m_pMyMenu;
	};
	
	//____ MenuSeparator ________________________________________________________
	
	class MenuSeparator : public MenuItem
	{
	public:
		MenuSeparator();
		virtual ~MenuSeparator() {};
	};
	
	//____ MenuEntry ____________________________________________________________
	
	class MenuEntry : public MenuItem
	{
		friend class Wdg_Menu;
		friend class Menu;
	public:
		MenuEntry();
		MenuEntry( int id, const String& text, const String& helpText, const Skin_p& pIcon, uint16_t navKey,
			WgModifierKeys accelModif = WG_MODKEY_NONE, uint16_t accelKey = 0, const String& accelText = String() );
		virtual ~MenuEntry();
	
		void 	setText(const String& text);
		void 	setHelpText(const String& helpText);
		void 	setIcon(const Skin_p& pIcon);
		void 	setNavKey(uint16_t navKey);
		void 	setAccelModifier(WgModifierKeys accelModif);
		void 	setAccelKey(uint16_t accelKey);
		void 	setAccelText(const String& accelText);
	
		inline bool isEnabled() { return m_bEnabled; }
		inline void	setEnabled(bool bEnabled) { m_bEnabled = bEnabled; }			// Need to force a redraw here...
	
		inline String getText() { return m_text; }
		inline String getHelpText() { return m_helpText; }
		inline String getAccelText() { return m_accelText; }
	
		inline const Skin_p getIcon() { return m_pIcon; }
		inline uint16_t			getNavKey()		{ return m_navKey; }
		inline WgModifierKeys	getAccelModif()	{ return m_accelModif; }
		inline uint16_t			getAccelKey()	{ return m_accelKey; }
	
	protected:
		uint16_t			m_minWidth;
	private:
		String		m_text;
		String		m_accelText;
		String		m_helpText;
	
		Skin_p		m_pIcon;
		bool			m_bEnabled;
	
		uint16_t			m_navKey;
		WgModifierKeys	m_accelModif;
		uint16_t			m_accelKey;
	};
	
	//____ MenuCheckBox _________________________________________________________
	
	class MenuCheckBox : public MenuEntry
	{
	public:
		MenuCheckBox();
		MenuCheckBox(	int id, const String& text, const String& helpText, uint16_t navKey, bool bChecked,
						WgModifierKeys accelModif = WG_MODKEY_NONE, uint16_t accelKey = 0, const String& accelText = String() );
		virtual ~MenuCheckBox() {};
	
		inline bool isChecked() { return m_bChecked; }
		inline void	check() { m_bChecked = true; }			// Need to force a redraw here...
		inline void uncheck() { m_bChecked = false; }		// Need to force a redraw here...
	
	private:
		bool			m_bChecked;
	};
	
	//____ MenuRadioButton ______________________________________________________
	
	class MenuRadioButton : public MenuEntry
	{
	public:
		MenuRadioButton();
		MenuRadioButton(	int id, const String& text, const String& helpText, uint16_t navKey, bool bSelected,
						WgModifierKeys accelModif = WG_MODKEY_NONE, uint16_t accelKey = 0, const String& accelText = String() );
		virtual ~MenuRadioButton() {};
	
		inline	bool isSelected() { return m_bSelected; }
				bool select();
	
	private:
		bool			m_bSelected;
	
	};
	
	//____ MenuSubMenu __________________________________________________________
	
	class MenuSubMenu : public MenuEntry
	{
	public:
		MenuSubMenu();
		MenuSubMenu(	int id, const String& text, const String& helpText, const Skin_p& pIcon, uint16_t navKey, const Menu_p& pSubMenu,
						WgModifierKeys accelModif = WG_MODKEY_NONE, uint16_t accelKey = 0, const String& accelText = String() );
		virtual ~MenuSubMenu() {};
	
		inline Menu_p	getSubMenu()					{return m_pSubMenu;};
		void				setSubMenu(const Menu_p& pSubMenu);
	
		Menu_p		m_pSubMenu;
	};
	
	
	

} // namespace wg
#endif //WG_MENUITEM_DOT_H

