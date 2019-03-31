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
#ifndef WG_IMODSPAN_DOT_H
#define WG_IMODSPAN_DOT_H
#pragma once

#include <wg_islidespan.h>

namespace wg 
{
	
	
	class IModSpan;
	typedef	StrongInterfacePtr<IModSpan>		IModSpan_p;
	typedef	WeakInterfacePtr<IModSpan>			IModSpan_wp;
	
	
	class IModSpan : public ISlideSpan
	{
	public:
		/** @private */

		IModSpan(CSpan* pComponent) : ISlideSpan(pComponent) {}
	
		//.____ Control ___________________________________________	
	
		inline bool	setMin( int min ) { return m_pComponent->setMin(min); }
		inline bool	setMax( int max ) { return m_pComponent->setMax(max); }
		inline bool	setRange( int min, int max ) { return m_pComponent->setRange(min,max); }
	
		inline void	setSpan( int begin, int length ) { m_pComponent->setSpan(begin,length); }
		inline void	setLength( int length ) { m_pComponent->setLength(length); }
	
		inline void	setRelativeSpan( float begin, float length ) { m_pComponent->setRelativeSpan(begin,length); }
		inline void	setRelativeLength( float length ) { m_pComponent->setRelativeLength(length); }

		//.____ Misc __________________________________________________

		inline IModSpan_p		ptr() { return IModSpan_p(this); }

	};
	
	

} // namespace wg
#endif //WG_IMODSPAN_DOT_H
