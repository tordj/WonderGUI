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

#ifndef WG_IVALUEDISPLAY_DOT_H
#define WG_IVALUEDISPLAY_DOT_H
#pragma once

#include <wg_irovaluedisplay.h>
#include <wg_cvaluedisplay.h>

namespace wg
{

	class CharSeq;
	class String;
	class CharBuffer;

	class IValueDisplay;
	typedef	StrongInterfacePtr<IValueDisplay>	IValueDisplay_p;
	typedef	WeakInterfacePtr<IValueDisplay>		IValueDisplay_wp;

	/**
	 * @brief Interface for modifying a ValueDisplay component
	 *
	 * This interface provides access to modify the value and appearance of
	 * a ValueDisplay component.
	 *
	*/

	class IValueDisplay : public IROValueDisplay
	{
	public:
		/** @private */

		IValueDisplay(CValueDisplay * pComponent) : IROValueDisplay(pComponent) {}

		//.____ Content _____________________________________________

		void					clear();
		bool					set( int64_t value, int scale = 1);


	/*	For the future...
		inline void				set( float value );
		inline void				set( double value );
	*/

		inline int64_t			value() const { return _comp()->value(); }
		inline int				scale() const { return _comp()->scale(); }

		bool					setRange( int64_t min, int64_t max );
		inline int64_t			min() const { return _comp()->min(); }
		inline int64_t			max() const { return _comp()->max(); }

		//.____ Misc __________________________________________________

		inline IValueDisplay_p		ptr() { return IValueDisplay_p(this); }


	private:
		inline	CValueDisplay * 		_comp() { return static_cast<CValueDisplay*>(m_pComponent); }
		inline	const CValueDisplay * _comp() const { return static_cast<CValueDisplay*>(m_pComponent); }
	};



} // namespace wg
#endif //WG_IVALUEDISPLAY_DOT_H
