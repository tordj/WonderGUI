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

#include <wg_c_printer.h>
#include <wg_printer.h>

using namespace wg;

inline Printer* getPtr(wg_obj obj) {
	return static_cast<Printer*>(reinterpret_cast<Object*>(obj));
}
wg_obj wg_createPrinter()
{
  auto pPrinter = Printer::create();
  pPrinter->retain();
  return static_cast<Object*>(pPrinter);
}


void wg_setPrinterGfxDevice( wg_obj printer, wg_obj gfxDevice )
{
	auto pDevice = static_cast<GfxDevice*>(reinterpret_cast<Object*>(gfxDevice));
	getPtr(printer)->setGfxDevice(pDevice);
}

void wg_setPrinterFont( wg_obj printer, wg_obj font )
{
	auto pFont = static_cast<Font*>(reinterpret_cast<Object*>(font));
	getPtr(printer)->setFont(pFont);

}

void wg_setPrinterOrigo( wg_obj printer, wg_coordSPX coord )
{
	getPtr(printer)->setOrigo({coord.x, coord.y});
}

void wg_setPrinterLineWidth( wg_obj printer, wg_spx width )
{
	getPtr(printer)->setLineWidth(width);
}

void wg_resetPrinterCursor( wg_obj printer )
{
	getPtr(printer)->resetCursor();
}

void wg_setPrinterCursor( wg_obj printer, wg_coordSPX pos )
{
	getPtr(printer)->setCursor({pos.x,pos.y});
}

void wg_printerCRLF( wg_obj printer )
{
	getPtr(printer)->crlf();
}

void wg_printerCRFLWithFontSizes( wg_obj printer, wg_spx maxFontSizeThisLine, wg_spx maxFontSizeNextLine )
{
	getPtr(printer)->crlf(maxFontSizeThisLine, maxFontSizeNextLine);
}

void wg_print( wg_obj printer, const char * pText )
{
	getPtr(printer)->print(pText);
}

wg_coordSPX wg_printAt( wg_obj printer, wg_coordSPX pos, const char * pText )
{
	getPtr(printer)->printAt({pos.x,pos.y},pText);
}

void wg_printJustified( wg_obj printer, wg_placement xPlacement, const char * pText )
{
	getPtr(printer)->printJustified( Placement(xPlacement),pText);
}

void wg_printInBox( wg_obj printer, const wg_rectSPX box, wg_placement placement, const char * pText )
{
	getPtr(printer)->printInBox( {box.x,box.y,box.w,box.h}, Placement(placement),pText);
}

wg_spx wg_printerLineHeight( wg_obj printer )
{
	return getPtr(printer)->lineHeight();
}

wg_sizeSPX wg_printerTextSize( wg_obj printer, const char * pText )
{
	auto size = getPtr(printer)->textSize(pText);
	return {size.w,size.h};
}
