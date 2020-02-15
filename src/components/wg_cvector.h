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
#ifndef WG_CVECTOR_DOT_H
#define WG_CVECTOR_DOT_H
#pragma once

#include <vector>

#include <wg_component.h>

namespace wg
{

	//____ CVector _____________________________________________________
	
	template<class EntryType>
	class CVector : public Component
	{
	public:

		class Holder	/** @private */
		{
		public:
			virtual void	_didAddEntries(EntryType * pEntry, int nb) = 0;
			virtual void	_didMoveEntries(EntryType * pFrom, EntryType * pTo, int nb) = 0;
			virtual void	_willEraseEntries(EntryType * pSlot, int nb) = 0;
		};
		
		using		iterator = typename std::vector<EntryType>::iterator;

		CVector(Holder * pHolder) : m_pHolder(pHolder) {}

		//.____ Operators __________________________________________

		inline iterator operator<<(const EntryType& entry) { return pushBack(entry); }
		inline iterator operator<<(const std::initializer_list<const EntryType>& entries) { return pushBack(entries); }

		inline EntryType& operator[](int index) { return m_entries[index]; }
		inline const EntryType& operator[](int index) const { return m_entries[index]; }

		//.____ Content _______________________________________________________

		inline int		size() const { return m_entries.size(); }
		inline bool		isEmpty() const { return m_entries.empty(); }

		inline int		capacity() const { return m_entries.capacity(); }

		inline void		reserve(int amount) { m_entries.reserve( amount ); }
		inline void		shrinkToFit() { m_entries.shrink_to_fit(); }

		iterator		pushBack( const EntryType& entry );
		iterator		pushBack( const std::initializer_list<const EntryType>& entries );

		iterator		insert( int index, const EntryType& entry );

		iterator		erase( int index );
		iterator		erase( int index, int amount );

		void			clear();

		//.____ Misc _______________________________________________________

		inline iterator	begin() const { return m_entries.begin(); }
		inline iterator	end() const { return m_entries.end(); }

		
	protected:
//		~CVector() {}
	
		Object *		_object() override;
		const Object *	_object() const override;
	
		Holder *				m_pHolder;
		std::vector<EntryType>	m_entries;
	};
} //namespace

#endif //WG_CVECTOR_DOT_H