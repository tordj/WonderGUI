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
#ifndef WG_SLIDESPAN_DOT_H
#define WG_SLIDESPAN_DOT_H
#pragma once

#include <wg_span.h>

namespace wg 
{
	
	
	class SlideSpan;
	typedef	StrongInterfacePtr<SlideSpan>		SlideSpan_p;
	typedef	WeakInterfacePtr<SlideSpan>	SlideSpan_wp;
	
	
	class SlideSpan : public Span
	{
	public:
		//.____ Creation __________________________________________

		SlideSpan(SpanItem* pItem) : Span(pItem) {}
		
		//.____ Control ___________________________________________	

		inline void	setBegin( int begin ) { m_pItem->setBegin(begin); }
		inline void	setRelativePos( float pos ) { m_pItem->setRelativePos(pos); }
		inline void	setRelativeBegin( float begin ) { m_pItem->setRelativeBegin(begin); }
	
		inline bool	stepForward() { return m_pItem->stepForward(); }
		inline bool	stepBackward() { return m_pItem->stepBackward(); }
		inline bool	skipForward() { return m_pItem->skipForward(); }
		inline bool	skipBackward() { return m_pItem->skipBackward(); }

		//.____ Misc __________________________________________________

		inline SlideSpan_p		ptr() { return SlideSpan_p(this); }

	};
	
	

} // namespace wg
#endif //WG_SLIDESPAN_DOT_H
