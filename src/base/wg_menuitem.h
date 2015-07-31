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
	
	class WgChar;
	class WgGfxAnim;
	
	class WgMenu;
	//typedef class WgMenu WgMenu;
	
	typedef	WgStrongPtr<WgMenu,WgWidget_p>		WgMenu_p;
	typedef	WgWeakPtr<WgMenu,WgWidget_wp>	WgMenu_wp;
	
	enum WgMenuItemType
	{
			ENTRY,
			CHECKBOX,
			RADIOBUTTON,
			SUBMENU,
			SEPARATOR
	};
	
	
	//____ WgMenuItem _____________________________________________________________
	
	class WgMenuItem : public WgLink
	{
		friend class Wdg_Menu;
		friend class WgMenu;
	public:
		virtual ~WgMenuItem() {};
		LINK_METHODS( WgMenuItem );
	
		inline WgMenuItemType getType() const { return m_type; }
		inline void setId(int id) { m_id = id; }
		inline int getId() const { return m_id; }
	
		inline bool isVisible() const { return m_bVisible; }
	
		void setVisible(bool bVisible);
		void modified();
	
	protected:
		WgMenuItem(WgMenuItemType _type, int id) {m_type = _type; m_id = id; m_pMyMenu = 0; m_bVisible = true; }
	
		virtual void setMyMenu( WgMenu * pMenu ) { m_pMyMenu = pMenu; }
	
		WgMenuItemType	m_type;
	    int				m_id;
		bool			m_bVisible;
	
		WgMenu *	m_pMyMenu;
	};
	
	//____ WgMenuSeparator ________________________________________________________
	
	class WgMenuSeparator : public WgMenuItem
	{
	public:
		WgMenuSeparator();
		virtual ~WgMenuSeparator() {};
	};
	
	//____ WgMenuEntry ____________________________________________________________
	
	class WgMenuEntry : public WgMenuItem
	{
		friend class Wdg_Menu;
		friend class WgMenu;
	public:
		WgMenuEntry();
		WgMenuEntry( int id, const WgString& text, const WgString& helpText, const WgSkin_p& pIcon, Uint16 navKey,
			WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
		virtual ~WgMenuEntry();
	
		void 	setText(const WgString& text);
		void 	setHelpText(const WgString& helpText);
		void 	setIcon(const WgSkin_p& pIcon);
		void 	setNavKey(Uint16 navKey);
		void 	setAccelModifier(WgModifierKeys accelModif);
		void 	setAccelKey(Uint16 accelKey);
		void 	setAccelText(const WgString& accelText);
	
		inline bool isEnabled() { return m_bEnabled; }
		inline void	setEnabled(bool bEnabled) { m_bEnabled = bEnabled; }			// Need to force a redraw here...
	
		inline WgString getText() { return m_text; }
		inline WgString getHelpText() { return m_helpText; }
		inline WgString getAccelText() { return m_accelText; }
	
		inline const WgSkin_p getIcon() { return m_pIcon; }
		inline Uint16			getNavKey()		{ return m_navKey; }
		inline WgModifierKeys	getAccelModif()	{ return m_accelModif; }
		inline Uint16			getAccelKey()	{ return m_accelKey; }
	
	protected:
		Uint16			m_minWidth;
	private:
		WgString		m_text;
		WgString		m_accelText;
		WgString		m_helpText;
	
		WgSkin_p		m_pIcon;
		bool			m_bEnabled;
	
		Uint16			m_navKey;
		WgModifierKeys	m_accelModif;
		Uint16			m_accelKey;
	};
	
	//____ WgMenuCheckBox _________________________________________________________
	
	class WgMenuCheckBox : public WgMenuEntry
	{
	public:
		WgMenuCheckBox();
		WgMenuCheckBox(	int id, const WgString& text, const WgString& helpText, Uint16 navKey, bool bChecked,
						WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
		virtual ~WgMenuCheckBox() {};
	
		inline bool isChecked() { return m_bChecked; }
		inline void	check() { m_bChecked = true; }			// Need to force a redraw here...
		inline void uncheck() { m_bChecked = false; }		// Need to force a redraw here...
	
	private:
		bool			m_bChecked;
	};
	
	//____ WgMenuRadioButton ______________________________________________________
	
	class WgMenuRadioButton : public WgMenuEntry
	{
	public:
		WgMenuRadioButton();
		WgMenuRadioButton(	int id, const WgString& text, const WgString& helpText, Uint16 navKey, bool bSelected,
						WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
		virtual ~WgMenuRadioButton() {};
	
		inline	bool isSelected() { return m_bSelected; }
				bool select();
	
	private:
		bool			m_bSelected;
	
	};
	
	//____ WgMenuSubMenu __________________________________________________________
	
	class WgMenuSubMenu : public WgMenuEntry
	{
	public:
		WgMenuSubMenu();
		WgMenuSubMenu(	int id, const WgString& text, const WgString& helpText, const WgSkin_p& pIcon, Uint16 navKey, const WgMenu_p& pSubMenu,
						WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
		virtual ~WgMenuSubMenu() {};
	
		inline WgMenu_p	getSubMenu()					{return m_pSubMenu;};
		void				setSubMenu(const WgMenu_p& pSubMenu);
	
		WgMenu_p		m_pSubMenu;
	};
	
	
	

} // namespace wg
#endif //WG_MENUITEM_DOT_H

