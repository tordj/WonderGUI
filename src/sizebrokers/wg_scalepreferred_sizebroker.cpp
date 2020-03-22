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
#include <wg_scalepreferred_sizebroker.h>

namespace wg
{


	const TypeInfo ScalePreferredSizeBroker::TYPEINFO = { "ScalePreferredSizeBroker", &SizeBroker::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ScalePreferredSizeBroker::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setItemLengths() _______________________________________________________

	MU ScalePreferredSizeBroker::setItemLengths( SizeBrokerItem * pItems, int nItems, MU totalLength ) const
	{
		return setPreferredLengths( pItems, nItems );
	}

	//____ setPreferredLengths() __________________________________________________

	MU ScalePreferredSizeBroker::setPreferredLengths( SizeBrokerItem * pItems, int nItems ) const
	{
		MU total = 0;
		for( int i = 0 ; i < nItems ; i++ )
		{
			MU def = pItems[i].preferred * pItems[i].weight;
			pItems[i].output = def;
			total += def;
		}
		return total;
	}

	//____ MayLaterPreferredLengths() _____________________________________________

	bool  ScalePreferredSizeBroker::mayAlterPreferredLengths() const
	{
		return true;
	}

} // namespace wg
