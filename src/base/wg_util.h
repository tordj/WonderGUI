#ifndef WG_UTIL_DOT_H
#define WG_UTIL_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_types.h>

namespace wg
{

	class Rect;
	class Surface;

	//____ Util _________________________________________________________________

	namespace Util		/** @private */
	{
		double squareRoot(double a);
		double powerOfTen(int num);

		bool		markTestStretchRect( Coord ofs, Surface * pSurface, const Rect& source, const Rect& area, int opacityTreshold );

		bool		pixelTypeToFormat( PixelType type, PixelFormat& wFormat );

		Coord 	origoToOfs( Origo origo, Size base );
		Rect		origoToRect( Origo origo, Size base, Size rect );

		int 		sizeFromPolicy( int defaultSize, int specifiedSize, SizePolicy policy );

	    inline Orientation dirToOrient( Direction dir ) { return (dir == Direction::Up || dir == Direction::Down) ? Orientation::Vertical : Orientation::Horizontal; }


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


		inline int _stateToIndex(StateEnum state)
		{
			static int	s_stateToIndexTable[StateEnum_MaxValue+1] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 8, 9, 10, 11, 12, 13 };
			return s_stateToIndexTable[(uint8_t)state];
		}


		extern uint8_t _limitUint8Table[768];

		inline uint8_t limitUint8( int value )
		{
			return _limitUint8Table[value+256];
		}
	}




} // namespace wg
#endif // WG_UTIL_DOT_H
