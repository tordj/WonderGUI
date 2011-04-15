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

#include <wg_color.h>

WgColor	WgColor::g_white(0xFFFFFFFF);
WgColor	WgColor::g_black(0xFF000000);
WgColor	WgColor::g_none(0x00FFFFFF);


//____ blend() ________________________________________________________________

WgColor blend( const WgColor& start, const WgColor& dest, float grade )
{
	WgColor col;

	col.r = start.r + (Uint8) (((int)dest.r) - ((int)start.r)*grade);
	col.g = start.g + (Uint8) (((int)dest.g) - ((int)start.g)*grade);
	col.b = start.b + (Uint8) (((int)dest.b) - ((int)start.b)*grade);
	col.a = start.a + (Uint8) (((int)dest.a) - ((int)start.a)*grade);

	return col;
}

