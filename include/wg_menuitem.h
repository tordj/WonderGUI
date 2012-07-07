#ifndef	WG_MENUITEM_DOT_H
#define	WG_MENUITEM_DOT_H
#ifndef	WG_CHAIN_DOT_H
#	include "wg_chain.h"
#endif

#ifndef	WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

class WgChar;

class WgText;
class WgGfxAnim;

class WgGizmoMenu;
typedef class WgGizmoMenu WgGizmoMenu;

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
	friend class WgGizmoMenu;
public:
	virtual ~WgMenuItem() {};
	LINK_METHODS( WgMenuItem );

	inline WgMenuItemType GetType() const { return m_type; }
	inline void SetId(int id) { m_id = id; }
	inline int GetId() const { return m_id; }

	inline bool IsSetToVisible() const { return m_bVisible; }

	inline void SetVisible(bool bVisible) { bVisible ? Show() : Hide(); }


	void Show();
	void Hide();
	void Modified();

protected:
	WgMenuItem(WgMenuItemType _type) {m_type = _type; m_id = 0; m_pMyMenu = 0; m_bVisible = true; }

	virtual void SetMyMenu( WgGizmoMenu * pMenu ) { m_pMyMenu = pMenu; }

	WgMenuItemType	m_type;
    int				m_id;
	bool			m_bVisible;

	WgGizmoMenu *	m_pMyMenu;
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
	friend class WgGizmoMenu;
public:
	WgMenuEntry();
	WgMenuEntry( const WgString& text, const WgString& helpText, const WgBlocksetPtr& pIcon, Uint16 navKey,
		WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
	virtual ~WgMenuEntry();

	void 	SetText(const WgString& text);
	void 	SetHelpText(const WgString& helpText);
	void 	SetIcon(const WgBlocksetPtr& pIcon);
	void 	SetNavKey(Uint16 navKey);
	void 	SetAccelModifier(WgModifierKeys accelModif);
	void 	SetAccelKey(Uint16 accelKey);
	void 	SetAccelText(const WgString& accelText);

	inline bool IsEnabled() { return m_bEnabled; }
	inline void	Enable() { m_bEnabled = true; }			// Need to force a redraw here...
	inline void Disable() { m_bEnabled = false; }		// Need to force a redraw here...

	inline WgString GetText() { return m_text; }
	inline WgString GetHelpText() { return m_helpText; }
	inline WgString GetAccelText() { return m_accelText; }

	inline const WgBlocksetPtr GetIcon() { return m_pIcon; }
	inline Uint16			GetNavKey()		{ return m_navKey; }
	inline WgModifierKeys	GetAccelModif()	{ return m_accelModif; }
	inline Uint16			GetAccelKey()	{ return m_accelKey; }

protected:
	Uint16			m_minWidth;
private:
	WgString		m_text;
	WgString		m_accelText;
	WgString		m_helpText;

	WgBlocksetPtr	m_pIcon;
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
	WgMenuCheckBox(	const WgString& text, const WgString& helpText, Uint16 navKey, bool bChecked,
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
	WgMenuRadioButton(	const WgString& text, const WgString& helpText, Uint16 navKey, bool bSelected,
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
	WgMenuSubMenu(	const WgString& text, const WgString& helpText, const WgBlocksetPtr& pIcon, Uint16 navKey, WgGizmoMenu * pSubMenu,
					WgModifierKeys accelModif = WG_MODKEY_NONE, Uint16 accelKey = 0, const WgString& accelText = WgString() );
	virtual ~WgMenuSubMenu() {};

	inline WgGizmoMenu *GetSubMenu()					{return m_pSubMenu;};
	void				SetSubMenu(WgGizmoMenu * subMenu);

private:
	void				SetMyMenu( WgGizmoMenu * pMenu );

	WgGizmoMenu *		m_pSubMenu;
};



#endif //WG_MENUITEM_DOT_H

