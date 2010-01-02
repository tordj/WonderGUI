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


#include <wg_texttool.h>
#include <wg_valueformat.h>


//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat()
{
	integers		= 1;
	decimals		= 0;
	grouping		= 0;
	separator		= 0xA0 /*0xA0=NO_BREAK_SPACE*/;
	period			= 0x2e;
	prefix[0]		= 0;
	suffix[0]		= 0;
	bPlus			= false;
	bZeroIsNegative = false;
	bForcePeriod	= false;
	bSetTextProp	= false;
	bForceDecimals	= true;
	noDecimalThreshold	= 0;
}

//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat( const WgValueFormat& in )
{
	integers	= in.integers;
	decimals	= in.decimals;
	grouping	= in.grouping;
	separator	= in.separator;
	period		= in.period;

	for( int i = 0 ; i < 4 ; i++ )
	{
		prefix[i]	= in.prefix[i];
		suffix[i]	= in.suffix[i];
	}

	bPlus			= in.bPlus;
	bZeroIsNegative = in.bZeroIsNegative;
	bForcePeriod	= in.bForcePeriod;
	pTextProperties	= in.pTextProperties;
	bSetTextProp	= in.bSetTextProp;
	bForceDecimals	= in.bForceDecimals;
	noDecimalThreshold = in.noDecimalThreshold;
}

//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat( int nInt, int nDec, int grouping, bool bPlus,
								Uint16 _separator, Uint16 period, bool bForcePeriod,
								const char * pPrefix, const char * pSuffix )
{
	separator		= _separator /*0xA0=NO_BREAK_SPACE*/;
	period			= 0x2e;
	prefix[0]		= 0;
	suffix[0]		= 0;

	bSetTextProp	= false;
	bForceDecimals	= true;
	bZeroIsNegative = false;
	noDecimalThreshold	= 0;

	setFormat( nInt, nDec, grouping, bPlus, separator, period, bForcePeriod );
	setPrefix( pPrefix );
	setSuffix( pSuffix );

}

//____ setFormat() ____________________________________________________________

void WgValueFormat::setFormat( int _nInt, int _nDec, int _grouping, bool _bPlus,
															Uint16 _separator, Uint16 _period, bool _bForcePeriod )
{
	integers		= _nInt;
	decimals		= _nDec;
	grouping		= _grouping;
	bPlus			= _bPlus;
	bForcePeriod	= _bForcePeriod;
	if( _separator )
		separator = _separator;
	if( _period )
		period		= _period;
}

//____ setPrefix(1) ___________________________________________________________

void WgValueFormat::setPrefix( const Uint16 * pText )
{
	if( !pText )
		prefix[0] = 0;
	else
	{
		for( int i = 0 ; i < 4 ; i++ )
		{
			prefix[i] = pText[i];
			if( pText[i] == 0 )
				return;
		}
	}
}

//____ setPrefix(2) ___________________________________________________________

void WgValueFormat::setPrefix( const char * pText )
{
	if( !pText )
		prefix[0] = 0;
	else
		WgTextTool::readString( pText, prefix, 4 );
}

//____ setSuffix(1) ___________________________________________________________

void WgValueFormat::setSuffix( const Uint16 * pText )
{
	if( !pText )
		suffix[0] = 0;
	else
	{
		for( int i = 0 ; i < 4 ; i++ )
		{
			suffix[i] = pText[i];
			if( pText[i] == 0 )
				return;
		}
	}
}

//____ setSuffix(2) ___________________________________________________________

void WgValueFormat::setSuffix( const char * pText )
{
	if( !pText )
		suffix[0] = 0;
	else
		WgTextTool::readString( pText, suffix, 4 );
}

