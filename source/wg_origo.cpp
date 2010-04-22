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


#include <wg_origo.h>


WgOrigo	WgOrigo::g_topLeft 		= WgOrigo::relative(0, 0);
WgOrigo	WgOrigo::g_topCenter 	= WgOrigo::relative(0.5, 0);
WgOrigo	WgOrigo::g_topRight 	= WgOrigo::relative(1.0, 0);

WgOrigo	WgOrigo::g_midLeft 		= WgOrigo::relative(0, 0.5);
WgOrigo	WgOrigo::g_midCenter	= WgOrigo::relative(0.5, 0.5);
WgOrigo	WgOrigo::g_midRight 	= WgOrigo::relative(1.0, 0.5);

WgOrigo	WgOrigo::g_bottomLeft = WgOrigo::relative(0, 1.0);
WgOrigo	WgOrigo::g_bottomCenter= WgOrigo::relative(0.5, 1.0);
WgOrigo	WgOrigo::g_bottomRight= WgOrigo::relative(1.0, 1.0);


//____ grid() _________________________________________________________________

WgOrigo WgOrigo::grid( int x, int y, int columns, int lines )
{
	WgOrigo o;
	o.xa = ((1+(x-1)*2)*(1.f/(columns*2))); 
	o.xhs = 0.5f;
	o.ya = ((1+(y-1)*2)*(1.f/(lines*2))); 
	o.yhs = 0.5f;
	return o;
}

//____ relative() _____________________________________________________________

WgOrigo WgOrigo::relative( float x, float y )
{
	LIMIT(x, 0.f, 1.f);
	LIMIT(y, 0.f, 1.f);

	WgOrigo o;
	o.xa = o.xhs = (x * 1.f);
	o.ya = o.yhs = (y * 1.f);
	return o;
}

//____ columns() ______________________________________________________________

WgOrigo WgOrigo::columns( int x, int columns, float y )
{
	LIMIT(y, 0.f, 1.f);

	WgOrigo o;
	o.xa = ((1+(x-1)*2)*(1.f/(columns*2))); 
	o.xhs = 0.5f;
	o.ya = o.yhs = (y * 1.f);
	return o;
}

//____ lines() ________________________________________________________________

WgOrigo WgOrigo::lines( int y, int lines, float x )
{
	LIMIT(x, 0.f, 1.f);

	WgOrigo o;
	o.xa = o.xhs = (x * 1.f);
	o.ya = ((1+(y-1)*2)*(1.f/(lines*2))); 
	o.yhs = 0.5f;
	return o;
}

//____ specific() _____________________________________________________________

WgOrigo WgOrigo::specific( float anchX, float anchY, float hsX, float hsY )
{
	LIMIT(anchX, 0.f, 1.f);
	LIMIT(anchY, 0.f, 1.f);
	LIMIT(hsX, 0.f, 1.f);
	LIMIT(hsY, 0.f, 1.f);

	WgOrigo o;
	o.xa = (anchX * 1.f);
	o.xhs = (hsX * 1.f);
	o.ya = (anchY * 1.f);
	o.yhs = (hsY * 1.f);
	return o;
}

//____ specificUnlimited() _____________________________________________________________
// this is sort of needed for child geo policy CLIPPED...

WgOrigo WgOrigo::specificUnlimited( float anchX, float anchY, float hsX, float hsY )
{
	WgOrigo o;
	o.xa = (anchX * 1.f);
	o.xhs = (hsX * 1.f);
	o.ya = (anchY * 1.f);
	o.yhs = (hsY * 1.f);
	return o;
}

//____ cornerTopLeft() ________________________________________________________

WgOrigo WgOrigo::cornerTopLeft( float x, float y )
{
	LIMIT(x, 0.f, 1.f);
	LIMIT(y, 0.f, 1.f);

	WgOrigo o;
	o.xa = (x * 1.f);
	o.xhs = 0.f;
	o.ya = (y * 1.f);
	o.yhs = 0.f;
	return o;
}

//____ cornerBottomRight() ____________________________________________________

WgOrigo WgOrigo::cornerBottomRight( float x, float y )
{
	LIMIT(x, 0.f, 1.f);
	LIMIT(y, 0.f, 1.f);

	WgOrigo o;
	o.xa = (x * 1.f);
	o.xhs = 1.f;
	o.ya = (y * 1.f);
	o.yhs = 1.f;
	return o;
}
