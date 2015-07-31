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
	
	
	class WgColumnHeaderHolder : public WgFieldHolder
	{
	};
	
	
	class WgColumnHeaderField : public WgField, protected WgIconHolder, protected WgTextHolder
	{
	public:
		WgColumnHeaderField(WgColumnHeaderHolder * pHolder);
	
		//____ Subfields __________________________________
	
		WgIconField		icon;
		WgIconField		arrow;
		WgTextField		label;
	
		//____ Methods _____________________________________
	
		void				setSkin( const WgSkin_p& pSkin );
		inline WgSkin_p	skin() const { return m_pSkin; }
	
		WgObject * 			_object() { return m_pHolder->_object(); };
	//private:
		void				_onFieldDirty( WgField * pField );
		void 				_onFieldResize( WgField * pField );
	
	
		WgColumnHeaderHolder *	m_pHolder;
		WgSkin_p			m_pSkin;
		int					m_height;
		int					m_width;
		WgState				m_state;
		bool				m_bPressed;
	
	};
	
	

} // namespace wg
#endif //WG_COLUMNHEADERFIELD_DOT_H
