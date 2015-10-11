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

namespace wg 
{
	
	class ValueFormatter;
	typedef	WgStrongPtr<ValueFormatter,Object_p>		ValueFormatter_p;
	typedef	WgWeakPtr<ValueFormatter,Object_wp>	ValueFormatter_wp;
	
	//____ ValueFormatter __________________________________________________________
	
	class ValueFormatter : public Object
	{
	public:
		
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static ValueFormatter_p	cast( const Object_p& pObject );
	
		virtual String format( int64_t value, int scale ) const = 0;
		virtual String format( double value ) const = 0;
	};
	

} // namespace wg
#endif //WG_VALUEFORMATTER_DOT_H
