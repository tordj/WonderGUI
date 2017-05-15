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

#ifndef WG_MODVALUE_DOT_H
#define WG_MODVALUE_DOT_H
#pragma once

#include <wg_value.h>
#include <wg_modvalueitem.h>

namespace wg 
{
	
	class CharSeq;
	class String;
	class CharBuffer;
	
	class ModValue;
	typedef	StrongInterfacePtr<ModValue,Value_p>	ModValue_p;
	typedef	WeakInterfacePtr<ModValue,Value_wp>	ModValue_wp;
	
	/**
	 * @brief Interface to a value item where the value is modifiable through the api
	 * 
	 * The value in a modifiable value item can be set through the API, but isn't
	 * editable through the UI.
	 * 
	*/
	
	class ModValue : public Value
	{
	public:
		//.____ Creation __________________________________________

		ModValue(ModValueItem * pItem) : Value(pItem) {}
	
		//.____ Content _____________________________________________
	
		void					clear();
		bool					set( int64_t value, int scale = 1);
	

	/*	For the future...
		inline void				set( float value );
		inline void				set( double value );
	*/
	
		inline int64_t			value() const { return _item()->value(); }
		inline int				scale() const { return _item()->scale(); }
	
		bool					setRange( int64_t min, int64_t max );
		inline int64_t			min() const { return _item()->min(); }
		inline int64_t			max() const { return _item()->max(); }

		//.____ Misc __________________________________________________

		inline ModValue_p		ptr() { return ModValue_p(_object(), this); }


	private:
		inline	ModValueItem * 		_item() { return static_cast<ModValueItem*>(m_pItem); }
		inline	const ModValueItem * _item() const { return static_cast<ModValueItem*>(m_pItem); }
	};
	
	

} // namespace wg
#endif //WG_MODVALUE_DOT_H
