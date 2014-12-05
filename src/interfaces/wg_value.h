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

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

class WgValueField;

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


	virtual void				SetFormat( const WgValueFormatPtr& format ) = 0;
	virtual WgValueFormatPtr	Format() const = 0;

	virtual void				SetProperties( const WgTextpropPtr& pProp ) = 0;
	virtual void				ClearProperties() = 0;
	virtual WgTextpropPtr		Properties() const = 0;

	virtual void				SetAlignment( WgOrigo alignment ) = 0;
	virtual WgOrigo				Alignment() const = 0;

	virtual void				SetTintMode( WgTintMode mode ) = 0;
	virtual WgTintMode			TintMode() const = 0;

	virtual void				SetAutoEllipsis(bool bAutoEllipsis) = 0;
	virtual bool				AutoEllipsis() const = 0;

	virtual WgState				State() const = 0;
	virtual int					TextLength() const = 0;
protected:
	WgObject * 			_object() const;


	WgValueField *		m_pField;


};


#endif //WG_VALUE_DOT_H
