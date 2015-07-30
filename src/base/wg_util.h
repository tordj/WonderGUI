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

class WgRect;


//____ WgUtil _________________________________________________________________

namespace WgUtil
{
	bool		markTestStretchRect( WgCoord ofs, const WgSurfacePtr& pSurface, const WgRect& source, const WgRect& area, int opacityTreshold );

	bool		pixelTypeToFormat( WgPixelType type, WgPixelFormat& wFormat );

	WgCoord 	origoToOfs( WgOrigo origo, WgSize base );
	WgRect		origoToRect( WgOrigo origo, WgSize base, WgSize rect );

	int 		sizeFromPolicy( int defaultSize, int specifiedSize, WgSizePolicy policy );

    inline WgOrientation dirToOrient( WgDirection dir ) { return (dir == WG_UP || dir == WG_DOWN) ? WG_VERTICAL : WG_HORIZONTAL; }
    

	template <class T> inline T max (T a, T b) {return (a>b?a:b); }
	template <class T> inline T min (T a, T b) {return (a<b?a:b); }


	// A simple checksum algorithm that just performs a long division
	// with a standard CRC polynomial. Quicker and less complex than a standard
	// CRC on modern hardware with FPU but probably gives a checksum of
	// somewhat lower quality.

	class Checksum8
	{
	public:
		Checksum8() { remainder = 0; }

		inline void add8( Uint8 x ) { remainder = ((remainder << 8) + x)%dividend;}
		inline void add16( Uint16 x ) { remainder = ((remainder << 16) + x)%dividend;}
		inline void add32( Uint32 x ) { remainder = (Uint32)(((((Uint64)remainder) << 32) + x)%dividend);}

		void add( const void * pData, Uint32 nBytes );

		Uint8 getChecksum() { return (remainder & 0xFF); }
		inline void clear() { remainder = 0; }

	private:
		Uint32	remainder;
		const static int dividend = 0x107;
	};


	Uint32 decodeBase64( const char * pSrc, Uint32 nIn, char * pDest );

	template<typename T>
	inline std::string toString(T value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}

	// These two are needed to print the value as an integer instead of an ascii character
	template<> inline std::string toString(Sint8 value) { return toString<int>(value); }
	template<> inline std::string toString(Uint8 value) { return toString<Uint32>(value); }

	template<>
	inline std::string toString(bool value)
	{
		return value ? "true" : "false";
	}

	template<typename T0, typename T1, typename T2, typename T3>
	inline std::string toString(const T0& a, const T1& b, const T2& c, const T3& d);

	template<>
	inline std::string toString(WgOrigo value)
	{
		switch( value )
		{
			case WG_NORTHWEST:
				return "northwest";
			case WG_NORTH:
				return "north";
			case WG_NORTHEAST:
				return "northeast";
			case WG_EAST:
				return "east";
			case WG_SOUTHEAST:
				return "southeast";
			case WG_SOUTH:
				return "south";
			case WG_SOUTHWEST:
				return "southwest";
			case WG_WEST:
				return "west";
			case WG_CENTER:
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
	inline bool fromString(const std::string& str, Sint8& a)
	{
		int i;
		if(fromString<int>(str, i))
		{
			if(Sint8(i) == i)
			{
				a = i;
				return true;
			}
		}
		return false;
	}

	template<>
	inline bool fromString(const std::string& str, Uint8& a)
	{
		Uint32 i;
		if(fromString<Uint32>(str, i))
		{
			if(Uint8(i) == i)
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
	inline bool fromString(const std::string& str, WgOrigo& a)
	{
		if(str.empty() || str == "northwest") a = WG_NORTHWEST;
		else if(str == "north") a = WG_NORTH;
		else if(str == "northeast") a = WG_NORTHEAST;
		else if(str == "east") a = WG_EAST;
		else if(str == "southeast") a = WG_SOUTHEAST;
		else if(str == "south") a = WG_SOUTH;
		else if(str == "southwest") a = WG_SOUTHWEST;
		else if(str == "west") a = WG_WEST;
		else if(str == "center") a = WG_CENTER;
		else
		{
			a = WG_NORTHWEST;
			return false;
		}
		return true;
	}


	inline bool 	toBool(const std::string& value, bool def = false)	{ bool 	 v = def; fromString(value, v); return v; }
	inline Sint8	toSint8(const std::string& value, Sint8 def = 0)	{ Sint8	 v = def; fromString(value, v); return v; }
	inline Sint16	toSint16(const std::string& value, Sint16 def = 0)	{ Sint16 v = def; fromString(value, v); return v; }
	inline int		toSint32(const std::string& value, int def = 0)	{ int v = def; fromString(value, v); return v; }
	inline Sint64	toSint64(const std::string& value, Sint64 def = 0)	{ Sint64 v = def; fromString(value, v); return v; }
	inline Uint8	toUint8(const std::string& value, Uint8 def = 0)	{ Uint8	 v = def; fromString(value, v); return v; }
	inline Uint16	toUint16(const std::string& value, Uint16 def = 0)	{ Uint16 v = def; fromString(value, v); return v; }
	inline Uint32	toUint32(const std::string& value, Uint32 def = 0)	{ Uint32 v = def; fromString(value, v); return v; }
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



	inline int _stateToIndex(WgStateEnum state)
	{ 
		static int	s_stateToIndexTable[WG_MAX_STATE_VALUE+1] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 8, 9, 10, 11, 12, 13 };
		return s_stateToIndexTable[state]; 
	}

}



#endif // WG_UTIL_DOT_H
