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
typedef	WgIStrongPtr<WgValue,WgInterfacePtr>	WgValuePtr;
typedef	WgIWeakPtr<WgValue,WgInterfaceWeakPtr>	WgValueWeakPtr;

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

	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgValuePtr			Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.
	inline WgValuePtr			Ptr() { return WgValuePtr(_object(),this); }

	inline void				SetFormatter( const WgValueFormatterPtr& pFormatter ) { m_pField->SetFormatter(pFormatter); }
	inline void				ClearFormatter() { m_pField->ClearFormatter(); }
	inline WgValueFormatterPtr	Formatter() const { return m_pField->Formatter(); }

	inline void				SetStyle( const WgTextStylePtr& pStyle ) { m_pField->SetStyle(pStyle); }
	inline void				ClearStyle() { m_pField->ClearStyle(); }
	inline WgTextStylePtr	Style() const { return m_pField->Style(); }

	inline void				SetPresenter( const WgTextPresenterPtr& pPresenter ) { m_pField->SetPresenter(pPresenter); }
	inline void				ClearPresenter() { m_pField->ClearPresenter(); }
	inline WgTextPresenterPtr	Presenter() const { return m_pField->Presenter(); }

	inline WgState			State() const { return m_pField->State(); }
protected:
	WgObject * 			_object() const;


	WgValueField *		m_pField;
};


#endif //WG_VALUE_DOT_H
