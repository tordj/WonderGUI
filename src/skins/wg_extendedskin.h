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

		bool					isInstanceOf( const char * pClassName ) const override;
		const char *			className( void ) const override;
		static const char		CLASSNAME[];
		static ExtendedSkin_p	cast( Object * pObject );

		//.____ Geometry _________________________________________________

		virtual SizeQ	minSize() const override;
		virtual SizeQ	preferredSize() const override;
		virtual SizeQ	sizeForContent( const SizeQ contentSize ) const override;

		virtual void	setContentPadding( Border padding );
		virtual SizeQ	contentPadding() const override;
		virtual CoordQ	contentOfs( State state ) const override;
		virtual RectQ	contentRect( const RectQ& canvas, State state ) const override;

		//.____ Behavior _______________________________________________________

		virtual void	clearContentShift();
		virtual void	setContentShift(State state, Coord shift);
		virtual void	setContentShift(std::initializer_list< std::pair<State, Coord> > StateShifts);
		virtual CoordQ	contentShift(State state) const;

		//.____ Misc ____________________________________________________

		virtual	bool	isStateIdentical( State state, State comparedTo ) const override;


	protected:
		void _refreshUnsetStates();

		Border				m_contentPadding;
		Coord				m_contentShift[StateEnum_Nb];
		Bitmask<uint32_t>	m_contentShiftStateMask = 1;		// Bitfield with one bit set for each stateIndex that has been explicitly set.
	};



} // namespace wg
#endif //WG_EXTENDEDSKIN_DOT_H
