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

#ifndef WG_PAYLOAD_DOT_H
#define	WG_PAYLOAD_DOT_H
#pragma once

#include <cstdint>
#include <wg_pointers.h>

#include <functional>

namespace wg
{

	//____ BasicDataset _______________________________________________________

	class BasicDataset;
	typedef StrongPtr<BasicDataset>   BasicDataset_p;
	typedef WeakPtr<BasicDataset>   BasicDataset_wp;

	class BasicDataset : public Object
	{
	public:

		//.____ Creation __________________________________________

		static BasicDataset_p	create() { return BasicDataset_p(new BasicDataset()); }

		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		BasicDataset() {}
	};

	//____ Dataset<> _________________________________________________________________

	template<class Type> class Dataset;

	template<class Type> using Dataset_p = StrongPtr<Dataset<Type>>;
	template<class Type> using Dataset_wp = WeakPtr<Dataset<Type>>;

	template<class Type> class Dataset : public BasicDataset
	{
	public:

		//.____ Creation __________________________________________

		static Dataset_p<Type> create() { return Dataset_p<Type>(new Dataset<Type>()); }
		static Dataset_p<Type> create( const Type& load) { auto p = new Dataset<Type>(); p->data = load; return Dataset_p<Type>(p); }

		//.____ Identification _________________________________________________

		const TypeInfo&			typeInfo(void) const override { return TYPEINFO; }
		static TypeInfo	TYPEINFO;

        //.____ Content _______________________________________________________

        Type    data;

	protected:

		static char s_className[64];

	};

	// Generating

	template<class Type> char Dataset<Type>::s_className[64] = { "Dataset type 0x" };
	template<class Type> TypeInfo Dataset<Type>::TYPEINFO = [](char* p)
	{
		uintptr_t x = reinterpret_cast<uintptr_t>(p);

		char * pWrite = p + 15 + 16;
		*pWrite-- = 0;
		for (int i = 0; i < 16; i++)
		{
			char c = char(x & 0xF);
			*pWrite-- = c + (c < 10 ? '0' : 'a' - 10);
			x >>= 4;
		}

		TypeInfo ti = { p, &BasicDataset::TYPEINFO };
		return ti;
	}(s_className);

} // namespace wg
#endif //WG_PAYLOAD_DOT_H
