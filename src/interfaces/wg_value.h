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

#ifndef WG_VALUE_DOT_H
#define WG_VALUE_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_textstyle.h>
#include <wg_textmapper.h>


//#ifndef WG_VALUEFORMAT_DOT_H
//#	include <wg_valueformat.h>
//#endif

#include <wg_valueitem.h>

namespace wg 
{
	
	
	class Value;
	typedef	StrongInterfacePtr<Value>	Value_p;
	typedef	WeakInterfacePtr<Value,Interface_wp>	Value_wp;
	
	/**
	 * @brief Interface to a basic value display item
	 * 
	 * The value in a basic value item is set by the widget itself and can
	 * not be modified directly either through the API or UI. Only the formatting
	 * and appearance of the value can be modified through this API.
	 * 
	 */
	
	class Value : public Interface
	{
	public:
		//.____ Creation __________________________________________

		Value(ValueItem* pItem) : m_pItem(pItem) {}
	
		//.____ Appearance _____________________________________________

		inline void				setFormatter( ValueFormatter * pFormatter ) { m_pItem->setFormatter(pFormatter); }
		inline void				clearFormatter() { m_pItem->clearFormatter(); }
		inline ValueFormatter_p	formatter() const { return m_pItem->formatter(); }
	
		inline void				setStyle( TextStyle * pStyle ) { m_pItem->setStyle(pStyle); }
		inline void				clearStyle() { m_pItem->clearStyle(); }
		inline TextStyle_p		style() const { return m_pItem->style(); }
	
		inline void				setTextMapper( TextMapper * pTextMapper ) { m_pItem->setTextMapper(pTextMapper); }
		inline void				clearTextMapper() { m_pItem->clearTextMapper(); }
		inline TextMapper_p		textMapper() const { return m_pItem->textMapper(); }

		//.____ State _______________________________________________

		inline State			state() const { return m_pItem->state(); }

		//.____ Misc __________________________________________________

		inline Value_p			ptr() { return Value_p(this); }


	protected:
		Object * 		_object() const;

		ValueItem *		m_pItem;
	};
	
	

} // namespace wg
#endif //WG_VALUE_DOT_H
