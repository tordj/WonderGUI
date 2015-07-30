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

#include <wg_timeformatter.h>

const char WgTimeFormatter::CLASSNAME[] = {"TimeFormatter"};

//____ Constructors ____________________________________________________________

WgTimeFormatter::WgTimeFormatter( const WgCharSeq& format24 )
{
	m_format24 = format24;
}

WgTimeFormatter::WgTimeFormatter( const WgCharSeq& formatAM, const WgCharSeq& formatPM )
{ 
	m_formatAM = formatAM;
	m_formatPM = formatPM;
}


//____ isInstanceOf() _________________________________________________________

bool WgTimeFormatter::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgValueFormatter::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgTimeFormatter::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgTimeFormatter_p WgTimeFormatter::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgTimeFormatter_p( static_cast<WgTimeFormatter*>(pObject.rawPtr()) );

	return 0;
}

//____ format() _______________________________________________________________

WgString WgTimeFormatter::format( Sint64 value, int scale ) const
{
	Sint64 seconds = value / scale;
	
	// Get right format string

	const WgString * pFormat;
	
	if( !m_format24.isEmpty() )
		pFormat = &m_format24;
	else
	{
		seconds %= 3600*24;
		pFormat = seconds < 3600*12 ? &m_formatAM : &m_formatPM;
	}
	
	if( pFormat->isEmpty() )
		return WgString();
	
	//
	
	WgCharBuffer output(pFormat->length());
	
	const WgChar * pSrc = pFormat->chars();

	int unit=1;			// Default to seconds.
	
	while( !pSrc->isEndOfText() )
	{
		Uint16 glyph = pSrc->getGlyph();
		Uint16 decoration = pSrc->propHandle();
		
		if( glyph == '%' )
		{
			int chars = 0;
			pSrc++;
			if( pSrc->isEndOfText() )
				goto error;
			glyph = pSrc->getGlyph();
			while( glyph >= '0' && glyph <= '9' )
			{
				chars *= 10;
				chars += glyph - '0';
				pSrc++;
				if( pSrc->isEndOfText() )
					goto error;
				glyph = pSrc->getGlyph();
			}	

			int num;
			switch( glyph )
			{
				case 'D':
					unit = 3600*24;
					break; 
				case 'H':
					unit = 3600;
					break;
				case 'M':
					unit = 60;
					break;
				case 'S':
					unit = 1;
					break;
				case 'F':
					if( chars == 0 )
						chars = 1;							// We don't accept unlimited fractional numbers.

					unit *= scale;							// Shift unit up so we can add fractional parts to seconds
					seconds = seconds*scale + value%scale;	// Add fractional parts to seconds

					// Create the distance between unit and seconds, corresponding to number of chars.
					for(int i = 0 ; i < chars ; i++ )
						if( unit % 10 == 0 )				// Prefer scaling down unit if possible to avoid overflow in seconds.
							unit /= 10;
						else
							seconds *= 10;
					break;
				default:
					goto error;
			}
			pSrc++;

			num = seconds / unit;
			seconds %= unit;					// Seconds left for smaller units.

			int		bar = 1;

			if( chars == 0 )
			{
				while( num > bar*10 )
					bar *= 10;
			}
			else
			{
				for( int i = 1 ; i < chars ; i++ )
					bar *= 10;

				// Detect overflow.

				if( num >= bar*10)
					goto overflow;
			}


			while( bar > 0 )
			{
				output.pushBack( WgChar( '0' + num/bar, 0 ) );		//TODO: Add property
				
				num %= bar;
				bar /= 10;		
			}				
			
		}
		else
			output.pushBack( * pSrc++ );
		
	}
	
	return &output;
error:
	return WgString("#ERROR#");
overflow:
	return WgString("#OVERFLOW#");
	
}

WgString WgTimeFormatter::format( double value ) const
{
	return format( (Sint64)(value*1000000), 1000000);
}
