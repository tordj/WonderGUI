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
	class GfxDevice;
	class Patches;

	//____ Util _________________________________________________________________

	namespace Util		/** @private */
	{

		//____ pointsToPixels() _______________________________________________

		inline CoordI pointsToPixels(const CoordI& points)
		{
			return { (points.x * MU::qpixPerPoint()) >> 2, (points.y * MU::qpixPerPoint()) >> 2 };
		}

		inline SizeI pointsToPixels(const SizeI& points)
		{
			return { (points.w * MU::qpixPerPoint()) >> 2, (points.h * MU::qpixPerPoint()) >> 2 };
		}

		inline BorderI pointsToPixels(const BorderI& points)
		{
			return { (points.top * MU::qpixPerPoint()) >> 2, (points.right * MU::qpixPerPoint()) >> 2,
					 (points.bottom * MU::qpixPerPoint()) >> 2, (points.left * MU::qpixPerPoint()) >> 2 };
		}

		inline RectI pointsToPixels(const RectI& points)
		{
			return { (points.x * MU::qpixPerPoint()) >> 2, (points.y * MU::qpixPerPoint()) >> 2,
					 (points.w * MU::qpixPerPoint()) >> 2, (points.h * MU::qpixPerPoint()) >> 2 };
		}



		double squareRoot(double a);
		double powerOfTen(int num);

		bool		markTestStretchRect( Coord ofs, Surface * pSurface, const RectI& source, const Rect& area, int opacityTreshold );
		bool		markTestTileRect(Coord ofs, Surface* pSurface, const Rect& area, int opacityTreshold);

		bool		markTestNinePatch( Coord ofs, Surface * pSurface, const RectI& source, const Rect& dest, int opacityTreshold, const BorderI& sourceFrame);


		bool		pixelFormatToDescription( PixelFormat format, PixelDescription& output );

		Coord 		origoToOfs( Origo origo, Size base );
		Rect		origoToRect( Origo origo, Size base, Size rect );

		Size		scaleToFit(Size object, Size boundaries);

		MU 			sizeFromPolicy( MU defaultSize, MU specifiedSize, SizePolicy policy );

		inline Axis dirToAxis( Direction dir ) { return (dir == Direction::Up || dir == Direction::Down) ? Axis::Y : Axis::X; }

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


		struct ClipPopData				/** @private */
		{
			ClipPopData() : bInitialized(false) {}
			ClipPopData(int _nRects, const RectI * _pRects, int _reservedMem ) : nRects(_nRects), pRects(_pRects), reservedMem(_reservedMem), bInitialized(true) {}

			int nRects;
			const RectI * pRects;
			int reservedMem;
			bool bInitialized;
		};

		ClipPopData	patchesToClipList( GfxDevice * pDevice, const Rect& clip, const Patches& patches );
		ClipPopData	patchesToClipList( GfxDevice * pDevice, const Patches& patches );
		ClipPopData limitClipList( GfxDevice * pDevice, const Rect& clip );
		ClipPopData pushClipList(GfxDevice * pDevice);
		void 		popClipList( GfxDevice * pDevice, const ClipPopData& popData );


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
