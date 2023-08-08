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
#ifndef WG_DYNAMICVECTOR_DOT_H
#define WG_DYNAMICVECTOR_DOT_H
#pragma once


#include <wg_staticvector.h>

namespace wg
{

	//____ DynamicVector _____________________________________________________
	
	template<class EntryType>
	class DynamicVector : public StaticVector<EntryType>
	{
	public:

		using iterator = typename StaticVector<EntryType>::iterator;

		class Holder	/** @private */
		{
		public:
			virtual void	_didAddEntries(EntryType * pEntry, int nb) = 0;
			virtual void	_didMoveEntries(EntryType * pFrom, EntryType * pTo, int nb) = 0;
			virtual void	_willEraseEntries(EntryType * pEntry, int nb) = 0;
		};
		
		DynamicVector(Holder * pHolder) : StaticVector<EntryType>(), m_pHolder(pHolder) {}

		//.____ Content _______________________________________________________

		inline int		capacity() const { return (int) StaticVector<EntryType>::m_entries.capacity(); }

		inline void		reserve(int amount) { StaticVector<EntryType>::m_entries.reserve( amount ); }
		inline void		shrinkToFit() { StaticVector<EntryType>::m_entries.shrink_to_fit(); }

		iterator		pushBack( const struct EntryType::Blueprint& entry );
		iterator		pushBack( const std::initializer_list<struct EntryType::Blueprint>& entries );

		iterator		insert( int index, const struct EntryType::Blueprint& entry );

		iterator		erase( int index );
		iterator		erase( int index, int amount );

		void			clear();

		//.____ Operators __________________________________________

		inline iterator operator<<(const EntryType& entry) { return pushBack(entry); }

	protected:

		Holder* m_pHolder;


		virtual void	_didAddEntries(EntryType* pEntry, int nb) { m_pHolder->_didAddEntries(pEntry, nb); }
		virtual void	_didMoveEntries(EntryType* pFrom, EntryType* pTo, int nb) { m_pHolder->_didMoveEntries(pFrom, pTo, nb); }
		virtual void	_willEraseEntries(EntryType* pEntry, int nb) { m_pHolder->_willEraseEntries(pEntry, nb); }
	};


	//____ pushBack() _________________________________________________________________

	template < class EntryType>
	typename DynamicVector<EntryType>::iterator DynamicVector<EntryType>::pushBack(const struct EntryType::Blueprint& entry)
	{
		
		StaticVector<EntryType>::m_entries.emplace_back(entry);
		_didAddEntries(&StaticVector<EntryType>::m_entries.back(), 1);
		return StaticVector<EntryType>::m_entries.end() - 1;
	}

	template < class EntryType>
	typename DynamicVector<EntryType>::iterator DynamicVector<EntryType>::pushBack( const std::initializer_list<struct EntryType::Blueprint>& entries)
	{
		int amount = (int)StaticVector<EntryType>::m_entries.size();

		for (auto& entry : entries)
			StaticVector<EntryType>::m_entries.emplace_back(entry);

		_didAddEntries(&StaticVector<EntryType>::m_entries[amount], int(StaticVector<EntryType>::m_entries.size()) - amount);
		return StaticVector<EntryType>::m_entries.begin() + amount;
	}

	//____ insert() ______________________________________________________________

	template < class EntryType>
	typename DynamicVector<EntryType>::iterator DynamicVector<EntryType>::insert(int index, const struct EntryType::Blueprint& entry)
	{
		//TODO: Add assert

		auto it = StaticVector<EntryType>::m_entries.emplace(StaticVector<EntryType>::m_entries.begin() + index, entry);
		_didAddEntries(&StaticVector<EntryType>::m_entries[index], 1);
		return it;
	}

	//____ erase() ________________________________________________________________

	template < class EntryType>
	typename DynamicVector<EntryType>::iterator DynamicVector<EntryType>::erase(int index)
	{
		//TODO: Add assert

		_willEraseEntries(&StaticVector<EntryType>::m_entries[index], 1);
		return StaticVector<EntryType>::m_entries.erase(StaticVector<EntryType>::m_entries.begin() + index);
	}


	template < class EntryType>
	typename DynamicVector<EntryType>::iterator DynamicVector<EntryType>::erase(int index, int amount)
	{
		//TODO: Add assert

		_willEraseEntries(&StaticVector<EntryType>::m_entries[index], amount);
		return StaticVector<EntryType>::m_entries.erase(StaticVector<EntryType>::m_entries.begin() + index, StaticVector<EntryType>::m_entries.begin() + index + amount);
	}

	//____ clear() ____________________________________________________________

	template < class EntryType>
	void DynamicVector<EntryType>::clear()
	{
		_willEraseEntries(&StaticVector<EntryType>::m_entries[0], (int)StaticVector<EntryType>::m_entries.size());
		StaticVector<EntryType>::m_entries.clear();
	}



} //namespace

#endif //WG_DYNAMICVECTOR_DOT_H
