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

namespace wg
{

	const TypeInfo SizeBroker::TYPEINFO = { "SizeBroker", &Object::TYPEINFO };

	//____ constructor ____________________________________________________________

	SizeBroker::SizeBroker()
	{
	}

	//____ Destructor _____________________________________________________________

	SizeBroker::~SizeBroker()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SizeBroker::typeInfo(void) const
	{
		return TYPEINFO;
	}

	/*
	int SizeBroker::setItemLengths( SizeBrokerItem * pItems, int nItems, int _totalLength ) const
	{
		int totalLength = 0;
		for( int i = 0 ; i < nItems ; i++ )
		{
			totalLength += pItems[i].preferred;
			pItems[i].output = pItems[i].preferred;
		}
		return totalLength;
	}

	int SizeBroker::setPreferredLengths( SizeBrokerItem * pItems, int nItems ) const
	{
		int totalLength = 0;
		for( int i = 0 ; i < nItems ; i++ )
		{
			totalLength += pItems[i].preferred;
			pItems[i].output = pItems[i].preferred;
		}
		return totalLength;
	}

	bool SizeBroker::mayAlterPreferredLengths() const
	{
		return false;
	}
	*/


} // namespace wg
