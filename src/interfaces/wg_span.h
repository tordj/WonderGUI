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

#ifndef WG_SPAN_DOT_H
#define WG_SPAN_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_spanitem.h>
#include <wg_pointers.h>

namespace wg 
{
	
	class Span;
	typedef	StrongInterfacePtr<Span>			Span_p;
	typedef	WeakInterfacePtr<Span>		Span_wp;
	
	
	class Span : public Interface
	{
	public:
		/** @private */

		Span(SpanItem* pItem) : m_pItem(pItem) {}
		
		//.____ Control ___________________________________________	

		inline int		min() const { return m_pItem->min; }
		inline int		max() const { return m_pItem->max; }
		inline int 		begin() const { return m_pItem->begin; }
		inline int		length() const { return m_pItem->length; }
	
		inline float	relativePos() const { return m_pItem->relativePos(); }
		inline float	relativeBegin() const { return m_pItem->relativeBegin(); }
		inline float	relativeLength() const { return m_pItem->relativeLength(); };
	
		//.____ Misc __________________________________________________

		inline Span_p			ptr() { return Span_p(this); }

	protected:
		virtual Object * 		_object() const;
	
		SpanItem *		m_pItem;
	};
	
	

} // namespace wg
#endif //WG_SPAN_DOT_H
