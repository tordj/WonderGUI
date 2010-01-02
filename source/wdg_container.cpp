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

#include	<wdg_container.h>

static const char	Wdg_Type[] = {"TordJ/Container"};

//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_Container::NewOfMyType() const
{
	return new Wdg_Container;
}


//____ Init() _________________________________________________________________

void Wdg_Container::Init( void )
{
}

//____ Destructor _____________________________________________________________

Wdg_Container::~Wdg_Container( void )
{
}


//____ Type() _________________________________________________________________

const char * Wdg_Container::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_Container::GetMyType( void )
{
	return Wdg_Type;
}


//____ DoMyOwnMarkTest() ______________________________________________________

bool Wdg_Container::DoMyOwnMarkTest( int _x, int _y )
{
	return false;		// We never have opaque pixels.
}
