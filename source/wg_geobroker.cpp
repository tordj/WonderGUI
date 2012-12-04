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
#include <wg_geobroker.h>



int default_broker(WgGeoBroker::ItemData* pItems, int nItems, int totalSpace)
{
	int nUsedSpace = 0;
	for( int i = 0 ; i < nItems ; i++ )
	{
		pItems[i].ofs_out = nUsedSpace;
		nUsedSpace += pItems[i].def;
		pItems[i].size_out = pItems[i].def;
	}
	return nUsedSpace;
}




WgGeoBroker::WgGeoBroker() : m_function(default_broker) {}
WgGeoBroker::WgGeoBroker( int(*fp)(ItemData*,int,int) ) : m_function(fp) {}


const WgGeoBroker	WgGeoBroker::none = WgGeoBroker(default_broker);








