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


class WgValue;
typedef	WgIStrongPtr<WgValue,WgInterface_p>	WgValue_p;
typedef	WgIWeakPtr<WgValue,WgInterface_wp>	WgValue_wp;

/**
 * @brief Interface to a basic value display field
 * 
 * The value in a basic value field is set by the widget itself and can
 * not be modified directly either through the API or UI. Only the formatting
 * and appearance of the value can be modified through this API.
 * 
 */

class WgValue : public WgInterface
{
public:
	WgValue(WgValueField* pField) : m_pField(pField) {}

	virtual bool				isInstanceOf( const char * pClassName ) const;
	virtual const char *		className( void ) const;
	static const char			CLASSNAME[];
	static WgValue_p			cast( const WgInterface_p& pInterface );				// Provided just for completeness sake.
	inline WgValue_p			ptr() { return WgValue_p(_object(),this); }

	inline void				setFormatter( const WgValueFormatter_p& pFormatter ) { m_pField->setFormatter(pFormatter); }
	inline void				clearFormatter() { m_pField->clearFormatter(); }
	inline WgValueFormatter_p	formatter() const { return m_pField->formatter(); }

	inline void				setStyle( const WgTextStyle_p& pStyle ) { m_pField->setStyle(pStyle); }
	inline void				clearStyle() { m_pField->clearStyle(); }
	inline WgTextStyle_p	style() const { return m_pField->style(); }

	inline void				setPresenter( const WgTextPresenter_p& pPresenter ) { m_pField->setPresenter(pPresenter); }
	inline void				clearPresenter() { m_pField->clearPresenter(); }
	inline WgTextPresenter_p	presenter() const { return m_pField->presenter(); }

	inline WgState			state() const { return m_pField->state(); }
protected:
	WgObject * 			_object() const;


	WgValueField *		m_pField;
};


#endif //WG_VALUE_DOT_H
