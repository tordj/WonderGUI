#include <wg_menuitem.h>

#include <wg_char.h>
#include <wg_texttool.h>

#include <wg_menu.h>

namespace wg 
{
	
	
	
	//____ MenuItem::setVisible() _______________________________________________
	
	void MenuItem::setVisible( bool bVisible )
	{
		if( m_pMyMenu && m_bVisible != bVisible )
		{
			m_bVisible = bVisible;
			modified();
		}
	}
	
	//____ MenuItem::modified() ___________________________________________
	
	void MenuItem::modified()
	{
		if(m_pMyMenu)
			m_pMyMenu->_itemModified();
	}
	
	//____ MenuSeparator::Constructor ___________________________________________
	
	MenuSeparator::MenuSeparator():MenuItem(SEPARATOR,0)
	{
	}
	
	
	//____ MenuEntry::Constructor _______________________________________________
	
	MenuEntry::MenuEntry() : MenuItem(ENTRY,INT_MIN)
	{
		m_bEnabled		= true;
		m_navKey		= 0;
		m_accelModif	= MODKEY_NONE;
		m_accelKey		= 0;
	}
	
	MenuEntry::MenuEntry(	int id, const String& text, const String& helpText, const Skin_p& pIcon, uint16_t navKey,
								ModifierKeys accelModif, uint16_t accelKey, const String& accelText ):MenuItem(ENTRY,id)
	{
	
		m_text			= text;
		m_accelText		= accelText;
		m_helpText		= helpText;
	
		m_pIcon			= pIcon;
		m_bEnabled		= true;
	
		m_navKey		= navKey;
		m_accelModif	= accelModif;
		m_accelKey		= accelKey;
	}
	
	void MenuEntry::setText(const String& text)
	{
		m_text = text;
		modified();
	}
	
	void MenuEntry::setHelpText(const String& helpText)
	{
		m_helpText		= helpText;
		modified();
	}
	
	void MenuEntry::setIcon(const Skin_p& pIcon)
	{
		m_pIcon		= pIcon;
		modified();
	}
	
	void MenuEntry::setNavKey(uint16_t navKey)
	{
		m_navKey		= navKey;
		modified();
	}
	
	void MenuEntry::setAccelModifier(ModifierKeys accelModif)
	{
		m_accelModif	= accelModif;
		modified();
	}
	
	void MenuEntry::setAccelKey(uint16_t accelKey)
	{
		m_accelKey		= accelKey;
		modified();
	}
	
	void MenuEntry::setAccelText(const String& accelText)
	{
		m_accelText		= accelText;
		modified();
	}
	
	//____ MenuEntry::Destructor _______________________________________________
	
	MenuEntry::~MenuEntry()
	{
	}
	
	
	//____ MenuCheckBox::Constructor ____________________________________________
	
	MenuCheckBox::MenuCheckBox()
	{
		m_type = CHECKBOX;
		m_bChecked = false;
	}
	
	MenuCheckBox::MenuCheckBox(	int id, const String& text, const String& helpText, uint16_t navKey, bool bChecked,
									ModifierKeys accelModif, uint16_t accelKey, const String& accelText )
							:MenuEntry( id, text, helpText, 0, navKey, accelModif, accelKey, accelText )
	{
		m_type = CHECKBOX;
		m_bChecked = bChecked;
	}
	
	//____ MenuRadioButton::Constructor _________________________________________
	
	MenuRadioButton::MenuRadioButton()
	{
		m_type = RADIOBUTTON;
		m_bSelected = false;
	}
	
	MenuRadioButton::MenuRadioButton(	int id, const String& text, const String& helpText,
											uint16_t navKey, bool bSelected,
											ModifierKeys accelModif, uint16_t accelKey, const String& accelText )
							:MenuEntry( id, text, helpText, 0, navKey, accelModif, accelKey, accelText )
	{
		m_type = RADIOBUTTON;
		m_bSelected = bSelected;
	}
	
	//____ MenuRadioButton::select() ____________________________________________
	
	bool MenuRadioButton::select()
	{
		// Select me and unselect any adjacent RadioButton.
	
		m_bSelected = true;
	
		MenuItem * pItem = this->next();
		while( pItem && pItem->getType() == RADIOBUTTON )
		{
			((MenuRadioButton*)pItem)->m_bSelected = false;
			pItem = pItem->next();
		}
	
		pItem = this->prev();
		while( pItem && pItem->getType() == RADIOBUTTON )
		{
			((MenuRadioButton*)pItem)->m_bSelected = false;
			pItem = pItem->prev();
		}
	
	
		//TODO: Need to force a redraw here...
	
		return true;
	}
	
	
	//____ MenuSubMenu::Constructor _____________________________________________
	
	MenuSubMenu::MenuSubMenu()
	{
		m_type = SUBMENU;
		m_pSubMenu = 0;
	}
	
	
	MenuSubMenu::MenuSubMenu(	int id, const String& text, const String& helpText,
									const Skin_p& pIcon, uint16_t navKey, const Menu_p& pSubMenu,
									ModifierKeys accelModif, uint16_t accelKey, const String& accelText )
							:MenuEntry( id, text, helpText, pIcon, navKey, accelModif, accelKey, accelText )
	{
		m_type = SUBMENU;
		m_pSubMenu = pSubMenu;
	}
	
	void MenuSubMenu::setSubMenu(const Menu_p& pSubMenu)
	{
		m_pSubMenu= pSubMenu;
	};
	

} // namespace wg
