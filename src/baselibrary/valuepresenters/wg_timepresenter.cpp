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

#include <wg_timepresenter.h>

namespace wg
{

	const TypeInfo TimePresenter::TYPEINFO = { "TimePresenter", &ValuePresenter::TYPEINFO };

	//____ constructors ____________________________________________________________

	TimePresenter::TimePresenter( const CharSeq& format24 )
	{
		m_format24 = format24;
	}

	TimePresenter::TimePresenter( const CharSeq& formatAM, const CharSeq& formatPM )
	{
		m_formatAM = formatAM;
		m_formatPM = formatPM;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TimePresenter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ format() _______________________________________________________________

	String TimePresenter::format( int64_t value, int scale ) const
	{
		int64_t seconds = value / scale;

		// Get right format string

		const String * pFormat;

		if( !m_format24.isEmpty() )
			pFormat = &m_format24;
		else
		{
			seconds %= 3600*24;
			pFormat = seconds < 3600*12 ? &m_formatAM : &m_formatPM;
		}

		if( pFormat->isEmpty() )
			return String();

		//

		CharBuffer output(pFormat->length());

		const Char * pSrc = pFormat->chars();

		int unit=1;			// Default to seconds.

		while( !pSrc->isEndOfText() )
		{
			uint16_t glyph = pSrc->code();
//			uint16_t style = pSrc->styleHandle();

			if( glyph == '%' )
			{
				int chars = 0;
				pSrc++;
				if( pSrc->isEndOfText() )
					goto error;
				glyph = pSrc->code();
				while( glyph >= '0' && glyph <= '9' )
				{
					chars *= 10;
					chars += glyph - '0';
					pSrc++;
					if( pSrc->isEndOfText() )
						goto error;
					glyph = pSrc->code();
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

				num = (int) (seconds / unit);
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
					output.pushBack( Char( '0' + num/bar, 0 ) );		//TODO: Add property

					num %= bar;
					bar /= 10;
				}

			}
			else
				output.pushBack( * pSrc++ );

		}

		return &output;
	error:
		return String("#ERROR#");
	overflow:
		return String("#OVERFLOW#");

	}

	String TimePresenter::format( double value ) const
	{
		return format( (int64_t)(value*1000000), 1000000);
	}

} // namespace wg
