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


class WgStandardFormatter;
typedef	WgStrongPtr<WgStandardFormatter,WgValueFormatterPtr>		WgStandardFormatterPtr;
typedef	WgWeakPtr<WgStandardFormatter,WgValueFormatterWeakPtr>	WgStandardFormatterWeakPtr;

//____ WgStandardFormatter __________________________________________________________

class WgStandardFormatter : public WgValueFormatter
{
public:	
	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgStandardFormatterPtr	Cast( const WgObjectPtr& pObject );

	static WgStandardFormatterPtr	Create() { return WgStandardFormatterPtr(new WgStandardFormatter()); }
	static WgStandardFormatterPtr	Create( const WgCharSeq& format ) { return WgStandardFormatterPtr(new WgStandardFormatter(format)); }

	WgString 		Format( Sint64 value, int scale ) const;
	WgString 		Format( double value ) const;

protected:
	WgStandardFormatter();
	WgStandardFormatter( const WgCharSeq& format );
	~WgStandardFormatter();


};

#endif //WG_STANDARDFORMATTER_DOT_H