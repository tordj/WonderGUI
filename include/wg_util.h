#ifndef WG_UTIL_DOT_H
#define WG_UTIL_DOT_H

#include <string>
#include <sstream>
#include <vector>
#include <wg_origo.h>
#include <wg_geo.h>

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgBlock;
class WgRect;


//____ WgUtil _________________________________________________________________

namespace WgUtil
{
	bool		AdjustScaledArea(const WgBlock& block, WgRect& area);

	bool		MarkTestBlock( WgCoord ofs, const WgBlock& block, WgRect area );

	WgCoord 		OrientationToOfs( WgOrientation orientation, WgSize base );
	WgRect			OrientationToRect( WgOrientation orientation, WgSize base, WgSize rect );

	template <class T> inline T Max (T a, T b) {return (a>b?a:b); }
	template <class T> inline T Min (T a, T b) {return (a<b?a:b); }


	// A simple checksum algorithm that just performs a long division
	// with a standard CRC polynomial. Quicker and less complex than a standard
	// CRC on modern hardware with FPU but probably gives a checksum of
	// somewhat lower quality.

	class Checksum8
	{
	public:
		Checksum8() { remainder = 0; }

		inline void Add8( Uint8 x ) { remainder = ((remainder << 8) + x)%dividend;}
		inline void Add16( Uint16 x ) { remainder = ((remainder << 16) + x)%dividend;}
		inline void Add32( Uint32 x ) { remainder = (Uint32)(((((Uint64)remainder) << 32) + x)%dividend);}

		void Add( const void * pData, Uint32 nBytes );

		Uint8 GetChecksum() { return (remainder & 0xFF); }
		inline void Clear() { remainder = 0; }

	private:
		Uint32	remainder;
		const static int dividend = 0x107;
	};


	Uint32 DecodeBase64( const char * pSrc, Uint32 nIn, char * pDest );

	template<typename T>
	inline std::string ToString(T value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}

	// These two are needed to print the value as an integer instead of an ascii character
	template<> inline std::string ToString(Sint8 value) { return ToString<int>(value); }
	template<> inline std::string ToString(Uint8 value) { return ToString<Uint32>(value); }

	template<>
	inline std::string ToString(bool value)
	{
		return value ? "true" : "false";
	}

	template<typename T0, typename T1, typename T2, typename T3>
	inline std::string ToString(const T0& a, const T1& b, const T2& c, const T3& d);

	template<>
	inline std::string ToString(WgOrigo value)
	{
		if(value == WgOrigo::topLeft()) return "topleft";
		if(value == WgOrigo::topCenter()) return "topcenter";
		if(value == WgOrigo::topRight()) return "topright";
		if(value == WgOrigo::midLeft()) return "midleft";
		if(value == WgOrigo::midCenter()) return "midcenter";
		if(value == WgOrigo::midRight()) return "midright";
		if(value == WgOrigo::bottomLeft()) return "bottomleft";
		if(value == WgOrigo::bottomCenter()) return "bottomcenter";
		if(value == WgOrigo::bottomRight()) return "bottomright";

		return ToString(value.anchorX(), value.anchorY(), value.hotspotX(), value.hotspotY());
	}

	template<>
	inline std::string ToString(WgOrientation value)
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
	inline std::string ToString(const T0& a, const T1& b)
	{
		return ToString<T0>(a) + ", " + ToString<T1>(b);
	}

	template<typename T0, typename T1, typename T2>
	inline std::string ToString(const T0& a, const T1& b, const T2& c)
	{
		return ToString(a, b) + ", " + ToString<T2>(c);
	}

	template<typename T0, typename T1, typename T2, typename T3>
	inline std::string ToString(const T0& a, const T1& b, const T2& c, const T3& d)
	{
		return ToString(a, b) + ", " + ToString(c, d);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	inline std::string ToString(const T0& a, const T1& b, const T2& c, const T3& d, const T4& e)
	{
		return ToString(a, b, c) + ", " + ToString(d, e);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	inline std::string ToString(const T0& a, const T1& b, const T2& c, const T3& d, const T4& e, const T5& f)
	{
		return ToString(a, b, c, d) + ", " + ToString(e, f);
	}

	template<typename T>
	inline bool FromString(const std::string& str, T& a)
	{
		std::istringstream is(str);
		is >> a;
		std::ws(is);
		return is.eof() == true && is.fail() == false;
	}

	template<>
	inline bool FromString(const std::string& str, Sint8& a)
	{
		int i;
		if(FromString<int>(str, i))
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
	inline bool FromString(const std::string& str, Uint8& a)
	{
		Uint32 i;
		if(FromString<Uint32>(str, i))
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
	inline bool FromString(const std::string& str, bool& a)
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
			FromString<float>(str, f);
			return f != 0;
		}
		return true;
	}

	template<typename T0, typename T1, typename T2, typename T3>
	inline int FromString(const std::string& str, T0& a, T1& b, T2& c, T3& d);

	template<>
	inline bool FromString(const std::string& str, WgOrigo& a)
	{
		if(str.empty() || str == "topleft") a = WgOrigo::topLeft();
		else if(str == "topcenter") a = WgOrigo::topCenter();
		else if(str == "topright") a = WgOrigo::topRight();
		else if(str == "midleft") a = WgOrigo::midLeft();
		else if(str == "midcenter") a = WgOrigo::midCenter();
		else if(str == "midright") a = WgOrigo::midRight();
		else if(str == "bottomleft") a = WgOrigo::bottomLeft();
		else if(str == "bottomcenter") a = WgOrigo::bottomCenter();
		else if(str == "bottomright") a = WgOrigo::bottomRight();
		else
		{
			float anchorx = 0, anchory = 0, hotspotx = 0, hotspoty = 0;
			FromString(str, anchorx, anchory, hotspotx, hotspoty);
			a = WgOrigo::specific(anchorx, anchory, hotspotx, hotspoty);
		}
		return true;
	}

	template<>
	inline bool FromString(const std::string& str, WgOrientation& a)
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


	inline bool 	ToBool(const std::string& value, bool def = false)	{ bool 	 v = def; FromString(value, v); return v; }
	inline Sint8	ToSint8(const std::string& value, Sint8 def = 0)	{ Sint8	 v = def; FromString(value, v); return v; }
	inline Sint16	ToSint16(const std::string& value, Sint16 def = 0)	{ Sint16 v = def; FromString(value, v); return v; }
	inline int		ToSint32(const std::string& value, int def = 0)	{ int v = def; FromString(value, v); return v; }
	inline Sint64	ToSint64(const std::string& value, Sint64 def = 0)	{ Sint64 v = def; FromString(value, v); return v; }
	inline Uint8	ToUint8(const std::string& value, Uint8 def = 0)	{ Uint8	 v = def; FromString(value, v); return v; }
	inline Uint16	ToUint16(const std::string& value, Uint16 def = 0)	{ Uint16 v = def; FromString(value, v); return v; }
	inline Uint32	ToUint32(const std::string& value, Uint32 def = 0)	{ Uint32 v = def; FromString(value, v); return v; }
	inline float	ToFloat(const std::string& value, float def = 0)	{ float	 v = def; FromString(value, v); return v; }

	inline WgOrigo ToOrigo(const std::string& value, WgOrigo def = WgOrigo::topLeft())
	{
		if(value.empty())
			return def;
		WgOrigo o = def;
		FromString(value, o);
		return o;
	}

	inline void Tokenize(const std::string& str, std::vector<std::string>& tokens)
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
	inline int FromString(const std::string& str, T0& a, T1& b)
	{
		std::vector<std::string> tokens;
		Tokenize(str, tokens); tokens.resize(2);
		if(!FromString<T0>(tokens[0], a)) return 0;
		if(!FromString<T1>(tokens[1], b)) return 1;
		return 2;
	}

	template<typename T0, typename T1, typename T2, typename T3>
	inline int FromString(const std::string& str, T0& a, T1& b, T2& c, T3& d)
	{
		std::vector<std::string> tokens;
		Tokenize(str, tokens); tokens.resize(4);
		if(!FromString<T0>(tokens[0], a)) return 0;
		if(!FromString<T1>(tokens[1], b)) return 1;
		if(!FromString<T2>(tokens[2], c)) return 2;
		if(!FromString<T3>(tokens[3], d)) return 3;
		return 4;
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	inline int FromString(const std::string& str, T0& a, T1& b, T2& c, T3& d, T4& e)
	{
		std::vector<std::string> tokens;
		Tokenize(str, tokens); tokens.resize(5);
		if(!FromString<T0>(tokens[0], a)) return 0;
		if(!FromString<T1>(tokens[1], b)) return 1;
		if(!FromString<T2>(tokens[2], c)) return 2;
		if(!FromString<T3>(tokens[3], d)) return 3;
		if(!FromString<T4>(tokens[4], e)) return 4;
		return 5;
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	inline int FromString(const std::string& str, T0& a, T1& b, T2& c, T3& d, T4& e, T5& f)
	{
		std::vector<std::string> tokens;
		Tokenize(str, tokens); tokens.resize(6);
		if(!FromString<T0>(tokens[0], a)) return 0;
		if(!FromString<T1>(tokens[1], b)) return 1;
		if(!FromString<T2>(tokens[2], c)) return 2;
		if(!FromString<T3>(tokens[3], d)) return 3;
		if(!FromString<T4>(tokens[4], e)) return 4;
		if(!FromString<T5>(tokens[5], f)) return 5;
		return 6;
	}

	inline void XMLEscape(const std::string& text, std::string& escaped )
	{
		escaped = text;
		int ofs;
		ofs =-5; while((ofs = escaped.find_first_of('&', ofs+5)) >= 0)	escaped.replace(ofs, 1, "&amp;");
		ofs =-4; while((ofs = escaped.find_first_of('<', ofs+4)) >= 0)	escaped.replace(ofs, 1, "&lt;");
		ofs =-4; while((ofs = escaped.find_first_of('>', ofs+4)) >= 0)	escaped.replace(ofs, 1, "&gt;");
		ofs =-6; while((ofs = escaped.find_first_of('"', ofs+6)) >= 0)	escaped.replace(ofs, 1, "&quot;");
		ofs =-6; while((ofs = escaped.find_first_of('\'',ofs+6)) >= 0)	escaped.replace(ofs, 1, "&apos;");
	}

}



#endif // WG_UTIL_DOT_H
