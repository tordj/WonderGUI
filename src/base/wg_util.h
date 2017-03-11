#ifndef WG_UTIL_DOT_H
#define WG_UTIL_DOT_H

#include <string>
#include <sstream>
#include <vector>
#include <wg_geo.h>

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

namespace wg
{

	class Rect;


	//____ Util _________________________________________________________________

	namespace Util
	{
		Coord 		layoutToCanvas( Coord& pos, float scaleFactor );
		Rect 		layoutToCanvas( Rect& geo, float scaleFactor );

		Coord 		canvasToLayout( const Coord& pos, float scaleFactor );
		Rect 		canvasToLayout( const Rect& geo, float scaleFactor );


		bool		markTestStretchRect( Coord ofs, const Surface_p& pSurface, const Rect& source, const Rect& area, int opacityTreshold );

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


		uint32_t decodeBase64( const char * pSrc, uint32_t nIn, char * pDest );

		template<typename T>
		inline std::string toString(T value)
		{
			std::ostringstream os;
			os << value;
			return os.str();
		}

		// These two are needed to print the value as an integer instead of an ascii character
		template<> inline std::string toString(int8_t value) { return toString<int>(value); }
		template<> inline std::string toString(uint8_t value) { return toString<uint32_t>(value); }

		template<>
		inline std::string toString(bool value)
		{
			return value ? "true" : "false";
		}

		template<typename T0, typename T1, typename T2, typename T3>
		inline std::string toString(const T0& a, const T1& b, const T2& c, const T3& d);

		template<>
		inline std::string toString(Origo value)
		{
			switch( value )
			{
				case Origo::NorthWest:
					return "northwest";
				case Origo::North:
					return "north";
				case Origo::NorthEast:
					return "northeast";
				case Origo::East:
					return "east";
				case Origo::SouthEast:
					return "southeast";
				case Origo::South:
					return "south";
				case Origo::SouthWest:
					return "southwest";
				case Origo::West:
					return "west";
				case Origo::Center:
					return "center";
			}

			return "northwest";				// Should never get here!!!
		}


		template<typename T0, typename T1>
		inline std::string toString(const T0& a, const T1& b)
		{
			return toString<T0>(a) + ", " + toString<T1>(b);
		}

		template<typename T0, typename T1, typename T2>
		inline std::string toString(const T0& a, const T1& b, const T2& c)
		{
			return toString(a, b) + ", " + toString<T2>(c);
		}

		template<typename T0, typename T1, typename T2, typename T3>
		inline std::string toString(const T0& a, const T1& b, const T2& c, const T3& d)
		{
			return toString(a, b) + ", " + toString(c, d);
		}

		template<typename T0, typename T1, typename T2, typename T3, typename T4>
		inline std::string toString(const T0& a, const T1& b, const T2& c, const T3& d, const T4& e)
		{
			return toString(a, b, c) + ", " + toString(d, e);
		}

		template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline std::string toString(const T0& a, const T1& b, const T2& c, const T3& d, const T4& e, const T5& f)
		{
			return toString(a, b, c, d) + ", " + toString(e, f);
		}

		template<typename T>
		inline bool fromString(const std::string& str, T& a)
		{
			std::istringstream is(str);
			is >> a;
			std::ws(is);
			return is.eof() == true && is.fail() == false;
		}

		template<>
		inline bool fromString(const std::string& str, int8_t& a)
		{
			int i;
			if(fromString<int>(str, i))
			{
				if(int8_t(i) == i)
				{
					a = i;
					return true;
				}
			}
			return false;
		}

		template<>
		inline bool fromString(const std::string& str, uint8_t& a)
		{
			uint32_t i;
			if(fromString<uint32_t>(str, i))
			{
				if(uint8_t(i) == i)
				{
					a = i;
					return true;
				}
			}
			return false;
		}

		template<>
		inline bool fromString(const std::string& str, bool& a)
		{
			std::string b;
			std::istringstream is(str);
			is >> b;
			if(b == "true")
				a = true;
			else if(b == "false")
				a = false;
			else
			{
				float f = 0;
				fromString<float>(str, f);
				return f != 0;
			}
			return true;
		}

		template<typename T0, typename T1, typename T2, typename T3>
		inline int fromString(const std::string& str, T0& a, T1& b, T2& c, T3& d);

		template<>
		inline bool fromString(const std::string& str, Origo& a)
		{
			if(str.empty() || str == "northwest") a = Origo::NorthWest;
			else if(str == "north") a = Origo::North;
			else if(str == "northeast") a = Origo::NorthEast;
			else if(str == "east") a = Origo::East;
			else if(str == "southeast") a = Origo::SouthEast;
			else if(str == "south") a = Origo::South;
			else if(str == "southwest") a = Origo::SouthWest;
			else if(str == "west") a = Origo::West;
			else if(str == "center") a = Origo::Center;
			else
			{
				a = Origo::NorthWest;
				return false;
			}
			return true;
		}


		inline bool 	toBool(const std::string& value, bool def = false)	{ bool 	 v = def; fromString(value, v); return v; }
		inline int8_t	toint8_t(const std::string& value, int8_t def = 0)	{ int8_t	 v = def; fromString(value, v); return v; }
		inline int16_t	toint16_t(const std::string& value, int16_t def = 0)	{ int16_t v = def; fromString(value, v); return v; }
		inline int		toint32_t(const std::string& value, int def = 0)	{ int v = def; fromString(value, v); return v; }
		inline int64_t	toint64_t(const std::string& value, int64_t def = 0)	{ int64_t v = def; fromString(value, v); return v; }
		inline uint8_t	touint8_t(const std::string& value, uint8_t def = 0)	{ uint8_t	 v = def; fromString(value, v); return v; }
		inline uint16_t	touint16_t(const std::string& value, uint16_t def = 0)	{ uint16_t v = def; fromString(value, v); return v; }
		inline uint32_t	touint32_t(const std::string& value, uint32_t def = 0)	{ uint32_t v = def; fromString(value, v); return v; }
		inline float	toFloat(const std::string& value, float def = 0)	{ float	 v = def; fromString(value, v); return v; }

		inline void tokenize(const std::string& str, std::vector<std::string>& tokens)
		{
			if(str.empty())
				return;
			int i0 = 0;
			int i1 = str.find(',');
			do
			{
				tokens.push_back(str.substr(i0, i1 - i0));
				i0 = i1 + 1;
				i1 = str.find(',', i0);
			}
			while(i0 > 0);
		}

		template<typename T0, typename T1>
		inline int fromString(const std::string& str, T0& a, T1& b)
		{
			std::vector<std::string> tokens;
			tokenize(str, tokens); tokens.resize(2);
			if(!fromString<T0>(tokens[0], a)) return 0;
			if(!fromString<T1>(tokens[1], b)) return 1;
			return 2;
		}

		template<typename T0, typename T1, typename T2, typename T3>
		inline int fromString(const std::string& str, T0& a, T1& b, T2& c, T3& d)
		{
			std::vector<std::string> tokens;
			tokenize(str, tokens); tokens.resize(4);
			if(!fromString<T0>(tokens[0], a)) return 0;
			if(!fromString<T1>(tokens[1], b)) return 1;
			if(!fromString<T2>(tokens[2], c)) return 2;
			if(!fromString<T3>(tokens[3], d)) return 3;
			return 4;
		}

		template<typename T0, typename T1, typename T2, typename T3, typename T4>
		inline int fromString(const std::string& str, T0& a, T1& b, T2& c, T3& d, T4& e)
		{
			std::vector<std::string> tokens;
			tokenize(str, tokens); tokens.resize(5);
			if(!fromString<T0>(tokens[0], a)) return 0;
			if(!fromString<T1>(tokens[1], b)) return 1;
			if(!fromString<T2>(tokens[2], c)) return 2;
			if(!fromString<T3>(tokens[3], d)) return 3;
			if(!fromString<T4>(tokens[4], e)) return 4;
			return 5;
		}

		template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline int fromString(const std::string& str, T0& a, T1& b, T2& c, T3& d, T4& e, T5& f)
		{
			std::vector<std::string> tokens;
			tokenize(str, tokens); tokens.resize(6);
			if(!fromString<T0>(tokens[0], a)) return 0;
			if(!fromString<T1>(tokens[1], b)) return 1;
			if(!fromString<T2>(tokens[2], c)) return 2;
			if(!fromString<T3>(tokens[3], d)) return 3;
			if(!fromString<T4>(tokens[4], e)) return 4;
			if(!fromString<T5>(tokens[5], f)) return 5;
			return 6;
		}

		inline void xmlEscape(const std::string& text, std::string& escaped )
		{
			escaped = text;
			int ofs;
			ofs =-5; while((ofs = escaped.find_first_of('&', ofs+5)) >= 0)	escaped.replace(ofs, 1, "&amp;");
			ofs =-4; while((ofs = escaped.find_first_of('<', ofs+4)) >= 0)	escaped.replace(ofs, 1, "&lt;");
			ofs =-4; while((ofs = escaped.find_first_of('>', ofs+4)) >= 0)	escaped.replace(ofs, 1, "&gt;");
			ofs =-6; while((ofs = escaped.find_first_of('"', ofs+6)) >= 0)	escaped.replace(ofs, 1, "&quot;");
			ofs =-6; while((ofs = escaped.find_first_of('\'',ofs+6)) >= 0)	escaped.replace(ofs, 1, "&apos;");
		}



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
