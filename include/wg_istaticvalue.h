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

#ifndef WG_ISTATICVALUE_DOT_H
#define WG_ISTATICVALUE_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

class WgIStaticValue;
typedef	WgISmartPtr<WgIStaticValue,WgInterfacePtr>		WgIStaticValuePtr;
typedef	WgWeakPtr<WgIStaticValue,WgInterfacePtr>		WgIStaticValueWeakPtr;

/**
 * @brief Interface to a value field displaying a static (non-modifiable) value
 * 
 * The value in a static value field is set by the widget itself and can
 * not be modified directly either through the API or UI. Only the formatting
 * and appearance of the value can be modified through this API.
 * 
 */

class WgIStaticValue : public WgInterface
{
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgIStaticValuePtr	Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.

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

};


#endif //WG_ISTATICVALUE_DOT_H