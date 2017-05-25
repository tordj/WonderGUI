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
#ifndef WG_MODSPAN_DOT_H
#define WG_MODSPAN_DOT_H
#pragma once

#include <wg_slidespan.h>

namespace wg 
{
	
	
	class ModSpan;
	typedef	StrongInterfacePtr<ModSpan>		ModSpan_p;
	typedef	WeakInterfacePtr<ModSpan>	ModSpan_wp;
	
	
	class ModSpan : public SlideSpan
	{
	public:
		//.____ Creation __________________________________________

		ModSpan(SpanItem* pItem) : SlideSpan(pItem) {}
	
		//.____ Control ___________________________________________	
	
		inline bool	setMin( int min ) { return m_pItem->setMin(min); }
		inline bool	setMax( int max ) { return m_pItem->setMax(max); }
		inline bool	setRange( int min, int max ) { return m_pItem->setRange(min,max); }
	
		inline void	setSpan( int begin, int length ) { m_pItem->setSpan(begin,length); }
		inline void	setLength( int length ) { m_pItem->setLength(length); }
	
		inline void	setRelativeSpan( float begin, float length ) { m_pItem->setRelativeSpan(begin,length); }
		inline void	setRelativeLength( float length ) { m_pItem->setRelativeLength(length); }

		//.____ Misc __________________________________________________

		inline ModSpan_p		ptr() { return ModSpan_p(_object(), this); }

	};
	
	

} // namespace wg
#endif //WG_MODSPAN_DOT_H
