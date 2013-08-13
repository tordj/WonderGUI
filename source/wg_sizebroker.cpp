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

//____ IsInstanceOf() _________________________________________________________

bool WgSizeBroker::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgSizeBroker::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgSizeBrokerPtr WgSizeBroker::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgSizeBrokerPtr( static_cast<WgSizeBroker*>(pObject.GetRealPtr()) );

	return 0;
}


/*
int WgSizeBroker::SetItemLengths( WgSizeBrokerItem * pItems, int nItems, int _totalLength ) const
{
	int totalLength = 0;
	for( int i = 0 ; i < nItems ; i++ )
	{
		totalLength += pItems[i].preferred;
		pItems[i].output = pItems[i].preferred;
	}
	return totalLength;
}

int WgSizeBroker::SetPreferredLengths( WgSizeBrokerItem * pItems, int nItems ) const
{
	int totalLength = 0;
	for( int i = 0 ; i < nItems ; i++ )
	{	
		totalLength += pItems[i].preferred;
		pItems[i].output = pItems[i].preferred;
	}
	return totalLength;
}

bool WgSizeBroker::MayAlterPreferredLengths() const
{
	return false;
}
*/

