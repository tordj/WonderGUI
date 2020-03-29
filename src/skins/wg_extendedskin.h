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
#ifndef WG_EXTENDEDSKIN_DOT_H
#define WG_EXTENDEDSKIN_DOT_H
#pragma once

#include <wg_skin.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class ExtendedSkin;
	typedef	StrongPtr<ExtendedSkin>	ExtendedSkin_p;

	class ExtendedSkin : public Skin
	{
	public:
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________


		void			setContentPadding( BorderI padding );
		inline BorderI	contentPadding() const { return m_contentPadding; }
		Border			contentPaddingAdapted() const;

		//.____ Behavior _______________________________________________________

		void			clearContentShift();
		void			setContentShift(State state, CoordI shift);
		void			setContentShift(std::initializer_list< std::pair<State, CoordI> > StateShifts);
		CoordI			contentShift(State state) const;
		Coord			contentShiftAdapted(State state) const;

		//.____ Misc ____________________________________________________

		virtual	bool	isStateIdentical( State state, State comparedTo ) const override;

		virtual SizeI	_minSize() const override;
		virtual SizeI	_preferredSize() const override;
		virtual SizeI	_sizeForContent(const SizeI contentSize) const override;
		virtual BorderI	_contentPadding() const override;
		virtual SizeI	_contentPaddingSize() const override;
		virtual CoordI	_contentOfs(State state) const override;
		virtual RectI	_contentRect(const RectI& canvas, State state) const override;


	protected:
		void _refreshUnsetStates();

		BorderI				m_contentPadding;                   // Unit: Points
		CoordI				m_contentShift[StateEnum_Nb];       // Unit: Points
		Bitmask<uint32_t>	m_contentShiftStateMask = 1;		// Bitfield with one bit set for each stateIndex that has been explicitly set.
	};


} // namespace wg
#endif //WG_EXTENDEDSKIN_DOT_H
