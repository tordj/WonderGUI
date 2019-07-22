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

#ifndef WG_UTIL_DOT_H
#define WG_UTIL_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_types.h>

namespace wg
{

	class Surface;

	//____ Util _________________________________________________________________

	namespace Util		/** @private */
	{

		//____ pixelAligned() _________________________________________________________

		inline CoordQ pixelAligned(CoordQ input)
		{
			CoordQ p = input;
			p.x.pixelAlign();
			p.y.pixelAlign();
			return p;
		}

		inline BorderQ pixelAligned(BorderQ input)
		{
			BorderQ b = input;
			b.top.pixelAlign();
			b.left.pixelAlign();
			b.bottom.pixelAlign();
			b.right.pixelAlign();
			return b;
		}

		inline SizeQ pixelAligned(SizeQ input)
		{
			SizeQ sz = input;
			sz.w.pixelAlign();
			sz.h.pixelAlign();
			return sz;
		}

		inline RectQ pixelAligned(RectQ input)
		{
			RectQ r = input;
			r.x.pixelAlign();
			r.y.pixelAlign();
			r.w.pixelAlign();
			r.h.pixelAlign();
			return r;
		}

		inline Border toPixels(Border points)
		{
			return { (points.right * Base::pixelQuartersPerPoint()) >> 2, (points.bottom * Base::pixelQuartersPerPoint()) >> 2,
					 (points.left * Base::pixelQuartersPerPoint()) >> 2, (points.top * Base::pixelQuartersPerPoint()) >> 2 };
		}


		double squareRoot(double a);
		double powerOfTen(int num);

		bool		markTestStretchRect( Coord ofs, Surface * pSurface, const Rect& source, const Rect& area, int opacityTreshold );

		bool		markTestNinePatch( Coord ofs, Surface * pSurface, const Rect& source, const Rect& dest, int opacityTreshold, Border sourceFrame);


		bool		pixelFormatToDescription( PixelFormat format, PixelDescription& wFormat );

		Coord 		origoToOfs( Origo origo, Size base );
		Rect		origoToRect( Origo origo, Size base, Size rect );

		Size		scaleToFit(Size object, Size boundaries);

		int 		sizeFromPolicy( int defaultSize, int specifiedSize, SizeQolicy policy );

		inline Orientation dirToOrient( Direction dir ) { return (dir == Direction::Up || dir == Direction::Down) ? Orientation::Vertical : Orientation::Horizontal; }

		int			bestStateIndexMatch(int wantedStateIndex, Bitmask<uint32_t> availableStateIndices);

		inline int _stateToIndex(StateEnum state)
		{
			static int	s_stateToIndexTable[StateEnum_MaxValue + 1] = { 0, 1, 0, 0, 2, 3, 4, 5, 6, 7, 0, 0, 8, 9, 10, 11,
																		0, 0, 0, 0, 12, 13, 0, 0, 0, 0, 0, 0, 14, 15, 0, 0,
																		16, 0, 0, 0, 0, 0, 0, 0, 17 };
			return s_stateToIndexTable[(uint8_t)state];
		}

		inline StateEnum _indexToState(int index)
		{
			static StateEnum	s_indexToStateTable[StateEnum_Nb] = { StateEnum::Normal, StateEnum::Focused, StateEnum::Hovered, StateEnum::HoveredFocused,
				StateEnum::Pressed, StateEnum::PressedFocused, StateEnum::Selected, StateEnum::SelectedFocused, StateEnum::SelectedHovered,
				StateEnum::SelectedHoveredFocused, StateEnum::SelectedPressed, StateEnum::SelectedPressedFocused, StateEnum::Targeted,
				StateEnum::TargetedFocused, StateEnum::TargetedSelected, StateEnum::TargetedSelectedFocused, StateEnum::Disabled, StateEnum::DisabledSelected };

			return s_indexToStateTable[index];
		}

		uint32_t mostSignificantBit(uint32_t value);



		// A simple checksum algorithm that just performs a long division
		// with a standard CRC polynomial. Quicker and less complex than a standard
		// CRC on modern hardware with FPU but probably gives a checksum of
		// somewhat lower quality.

		class Checksum8
		{
		public:
			Checksum8() { remainder = 0; }

			inline void add8( uint8_t x ) { remainder = ((remainder << 8) + x)%dividend;}
			inline void add16( uint16_t x ) { remainder = ((remainder << 16) + x)%dividend;}
			inline void add32( uint32_t x ) { remainder = (uint32_t)(((((uint64_t)remainder) << 32) + x)%dividend);}

			void add( const void * pData, uint32_t nBytes );

			uint8_t getChecksum() { return (remainder & 0xFF); }
			inline void clear() { remainder = 0; }

		private:
			uint32_t	remainder;
			const static int dividend = 0x107;
		};

		extern uint8_t _limitUint8Table[768];

		inline uint8_t limitUint8( int value )
		{
			return _limitUint8Table[value+256];
		}
	}





} // namespace wg
#endif // WG_UTIL_DOT_H
