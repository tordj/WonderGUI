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

class WgChar;
class WgGfxAnim;

class WgMenu;
//typedef class WgMenu WgMenu;

typedef	WgStrongPtr<WgMenu,WgWidgetPtr>		WgMenuPtr;
typedef	WgWeakPtr<WgMenu,WgWidgetWeakPtr>	WgMenuWeakPtr;

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

	inline WgMenuItemType GetType() const { return m_type; }
	inline void SetId(int id) { m_id = id; }
	inline int GetId() const { return m_id; }

	inline bool IsVisible() const { return m_bVisible; }

	void SetVisible(bool bVisible);
	void Modified();

protected:
	WgMenuItem(WgMenuItemType _type, int id) {m_type = _type; m_id = id; m_pMyMenu = 0; m_bVisible = true; }

	virtual void SetMyMenu( WgMenu * pMenu ) { m_pMyMenu = pMenu; }

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
	WgMenuEntry( int id, const WgString& text, const WgString& helpText, const WgSkinPtr& pIcon, Uint16 navKey,
		WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
	virtual ~WgMenuEntry();

	void 	SetText(const WgString& text);
	void 	SetHelpText(const WgString& helpText);
	void 	SetIcon(const WgSkinPtr& pIcon);
	void 	SetNavKey(Uint16 navKey);
	void 	SetAccelModifier(WgModifierKeys accelModif);
	void 	SetAccelKey(Uint16 accelKey);
	void 	SetAccelText(const WgString& accelText);

	inline bool IsEnabled() { return m_bEnabled; }
	inline void	SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }			// Need to force a redraw here...

	inline WgString GetText() { return m_text; }
	inline WgString GetHelpText() { return m_helpText; }
	inline WgString GetAccelText() { return m_accelText; }

	inline const WgSkinPtr GetIcon() { return m_pIcon; }
	inline Uint16			GetNavKey()		{ return m_navKey; }
	inline WgModifierKeys	GetAccelModif()	{ return m_accelModif; }
	inline Uint16			GetAccelKey()	{ return m_accelKey; }

protected:
	Uint16			m_minWidth;
private:
	WgString		m_text;
	WgString		m_accelText;
	WgString		m_helpText;

	WgSkinPtr		m_pIcon;
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

	inline bool IsChecked() { return m_bChecked; }
	inline void	Check() { m_bChecked = true; }			// Need to force a redraw here...
	inline void Uncheck() { m_bChecked = false; }		// Need to force a redraw here...

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

	inline	bool IsSelected() { return m_bSelected; }
			bool Select();

private:
	bool			m_bSelected;

};

//____ WgMenuSubMenu __________________________________________________________

class WgMenuSubMenu : public WgMenuEntry
{
public:
	WgMenuSubMenu();
	WgMenuSubMenu(	int id, const WgString& text, const WgString& helpText, const WgSkinPtr& pIcon, Uint16 navKey, const WgMenuPtr& pSubMenu,
					WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
	virtual ~WgMenuSubMenu() {};

	inline WgMenuPtr	GetSubMenu()					{return m_pSubMenu;};
	void				SetSubMenu(const WgMenuPtr& pSubMenu);

	WgMenuPtr		m_pSubMenu;
};



#endif //WG_MENUITEM_DOT_H

