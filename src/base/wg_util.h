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

		//____ pixelAligned() _________________________________________________________

		inline BorderI pixelAligned(const BorderI& input)
		{
			return { input.top & (int) 0xFFFFFFFC, input.left & (int) 0xFFFFFFFC, input.bottom & (int) 0xFFFFFFFC, input.right & (int) 0xFFFFFFFC };
		}

		inline SizeI pixelAligned(const SizeI input)
		{
			return { input.w & (int) 0xFFFFFFFC, input.h & (int) 0xFFFFFFFC };
		}

		inline CoordI pixelAligned(const CoordI input)
		{
			return { input.x & (int) 0xFFFFFFFC, input.y & (int) 0xFFFFFFFC };
		}

		inline RectI pixelAligned(const RectI& input)
		{
			return { input.x & (int) 0xFFFFFFFC, input.y & (int) 0xFFFFFFFC, input.w & (int) 0xFFFFFFFC, input.h & (int) 0xFFFFFFFC };
		}

		inline Coord pixelAligned(const Coord input)
		{
			Coord p = input;
			p.x.pixelAlign();
			p.y.pixelAlign();
			return p;
		}

		inline Border pixelAligned(const Border& input)
		{
			Border b = input;
			b.top.pixelAlign();
			b.left.pixelAlign();
			b.bottom.pixelAlign();
			b.right.pixelAlign();
			return b;
		}

		inline Size pixelAligned(const Size input)
		{
			Size sz = input;
			sz.w.pixelAlign();
			sz.h.pixelAlign();
			return sz;
		}

		inline Rect pixelAligned(const Rect& input)
		{
			Rect r = input;
			r.x.pixelAlign();
			r.y.pixelAlign();
			r.w.pixelAlign();
			r.h.pixelAlign();
			return r;
		}

		//____ pointsToRawAligned() _________________________________________________________

		inline CoordI pointsToRawAligned(const CoordI points)
		{
			return { (points.x* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC, (points.y* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC };
		}

		inline SizeI pointsToRawAligned(const SizeI points)
		{
			return { (points.w* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC, (points.h* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC };
		}

		inline BorderI pointsToRawAligned(const BorderI& points)
		{
			return { (points.top* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC, (points.right* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC,
					 (points.bottom* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC, (points.left* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC };
		}

		inline RectI pointsToRawAligned(const RectI& points)
		{
			return { (points.x* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC, (points.y* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC,
					 (points.w* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC, (points.h* Base::pixelQuartersPerPoint()) & (int) 0xFFFFFFFC };
		}

		//____ rawToPixels() __________________________________________________

		inline CoordI rawToPixels(const CoordI quarterPixels)
		{
			return { quarterPixels.x >> 2, quarterPixels.y >> 2 };
		}

		inline SizeI rawToPixels(const SizeI quarterPixels)
		{
			return { quarterPixels.w >> 2, quarterPixels.h >> 2 };
		}

		inline BorderI rawToPixels(const BorderI& quarterPixels)
		{
			return { quarterPixels.top >> 2, quarterPixels.right >> 2, quarterPixels.bottom >> 2, quarterPixels.left >> 2 };
		}

		inline RectI rawToPixels(const RectI& quarterPixels)
		{
			return { quarterPixels.x >> 2, quarterPixels.y >> 2, quarterPixels.w >> 2, quarterPixels.h >> 2 };
		}

		//____ pixelsToRaw() __________________________________________________

		inline CoordI pixelsToRaw(const CoordI pixels)
		{
			return { pixels.x << 2, pixels.y << 2 };
		}

		inline SizeI pixelsToRaw(const SizeI pixels)
		{
			return { pixels.w << 2, pixels.h << 2 };
		}

		inline BorderI pixelsToRaw(const BorderI& pixels)
		{
			return { pixels.top << 2, pixels.right << 2, pixels.bottom << 2, pixels.left << 2 };
		}

		inline RectI pixelsToRaw(const RectI& pixels)
		{
			return { pixels.x << 2, pixels.y << 2, pixels.w << 2, pixels.h << 2 };
		}


		//____ pointsToPixels() _______________________________________________

		inline BorderI pointsToPixels(const BorderI& points)
		{
			return { (points.right * Base::pixelQuartersPerPoint()) >> 2, (points.bottom * Base::pixelQuartersPerPoint()) >> 2,
					 (points.left * Base::pixelQuartersPerPoint()) >> 2, (points.top * Base::pixelQuartersPerPoint()) >> 2 };
		}

/*
		inline Coord pixelsToQPix(CoordI pixels)
		{
			return Coord(QPix::fromPixel(pixels.x), QPix::fromPixel(pixels.y));
		}

		inline Size pixelsToQPix(SizeI pixels)
		{
			return Size(QPix::fromPixel(pixels.w), QPix::fromPixel(pixels.h));
		}

		inline Rect pixelsToQPix(RectI pixels)
		{
			return Rect(QPix::fromPixel(pixels.x), QPix::fromPixel(pixels.y), QPix::fromPixel(pixels.w), QPix::fromPixel(pixels.h));
		}
*/
		//____ rawToQpix() ____________________________________________________

		inline Coord rawToQpix(const CoordI raw)
		{
			Coord c;
			c.x.value = raw.x;
			c.y.value = raw.y;
			return c;
		}

		inline Size rawToQpix(const SizeI raw)
		{
			Size sz;
			sz.w.value = raw.w;
			sz.h.value = raw.h;
			return sz;
		}

		inline Border rawToQpix(const BorderI raw)
		{
			Border b;
			b.top.value = raw.top;
			b.right.value = raw.right;
			b.bottom.value = raw.bottom;
			b.left.value = raw.left;
			return b;
		}

		inline Rect rawToQpix(const RectI& raw)
		{
			Rect r;
			r.x.value = raw.x;
			r.y.value = raw.y;
			r.w.value = raw.w;
			r.h.value = raw.h;
			return r;
		}

		//____ qpixToRaw() ____________________________________________________

		inline CoordI qpixToRaw(const Coord c)
		{
			return { c.x.value, c.y.value };
		}

		inline SizeI qpixToRaw(const Size sz)
		{
			return { sz.w.value, sz.h.value };
		}

		inline BorderI qpixToRaw(const Border b)
		{
			return { b.top.value, b.right.value, b.bottom.value, b.left.value };
		}

		inline RectI qpixToRaw(const Rect& r)
		{
			return { r.x.value, r.y.value, r.w.value, r.h.value };
		}


		double squareRoot(double a);
		double powerOfTen(int num);

		bool		markTestStretchRect( CoordI ofs, Surface * pSurface, const RectI& source, const RectI& area, int opacityTreshold );

		bool		markTestNinePatch( CoordI ofs, Surface * pSurface, const RectI& source, const RectI& dest, int opacityTreshold, BorderI sourceFrame);


		bool		pixelFormatToDescription( PixelFormat format, PixelDescription& wFormat );

		CoordI 		origoToOfs( Origo origo, SizeI base );
		RectI		origoToRect( Origo origo, SizeI base, SizeI rect );

		SizeI		scaleToFit(SizeI object, SizeI boundaries);

		int 		sizeFromPolicy( int defaultSize, int specifiedSize, SizePolicy policy );

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


		struct ClipPopData				/** @private */
		{
			int nRects;
			const RectI * pRects;
			int reservedMem;
		};
		
		ClipPopData	patchesToClipList( GfxDevice * pDevice, const RectI& clip, const Patches& patches );
		ClipPopData	patchesToClipList( GfxDevice * pDevice, const Patches& patches );
		ClipPopData limitClipList( GfxDevice * pDevice, const RectI& clip );
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
