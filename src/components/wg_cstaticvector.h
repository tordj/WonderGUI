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
#ifndef WG_CSTATICStaticVector_DOT_H
#define WG_CSTATICStaticVector_DOT_H
#pragma once

#include <vector>

#include <wg_component.h>

namespace wg
{

	//____ CStaticVector _____________________________________________________
	
	template<class EntryType>
	class CStaticVector : public Component
	{
	public:

		class Holder	/** @private */
		{
		public:
			virtual Object * _object() = 0;
		};
		
		using		iterator = typename std::vector<EntryType>::iterator;
		using		const_iterator = typename std::vector<EntryType>::const_iterator;

		CStaticVector(Holder * pHolder) : m_pHolder(pHolder) {}

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline int		size() const { return (int) m_entries.size(); }
		inline bool		isEmpty() const { return m_entries.empty(); }

		//.____ Misc _______________________________________________________

		inline iterator	begin() { return m_entries.begin(); }
		inline iterator	end() { return m_entries.end(); }

		inline const_iterator	begin() const { return m_entries.begin(); }
		inline const_iterator	end() const { return m_entries.end(); }

		inline EntryType& front() { return m_entries.front(); }
		inline const EntryType& front() const { return m_entries.front(); }

		inline EntryType& back() { return m_entries.back(); }
		inline const EntryType& back() const { return m_entries.back(); }

		//.____ Operators __________________________________________

		inline EntryType& operator[](int index) { return m_entries[index]; }
		inline const EntryType& operator[](int index) const { return m_entries[index]; }

	protected:
//		~CStaticVector() {}
	
		Object *		_object() override;
		const Object *	_object() const override;
	
		Holder *				m_pHolder;
		std::vector<EntryType>	m_entries;
	};

} //namespace

#endif //WG_CStaticVector_DOT_H
