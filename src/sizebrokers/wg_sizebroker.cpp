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

const char WgSizeBroker::CLASSNAME[] = {"SizeBroker"};

//____ Constructor ____________________________________________________________

WgSizeBroker::WgSizeBroker()
{
}

//____ Destructor _____________________________________________________________

WgSizeBroker::~WgSizeBroker()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgSizeBroker::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgSizeBroker::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgSizeBroker_p WgSizeBroker::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgSizeBroker_p( static_cast<WgSizeBroker*>(pObject.rawPtr()) );

	return 0;
}


/*
int WgSizeBroker::setItemLengths( WgSizeBrokerItem * pItems, int nItems, int _totalLength ) const
{
	int totalLength = 0;
	for( int i = 0 ; i < nItems ; i++ )
	{
		totalLength += pItems[i].preferred;
		pItems[i].output = pItems[i].preferred;
	}
	return totalLength;
}

int WgSizeBroker::setPreferredLengths( WgSizeBrokerItem * pItems, int nItems ) const
{
	int totalLength = 0;
	for( int i = 0 ; i < nItems ; i++ )
	{	
		totalLength += pItems[i].preferred;
		pItems[i].output = pItems[i].preferred;
	}
	return totalLength;
}

bool WgSizeBroker::mayAlterPreferredLengths() const
{
	return false;
}
*/

