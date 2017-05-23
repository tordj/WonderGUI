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
#pragma once

#include <wg_valueformatter.h>

namespace wg 
{
	
	
	class StandardFormatter;
	typedef	StrongPtr<StandardFormatter>		StandardFormatter_p;
	typedef	WeakPtr<StandardFormatter,ValueFormatter_wp>	StandardFormatter_wp;
	
	//____ StandardFormatter __________________________________________________________
	
	class StandardFormatter : public ValueFormatter
	{
	public:	
		//.____ Creation __________________________________________

		static StandardFormatter_p	create() { return StandardFormatter_p(new StandardFormatter()); }
		static StandardFormatter_p	create( const CharSeq& format ) { return StandardFormatter_p(new StandardFormatter(format)); }

		//.____ Identification __________________________________________

		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static StandardFormatter_p	cast( Object * pObject );
	
		//.____ Misc __________________________________________________
	
		String 		format( int64_t value, int scale ) const;
		String 		format( double value ) const;
	
	protected:
		StandardFormatter();
		StandardFormatter( const CharSeq& format );
		~StandardFormatter();
	
	
	};
	

} // namespace wg
#endif //WG_STANDARDFORMATTER_DOT_H
