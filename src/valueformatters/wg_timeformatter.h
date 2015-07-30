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


class WgTimeFormatter;
typedef	WgStrongPtr<WgTimeFormatter,WgValueFormatterPtr>		WgTimeFormatterPtr;
typedef	WgWeakPtr<WgTimeFormatter,WgValueFormatterWeakPtr>	WgTimeFormatterWeakPtr;

//____ WgTimeFormatter __________________________________________________________

// Format: %D days %2H:%2M:%2S.%3F
//  %4F
//  %4S.%4F

class WgTimeFormatter : public WgValueFormatter
{
public:
	
	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgTimeFormatterPtr	cast( const WgObjectPtr& pObject );

	static WgTimeFormatterPtr	create() { return WgTimeFormatterPtr(new WgTimeFormatter()); }
	static WgTimeFormatterPtr	create( const WgCharSeq& format24 ) { return WgTimeFormatterPtr(new WgTimeFormatter(format24)); }
	static WgTimeFormatterPtr	create( const WgCharSeq& formatAM, const WgCharSeq& formatPM ) { return WgTimeFormatterPtr(new WgTimeFormatter(formatAM,formatPM)); }

	WgString 		format( Sint64 value, int scale ) const;
	WgString 		format( double value ) const;
	
protected:
	WgTimeFormatter() {};
	WgTimeFormatter( const WgCharSeq& format24 );
	WgTimeFormatter( const WgCharSeq& formatAM, const WgCharSeq& formatPM );
	
    ~WgTimeFormatter() {} 
	
	WgString	m_format24;
	WgString	m_formatAM;
	WgString	m_formatPM;
};

#endif //WG_TIMEFORMATTER_DOT_H
