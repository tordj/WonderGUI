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

#include <wg_blockingcapsule.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo BlockingCapsule::TYPEINFO = { "BlockingCapsule", &Capsule::TYPEINFO };


	//____ create() _______________________________________________________________

	BlockingCapsule_p BlockingCapsule::create() 
	{ 
		return BlockingCapsule_p(new BlockingCapsule()); 
	}

	BlockingCapsule_p BlockingCapsule::create(const Blueprint& blueprint)
	{ 
		return BlockingCapsule_p(new BlockingCapsule(blueprint));
	}


	//____ constructor ____________________________________________________________

	BlockingCapsule::BlockingCapsule()
	{
	}

	//____ Destructor _____________________________________________________________

	BlockingCapsule::~BlockingCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BlockingCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _findWidget() _________________________________________________________

	Widget*  BlockingCapsule::_findWidget(const CoordSPX& ofs, SearchMode mode)
	{
		Widget * pFound = Capsule::_findWidget(ofs,mode);

		if( pFound == this || mode != SearchMode::ActionTarget)
			return pFound;

		bool bInList = false;			// Set if found widget or its ancestor is in list.
		for( auto& weakptr : blocked )
		{
			Widget * pTarget = weakptr.rawPtr();
			if( pTarget == nullptr )
				continue;
			
			Widget * pWidget = pFound;
			while( pWidget )
			{
				if( pWidget == pTarget )
				{
					bInList = true;
					break;
				}
				
				pWidget = pWidget->parent();
			}
		}

		bool bBlock = m_bInvertedBlock ? !bInList : bInList;
		
		if( bBlock )
			return this;
		else
			return pFound;
	}


} // namespace wg
