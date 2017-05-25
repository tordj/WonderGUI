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

#ifndef WG_TEXT_DOT_H
#define WG_TEXT_DOT_H
#pragma once

#include <wg_textitem.h>
#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_textstyle.h>

namespace wg 
{
	
	class Color;
	class CharSeq;
	class String;
	class CharBuffer;
	
	class Text;
	typedef	StrongInterfacePtr<Text>	Text_p;
	typedef	WeakInterfacePtr<Text,Interface_wp>		Text_wp;
	
	/**
	 * @brief Interface to a basic text item.
	 *
	 * Interface to a basic text item.
	 *
	 * The text in a basic text item is set by the widget itself and can
	 * not be modified directly either through the API or UI. Only the appearance
	 * of the text can be modified through this interface.
	 * 
	 */
	
	class Text : public Interface
	{
	public:
		//.____ Creation __________________________________________

		Text( TextItem * pItem ) : m_pItem(pItem) {};
	
		//.____ Appearance _____________________________________________

		inline void				setStyle( TextStyle * pStyle ) { m_pItem->setStyle(pStyle); }
		inline void				clearStyle() { m_pItem->clearStyle(); }
		inline TextStyle_p		style() const { return m_pItem->style(); }
	
		inline void				setTextMapper( TextMapper * pTextMapper ) { m_pItem->setTextMapper(pTextMapper); }
		inline void				clearTextMapper() { m_pItem->clearTextMapper(); }
		inline TextMapper_p		textMapper() const { return m_pItem->textMapper(); }

		//.____ State _______________________________________________

		inline State			state() const { return m_pItem->state(); }

		//.____ Content _____________________________________________

		inline 	String			get() const { return m_pItem->getString(); }
		inline int				length() const { return m_pItem->length(); }
		inline bool				isEmpty() const { return m_pItem->isEmpty(); }

		//.____ Misc __________________________________________________

		inline Text_p			ptr() { return Text_p(this); }

	protected:
		Object *				_object() const;
	
		TextItem * 			m_pItem;
	};
	
	

} // namespace wg
#endif //WG_TEXT_DOT_H
