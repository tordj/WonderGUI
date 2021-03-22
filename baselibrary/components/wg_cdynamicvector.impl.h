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

#ifndef	WG_CDYNAMICVECTOR_IMPL_DOT_H
#define	WG_CDYNAMICVECTOR_IMPL_DOT_H
#pragma once

#include <wg_cdynamicvector.h>
#include <wg_cstaticvector.impl.h>

namespace wg
{
	
	template<class EntryType> const TypeInfo CDynamicVector<EntryType>::TYPEINFO = { "CDynamicVector<Unknow>", &CStaticVector<EntryType>::TYPEINFO };

	//____ typeInfo() _________________________________________________________________

	template < class EntryType>
	const TypeInfo& CDynamicVector<EntryType>::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ pushBack() _________________________________________________________________

	template < class EntryType>
	typename CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::pushBack(const EntryType& entry)
	{
		CStaticVector<EntryType>::m_entries.push_back(entry);		
		_didAddEntries(&CStaticVector<EntryType>::m_entries.back(), 1);
		return CStaticVector<EntryType>::m_entries.end()-1;
	}

	template < class EntryType>
	typename CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::pushBack(std::initializer_list<EntryType> entries)
	{
		int amount = (int) CStaticVector<EntryType>::m_entries.size();

		for( auto& entry : entries )
			CStaticVector<EntryType>::m_entries.push_back(entry);

		_didAddEntries( &CStaticVector<EntryType>::m_entries[amount], int(CStaticVector<EntryType>::m_entries.size()) - amount);
		return CStaticVector<EntryType>::m_entries.begin()+amount;
	}

	//____ insert() ______________________________________________________________

	template < class EntryType>
	typename CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::insert(int index, const EntryType& entry)
	{
		//TODO: Add assert

		auto it = CStaticVector<EntryType>::m_entries.insert( CStaticVector<EntryType>::m_entries.begin() + index, entry);
		_didAddEntries( &CStaticVector<EntryType>::m_entries[index], 1);
		return it;
	}

	//____ erase() ________________________________________________________________

	template < class EntryType>
	typename CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::erase(int index)
	{
		//TODO: Add assert

		_willEraseEntries( &CStaticVector<EntryType>::m_entries[index], 1);
		return CStaticVector<EntryType>::m_entries.erase(CStaticVector<EntryType>::m_entries.begin()+index);
	}


	template < class EntryType>
	typename CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::erase(int index, int amount)
	{
		//TODO: Add assert

		_willEraseEntries( &CStaticVector<EntryType>::m_entries[index], amount);
		return CStaticVector<EntryType>::m_entries.erase(CStaticVector<EntryType>::m_entries.begin()+index,CStaticVector<EntryType>::m_entries.begin()+index+amount );
	}

	//____ clear() ____________________________________________________________

	template < class EntryType>
	void CDynamicVector<EntryType>::clear()
	{
		_willEraseEntries(&CStaticVector<EntryType>::m_entries[0], (int) CStaticVector<EntryType>::m_entries.size());
		CStaticVector<EntryType>::m_entries.clear();
	}



} // namespace wg


#endif //WG_CDYNAMICVECTOR_IMPL_DOT_H
