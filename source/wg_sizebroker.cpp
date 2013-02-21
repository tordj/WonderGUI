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
#include <wg_sizebroker.h>




WgSizeBroker::WgSizeBroker()
{
}

WgSizeBroker::~WgSizeBroker()
{
}

int WgSizeBroker::SetItemLengths( ItemData * pItems, int nItems, int totalLength ) const
{
	int nUsedSpace = 0;
	for( int i = 0 ; i < nItems ; i++ )
	{
		nUsedSpace += pItems[i].preferred;
		pItems[i].length = pItems[i].preferred;
	}
	return nUsedSpace;
}

int WgSizeBroker::PreferredLength( const ItemData * pItems, int nItems ) const
{
	int length = 0;
	for( int i = 0 ; i < nItems ; i++ )
		length += pItems[i].preferred;

	return length;
}

