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
#include <wg_patches.h>

namespace wg
{

	class GfxDevice;

	//____ Util _________________________________________________________________

	namespace Util		/** @private */
	{
		inline binalInt toBinalInt(float floatSPX)
		{
			return binalInt(floatSPX * BINAL_MUL/64);
		}

		inline binalInt toBinalInt(spx intSPX)
		{
			return binalInt(intSPX) * BINAL_MUL/64;
		}

	
		inline RectI roundToPixels(const RectSPX& r);
		inline CoordI roundToPixels(const CoordSPX& r);
		inline SizeI roundToPixels(const SizeSPX& r);
		inline BorderI roundToPixels(const BorderSPX& r);
		inline int roundToPixels(spx r);

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

		inline spx align(spx x)
		{
			return (x + 32) & 0xFFFFFFC0;
		}

		inline CoordSPX align(const CoordSPX& coord)
		{
			return { (coord.x + 32) & -64, (coord.y + 32) & -64 };
		}

		inline SizeSPX align(const SizeSPX& size)
		{
			return { (size.w + 32) & -64, (size.h + 32) & -64 };
		}

		inline BorderSPX align(const BorderSPX& border)
		{
			return { (border.top + 32) & -64, (border.right + 32) & -64,
					 (border.bottom + 32) & -64, (border.left + 32) & -64 };
		}

		inline RectSPX align(const RectSPX& rect)
		{
			RectSPX out;

			out.x = (rect.x + 32) & -64;
			out.y = (rect.y + 32) & -64;

			out.w = ((rect.x + rect.w + 32) & -64) - out.x;
			out.h = ((rect.y + rect.h + 32) & -64) - out.y;

			return out;
		}

		inline spx alignUp(spx x)
		{
			return (x + 63) & 0xFFFFFFC0;
		}

		inline CoordSPX alignUp(const CoordSPX& coord)
		{
			return { (coord.x + 63) & -64, (coord.y + 63) & -64 };
		}

		inline SizeSPX alignUp(const SizeSPX& size)
		{
			return { (size.w + 63) & -64, (size.h + 63) & -64 };
		}

		inline BorderSPX alignUp(const BorderSPX& border)
		{
			return { (border.top + 63) & -64, (border.right + 63) & -64,
					 (border.bottom + 63) & -64, (border.left + 63) & -64 };
		}

		inline RectSPX alignUp(const RectSPX& rect)
		{
			//NOTE: Actually aligns down position in order to expand rectangle outwards.

			RectSPX out;

			out.x = (rect.x) & -64;
			out.y = (rect.y) & -64;

			out.w = ((rect.x + rect.w + 63) & -64) - out.x;
			out.h = ((rect.y + rect.h + 63) & -64) - out.y;

			return out;
		}

		inline spx alignDown(spx x)
		{
			return (x) & 0xFFFFFFC0;
		}

		inline CoordSPX alignDown(const CoordSPX& coord)
		{
			return { (coord.x) & -64, (coord.y) & -64 };
		}

		inline SizeSPX alignDown(const SizeSPX& size)
		{
			return { (size.w) & -64, (size.h) & -64 };
		}

		inline BorderSPX alignDown(const BorderSPX& border)
		{
			return { (border.top) & -64, (border.right) & -64,
					 (border.bottom) & -64, (border.left) & -64 };
		}

		inline RectSPX alignDown(const RectSPX& rect)
		{
			//NOTE: Actually aligns down position in order to expand rectangle outwards.

			RectSPX out;

			out.x = (rect.x + 63) & -64;
			out.y = (rect.y + 63) & -64;

			out.w = ((rect.x + rect.w) & -64) - out.x;
			out.h = ((rect.y + rect.h) & -64) - out.y;

			return out;
		}




		double	squareRoot(double a);
		double	powerOfTen(int num);
		int		gcd(int a, int b);

		bool		markTestStretchRect( CoordSPX ofs, Surface * pSurface, const RectSPX& source, const RectSPX& area, int opacityTreshold );
		bool		markTestTileRect(CoordSPX ofs, Surface* pSurface, const RectSPX& area, int scale, int opacityTreshold);
		bool		markTestNinePatch(CoordSPX ofs, Surface* pSurface, const NinePatch& patch, const RectSPX& _dest, int scale, int opacityTreshold);


		bool		pixelFormatToDescription( PixelFormat format, PixelDescription& output );
		PixelFormat	pixelDescriptionToFormat(const PixelDescription& description);

		CoordSPX 	placementToOfs( Placement placement, SizeSPX base );
		RectSPX		placementToRect( Placement placement, SizeSPX base, SizeSPX rect );

		SizeSPX		scaleToFit(SizeSPX object, SizeSPX boundaries);

		spx 		sizeFromConstraint( spx defaultSize, spx specifiedSize, SizeConstraint policy );

		inline Axis dirToAxis( Direction dir ) { return (dir == Direction::Up || dir == Direction::Down) ? Axis::Y : Axis::X; }

		int			bestStateIndexMatch(int wantedStateIndex, Bitmask<uint32_t> availableStateIndices);


		uint32_t mostSignificantBit(uint32_t value);


		struct ClipPopData				/** @private */
		{
			ClipPopData() : bInitialized(false) {}
			ClipPopData(int _nRects, const RectSPX * _pRects, int _reservedMem ) : nRects(_nRects), pRects(_pRects), reservedMem(_reservedMem), bInitialized(true) {}

			int nRects;
			const RectSPX * pRects;
			int reservedMem;
			bool bInitialized;
		};

		ClipPopData	patchesToClipList( GfxDevice * pDevice, const RectSPX& clip, const PatchesSPX& patches );
		ClipPopData	patchesToClipList( GfxDevice * pDevice, const PatchesSPX& patches );
		ClipPopData limitClipList( GfxDevice * pDevice, const RectSPX& clip );
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
			return (in >> 8 | in << 8);
		}

		constexpr bool isSystemBigEndian()
		{
	#if WG_IS_LITTLE_ENDIAN
			return false;
	#else
			return true;
	#endif
		}

		inline int readUTF8Char( const char *& pStr );



	}

	RectI Util::roundToPixels(const RectSPX& r)
	{
		int x2 = r.x + r.w;
		int y2 = r.y + r.h;

		RectI out;

		out.x = (r.x + 32) >> 6;
		out.y = (r.y + 32) >> 6;

		out.w = ((x2 + 32) >> 6) - out.x;
		out.h = ((y2 + 32) >> 6) - out.y;

		return out;
	}

	CoordI Util::roundToPixels(const CoordSPX& r)
	{
		return { (r.x + 32) >> 6, (r.y + 32) >> 6 };
	}

	SizeI Util::roundToPixels(const SizeSPX& r)
	{
		return { (r.w + 32) >> 6, (r.h + 32) >> 6 };
	}

	BorderI Util::roundToPixels(const BorderSPX& r)
	{
		return { (r.top + 32) >> 6, (r.right + 32) >> 6, (r.bottom + 32) >> 6, (r.left + 32) >> 6 };
	}


	int Util::roundToPixels(spx r)
	{
		return (r + 32) >> 6;
	}

	int Util::readUTF8Char( const char *& _pStr )
	{
		const uint8_t *& pStr = (const uint8_t *&) _pStr;

		int g = * pStr++;
		if( g < 128 )
			return g;											// 1 byte character (7 bits data).

		if( (g & 0xE0) == 0xC0 )					// 2 bytes character (11 bits data).
		{
			uint8_t c1 = pStr[0];
			if( (c1 & 0xC0) != 0x80 )
				return 0xFFFD;
			
			pStr += 1;
			return ((g & 0x1F) << 6) | int(c1 & 0x3F);
		}
		else if( (g & 0xF0) == 0xE0 )			// 3 bytes character (16 bits data).
		{
			uint8_t c1 = pStr[0];
			if( (c1 & 0xC0) != 0x80 )
				return 0xFFFD;

			uint8_t c2 = pStr[1];
			if( (c2 & 0xC0) != 0x80 )
				return 0xFFFD;

			pStr += 2;
			return ((g & 0x0F) << 12) | (int(c1 & 0x3F) << 6) | int(c2 & 0x3F) ;
		}
		else if( (g & 0xF8) == 0xF0 )			// 4 bytes character (21 bits data).
		{
			uint8_t c1 = pStr[0];
			if( (c1 & 0xC0) != 0x80 )
				return 0xFFFD;

			uint8_t c2 = pStr[1];
			if( (c2 & 0xC0) != 0x80 )
				return 0xFFFD;

			uint8_t c3 = pStr[2];
			if( (c3 & 0xC0) != 0x80 )
				return 0xFFFD;

			pStr += 3;
			return ((g & 0x07) << 18) | (int(c1 & 0x3F) << 12) | (int(c2 & 0x3F) << 6) | int(c3 & 0x3F);
		}
		else
			return 0xFFFD;					// Broken character (or more than 4 bytes which we don't support)

		return g;
	}

} // namespace wg
#endif // WG_UTIL_DOT_H
