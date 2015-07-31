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
#ifndef	WG_STANDARDFORMATTER_DOT_H
#define WG_STANDARDFORMATTER_DOT_H

#include <wg_valueformatter.h>

namespace wg 
{
	
	
	class WgStandardFormatter;
	typedef	WgStrongPtr<WgStandardFormatter,WgValueFormatter_p>		WgStandardFormatter_p;
	typedef	WgWeakPtr<WgStandardFormatter,WgValueFormatter_wp>	WgStandardFormatter_wp;
	
	//____ WgStandardFormatter __________________________________________________________
	
	class WgStandardFormatter : public WgValueFormatter
	{
	public:	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static WgStandardFormatter_p	cast( const WgObject_p& pObject );
	
		static WgStandardFormatter_p	create() { return WgStandardFormatter_p(new WgStandardFormatter()); }
		static WgStandardFormatter_p	create( const WgCharSeq& format ) { return WgStandardFormatter_p(new WgStandardFormatter(format)); }
	
		WgString 		format( Sint64 value, int scale ) const;
		WgString 		format( double value ) const;
	
	protected:
		WgStandardFormatter();
		WgStandardFormatter( const WgCharSeq& format );
		~WgStandardFormatter();
	
	
	};
	

} // namespace wg
#endif //WG_STANDARDFORMATTER_DOT_H
