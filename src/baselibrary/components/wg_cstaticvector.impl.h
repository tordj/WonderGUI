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

#ifndef	WG_CSTATICVECTOR_IMPL_DOT_H
#define	WG_CSTATICVECTOR_IMPL_DOT_H
#pragma once

#include <wg_cstaticvector.h>

namespace wg
{
	template<class EntryType> const TypeInfo CStaticVector<EntryType>::TYPEINFO = { "CStaticVector<Unknow>", &Component::TYPEINFO };

	//____ typeInfo() ________________________________________________________

	template < class EntryType>
	const TypeInfo& CStaticVector<EntryType>::typeInfo(void) const
	{
		return TYPEINFO;
		
	}

	//____ _object() __________________________________________________________

	template < class EntryType>
	const Object * 	CStaticVector<EntryType>::_object() const
	{ 
		return m_pHolder->_object(); 
	}

	template < class EntryType>
	Object * CStaticVector<EntryType>::_object() 
	{ 
		return m_pHolder->_object(); 
	}


} // namespace wg


#endif //WG_CSTATICVECTOR_IMPL_DOT_H
