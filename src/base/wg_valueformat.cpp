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

#include <cmath>

#include <wg_texttool.h>
#include <wg_valueformat.h>
#include <wg_charseq.h>
#include <wg_char.h>
#include <wg_string.h>
#include <assert.h>

const char WgValueFormat::CLASSNAME[] = {"ValueFormat"};





//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat()
{
	integers		= 1;
	decimals		= 0;
	grouping		= 0;
	separator		= 0xA0 /*0xA0=NO_BREAK_SPACE*/;
	period			= 0x2e;
	bPlus			= false;
	bZeroIsNegative = false;
	bForcePeriod	= false;
	bSetTextprop	= false;
	bForceDecimals	= true;
	noDecimalThreshold	= 0;
	scale			= 1;
}

//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat( const WgCharSeq& format )
{
	setFormat( format );
}

//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat( const WgValueFormat_p& pIn )
{
	setFormat(pIn);
}

//____ WgValueFormat() ________________________________________________________

WgValueFormat::WgValueFormat( int nInt, int nDec, int grouping, bool bPlus,
								Uint16 _separator, Uint16 period, bool bForcePeriod,
								const char * pPrefix, const char * pSuffix )
{
	separator		= _separator /*0xA0=NO_BREAK_SPACE*/;
	period			= 0x2e;

	bSetTextprop	= false;
	bForceDecimals	= true;
	bZeroIsNegative = false;
	noDecimalThreshold	= 0;

	setFormat( nInt, nDec, grouping, bPlus, separator, period, bForcePeriod );
	setPrefix( pPrefix );
	setSuffix( pSuffix );

}

//____ isInstanceOf() _________________________________________________________

bool WgValueFormat::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgValueFormat::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgValueFormat_p WgValueFormat::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgValueFormat_p( static_cast<WgValueFormat*>(pObject.rawPtr()) );

	return 0;
}


//____ setFormat() ____________________________________________________________

void WgValueFormat::setFormat( const WgCharSeq& format )
{
	integers		= 1;
	decimals		= 0;
	grouping		= 0;
	separator		= 0xA0 /*0xA0=NO_BREAK_SPACE*/;
	period			= 0x2e;
	bPlus			= false;
	bZeroIsNegative = false;
	bForcePeriod	= false;
	bSetTextprop	= false;
	bForceDecimals	= true;
	noDecimalThreshold	= 0;
	scale			= 1;

	WgCharSeq::WgCharBasket basket = format.getWgChars();

	const WgChar * pBeg = basket.ptr;
	const WgChar * pEnd = basket.ptr + basket.length;

	// Copy prefix

	const WgChar * p = pBeg;
	while(  p < pEnd && p->getGlyph() != '1' )
	{
		assert( p->getGlyph() < '0' || p->getGlyph() > '9' );		// No numerics allowed in prefix.
		p++;
	}

	prefix = WgCharSeq( pBeg, p - pBeg );

	pBeg = p;

	//

	if( pBeg == pEnd )
		return;

	// Save textprops of the '1' character.

	pTextProperties = pBeg->getProperties();
	pBeg++;

	//

	if( pBeg == pEnd )
		return;

	// Check for separator character

	bool	bHasSeparator = false;

	if( pEnd - pBeg >= 2 )
	{
		Uint16 sep = pBeg->getGlyph();
		Uint16 after = pBeg[1].getGlyph();

		if( (sep < '0' || sep > '9') && after == '0' )
		{
			separator = sep;
			pBeg++;
			bHasSeparator = true;
		}
	}

	// 

	if( pBeg == pEnd )
		return;

	// Count zeroes for grouping

	int nZeroes = 0;

	while( pBeg != pEnd && pBeg->getGlyph() == '0' )
	{
		pBeg++;
		nZeroes++;
	}

	if( bHasSeparator && nZeroes > 0 )
		grouping = nZeroes;

	// 

	if( pBeg == pEnd )
		return;

	// Check for optional decimal delimeter with 1-6 following zeroes or x:es

	if( pEnd - pBeg >= 2 )
	{
		Uint16 sep = pBeg->getGlyph();
		Uint16 after = pBeg[1].getGlyph();

		if( after == '0' || after == 'x' || after == 'X' )
		{
			period = sep;
			pBeg+=1;
			nZeroes = 0;
			while( pBeg != pEnd && pBeg->getGlyph() == '0' )
			{
				pBeg++;
				nZeroes++;
			}

			int nXes = 0;
			while( pBeg != pEnd && (pBeg->getGlyph() == 'x' || pBeg->getGlyph() == 'X') )
			{
				pBeg++;
				nXes++;
			}

			decimals = nZeroes;
			scale = (int) pow(10.0,nZeroes+nXes);
		}
	}

	// 

	if( pBeg == pEnd )
		return;

	// Copy suffix

	suffix = WgCharSeq( pBeg, pEnd - pBeg );
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

	scale			= 1;
}

//____ WgValueFormat() ________________________________________________________

void WgValueFormat::setFormat( const WgValueFormat_p& pIn )
{
	integers	= pIn->integers;
	decimals	= pIn->decimals;
	grouping	= pIn->grouping;
	separator	= pIn->separator;
	period		= pIn->period;

	prefix		= pIn->prefix;
	suffix		= pIn->suffix;

	bPlus			= pIn->bPlus;
	bZeroIsNegative = pIn->bZeroIsNegative;
	bForcePeriod	= pIn->bForcePeriod;
	pTextProperties	= pIn->pTextProperties;
	bSetTextprop	= pIn->bSetTextprop;
	bForceDecimals	= pIn->bForceDecimals;
	noDecimalThreshold = pIn->noDecimalThreshold;
	scale			= pIn->scale;
}


//____ setPrefix(1) ___________________________________________________________

void WgValueFormat::setPrefix( const WgString& str )
{
	prefix = str;
}

//____ setPrefix(2) ___________________________________________________________

void WgValueFormat::setPrefix( const WgCharSeq& seq )
{
	prefix = seq;
}

//____ setSuffix(1) ___________________________________________________________

void WgValueFormat::setSuffix( const WgString& str )
{
	suffix = str;
}

//____ setSuffix(2) ___________________________________________________________

void WgValueFormat::setSuffix( const WgCharSeq& seq )
{
	suffix = seq;
}


/*
//____ Constructor ____________________________________________________________

WgValueFormatter::WgValueFormatter()
{
}

WgValueFormatter::WgValueFormatter( const WgCharSeq& format ) : m_format(format)
{
}

//____ Destructor _____________________________________________________________

WgValueFormatter::~WgValueFormatter()
{
}

//____ setFormat() ____________________________________________________________

void WgValueFormatter::setFormat( const WgCharSeq& format )
{
	m_format.setFormat(format);
}

//____ prefix() _______________________________________________________________

WgString WgValueFormatter::prefix() const
{
	return m_format.prefix;
}

//____ suffix() _______________________________________________________________

WgString WgValueFormatter::suffix() const
{
	return m_format.suffix;
}

//____ format() _______________________________________________________________

WgString WgValueFormatter::format( Sint64 value ) const
{
	WgCharBuffer	buff;
	buff.setUnusedCapacity( 32,32 );
	_formatValue( &buff, value );
	buff.pushFront( m_format.prefix );
	buff.pushBack( m_format.suffix );

	if( value < 0 )
		buff.pushFront( WgChar('-') );
	else if( value >= 0 && m_format.bPlus )
		buff.pushFront( WgChar('+') );

	return WgString(&buff);
}

//____ formatNoPreSuffix() ____________________________________________________

WgString WgValueFormatter::formatNoPreSuffix( Sint64 value ) const
{
	WgCharBuffer	buff;
	buff.setUnusedCapacity( 32,32 );
	_formatValue( &buff, value );

	if( value < 0 )
		buff.pushFront( WgChar('-') );
	else if( value >= 0 && m_format.bPlus )
		buff.pushFront( WgChar('+') );

	return WgString(&buff);
}

//____ _formatValue() _________________________________________________________

void WgValueFormatter::_formatValue( WgCharBuffer * pBuffer, Sint64 value ) const
{
	const WgValueFormat& f = m_format;

	Sint64 absVal = value >= 0 ? value : -value;

	Sint64 intPart = absVal / m_format.scale;
	Sint64 decPart = absVal % m_format.scale;

	// Write period and decimal part

	if( f.decimals || f.bForcePeriod )
	{
		if( 0 == f.noDecimalThreshold || (int)absVal < f.noDecimalThreshold )
		{
			if( f.bForceDecimals || decPart != 0 )
			{
				pBuffer->pushBack( WgChar(f.period) );

				for( int i = f.decimals; i > 0 ; i-- )
				{
					decPart *= 10;
					pBuffer->pushBack( WgChar( (Uint16)(decPart/m_format.scale) + 0x30 ));
					decPart = decPart % m_format.scale;
				}
			}
		}
	}

	// Write integer part to temp area (backwards)

	Uint16	temp2[32];

	int			n = 0;

	while( intPart > 0 )
	{
		temp2[n++] = (Uint16) ((intPart % 10) + 0x30);
		intPart /= 10;
	}

	// Copy integer part acknowledge grouping

	if( f.grouping == 0 )
	{
		for( int i = 0 ; i < n ; i++ )
			pBuffer->pushFront( WgChar(temp2[i]) );
	}
	else
	{
		for( int i = 0 ; i < n ; i++ )
		{
			if( i != 0 && (i % f.grouping) == 0 )
				pBuffer->pushFront( WgChar(f.separator) );

			pBuffer->pushFront( WgChar(temp2[i]) );
		}
	}

	// Possibly fill out with zeroes, acknowledge grouping

	if( n < f.integers )
	{
		for( int i = n ; i < f.integers ; i++ )
		{
			if( i != 0 && (i % f.grouping) == 0 )
				pBuffer->pushFront( WgChar(f.separator) );

			pBuffer->pushFront( WgChar(0x30) );
		}
	}
}

*/
