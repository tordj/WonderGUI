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

#ifndef WG_ISPAN_DOT_H
#define WG_ISPAN_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_cspan.h>
#include <wg_pointers.h>

namespace wg
{

	class ISpan;
	typedef	StrongInterfacePtr<ISpan>	ISpan_p;
	typedef	WeakInterfacePtr<ISpan>		ISpan_wp;


	class ISpan : public Interface
	{
	public:
		/** @private */

		ISpan(CSpan* pComponent) : m_pComponent(pComponent) {}

		//.____ Control ___________________________________________

		inline int		min() const { return m_pComponent->min; }
		inline int		max() const { return m_pComponent->max; }
		inline int 		begin() const { return m_pComponent->begin; }
		inline int		length() const { return m_pComponent->length; }

		inline float	relativePos() const { return m_pComponent->relativePos(); }
		inline float	relativeBegin() const { return m_pComponent->relativeBegin(); }
		inline float	relativeLength() const { return m_pComponent->relativeLength(); };

		//.____ Misc __________________________________________________

		inline ISpan_p			ptr() { return ISpan_p(this); }

	protected:
		virtual Object * 		_object() const;

		CSpan *		m_pComponent;
	};



} // namespace wg
#endif //WG_ISPAN_DOT_H
