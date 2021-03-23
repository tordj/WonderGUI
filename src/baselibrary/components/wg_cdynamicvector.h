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
#ifndef WG_CDYNAMICVECTOR_DOT_H
#define WG_CDYNAMICVECTOR_DOT_H
#pragma once


#include <wg_cstaticvector.h>

namespace wg
{

	//____ CDynamicVector _____________________________________________________
	
	template<class EntryType>
	class CDynamicVector : public CStaticVector<EntryType>
	{
	public:

		using iterator = typename CStaticVector<EntryType>::iterator;

		class Holder : public CStaticVector<EntryType>::Holder	/** @private */
		{
		public:
			virtual void	_didAddEntries(EntryType * pEntry, int nb) = 0;
			virtual void	_didMoveEntries(EntryType * pFrom, EntryType * pTo, int nb) = 0;
			virtual void	_willEraseEntries(EntryType * pEntry, int nb) = 0;
		};
		
		CDynamicVector(Holder * pHolder) : CStaticVector<EntryType>(pHolder) {}

		//.____ Identification _________________________________________________ 

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline int		capacity() const { return (int) CStaticVector<EntryType>::m_entries.capacity(); }

		inline void		reserve(int amount) { CStaticVector<EntryType>::m_entries.reserve( amount ); }
		inline void		shrinkToFit() { CStaticVector<EntryType>::m_entries.shrink_to_fit(); }

		iterator		pushBack( const EntryType& entry );
		iterator		pushBack( std::initializer_list<EntryType> entries );

		iterator		insert( int index, const EntryType& entry );

		iterator		erase( int index );
		iterator		erase( int index, int amount );

		void			clear();

		//.____ Operators __________________________________________

		inline iterator operator<<(const EntryType& entry) { return pushBack(entry); }
		inline iterator operator<<(std::initializer_list<EntryType> entries) { return pushBack(entries); }

	protected:
//		~CDynamicVector() {}
	
		virtual void	_didAddEntries(EntryType* pEntry, int nb) { static_cast<Holder*>(CStaticVector<EntryType>::m_pHolder)->_didAddEntries(pEntry, nb); }
		virtual void	_didMoveEntries(EntryType* pFrom, EntryType* pTo, int nb) { static_cast<Holder*>(CStaticVector<EntryType>::m_pHolder)->_didMoveEntries(pFrom, pTo, nb); }
		virtual void	_willEraseEntries(EntryType* pEntry, int nb) { static_cast<Holder*>(CStaticVector<EntryType>::m_pHolder)->_willEraseEntries(pEntry, nb); }
	};

} //namespace

#endif //WG_CDYNAMICVECTOR_DOT_H
