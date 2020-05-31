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

#ifndef	WG_CDYNAMICDynamicVector_IMPL_DOT_H
#define	WG_CDYNAMICDynamicVector_IMPL_DOT_H
#pragma once

#include <wg_cdynamicvector.h>

namespace wg
{
	template<class EntryType> const TypeInfo CDynamicVector<EntryType>::TYPEINFO = { "CDynamicVector<Unknow>", &CStaticVector<EntryType>::TYPEINFO };

	//____ pushBack() _________________________________________________________________

	template < class EntryType>
	CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::pushBack(const EntryType& entry)
	{
		m_entries.push_back(entry);		
		_didAddEntries(&m_entries.back(), 1);
		return m_entries.end()-1;
	}

	template < class EntryType>
	CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::pushBack(const std::initializer_list<const EntryType>& entries)
	{
		int offset = m_entries.size();
		
		m_entries.push_back(entries);
		_didAddEntries( &m_entries[offset], m_entries.size() - offset);
		return m_entries.begin()+offset;
	}

	//____ insert() ______________________________________________________________

	template < class EntryType>
	CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::insert(int index, const EntryType& entry)
	{
		//TODO: Add assert

		auto it = m_entries.insert( m_entries.begin() + index, entry);
		_didAddEntries( &m_entries[index], 1);
		return it;
	}

	//____ erase() ________________________________________________________________

	template < class EntryType>
	CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::erase(int index)
	{
		//TODO: Add assert

		_willEraseEntries( &m_entries[index], 1);
		return m_entries.erase(m_entries.begin()+index);
	}


	template < class EntryType>
	CDynamicVector<EntryType>::iterator CDynamicVector<EntryType>::erase(int index, int amount)
	{
		//TODO: Add assert

		_willEraseEntries( &m_entries[index], amount);
		return m_entries.erase(m_entries.begin()+index,m_entries.begin()+index+amount );
	}

	//____ clear() ____________________________________________________________

	template < class EntryType>
	void CDynamicVector<EntryType>::clear()
	{
		_willEraseEntries(&m_entries[0], m_entries.size());
		m_entries.clear();
	}



} // namespace wg


#endif //WG_CDYNAMICVECTOR_IMPL_DOT_H
