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

//____ IsInstanceOf() _________________________________________________________

bool WgScalePreferredSizeBroker::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSizeBroker::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgScalePreferredSizeBroker::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgScalePreferredSizeBrokerPtr WgScalePreferredSizeBroker::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgScalePreferredSizeBrokerPtr( static_cast<WgScalePreferredSizeBroker*>(pObject.RawPtr()) );

	return 0;
}

//____ SetItemLengths() _______________________________________________________

int WgScalePreferredSizeBroker::SetItemLengths( WgSizeBrokerItem * pItems, int nItems, int totalLength ) const
{
    return SetPreferredLengths( pItems, nItems );
}

//____ SetPreferredLengths() __________________________________________________

int WgScalePreferredSizeBroker::SetPreferredLengths( WgSizeBrokerItem * pItems, int nItems ) const
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

bool  WgScalePreferredSizeBroker::MayAlterPreferredLengths() const
{
    return true;
}
