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
#ifndef	WG_CVALUEDISPLAY_DOT_H
#define WG_CVALUEDISPLAY_DOT_H
#pragma once

#include <wg_cstaticvaluedisplay.h>

namespace wg
{


	//____ CValueDisplay ____________________________________________________________

	class CValueDisplay : public CStaticValueDisplay
	{
	public:
		CValueDisplay(Holder * pHolder);
		~CValueDisplay() {}


		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _____________________________________________

		void				clear();
		bool				set( double value );

		bool				setRange( double min, double max );
		inline double		min() const { return m_minValue; }
		inline double		max() const { return m_maxValue; }


	protected:

		bool				_set(double value) override;

		double				m_minValue;
		double				m_maxValue;
	};



} // namespace wg
#endif //WG_CVALUEDISPLAY_DOT_H
