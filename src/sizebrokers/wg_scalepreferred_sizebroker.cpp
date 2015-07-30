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


const char WgScalePreferredSizeBroker::CLASSNAME[] = {"ScalePreferredSizeBroker"};

//____ isInstanceOf() _________________________________________________________

bool WgScalePreferredSizeBroker::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSizeBroker::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgScalePreferredSizeBroker::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgScalePreferredSizeBroker_p WgScalePreferredSizeBroker::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgScalePreferredSizeBroker_p( static_cast<WgScalePreferredSizeBroker*>(pObject.rawPtr()) );

	return 0;
}

//____ setItemLengths() _______________________________________________________

int WgScalePreferredSizeBroker::setItemLengths( WgSizeBrokerItem * pItems, int nItems, int totalLength ) const
{
    return setPreferredLengths( pItems, nItems );
}

//____ setPreferredLengths() __________________________________________________

int WgScalePreferredSizeBroker::setPreferredLengths( WgSizeBrokerItem * pItems, int nItems ) const
{
    int total = 0;
    for( int i = 0 ; i < nItems ; i++ )
    {
        int def = (int) (pItems[i].preferred * pItems[i].weight);
        pItems[i].output = def;
        total += def;
    }
    return total;
}

//____ MayLaterPreferredLengths() _____________________________________________

bool  WgScalePreferredSizeBroker::mayAlterPreferredLengths() const
{
    return true;
}
