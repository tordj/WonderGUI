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

#ifndef	WG_COLOR_DOT_H
#define	WG_COLOR_DOT_H
#pragma once

#include	<wg_types.h>

namespace wg
{

	//____ Class Color _______________________________________________________________________

	/**

		Specifies a color in ARGB-format, where the alpha component controls transparency.
		Range for the individual color components is 0-255.

		Setting alpha to 0 gives full transparency while 255 gives full opacity.

		The different components can be accessed individually through the r, g, b, and a members,
		or as a group through the argb member.

		For convenience, the color class also provides static, predefined colors matching the standard web-palette.

	**/

	class Color
	{
	public:

		//.____ Creation __________________________________________

		inline Color() : argb(0) {};
		inline Color( const Color& col );
		inline Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );
		inline Color( uint32_t argb );
		inline Color( uint32_t rgb, uint8_t a );

		//.____ Operators ___________________________________________

		inline bool operator==(const Color& k) const;
		inline bool operator!=(const Color& k) const;

		Color	operator+(const Color& k) const;
		Color	operator-(const Color& k) const;

		Color	operator*(float f) const;
		Color	operator*(const Color& k) const;

		//.____ Properties __________________________________________

		union
		{
			struct
			{
				uint8_t b;
				uint8_t g;
				uint8_t r;
				uint8_t a;
			};
			uint32_t argb;
		};

		//.____ Definitions _________________________________________

		const static Color	Transparent;		// 0x000000
		const static Color 	AliceBlue;			// 0xF0F8FF
		const static Color 	AntiqueWhite;		// 0xFAEBD7
		const static Color 	Aqua;				// 0x00ffff
		const static Color 	Aquamarine;			// 0x7fffd4
		const static Color 	Azure;				// 0xf0ffff
		const static Color 	Beige;				// 0xf5f5dc
		const static Color 	Bisque;  			// 0xffe4c4
		const static Color 	Black;	  			// 0x000000
		const static Color 	BlanchedAlmond; 	// 0xffebcD
		const static Color 	Blue; 	  			// 0x0000ff
		const static Color 	BlueViolet; 		// 0x8a2be2
		const static Color 	Brown;   			// 0xa52a2a
		const static Color 	Burlywood;  		// 0xdeb887
		const static Color 	CadetBlue;  		// 0x5f9ea0
		const static Color 	Chartreuse; 		// 0x7fff00
		const static Color 	Chocolate;  		// 0xd2691e
		const static Color 	Coral;   			// 0xff7f50
		const static Color 	CornflowerBlue; 	// 0x6495eD
		const static Color 	Cornsilk; 			// 0xfff8dc
		const static Color 	Crimson;  			// 0xdc143c
		const static Color 	Cyan;  				// 0x00ffff
		const static Color 	DarkBlue;  			// 0x00008b
		const static Color 	DarkCyan;  			// 0x008b8b
		const static Color 	DarkGoldenrod;  	// 0xb8860b
		const static Color 	DarkGray;  			// 0xa9a9a9
		const static Color 	DarkGrey;  			// 0xa9a9a9
		const static Color 	DarkGreen;  		// 0x006400
		const static Color 	DarkKhaki;  		// 0xbdb76b
		const static Color 	DarkMagenta;  		// 0x8b008b
		const static Color 	DarkOliveGreen; 	// 0x556b2F
		const static Color 	DarkOrange;  		// 0xff8c00
		const static Color 	DarkOrchid;  		// 0x9932cc
		const static Color 	DarkRed;  			// 0x8b0000
		const static Color 	DarkSalmon;  		// 0xe9967a
		const static Color 	DarkSeaGreen;  		// 0x8fbc8f
		const static Color 	DarkSlateBlue;  	// 0x483d8b
		const static Color 	DarkSlateGray;  	// 0x2f4f4f
		const static Color 	DarkSlateGrey;  	// 0x2f4f4f
		const static Color 	DarkTurquoise;  	// 0x00ced1
		const static Color 	DarkViolet;  		// 0x9400d3
		const static Color 	DeepPink;  			// 0xff1493
		const static Color 	DeepSkyBlue; 		// 0x00bfff
		const static Color 	DimGray;  			// 0x696969
		const static Color 	DimGrey;  			// 0x696969
		const static Color 	DodgerBlue;  		// 0x1e90ff
		const static Color 	FireBrick;  		// 0xb22222
		const static Color 	FloralWhite; 		// 0xfffaf0
		const static Color 	ForestGreen; 		// 0x228b22
		const static Color 	Fuchsia;  			// 0xff00ff
		const static Color 	Gainsboro;  		// 0xdcdcdc
		const static Color 	GhostWhite;  		// 0xf8f8ff
		const static Color 	Gold;  				// 0xffd700
		const static Color 	Goldenrod;  		// 0xdaa520
		const static Color 	Gray;  				// 0x808080
		const static Color 	Grey;  				// 0x808080
		const static Color 	Green;  			// 0x008000
		const static Color 	GreenYellow; 		// 0xadff2f
		const static Color 	Honeydew;  			// 0xf0fff0
		const static Color 	HotPink;  			// 0xff69b4
		const static Color 	IndianRed;   		// 0xcd5c5c
		const static Color 	Indigo;   			// 0x4b0082
		const static Color 	Ivory;  			// 0xfffff0
		const static Color 	Khaki;  			// 0xf0e68c
		const static Color 	Lavender;  			// 0xe6e6fa
		const static Color 	LavenderBlush;  	// 0xfff0f5
		const static Color 	LawnGreen;  		// 0x7cfc00
		const static Color 	LemonCiffon;		// 0xfffacd
		const static Color 	LightBlue;  		// 0xadd8e6
		const static Color 	LightCoral;  		// 0xf08080
		const static Color 	LightCyan;  		// 0xe0ffff
		const static Color 	LightGoldenrodYellow;	// 0xfAFAD2
		const static Color 	LightGray;  		// 0xd3d3d3
		const static Color 	LightGrey;  		// 0xd3d3d3
		const static Color 	LightGreen;  		// 0x90ee90
		const static Color 	LightPink;  		// 0xffb6c1
		const static Color 	LightSalmon;  		// 0xffa07a
		const static Color 	LightSeaGreen;  	// 0x20b2aa
		const static Color 	LightSkyBlue;  		// 0x87cefa
		const static Color 	LightSlateGray;  	// 0x778899
		const static Color 	LightSlateGrey;  	// 0x778899
		const static Color 	LightSteelBlue;  	// 0xb0c4dE
		const static Color 	LightYellow;  		// 0xffffe0
		const static Color 	Lime;  				// 0x00ff00
		const static Color 	LimeGreen;  		// 0x32cd32
		const static Color 	Linen;  			// 0xfaf0e6
		const static Color 	Magenta;  			// 0xff00ff
		const static Color 	Maroon;  			// 0x800000
		const static Color 	MediumAquamarine;  	// 0x66cdaA
		const static Color 	MediumBlue;  		// 0x0000cd
		const static Color 	MediumOrchid;		// 0xba55d3
		const static Color 	MediumPurple;	 	// 0x9370d8
		const static Color 	MediumSeaGreen;  	// 0x3cb371
		const static Color 	MediumSlateBlue;  	// 0x7b68eE
		const static Color 	MediumSpringGreen;  // 0x00fa9A
		const static Color 	MediumTurquoise;  	// 0x48d1cC
		const static Color 	MediumVioletRed;  	// 0xc71585
		const static Color 	MidnightBlue;  		// 0x191970
		const static Color 	MintCream;  		// 0xf5fffa
		const static Color 	MistyRose;  		// 0xffe4e1
		const static Color 	Moccasin;  			// 0xffe4b5
		const static Color 	NavajoWhite;  		// 0xffdead
		const static Color 	Navy;  				// 0x000080
		const static Color 	OldLace;  			// 0xfdf5e6
		const static Color 	Olive;  			// 0x808000
		const static Color 	OliveDrab;  		// 0x6b8e23
		const static Color 	Orange;  			// 0xffa500
		const static Color 	OrangeRed;  		// 0xff4500
		const static Color 	Orchid;  			// 0xda70d6
		const static Color 	PaleGoldenrod;  	// 0xeee8aa
		const static Color 	PaleGreen;  		// 0x98fb98
		const static Color 	PaleTurquoise;  	// 0xafeeee
		const static Color 	PaleVioletRed;  	// 0xd87093
		const static Color 	PapayaWhip;  		// 0xffefd5
		const static Color 	PeachPuff;  		// 0xffdab9
		const static Color 	Peru;  				// 0xcd853f
		const static Color 	Pink;  				// 0xffc0cb
		const static Color 	Plum;  				// 0xdda0dd
		const static Color 	PowderBlue;  		// 0xb0e0e6
		const static Color 	Purple;  			// 0x800080
		const static Color 	Red;  				// 0xff0000
		const static Color 	RosyBrown;  		// 0xbc8f8f
		const static Color 	RoyalBlue;  		// 0x4169e1
		const static Color 	SaddleBrown; 		// 0x8b4513
		const static Color 	Salmon;  			// 0xfa8072
		const static Color 	SandyBrown;  		// 0xf4a460
		const static Color 	SeaGreen;  			// 0x2e8b57
		const static Color 	Seashell;  			// 0xfff5ee
		const static Color 	Sienna;  			// 0xa0522d
		const static Color 	Silver;  			// 0xc0c0c0
		const static Color 	SkyBlue;  			// 0x87ceeb
		const static Color 	SlateBlue;  		// 0x6a5acd
		const static Color 	SlateGray;  		// 0x708090
		const static Color 	SlateGrey;  		// 0x708090
		const static Color 	Snow;  				// 0xfffafa
		const static Color 	SpringGreen; 		// 0x00ff7f
		const static Color 	SteelBlue;  		// 0x4682b4
		const static Color 	Tan;  				// 0xd2b48c
		const static Color 	Teal;  				// 0x008080
		const static Color 	Thistle;  			// 0xd8bfd8
		const static Color 	Tomato;  			// 0xff6347
		const static Color 	Turquoise;  		// 0x40e0d0
		const static Color 	Violet;  			// 0xee82ee
		const static Color 	Wheat;  			// 0xf5deb3
		const static Color 	White;  			// 0xffffff
		const static Color 	WhiteSmoke;  		// 0xf5f5f5
		const static Color 	Yellow;  			// 0xffff00
		const static Color 	YellowGreen; 		// 0x9acd32


	};


	//____ HiColor ____________________________________________________________

	class HiColor
	{
		friend class Base;
	public:

		//.____ Creation __________________________________________

		inline	HiColor() : argb(0) {};
		inline	HiColor(const HiColor& hiColor);
				HiColor(Color lowColor);
		inline	HiColor(int r, int g, int b, int a = 4096);
		inline	HiColor(float r, float g, float b, float a = 1.f);

		//.____ Misc ________________________________________________

		static HiColor	mix(HiColor color1, HiColor color2, int balance);			// Balance: 0 -> 4096
		static HiColor	invert(HiColor color, int grade);						// Grade: 0 -> 4096
		static HiColor	min(HiColor color1, HiColor color2);
		static HiColor	max(HiColor color1, HiColor color2);
		static HiColor	blend(HiColor baseColor, HiColor blendColor, BlendMode operation);

		//.____ Operators ___________________________________________

		operator Color() const;

		inline bool operator==(const HiColor& k) const;
		inline bool operator!=(const HiColor& k) const;

		HiColor	operator+(const HiColor& k) const;
		HiColor	operator-(const HiColor& k) const;

		HiColor	operator*(float f) const;
		HiColor	operator*(const HiColor& k) const;

		//.____ Internal ____________________________________________

		static int16_t		unpackSRGBTab[256];
		static int16_t		unpackLinearTab[256];

		static uint8_t		packSRGBTab[4097];
		static uint8_t		packLinearTab[4097];

		//.____ Properties __________________________________________

		union
		{
			struct
			{
				int16_t b;
				int16_t g;
				int16_t r;
				int16_t a;
			};
			int64_t argb;
		};

		//.____ Definitions ___________________________________________________

		const static HiColor	Transparent;		// 0x000000
		const static HiColor 	AliceBlue;			// 0xF0F8FF
		const static HiColor 	AntiqueWhite;		// 0xFAEBD7
		const static HiColor 	Aqua;				// 0x00ffff
		const static HiColor 	Aquamarine;			// 0x7fffd4
		const static HiColor 	Azure;				// 0xf0ffff
		const static HiColor 	Beige;				// 0xf5f5dc
		const static HiColor 	Bisque;  			// 0xffe4c4
		const static HiColor 	Black;	  			// 0x000000
		const static HiColor 	BlanchedAlmond; 	// 0xffebcD
		const static HiColor 	Blue; 	  			// 0x0000ff
		const static HiColor 	BlueViolet; 		// 0x8a2be2
		const static HiColor 	Brown;   			// 0xa52a2a
		const static HiColor 	Burlywood;  		// 0xdeb887
		const static HiColor 	CadetBlue;  		// 0x5f9ea0
		const static HiColor 	Chartreuse; 		// 0x7fff00
		const static HiColor 	Chocolate;  		// 0xd2691e
		const static HiColor 	Coral;   			// 0xff7f50
		const static HiColor 	CornflowerBlue; 	// 0x6495eD
		const static HiColor 	Cornsilk; 			// 0xfff8dc
		const static HiColor 	Crimson;  			// 0xdc143c
		const static HiColor 	Cyan;  				// 0x00ffff
		const static HiColor 	DarkBlue;  			// 0x00008b
		const static HiColor 	DarkCyan;  			// 0x008b8b
		const static HiColor 	DarkGoldenrod;  	// 0xb8860b
		const static HiColor 	DarkGray;  			// 0xa9a9a9
		const static HiColor 	DarkGrey;  			// 0xa9a9a9
		const static HiColor 	DarkGreen;  		// 0x006400
		const static HiColor 	DarkKhaki;  		// 0xbdb76b
		const static HiColor 	DarkMagenta;  		// 0x8b008b
		const static HiColor 	DarkOliveGreen; 	// 0x556b2F
		const static HiColor 	DarkOrange;  		// 0xff8c00
		const static HiColor 	DarkOrchid;  		// 0x9932cc
		const static HiColor 	DarkRed;  			// 0x8b0000
		const static HiColor 	DarkSalmon;  		// 0xe9967a
		const static HiColor 	DarkSeaGreen;  		// 0x8fbc8f
		const static HiColor 	DarkSlateBlue;  	// 0x483d8b
		const static HiColor 	DarkSlateGray;  	// 0x2f4f4f
		const static HiColor 	DarkSlateGrey;  	// 0x2f4f4f
		const static HiColor 	DarkTurquoise;  	// 0x00ced1
		const static HiColor 	DarkViolet;  		// 0x9400d3
		const static HiColor 	DeepPink;  			// 0xff1493
		const static HiColor 	DeepSkyBlue; 		// 0x00bfff
		const static HiColor 	DimGray;  			// 0x696969
		const static HiColor 	DimGrey;  			// 0x696969
		const static HiColor 	DodgerBlue;  		// 0x1e90ff
		const static HiColor 	FireBrick;  		// 0xb22222
		const static HiColor 	FloralWhite; 		// 0xfffaf0
		const static HiColor 	ForestGreen; 		// 0x228b22
		const static HiColor 	Fuchsia;  			// 0xff00ff
		const static HiColor 	Gainsboro;  		// 0xdcdcdc
		const static HiColor 	GhostWhite;  		// 0xf8f8ff
		const static HiColor 	Gold;  				// 0xffd700
		const static HiColor 	Goldenrod;  		// 0xdaa520
		const static HiColor 	Gray;  				// 0x808080
		const static HiColor 	Grey;  				// 0x808080
		const static HiColor 	Green;  			// 0x008000
		const static HiColor 	GreenYellow; 		// 0xadff2f
		const static HiColor 	Honeydew;  			// 0xf0fff0
		const static HiColor 	HotPink;  			// 0xff69b4
		const static HiColor 	IndianRed;   		// 0xcd5c5c
		const static HiColor 	Indigo;   			// 0x4b0082
		const static HiColor 	Ivory;  			// 0xfffff0
		const static HiColor 	Khaki;  			// 0xf0e68c
		const static HiColor 	Lavender;  			// 0xe6e6fa
		const static HiColor 	LavenderBlush;  	// 0xfff0f5
		const static HiColor 	LawnGreen;  		// 0x7cfc00
		const static HiColor 	LemonCiffon;		// 0xfffacd
		const static HiColor 	LightBlue;  		// 0xadd8e6
		const static HiColor 	LightCoral;  		// 0xf08080
		const static HiColor 	LightCyan;  		// 0xe0ffff
		const static HiColor 	LightGoldenrodYellow;	// 0xfAFAD2
		const static HiColor 	LightGray;  		// 0xd3d3d3
		const static HiColor 	LightGrey;  		// 0xd3d3d3
		const static HiColor 	LightGreen;  		// 0x90ee90
		const static HiColor 	LightPink;  		// 0xffb6c1
		const static HiColor 	LightSalmon;  		// 0xffa07a
		const static HiColor 	LightSeaGreen;  	// 0x20b2aa
		const static HiColor 	LightSkyBlue;  		// 0x87cefa
		const static HiColor 	LightSlateGray;  	// 0x778899
		const static HiColor 	LightSlateGrey;  	// 0x778899
		const static HiColor 	LightSteelBlue;  	// 0xb0c4dE
		const static HiColor 	LightYellow;  		// 0xffffe0
		const static HiColor 	Lime;  				// 0x00ff00
		const static HiColor 	LimeGreen;  		// 0x32cd32
		const static HiColor 	Linen;  			// 0xfaf0e6
		const static HiColor 	Magenta;  			// 0xff00ff
		const static HiColor 	Maroon;  			// 0x800000
		const static HiColor 	MediumAquamarine;  	// 0x66cdaA
		const static HiColor 	MediumBlue;  		// 0x0000cd
		const static HiColor 	MediumOrchid;		// 0xba55d3
		const static HiColor 	MediumPurple;	 	// 0x9370d8
		const static HiColor 	MediumSeaGreen;  	// 0x3cb371
		const static HiColor 	MediumSlateBlue;  	// 0x7b68eE
		const static HiColor 	MediumSpringGreen;  // 0x00fa9A
		const static HiColor 	MediumTurquoise;  	// 0x48d1cC
		const static HiColor 	MediumVioletRed;  	// 0xc71585
		const static HiColor 	MidnightBlue;  		// 0x191970
		const static HiColor 	MintCream;  		// 0xf5fffa
		const static HiColor 	MistyRose;  		// 0xffe4e1
		const static HiColor 	Moccasin;  			// 0xffe4b5
		const static HiColor 	NavajoWhite;  		// 0xffdead
		const static HiColor 	Navy;  				// 0x000080
		const static HiColor 	OldLace;  			// 0xfdf5e6
		const static HiColor 	Olive;  			// 0x808000
		const static HiColor 	OliveDrab;  		// 0x6b8e23
		const static HiColor 	Orange;  			// 0xffa500
		const static HiColor 	OrangeRed;  		// 0xff4500
		const static HiColor 	Orchid;  			// 0xda70d6
		const static HiColor 	PaleGoldenrod;  	// 0xeee8aa
		const static HiColor 	PaleGreen;  		// 0x98fb98
		const static HiColor 	PaleTurquoise;  	// 0xafeeee
		const static HiColor 	PaleVioletRed;  	// 0xd87093
		const static HiColor 	PapayaWhip;  		// 0xffefd5
		const static HiColor 	PeachPuff;  		// 0xffdab9
		const static HiColor 	Peru;  				// 0xcd853f
		const static HiColor 	Pink;  				// 0xffc0cb
		const static HiColor 	Plum;  				// 0xdda0dd
		const static HiColor 	PowderBlue;  		// 0xb0e0e6
		const static HiColor 	Purple;  			// 0x800080
		const static HiColor 	Red;  				// 0xff0000
		const static HiColor 	RosyBrown;  		// 0xbc8f8f
		const static HiColor 	RoyalBlue;  		// 0x4169e1
		const static HiColor 	SaddleBrown; 		// 0x8b4513
		const static HiColor 	Salmon;  			// 0xfa8072
		const static HiColor 	SandyBrown;  		// 0xf4a460
		const static HiColor 	SeaGreen;  			// 0x2e8b57
		const static HiColor 	Seashell;  			// 0xfff5ee
		const static HiColor 	Sienna;  			// 0xa0522d
		const static HiColor 	Silver;  			// 0xc0c0c0
		const static HiColor 	SkyBlue;  			// 0x87ceeb
		const static HiColor 	SlateBlue;  		// 0x6a5acd
		const static HiColor 	SlateGray;  		// 0x708090
		const static HiColor 	SlateGrey;  		// 0x708090
		const static HiColor 	Snow;  				// 0xfffafa
		const static HiColor 	SpringGreen; 		// 0x00ff7f
		const static HiColor 	SteelBlue;  		// 0x4682b4
		const static HiColor 	Tan;  				// 0xd2b48c
		const static HiColor 	Teal;  				// 0x008080
		const static HiColor 	Thistle;  			// 0xd8bfd8
		const static HiColor 	Tomato;  			// 0xff6347
		const static HiColor 	Turquoise;  		// 0x40e0d0
		const static HiColor 	Violet;  			// 0xee82ee
		const static HiColor 	Wheat;  			// 0xf5deb3
		const static HiColor 	White;  			// 0xffffff
		const static HiColor 	WhiteSmoke;  		// 0xf5f5f5
		const static HiColor 	Yellow;  			// 0xffff00
		const static HiColor 	YellowGreen; 		// 0x9acd32

	private:
		static void	_initTables();
	};

	//-------------------------------------------------------------------
	inline bool Color::operator==( const Color& k ) const
	{
		if( argb == k.argb )
			return true;

		return false;
	}

	//-------------------------------------------------------------------
	inline bool Color::operator!=( const Color& k ) const
	{
		if( argb != k.argb )
			return true;

		return false;
	}



	//____ Color::Color() _________________________________________________________________

	inline Color::Color( const Color& _col )
	{
		argb = _col.argb;
	}

	inline Color::Color( uint32_t _argb )
	{
		argb = _argb;
	}

	inline Color::Color( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a )
	{
		a = _a;
		r = _r;
		g = _g;
		b = _b;
	}

	inline Color::Color( uint32_t rgb, uint8_t _a )
	{
		argb = rgb;
		a = _a;
	}

	//-------------------------------------------------------------------
	inline bool HiColor::operator==(const HiColor& k) const
	{
		if (argb == k.argb)
			return true;

		return false;
	}

	//-------------------------------------------------------------------
	inline bool HiColor::operator!=(const HiColor& k) const
	{
		if (argb != k.argb)
			return true;

		return false;
	}



	//____ HiColor::HiColor() _________________________________________________________________

	inline HiColor::HiColor(const HiColor& hiColor)
	{
		argb = hiColor.argb;
	}

	inline HiColor::HiColor(int r, int g, int b, int a)
	{
		this->a = (int16_t) a;
		this->r = (int16_t) r;
		this->g = (int16_t) g;
		this->b = (int16_t) b;
	}

	inline HiColor::HiColor(float r, float g, float b, float a)
	{
		this->a = int16_t(a * 4096);
		this->r = int16_t(r * 4096);
		this->g = int16_t(g * 4096);
		this->b = int16_t(b * 4096);
	}




} // namespace wg
#endif // WG_COLOR_DOT_H
