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
			return { input.top & (int) 0xFFFFFFFC, input.right & (int) 0xFFFFFFFC, input.bottom & (int) 0xFFFFFFFC, input.left & (int) 0xFFFFFFFC };
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
			return { (points.x* MU::qpixPerPoint()) & (int) 0xFFFFFFFC, (points.y* MU::qpixPerPoint()) & (int) 0xFFFFFFFC };
		}

		inline SizeI pointsToRawAligned(const SizeI points)
		{
			return { (points.w* MU::qpixPerPoint()) & (int) 0xFFFFFFFC, (points.h* MU::qpixPerPoint()) & (int) 0xFFFFFFFC };
		}

		inline BorderI pointsToRawAligned(const BorderI& points)
		{
			return { (points.top* MU::qpixPerPoint()) & (int) 0xFFFFFFFC, (points.right* MU::qpixPerPoint()) & (int) 0xFFFFFFFC,
					 (points.bottom* MU::qpixPerPoint()) & (int) 0xFFFFFFFC, (points.left* MU::qpixPerPoint()) & (int) 0xFFFFFFFC };
		}

		inline RectI pointsToRawAligned(const RectI& points)
		{
			return { (points.x* MU::qpixPerPoint()) & (int) 0xFFFFFFFC, (points.y* MU::qpixPerPoint()) & (int) 0xFFFFFFFC,
					 (points.w* MU::qpixPerPoint()) & (int) 0xFFFFFFFC, (points.h* MU::qpixPerPoint()) & (int) 0xFFFFFFFC };
		}

		//____ qpixToPixels() __________________________________________________

		inline int qpixToPixels(int quarterPixels)
		{
			return quarterPixels >> 2;
		}

		inline CoordI qpixToPixels(const CoordI quarterPixels)
		{
			return { quarterPixels.x >> 2, quarterPixels.y >> 2 };
		}

		inline SizeI qpixToPixels(const SizeI quarterPixels)
		{
			return { quarterPixels.w >> 2, quarterPixels.h >> 2 };
		}

		inline BorderI qpixToPixels(const BorderI& quarterPixels)
		{
			return { quarterPixels.top >> 2, quarterPixels.right >> 2, quarterPixels.bottom >> 2, quarterPixels.left >> 2 };
		}

		inline RectI qpixToPixels(const RectI& quarterPixels)
		{
			return { quarterPixels.x >> 2, quarterPixels.y >> 2, quarterPixels.w >> 2, quarterPixels.h >> 2 };
		}

		//____ pixelsToQpix() __________________________________________________

		inline int pixelsToQpix(int pixels)
		{
			return pixels << 2;
		}

		inline CoordI pixelsToQpix(const CoordI pixels)
		{
			return { pixels.x << 2, pixels.y << 2 };
		}

		inline SizeI pixelsToQpix(const SizeI pixels)
		{
			return { pixels.w << 2, pixels.h << 2 };
		}

		inline BorderI pixelsToQpix(const BorderI& pixels)
		{
			return { pixels.top << 2, pixels.right << 2, pixels.bottom << 2, pixels.left << 2 };
		}

		inline RectI pixelsToQpix(const RectI& pixels)
		{
			return { pixels.x << 2, pixels.y << 2, pixels.w << 2, pixels.h << 2 };
		}


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

/*
		inline Coord pixelsToQPix(CoordI pixels)
		{
			return Coord(MU::fromPixel(pixels.x), MU::fromPixel(pixels.y));
		}

		inline Size pixelsToQPix(SizeI pixels)
		{
			return Size(MU::fromPixel(pixels.w), MU::fromPixel(pixels.h));
		}

		inline Rect pixelsToQPix(RectI pixels)
		{
			return Rect(MU::fromPixel(pixels.x), MU::fromPixel(pixels.y), MU::fromPixel(pixels.w), MU::fromPixel(pixels.h));
		}
*/
		//____ qpixToMU() ____________________________________________________

		inline Coord qpixToMU(const CoordI raw)
		{
			Coord c;
			c.x.qpix = raw.x;
			c.y.qpix = raw.y;
			return c;
		}

		inline Size qpixToMU(const SizeI raw)
		{
			Size sz;
			sz.w.qpix = raw.w;
			sz.h.qpix = raw.h;
			return sz;
		}

		inline Border qpixToMU(const BorderI raw)
		{
			Border b;
			b.top.qpix = raw.top;
			b.right.qpix = raw.right;
			b.bottom.qpix = raw.bottom;
			b.left.qpix = raw.left;
			return b;
		}

		inline Rect qpixToMU(const RectI& raw)
		{
			Rect r;
			r.x.qpix = raw.x;
			r.y.qpix = raw.y;
			r.w.qpix = raw.w;
			r.h.qpix = raw.h;
			return r;
		}

		//____ MUToQpix() ____________________________________________________

		inline CoordI MUToQpix(const Coord& c)
		{
			return { c.x.qpix, c.y.qpix };
		}

		inline SizeI MUToQpix(const Size& sz)
		{
			return { sz.w.qpix, sz.h.qpix };
		}

		inline BorderI MUToQpix(const Border& b)
		{
			return { b.top.qpix, b.right.qpix, b.bottom.qpix, b.left.qpix };
		}

		inline RectI MUToQpix(const Rect& r)
		{
			return { r.x.qpix, r.y.qpix, r.w.qpix, r.h.qpix };
		}

		//____ MUToQpixAligned() ____________________________________________________

		inline CoordI MUToQpixAligned(const Coord& c)
		{
			return { c.x.qpix & (int) 0xFFFFFFFC, c.y.qpix & (int) 0xFFFFFFFC };
		}

		inline SizeI MUToQpixAligned(const Size& sz)
		{
			return { sz.w.qpix & (int)0xFFFFFFFC, sz.h.qpix & (int)0xFFFFFFFC };
		}

		inline BorderI MUToQpixAligned(const Border& b)
		{
			return { b.top.qpix & (int) 0xFFFFFFFC, b.right.qpix & (int)0xFFFFFFFC, b.bottom.qpix & (int)0xFFFFFFFC, b.left.qpix & (int)0xFFFFFFFC };
		}

		inline RectI MUToQpixAligned(const Rect& r)
		{
			return { r.x.qpix & (int)0xFFFFFFFC, r.y.qpix & (int)0xFFFFFFFC, r.w.qpix & (int)0xFFFFFFFC, r.h.qpix & (int)0xFFFFFFFC };
		}

		//____ alignQpix() ____________________________________________________

		inline int alignQpix(const int c)
		{
			return c & (int)0xFFFFFFFC;
		}


		inline CoordI alignQpix(const CoordI& c)
		{
			return { c.x & (int)0xFFFFFFFC, c.y & (int)0xFFFFFFFC };
		}

		inline SizeI alignQpix(const SizeI& sz)
		{
			return { sz.w & (int)0xFFFFFFFC, sz.h & (int)0xFFFFFFFC };
		}

		inline BorderI alignQpix(const BorderI& b)
		{
			return { b.top & (int)0xFFFFFFFC, b.right & (int)0xFFFFFFFC, b.bottom & (int)0xFFFFFFFC, b.left & (int)0xFFFFFFFC };
		}

		inline RectI alignQpix(const RectI& r)
		{
			return { r.x & (int)0xFFFFFFFC, r.y & (int)0xFFFFFFFC, r.w & (int)0xFFFFFFFC, r.h & (int)0xFFFFFFFC };
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
			ClipPopData() : bInitialized(false) {}
			ClipPopData(int _nRects, const RectI * _pRects, int _reservedMem ) : nRects(_nRects), pRects(_pRects), reservedMem(_reservedMem), bInitialized(true) {}

			int nRects;
			const RectI * pRects;
			int reservedMem;
			bool bInitialized;
		};

		ClipPopData	patchesToClipList( GfxDevice * pDevice, const RectI& clip, const Patches& patches );
		ClipPopData	patchesToClipList( GfxDevice * pDevice, const Patches& patches );
		ClipPopData limitClipList( GfxDevice * pDevice, const RectI& clip );
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
