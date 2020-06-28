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
#ifndef WG_STATESKIN_DOT_H
#define WG_STATESKIN_DOT_H
#pragma once

#include <wg_skin.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class StateSkin;
	typedef	StrongPtr<StateSkin>	StateSkin_p;
	typedef	WeakPtr<StateSkin>		StateSkin_wp;

	class StateSkin : public Skin
	{
	public:
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Border			contentPadding(State state) const override;
		Coord			contentOfs(State state) const override;
		Rect			contentRect(const Rect& canvas, State state) const override;

		//.____ Behavior _______________________________________________________

		void			clearContentShift();
		void			setContentShift(State state, CoordI shift);
		void			setContentShift(std::initializer_list< std::pair<State, CoordI> > StateShifts);
		CoordI			contentShift(State state) const;

		//.____ Misc ____________________________________________________

		virtual	bool	isStateIdentical( State state, State comparedTo, float fraction = 1.f, float fraction2 = -1.f) const override;

	protected:
		void _refreshUnsetStates();

        CoordI				m_contentShift[StateEnum_Nb];       // Unit: Points
		Bitmask<uint32_t>	m_contentShiftStateMask = 1;		// Bitfield with one bit set for each stateIndex that has been explicitly set.
	};


} // namespace wg
#endif //WG_STATESKIN_DOT_H
