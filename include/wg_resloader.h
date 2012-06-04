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
#ifndef WG_RESLOADER_DOT_H
#define WG_RESLOADER_DOT_H

#include <string>
class WgSurface;
class WgGlyphSet;
class WgResDB;

class WgResLoader
{
public:
	virtual WgSurface*	LoadSurface( const std::string& filename, bool bRequired ) = 0;
	virtual WgGlyphSet*	LoadGlyphSet( const std::string& filename ) = 0;
	virtual WgResDB*	LoadDb( const std::string& filename, WgResDB * pDependencies = 0 ) = 0;
	virtual std::string	LoadString( const std::string& token ) = 0;
};


#endif //WG_RESLOADER_DOT_H
