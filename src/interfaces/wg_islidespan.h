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
#ifndef WG_ISLIDESPAN_DOT_H
#define WG_ISLIDESPAN_DOT_H
#pragma once

#include <wg_ispan.h>

namespace wg
{


	class ISlideSpan;
	typedef	StrongInterfacePtr<ISlideSpan>	ISlideSpan_p;
	typedef	WeakInterfacePtr<ISlideSpan>	ISlideSpan_wp;


	class ISlideSpan : public ISpan
	{
	public:
		/** @private */

		ISlideSpan(CSpan* pComponent) : ISpan(pComponent) {}

		//.____ Control ___________________________________________

		inline void	setBegin( int begin ) { m_pComponent->setBegin(begin); }
		inline void	setRelativePos( float pos ) { m_pComponent->setRelativePos(pos); }
		inline void	setRelativeBegin( float begin ) { m_pComponent->setRelativeBegin(begin); }

		inline bool	stepForward() { return m_pComponent->stepForward(); }
		inline bool	stepBackward() { return m_pComponent->stepBackward(); }
		inline bool	skipForward() { return m_pComponent->skipForward(); }
		inline bool	skipBackward() { return m_pComponent->skipBackward(); }

		//.____ Misc __________________________________________________

		inline ISlideSpan_p		ptr() { return ISlideSpan_p(this); }

	};



} // namespace wg
#endif //WG_ISLIDESPAN_DOT_H
