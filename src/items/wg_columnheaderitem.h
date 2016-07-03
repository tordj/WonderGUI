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
#ifndef WG_COLUMNHEADERITEM_DOT_H
#define WG_COLUMNHEADERITEM_DOT_H

#ifndef WG_ICONITEM_DOT_H
#	include <wg_iconitem.h>
#endif

#ifndef WG_TEXTITEM_DOT_H
#	include <wg_textitem.h>
#endif

namespace wg 
{
	
	class ColumnHeaderItem : public Item
	{
	public:
		ColumnHeaderItem(Widget * pWidget);
	
		//____ Subitems __________________________________
	
		IconItem		icon;
		IconItem		arrow;
		TextItem		label;
	
		//____ Methods _____________________________________
	
		void			setSkin( const Skin_p& pSkin );
		inline Skin_p	skin() const { return m_pSkin; }
	
	//private:
		
		//TODO: Implement proper solution!!!
		// Subitems are currently bypassing ColumnHeaderItem and making direct calls to widget
		// How do we solve this?
	
	
		Widget *		m_pWidget;
		Skin_p			m_pSkin;
		int				m_height;
		int				m_width;
		State			m_state;
		bool			m_bPressed;
	
	};
	
	

} // namespace wg
#endif //WG_COLUMNHEADERITEM_DOT_H
