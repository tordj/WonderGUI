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
	
	class WgObject;
	
	//____ WgItemHolder ___________________________________________________________
	
	class WgItemHolder
	{
	public:
		virtual WgObject*	_object() = 0;
	};
	
	//____ WgItem _________________________________________________________________
	
	class WgItem
	{
	public:
		WgItem( WgItemHolder * pHolder ) : m_pHolder(pHolder) {}
	
		inline WgObject* _object() { return m_pHolder->_object(); }
	
	protected:
	
		WgItemHolder * m_pHolder;
	};
	
	
	

} // namespace wg
#endif //WG_ITEM_DOT_H
