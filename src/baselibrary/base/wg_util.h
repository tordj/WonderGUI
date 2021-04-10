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
#include <wg_surface.h>

namespace wg
{

	class GfxDevice;
	class Patches;

	//____ Util _________________________________________________________________

	namespace Util		/** @private */
	{
		inline pts spxToPts(spx value, int scale)
		{
			return pts(value)/scale;
		}

		inline Coord spxToPts(const CoordSPX& coord, int scale)
		{
			return Coord(pts(coord.x) / scale, pts(coord.y) / scale);
		}

		inline Size spxToPts(const SizeSPX& size, int scale)
		{
			return Size(pts(size.w) / scale, pts(size.h) / scale);
		}

		inline Border spxToPts(const BorderSPX& border, int scale)
		{
			return Border(	pts(border.top) / scale, pts(border.right) / scale,
							pts(border.bottom) / scale, pts(border.left) / scale);
		}

		inline Rect spxToPts(const RectSPX& rect, int scale)
		{
			return Rect(pts(rect.x) / scale, pts(rect.y) / scale,
						pts(rect.w) / scale, pts(rect.h) / scale);
		}


		inline spx ptsToSpx(pts value, int scale)
		{
			return spx(value * scale);
		}

		inline CoordSPX ptsToSpx(const Coord& coord, int scale)
		{
			return CoordSPX(spx(coord.x * scale), spx(coord.y * scale));
		}

		inline SizeSPX ptsToSpx(const Size& size, int scale)
		{
			return SizeSPX(spx(size.w * scale), spx(size.h * scale));
		}

		inline BorderSPX ptsToSpx(const Border& border, int scale)
		{
			return BorderSPX(spx(border.top * scale), spx(border.right * scale),
				spx(border.bottom * scale), spx(border.left * scale));
		}

		inline RectSPX ptsToSpx(const Rect& rect, int scale)
		{
			return RectSPX(spx(rect.x * scale), spx(rect.y * scale),
				spx(rect.w * scale), spx(rect.h * scale));
		}


		double	squareRoot(double a);
		double	powerOfTen(int num);
		int		gcd(int a, int b);

		bool		markTestStretchRect( Coord ofs, Surface * pSurface, const RectI& source, const Rect& area, int opacityTreshold );
		bool		markTestTileRect(Coord ofs, Surface* pSurface, const Rect& area, int opacityTreshold);
		bool		markTestNinePatch(Coord ofs, Surface* pSurface, const NinePatch& patch, const Rect& _dest, int opacityTreshold);


		bool		pixelFormatToDescription( PixelFormat format, PixelDescription& output );

		Coord 		placementToOfs( Placement placement, Size base );
		Rect		placementToRect( Placement placement, Size base, Size rect );

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

		inline uint32_t endianSwap( uint32_t in )
		{
			return ((in & 0xff000000) >> 24) | ((in & 0x00ff0000) >> 8) | ((in & 0x0000ff00) << 8) | (in << 24);
		}

		inline uint16_t endianSwap( uint16_t in )
		{
			return ((in & 0xff00) >> 8) | (in << 8);
		}

		inline bool isSystemBigEndian()
		{
	#if IS_LITTLE_ENDIAN
			return false;
	#else
			return true;
	#endif
		}




	}
/*
	void Util::roundToPixels(RectI& r)
	{		
		int x2 = r.x + r.w;
		int y2 = r.y + r.h;

		r.x = (r.x + 32) >> 6;
		r.y = (r.y + 32) >> 6;

		r.w = ((x2 + 32) >> 6) - r.x;
		r.h = ((y2 + 32) >> 6) - r.y;
	}

	void Util::roundToPixels(CoordI& r)
	{
		r.x = (r.x + 32) >> 6;
		r.y = (r.y + 32) >> 6;
	}

	void Util::roundToPixels(SizeI& r)
	{
		r.w = (r.w + 32) >> 6;
		r.h = (r.h + 32) >> 6;
	}

	void Util::roundToPixels(int& r)
	{
		r = (r + 32) >> 6;
	}
*/



} // namespace wg
#endif // WG_UTIL_DOT_H
