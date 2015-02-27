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


//____ IsInstanceOf() _________________________________________________________

bool WgTimeFormatter::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgValueFormatter::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgTimeFormatter::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgTimeFormatterPtr WgTimeFormatter::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTimeFormatterPtr( static_cast<WgTimeFormatter*>(pObject.RawPtr()) );

	return 0;
}

//____ Format() _______________________________________________________________

WgString WgTimeFormatter::Format( Sint64 value, int scale ) const
{
	Sint64 seconds = value / scale;
	
	// Get right format string

	const WgString * pFormat;
	
	if( !m_format24.IsEmpty() )
		pFormat = &m_format24;
	else
	{
		seconds %= 3600*24;
		pFormat = seconds < 3600*12 ? &m_formatAM : &m_formatPM;
	}
	
	if( pFormat->IsEmpty() )
		return WgString();
	
	//
	
	WgCharBuffer output(pFormat->Length());
	
	const WgChar * pSrc = pFormat->Chars();

	int unit=1;			// Default to seconds.
	
	while( !pSrc->IsEndOfText() )
	{
		Uint16 glyph = pSrc->Glyph();
		Uint16 decoration = pSrc->PropHandle();
		
		if( glyph == '%' )
		{
			int chars = 0;
			pSrc++;
			if( pSrc->IsEndOfText() )
				goto error;
			glyph = pSrc->Glyph();
			while( glyph >= '0' && glyph <= '9' )
			{
				chars *= 10;
				chars += glyph - '0';
				pSrc++;
				if( pSrc->IsEndOfText() )
					goto error;
				glyph = pSrc->Glyph();
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
				output.PushBack( WgChar( '0' + num/bar, 0 ) );		//TODO: Add property
				
				num %= bar;
				bar /= 10;		
			}				
			
		}
		else
			output.PushBack( * pSrc++ );
		
	}
	
	return &output;
error:
	return WgString("#ERROR#");
overflow:
	return WgString("#OVERFLOW#");
	
}

WgString WgTimeFormatter::Format( double value ) const
{
	return Format( (Sint64)(value*1000000), 1000000);
}
