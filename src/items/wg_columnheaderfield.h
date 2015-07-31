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
#ifndef WG_COLUMNHEADERFIELD_DOT_H
#define WG_COLUMNHEADERFIELD_DOT_H

#ifndef WG_ICONFIELD_DOT_H
#	include <wg_iconfield.h>
#endif

#ifndef WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

namespace wg 
{
	
	
	class ColumnHeaderHolder : public FieldHolder
	{
	};
	
	
	class ColumnHeaderField : public Field, protected IconHolder, protected TextHolder
	{
	public:
		ColumnHeaderField(ColumnHeaderHolder * pHolder);
	
		//____ Subfields __________________________________
	
		IconField		icon;
		IconField		arrow;
		TextField		label;
	
		//____ Methods _____________________________________
	
		void				setSkin( const Skin_p& pSkin );
		inline Skin_p	skin() const { return m_pSkin; }
	
		Object * 			_object() { return m_pHolder->_object(); };
	//private:
		void				_onFieldDirty( Field * pField );
		void 				_onFieldResize( Field * pField );
	
	
		ColumnHeaderHolder *	m_pHolder;
		Skin_p			m_pSkin;
		int					m_height;
		int					m_width;
		State				m_state;
		bool				m_bPressed;
	
	};
	
	

} // namespace wg
#endif //WG_COLUMNHEADERFIELD_DOT_H
