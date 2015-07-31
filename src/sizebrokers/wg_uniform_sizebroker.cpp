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
#include <wg_uniform_sizebroker.h>

namespace wg 
{
	
	
	const char UniformSizeBroker::CLASSNAME[] = {"UniformSizeBroker"};
	
	//____ isInstanceOf() _________________________________________________________
	
	bool UniformSizeBroker::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return SizeBroker::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * UniformSizeBroker::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	UniformSizeBroker_p UniformSizeBroker::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return UniformSizeBroker_p( static_cast<UniformSizeBroker*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setItemLengths() _______________________________________________________
	
	int UniformSizeBroker::setItemLengths( SizeBrokerItem * pItems, int nItems, int totalLength ) const
	{
	    if( nItems == 0 )
	        return 0;
	    
	    // Gather some data we need
	        
	    int staticLength = 0;               // Total preferred length of all static (non-stretching) items
	    int nUnifiedItems = 0;                // Number of static items
	    float extraWeight = 0;                // Total "extra weight", of all items with weight > 1.
	        
	    for( int i = 0 ; i < nItems ; i++ )
	    {
	        if( pItems[i].weight == 0.f )
	            staticLength += pItems[i].preferred;
	        else if( pItems[i].weight <= 1.f )
	            nUnifiedItems++;
	        else
	            extraWeight += pItems[i].weight-1.f;
	    }
	        
	    // Calculate baseLength and extraLengthPerWeightUnit
	        
	    int unifiedLength = nUnifiedItems > 0 ? (totalLength - staticLength) / nUnifiedItems : 0;
	    int paddingLength = totalLength - staticLength - unifiedLength*nUnifiedItems;
	    float paddingPerWeightUnit = extraWeight > 0 ? paddingLength / extraWeight : 0.f;
	
	    // Loop through items and set their length
	        
	    float paddingAcc = 0.0001f;
	    int total = 0;
	    int length;
	        
	    for( int i = 0 ; i < nItems ; i++ )
	    {
	        if( pItems[i].weight == 0 )
	        {
	            length = pItems[i].preferred;
	        }
	        else if( pItems[i].weight > 1 )
	        {
	            paddingAcc += (pItems[i].weight-1) * paddingPerWeightUnit;
	            length = (int) paddingAcc;
	            paddingAcc -= (int) paddingAcc;
	        }
	        else
	            length = unifiedLength;
	        
	        pItems[i].output = length;
	        total += length;
	    }
	    return total;
	}
	
	//____ setPreferredLengths() __________________________________________________
	
	int UniformSizeBroker::setPreferredLengths( SizeBrokerItem * pItems, int nItems ) const
	{
	    if( nItems == 0 )
	        return 0;
	    
	    int unifiedLength = _findLongestUnified(pItems, nItems);
	    int total = 0;
	    int length;
	    
	    for( int i = 0 ; i < nItems ; i++ )
	    {
	        if( pItems[i].weight == 0.f )
	            length = pItems[i].preferred;
	        else if( pItems[i].weight > 1.f )
	            length = 0;
	        else
	            length = unifiedLength;
	        
	        pItems[i].output = length;
	        total += length;
	    }
	    
	    return total;
	}
	
	//____ mayAlterPreferredLengths() _____________________________________________
	
	bool UniformSizeBroker::mayAlterPreferredLengths() const
	{
	    return true;
	}
	
	//____ _findLongestUnified() __________________________________________________
	
	int UniformSizeBroker::_findLongestUnified( SizeBrokerItem * pItems, int nItems ) const
	{
	    int longest = 0;
	    for( int i = 0 ; i < nItems ; i++ )
	        if( pItems[i].preferred > longest && pItems[i].weight == 1.f )
	            longest = pItems[i].preferred;
	    return longest;
	}
	
	//____ _setOutputs() __________________________________________________________
	
	void UniformSizeBroker::_setOutputs( SizeBrokerItem * pItems, int nItems, int value ) const
	{
	    for( int i = 0 ; i < nItems ; i++ )
	        pItems[i].output = value;
	}
	

} // namespace wg
