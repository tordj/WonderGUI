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

#ifndef	WG_ITEM_DOT_H

namespace wg 
{
	#define	WG_ITEM_DOT_H
	
	class Object;
	
	//____ ItemHolder ___________________________________________________________
	
	class ItemHolder
	{
	public:
		virtual Object*	_object() = 0;
	};
	
	//____ Item _________________________________________________________________
	
	class Item
	{
	public:
		Item( ItemHolder * pHolder ) : m_pHolder(pHolder) {}
	
		inline Object* _object() { return m_pHolder->_object(); }
	
	protected:
	
		ItemHolder * m_pHolder;
	};
	
	
	

} // namespace wg
#endif //WG_ITEM_DOT_H
