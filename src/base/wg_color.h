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

#ifndef	WG_TYPES_DOT_H
#	include	<wg_types.h>
#endif

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
		inline Color() {};
		inline Color( const Color& col );
		inline Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );
		inline Color( uint32_t argb );
		inline Color( uint32_t rgb, uint8_t a );
	
		inline bool operator==( const Color& k ) const;
		inline bool operator!=( const Color& k ) const;
	
		inline Color operator+( const Color& k ) const;
		inline Color operator-( const Color& k ) const;
	
		inline Color operator*( float f ) const;
		inline Color operator*( const Color& k ) const;
	
		static Color blend( const Color& start, const Color& dest, float grade );
	
		void	setCMYK( float c, float m, float y, float k, uint8_t alpha = 255 );
		void	getCMYK( float* c, float* m, float* y, float* k );
	
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
	
	
	
		const static Color	transparent;		// 0x00FFFFFF
		const static Color	none;				// 0x00000000	// Handled differently than 'transparent' when cascading colors.
	
		const static Color 	aliceblue;			// 0xF0F8FF
		const static Color 	antiquewhite;		// 0xFAEBD7
		const static Color 	aqua;				// 0x00ffff
		const static Color 	aquamarine;			// 0x7fffd4
		const static Color 	azure;				// 0xf0ffff
		const static Color 	beige;				// 0xf5f5dc
		const static Color 	bisque;  			// 0xffe4c4
		const static Color 	black;	  			// 0x000000
		const static Color 	blanchedalmond; 	// 0xffebcD
		const static Color 	blue; 	  			// 0x0000ff
		const static Color 	blueviolet; 		// 0x8a2be2
		const static Color 	brown;   			// 0xa52a2a
		const static Color 	burlywood;  		// 0xdeb887
		const static Color 	cadetblue;  		// 0x5f9ea0
		const static Color 	chartreuse; 		// 0x7fff00
		const static Color 	chocolate;  		// 0xd2691e
		const static Color 	coral;   			// 0xff7f50
		const static Color 	cornflowerblue; 	// 0x6495eD
		const static Color 	cornsilk; 			// 0xfff8dc
		const static Color 	crimson;  			// 0xdc143c
		const static Color 	cyan;  				// 0x00ffff
		const static Color 	darkblue;  			// 0x00008b
		const static Color 	darkcyan;  			// 0x008b8b
		const static Color 	darkgoldenrod;  	// 0xb8860b
		const static Color 	darkgray;  			// 0xa9a9a9
		const static Color 	darkgrey;  			// 0xa9a9a9
		const static Color 	darkgreen;  		// 0x006400
		const static Color 	darkkhaki;  		// 0xbdb76b
		const static Color 	darkmagenta;  		// 0x8b008b
		const static Color 	darkolivegreen; 	// 0x556b2F
		const static Color 	darkorange;  		// 0xff8c00
		const static Color 	darkorchid;  		// 0x9932cc
		const static Color 	darkred;  			// 0x8b0000
		const static Color 	darksalmon;  		// 0xe9967a
		const static Color 	darkseagreen;  		// 0x8fbc8f
		const static Color 	darkslateblue;  	// 0x483d8b
		const static Color 	darkslategray;  	// 0x2f4f4f
		const static Color 	darkslategrey;  	// 0x2f4f4f
		const static Color 	darkturquoise;  	// 0x00ced1
		const static Color 	darkviolet;  		// 0x9400d3
		const static Color 	deeppink;  			// 0xff1493
		const static Color 	deepskyblue; 		// 0x00bfff
		const static Color 	dimgray;  			// 0x696969
		const static Color 	dimgrey;  			// 0x696969
		const static Color 	dodgerblue;  		// 0x1e90ff
		const static Color 	firebrick;  		// 0xb22222
		const static Color 	floralwhite; 		// 0xfffaf0
		const static Color 	forestgreen; 		// 0x228b22
		const static Color 	fuchsia;  			// 0xff00ff
		const static Color 	gainsboro;  		// 0xdcdcdc
		const static Color 	ghostwhite;  		// 0xf8f8ff
		const static Color 	gold;  				// 0xffd700
		const static Color 	goldenrod;  		// 0xdaa520
		const static Color 	gray;  				// 0x808080
		const static Color 	grey;  				// 0x808080
		const static Color 	green;  			// 0x008000
		const static Color 	greenyellow; 		// 0xadff2f
		const static Color 	honeydew;  			// 0xf0fff0
		const static Color 	hotpink;  			// 0xff69b4
		const static Color 	indianred;   		// 0xcd5c5c
		const static Color 	indigo;   			// 0x4b0082
		const static Color 	ivory;  			// 0xfffff0
		const static Color 	khaki;  			// 0xf0e68c
		const static Color 	lavender;  			// 0xe6e6fa
		const static Color 	lavenderblush;  	// 0xfff0f5
		const static Color 	lawngreen;  		// 0x7cfc00
		const static Color 	lemonchiffon;		// 0xfffacd
		const static Color 	lightblue;  		// 0xadd8e6
		const static Color 	lightcoral;  		// 0xf08080
		const static Color 	lightcyan;  		// 0xe0ffff
		const static Color 	lightgoldenrodyellow;	// 0xfAFAD2
		const static Color 	lightgray;  		// 0xd3d3d3
		const static Color 	lightgrey;  		// 0xd3d3d3
		const static Color 	lightgreen;  		// 0x90ee90
		const static Color 	lightpink;  		// 0xffb6c1
		const static Color 	lightsalmon;  		// 0xffa07a
		const static Color 	lightseagreen;  	// 0x20b2aa
		const static Color 	lightskyblue;  		// 0x87cefa
		const static Color 	lightslategray;  	// 0x778899
		const static Color 	lightslategrey;  	// 0x778899
		const static Color 	lightsteelblue;  	// 0xb0c4dE
		const static Color 	lightyellow;  		// 0xffffe0
		const static Color 	lime;  				// 0x00ff00
		const static Color 	limegreen;  		// 0x32cd32
		const static Color 	linen;  			// 0xfaf0e6
		const static Color 	magenta;  			// 0xff00ff
		const static Color 	maroon;  			// 0x800000
		const static Color 	mediumaquamarine;  	// 0x66cdaA
		const static Color 	mediumblue;  		// 0x0000cd
		const static Color 	mediumorchid;		// 0xba55d3
		const static Color 	mediumpurple;	 	// 0x9370d8
		const static Color 	mediumseagreen;  	// 0x3cb371
		const static Color 	mediumslateblue;  	// 0x7b68eE
		const static Color 	mediumspringgreen;  // 0x00fa9A
		const static Color 	mediumturquoise;  	// 0x48d1cC
		const static Color 	mediumvioletred;  	// 0xc71585
		const static Color 	midnightblue;  		// 0x191970
		const static Color 	mintcream;  		// 0xf5fffa
		const static Color 	mistyrose;  		// 0xffe4e1
		const static Color 	moccasin;  			// 0xffe4b5
		const static Color 	navajowhite;  		// 0xffdead
		const static Color 	navy;  				// 0x000080
		const static Color 	oldlace;  			// 0xfdf5e6
		const static Color 	olive;  			// 0x808000
		const static Color 	olivedrab;  		// 0x6b8e23
		const static Color 	orange;  			// 0xffa500
		const static Color 	orangered;  		// 0xff4500
		const static Color 	orchid;  			// 0xda70d6
		const static Color 	palegoldenrod;  	// 0xeee8aa
		const static Color 	palegreen;  		// 0x98fb98
		const static Color 	paleturquoise;  	// 0xafeeee
		const static Color 	palevioletred;  	// 0xd87093
		const static Color 	papayawhip;  		// 0xffefd5
		const static Color 	peachpuff;  		// 0xffdab9
		const static Color 	peru;  				// 0xcd853f
		const static Color 	pink;  				// 0xffc0cb
		const static Color 	plum;  				// 0xdda0dd
		const static Color 	powderblue;  		// 0xb0e0e6
		const static Color 	purple;  			// 0x800080
		const static Color 	red;  				// 0xff0000
		const static Color 	rosybrown;  		// 0xbc8f8f
		const static Color 	royalblue;  		// 0x4169e1
		const static Color 	saddlebrown; 		// 0x8b4513
		const static Color 	salmon;  			// 0xfa8072
		const static Color 	sandybrown;  		// 0xf4a460
		const static Color 	seagreen;  			// 0x2e8b57
		const static Color 	seashell;  			// 0xfff5ee
		const static Color 	sienna;  			// 0xa0522d
		const static Color 	silver;  			// 0xc0c0c0
		const static Color 	skyblue;  			// 0x87ceeb
		const static Color 	slateblue;  		// 0x6a5acd
		const static Color 	slategray;  		// 0x708090
		const static Color 	slategrey;  		// 0x708090
		const static Color 	snow;  				// 0xfffafa
		const static Color 	springgreen; 		// 0x00ff7f
		const static Color 	steelblue;  		// 0x4682b4
		const static Color 	tan;  				// 0xd2b48c
		const static Color 	teal;  				// 0x008080
		const static Color 	thistle;  			// 0xd8bfd8
		const static Color 	tomato;  			// 0xff6347
		const static Color 	turquoise;  		// 0x40e0d0
		const static Color 	violet;  			// 0xee82ee
		const static Color 	wheat;  			// 0xf5deb3
		const static Color 	white;  			// 0xffffff
		const static Color 	whitesmoke;  		// 0xf5f5f5
		const static Color 	yellow;  			// 0xffff00
		const static Color 	yellowgreen; 		// 0x9acd32
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
	
	//-------------------------------------------------------------------
	inline Color Color::operator+( const Color& k ) const
	{
		Color kNewColor;
		kNewColor.r = r + k.r;
		kNewColor.g = g + k.g;
		kNewColor.b = b + k.b;
		kNewColor.a = a + k.a;
		return kNewColor;
	}
	
	//-------------------------------------------------------------------
	inline Color Color::operator-( const Color& k ) const
	{
		Color kNewColor;
		kNewColor.r = r - k.r;
		kNewColor.g = g - k.g;
		kNewColor.b = b - k.b;
		kNewColor.a = a - k.a;
		return kNewColor;
	}
	
	//-------------------------------------------------------------------
	inline Color Color::operator*( float f ) const
	{
		Color kNewColor;
		kNewColor.r = (uint8_t)( (float)r * f );
		kNewColor.g = (uint8_t)( (float)g * f );
		kNewColor.b = (uint8_t)( (float)b * f );
		kNewColor.a = (uint8_t)( (float)a * f );
		return kNewColor;
	}
	
	//-------------------------------------------------------------------
	inline Color Color::operator*( const Color& k ) const
	{
		Color kNewColor;
		kNewColor.r = (uint8_t)(((int)r * (int)k.r )/255);
		kNewColor.g = (uint8_t)(((int)g * (int)k.g )/255);
		kNewColor.b = (uint8_t)(((int)b * (int)k.b )/255);
		kNewColor.a = (uint8_t)(((int)a * (int)k.a )/255);
		return kNewColor;
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
	

} // namespace wg
#endif // WG_COLOR_DOT_H
