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
#include <wg_charseq.h>
#include <wg_char.h>
#include <assert.h>


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

WgValueFormat::WgValueFormat( const WgCharSeq& format )
{
	integers		= 1;
	decimals		= 0;
	grouping		= 0;
	separator		= 0xA0 /*0xA0=NO_BREAK_SPACE*/;
	period			= 0x2e;
	prefix[0]		= 0;
	prefix[1]		= 0;
	prefix[2]		= 0;
	prefix[3]		= 0;
	suffix[0]		= 0;
	suffix[1]		= 0;
	suffix[2]		= 0;
	suffix[3]		= 0;
	bPlus			= false;
	bZeroIsNegative = false;
	bForcePeriod	= false;
	bSetTextProp	= false;
	bForceDecimals	= true;
	noDecimalThreshold	= 0;


	WgCharSeq::WgCharBasket basket = format.GetWgChars();

	const WgChar * pBeg = basket.ptr;
	const WgChar * pEnd = basket.ptr + basket.length;

	// Copy prefix

	const WgChar * p = pBeg;
	while( p->GetGlyph() != '1' && p - pBeg < 4 )
	{
		assert( p->GetGlyph() < '0' || p->GetGlyph() > '9' );		// No numerics allowed in prefix.
		p++;
	}
	
	for( int i = 0 ; i < p - pBeg ; i++ )
		prefix[i] = pBeg[i].GetGlyph();



	pBeg = p;

	//

	if( pBeg == pEnd )
		return;

	// Save textprops of the '1' character.

	pTextProperties = pBeg->GetProperties();
	pBeg++;

	//

	if( pBeg == pEnd )
		return;

	// Check for separator character

	bool	bHasSeparator = false;

	Uint16	glyph = pBeg->GetGlyph();
	if( glyph < '0' || glyph > '9' )
	{
		separator = glyph;
		pBeg++;
		bHasSeparator = true;
	}

	// 

	if( pBeg == pEnd )
		return;

	// Count zeroes for grouping

	int nZeroes = 0;

	while( pBeg->GetGlyph() == '0' && pBeg != pEnd )
	{
		pBeg++;
		nZeroes++;
	}

	assert( nZeroes > 1 && nZeroes <= 6 );
	if( bHasSeparator )
		grouping = nZeroes;

	// 

	if( pBeg == pEnd )
		return;

	// Check for optional decimal delimeter with 1-6 following zeroes

	if( pEnd - pBeg >= 2 )
	{
		Uint16 sep = pBeg->GetGlyph();
		Uint16 after = pBeg[1].GetGlyph();

		if( after == '0' )
		{
			period = sep;
			pBeg+=2;
			nZeroes = 1;
			while( pBeg != pEnd && pBeg->GetGlyph() == '0' )
			{
				pBeg++;
				nZeroes++;
			}
			decimals = nZeroes;
		}
	}

	// 

	if( pBeg == pEnd )
		return;

	// Copy suffix

	int n = pEnd - pBeg;
	if( n > 4 )
		n = 4;

	for( int i = 0 ; i < n ; i++ )
		suffix[i] = pBeg[i].GetGlyph();	
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

