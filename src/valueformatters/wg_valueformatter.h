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
#ifndef	WG_VALUEFORMATTER_DOT_H
#define WG_VALUEFORMATTER_DOT_H

#ifndef WG_OBJECT_DOT_H
#	include <wg_object.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

class WgValueFormatter;
typedef	WgStrongPtr<WgValueFormatter,WgObject_p>		WgValueFormatter_p;
typedef	WgWeakPtr<WgValueFormatter,WgObject_wp>	WgValueFormatter_wp;

//____ WgValueFormatter __________________________________________________________

class WgValueFormatter : public WgObject
{
public:
	
	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgValueFormatter_p	cast( const WgObject_p& pObject );

	virtual WgString format( Sint64 value, int scale ) const = 0;
	virtual WgString format( double value ) const = 0;
};

#endif //WG_VALUEFORMATTER_DOT_H
