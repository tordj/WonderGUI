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

namespace wg
{
  
	//____ BasicPayload _______________________________________________________
	
	class BasicPayload;
	typedef StrongPtr<BasicPayload>   BasicPayload_p;
	typedef WeakPtr<BasicPayload>   BasicPayload_wp;

	class BasicPayload : public Object
	{
	public:

		//.____ Creation __________________________________________

		static BasicPayload_p	create() { return BasicPayload_p(new BasicPayload()); }

		//.____ Identification _________________________________________________

		virtual bool		isInstanceOf(const char * pClassName) const override;
		virtual const char * className(void) const override;
		static const char	CLASSNAME[];

	protected:
		BasicPayload() {}
	};

	//____ Payload<> _________________________________________________________________

	template<class Type> class Payload;

	template<class Type> using Payload_p = StrongPtr<Payload<Type>>;
	template<class Type> using Payload_wp = WeakPtr<Payload<Type>>;

	template<class Type> class Payload : public BasicPayload, public Type
	{
	public:

		//.____ Creation __________________________________________

		static Payload_p<Type> create() { return Payload_p<Type>(new Payload<Type>()); }
        static Payload_p<Type> create( const Type& load) { auto p = new Payload<Type>(); * static_cast<Type*>(p) = load; return Payload_p<Type>(p); }

		//.____ Identification _________________________________________________

		bool isInstanceOf(const char * pClassName) const override
		{
			if (pClassName == CLASSNAME)
				return true;

			return BasicPayload::isInstanceOf(pClassName);
		}

		const char *			className(void) const override { return CLASSNAME; }
		static const char *		CLASSNAME;

	protected:

		static char m_className[64];

	};

    // Generating 

	template<class Type> char Payload<Type>::m_className[64] = { "Payload type 0x" };
	template<class Type> const char * Payload<Type>::CLASSNAME = [](char* p) 
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

		return p; 
	}(m_className);

} // namespace wg
#endif //WG_PAYLOAD_DOT_H
