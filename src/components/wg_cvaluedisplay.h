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

	class CValueDisplay : public CStaticValueDisplay		/** @private */
	{
	public:
		CValueDisplay(Holder * pHolder);
		~CValueDisplay() {}

		// ModValue methods

		void				clear();
		bool				set( int64_t value, int scale );

		bool				setRange( int64_t min, int64_t max );
		inline int64_t		min() const { return m_minValue; }
		inline int64_t		max() const { return m_maxValue; }


	protected:

		void				_clear();
		bool				_set(int64_t value, int scale);

		bool				_setRange(int64_t min, int64_t max);


		inline void			_onValueModified() { _notify(ComponentNotif::ValueModified, 0, nullptr); }

		int64_t				m_minValue;
		int64_t				m_maxValue;
	};



} // namespace wg
#endif //WG_CVALUEDISPLAY_DOT_H
