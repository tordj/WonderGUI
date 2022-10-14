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

#ifndef	WG_PRINTER_DOT_H
#define	WG_PRINTER_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_types.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>

namespace wg
{

	class Printer;
	typedef	StrongPtr<Printer>	Printer_p;
	typedef	WeakPtr<Printer>		Printer_wp;


	/**
	 * @brief Reference counted container of arbitrary data.
	 *
	 * Blob is a container that can be used to wrap a non-WonderGUI
	 * object or any set of data into a reference counted WonderGUI object.
	 *
	 * When the Blob is destroyed, the external object is destroyed or the memory area
	 * released.
	 */

	class Printer : public Object
	{
	public:
		//.____ Creation __________________________________________

		static Printer_p	create();

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control ________________________________________________

		void		setGfxDevice(GfxDevice * pDevice);
		void		setFont(Font * pFont);
		
		void		setOrigo( CoordSPX origo );
		void		setLineWidth( spx width );		// Needed for printJustifiedLine()
		
		void		resetCursor();
		void		setCursor( CoordSPX pos );
		void		crlf();
		void		crlf( spx maxFontSizeThisLine, spx maxFontSizeNextLine );
		
		void		print( const char * pText );
		CoordSPX	printAt( CoordSPX pos, const char * pText );
		void		printJustified( Placement xPlacement, const char * pText );
		void		printInBox( const RectSPX& box, Placement placement, const char * pText );

		spx			lineHeight();
		SizeSPX		textSize( const char * pString );


	protected:
		Printer();
	
		
		uint32_t 	_parseChar( const char *& pStr );
		CoordSPX	_print( CoordSPX pos, const char * pBegin, const char * pEnd, spx origoX );
		CoordSPX	_printJustified( CoordSPX pos, spx lineWidth, Placement xPlacement, const char * pBegin );

		const char * _lineEnd( const char * pPos );
		SizeSPX		_textSize( const char * pBegin, const char * pEnd );
		
		GfxDevice_p	m_pGfxDevice;
		Font_p		m_pFont;
		
		CoordSPX	m_cursorPos;
		CoordSPX	m_cursorOrigo;
		
		spx			m_lineWidth;
		
	};


} // namespace wg
#endif //WG_PRINTER_DOT_H
