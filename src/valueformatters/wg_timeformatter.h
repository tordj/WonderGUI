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
#ifndef	WG_TIMEFORMATTER_DOT_H
#define WG_TIMEFORMATTER_DOT_H

#include <wg_valueformatter.h>

namespace wg 
{
	
	
	class TimeFormatter;
	typedef	WgStrongPtr<TimeFormatter,ValueFormatter_p>		TimeFormatter_p;
	typedef	WgWeakPtr<TimeFormatter,ValueFormatter_wp>	TimeFormatter_wp;
	
	//____ TimeFormatter __________________________________________________________
	
	// Format: %D days %2H:%2M:%2S.%3F
	//  %4F
	//  %4S.%4F
	
	class TimeFormatter : public ValueFormatter
	{
	public:
		
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static TimeFormatter_p	cast( const Object_p& pObject );
	
		static TimeFormatter_p	create() { return TimeFormatter_p(new TimeFormatter()); }
		static TimeFormatter_p	create( const CharSeq& format24 ) { return TimeFormatter_p(new TimeFormatter(format24)); }
		static TimeFormatter_p	create( const CharSeq& formatAM, const CharSeq& formatPM ) { return TimeFormatter_p(new TimeFormatter(formatAM,formatPM)); }
	
		String 		format( Sint64 value, int scale ) const;
		String 		format( double value ) const;
		
	protected:
		TimeFormatter() {};
		TimeFormatter( const CharSeq& format24 );
		TimeFormatter( const CharSeq& formatAM, const CharSeq& formatPM );
		
	    ~TimeFormatter() {} 
		
		String	m_format24;
		String	m_formatAM;
		String	m_formatPM;
	};
	

} // namespace wg
#endif //WG_TIMEFORMATTER_DOT_H
