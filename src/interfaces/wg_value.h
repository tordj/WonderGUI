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

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_TEXTPRESENTER_DOT_H
#	include <wg_textpresenter.h>
#endif


#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

#ifndef WG_VALUEFIELD_DOT_H
#	include <wg_valuefield.h>
#endif

namespace wg 
{
	
	
	class Value;
	typedef	WgIStrongPtr<Value,Interface_p>	Value_p;
	typedef	WgIWeakPtr<Value,Interface_wp>	Value_wp;
	
	/**
	 * @brief Interface to a basic value display field
	 * 
	 * The value in a basic value field is set by the widget itself and can
	 * not be modified directly either through the API or UI. Only the formatting
	 * and appearance of the value can be modified through this API.
	 * 
	 */
	
	class Value : public Interface
	{
	public:
		Value(ValueField* pField) : m_pField(pField) {}
	
		virtual bool				isInstanceOf( const char * pClassName ) const;
		virtual const char *		className( void ) const;
		static const char			CLASSNAME[];
		static Value_p			cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline Value_p			ptr() { return Value_p(_object(),this); }
	
		inline void				setFormatter( const ValueFormatter_p& pFormatter ) { m_pField->setFormatter(pFormatter); }
		inline void				clearFormatter() { m_pField->clearFormatter(); }
		inline ValueFormatter_p	formatter() const { return m_pField->formatter(); }
	
		inline void				setStyle( const TextStyle_p& pStyle ) { m_pField->setStyle(pStyle); }
		inline void				clearStyle() { m_pField->clearStyle(); }
		inline TextStyle_p	style() const { return m_pField->style(); }
	
		inline void				setPresenter( const TextPresenter_p& pPresenter ) { m_pField->setPresenter(pPresenter); }
		inline void				clearPresenter() { m_pField->clearPresenter(); }
		inline TextPresenter_p	presenter() const { return m_pField->presenter(); }
	
		inline State			state() const { return m_pField->state(); }
	protected:
		Object * 			_object() const;
	
	
		ValueField *		m_pField;
	};
	
	

} // namespace wg
#endif //WG_VALUE_DOT_H
