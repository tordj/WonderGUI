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

#include <wg_color.h>
#include <wg_util.h>

namespace wg 
{
	
	const Color Color::Transparent		( 0x00000000 );
	
	const Color Color::AliceBlue		( 0xFFF0F8FF );
	const Color Color::AntiqueWhite		( 0xFFFAEBD7 );
	const Color Color::Aqua				( 0xFF00ffff );
	const Color Color::Aquamarine		( 0xFF7fffd4 );
	const Color Color::Azure			( 0xFFf0ffff );
	const Color Color::Beige			( 0xFFf5f5dc );
	const Color Color::Bisque  			( 0xFFffe4c4 );
	const Color Color::Black	  		( 0xFF000000 );
	const Color Color::BlanchedAlmond 	( 0xFFffebcD );
	const Color Color::Blue 	  		( 0xFF0000ff );
	const Color Color::BlueViolet 		( 0xFF8a2be2 );
	const Color Color::Brown   			( 0xFFa52a2a );
	const Color Color::Burlywood  		( 0xFFdeb887 );
	const Color Color::CadetBlue  		( 0xFF5f9ea0 );
	const Color Color::Chartreuse 		( 0xFF7fff00 );
	const Color Color::Chocolate  		( 0xFFd2691e );
	const Color Color::Coral   			( 0xFFff7f50 );
	const Color Color::CornflowerBlue 	( 0xFF6495eD );
	const Color Color::Cornsilk 		( 0xFFfff8dc );
	const Color Color::Crimson  		( 0xFFdc143c );
	const Color Color::Cyan  			( 0xFF00ffff );
	const Color Color::DarkBlue  		( 0xFF00008b );
	const Color Color::DarkCyan  		( 0xFF008b8b );
	const Color Color::DarkGoldenrod  	( 0xFFb8860b );
	const Color Color::DarkGray  		( 0xFFa9a9a9 );
	const Color Color::DarkGrey  		( 0xFFa9a9a9 );
	const Color Color::DarkGreen  		( 0xFF006400 );
	const Color Color::DarkKhaki  		( 0xFFbdb76b );
	const Color Color::DarkMagenta  	( 0xFF8b008b );
	const Color Color::DarkOliveGreen 	( 0xFF556b2F );
	const Color Color::DarkOrange  		( 0xFFff8c00 );
	const Color Color::DarkOrchid  		( 0xFF9932cc );
	const Color Color::DarkRed  		( 0xFF8b0000 );
	const Color Color::DarkSalmon  		( 0xFFe9967a );
	const Color Color::DarkSeaGreen  	( 0xFF8fbc8f );
	const Color Color::DarkSlateBlue  	( 0xFF483d8b );
	const Color Color::DarkSlateGray  	( 0xFF2f4f4f );
	const Color Color::DarkSlateGrey  	( 0xFF2f4f4f );
	const Color Color::DarkTurquoise  	( 0xFF00ced1 );
	const Color Color::DarkViolet  		( 0xFF9400d3 );
	const Color Color::DeepPink  		( 0xFFff1493 );
	const Color Color::DeepSkyBlue 		( 0xFF00bfff );
	const Color Color::DimGray  		( 0xFF696969 );
	const Color Color::DimGrey  		( 0xFF696969 );
	const Color Color::DodgerBlue  		( 0xFF1e90ff );
	const Color Color::FireBrick  		( 0xFFb22222 );
	const Color Color::FloralWhite 		( 0xFFfffaf0 );
	const Color Color::ForestGreen 		( 0xFF228b22 );
	const Color Color::Fuchsia  		( 0xFFff00ff );
	const Color Color::Gainsboro  		( 0xFFdcdcdc );
	const Color Color::GhostWhite  		( 0xFFf8f8ff );
	const Color Color::Gold  			( 0xFFffd700 );
	const Color Color::Goldenrod  		( 0xFFdaa520 );
	const Color Color::Gray  			( 0xFF808080 );
	const Color Color::Grey  			( 0xFF808080 );
	const Color Color::Green  			( 0xFF008000 );
	const Color Color::GreenYellow 		( 0xFFadff2f );
	const Color Color::Honeydew  		( 0xFFf0fff0 );
	const Color Color::HotPink  		( 0xFFff69b4 );
	const Color Color::IndianRed   		( 0xFFcd5c5c );
	const Color Color::Indigo   		( 0xFF4b0082 );
	const Color Color::Ivory  			( 0xFFfffff0 );
	const Color Color::Khaki  			( 0xFFf0e68c );
	const Color Color::Lavender  		( 0xFFe6e6fa );
	const Color Color::LavenderBlush  	( 0xFFfff0f5 );
	const Color Color::LawnGreen  		( 0xFF7cfc00 );
	const Color Color::LemonCiffon		( 0xFFfffacd );
	const Color Color::LightBlue  		( 0xFFadd8e6 );
	const Color Color::LightCoral  		( 0xFFf08080 );
	const Color Color::LightCyan  		( 0xFFe0ffff );
	const Color Color::LightGoldenrodYellow	( 0xFFfAFAD2 );
	const Color Color::LightGray  		( 0xFFd3d3d3 );
	const Color Color::LightGrey  		( 0xFFd3d3d3 );
	const Color Color::LightGreen  		( 0xFF90ee90 );
	const Color Color::LightPink  		( 0xFFffb6c1 );
	const Color Color::LightSalmon  	( 0xFFffa07a );
	const Color Color::LightSeaGreen  	( 0xFF20b2aa );
	const Color Color::LightSkyBlue  	( 0xFF87cefa );
	const Color Color::LightSlateGray 	( 0xFF778899 );
	const Color Color::LightSlateGrey 	( 0xFF778899 );
	const Color Color::LightSteelBlue 	( 0xFFb0c4dE );
	const Color Color::LightYellow  	( 0xFFffffe0 );
	const Color Color::Lime  			( 0xFF00ff00 );
	const Color Color::LimeGreen  		( 0xFF32cd32 );
	const Color Color::Linen  			( 0xFFfaf0e6 );
	const Color Color::Magenta  		( 0xFFff00ff );
	const Color Color::Maroon  			( 0xFF800000 );
	const Color Color::MediumAquamarine ( 0xFF66cdaA );
	const Color Color::MediumBlue  		( 0xFF0000cd );
	const Color Color::MediumOrchid		( 0xFFba55d3 );
	const Color Color::MediumPurple		( 0xFF9370d8 );
	const Color Color::MediumSeaGreen 	( 0xFF3cb371 );
	const Color Color::MediumSlateBlue  ( 0xFF7b68eE );
	const Color Color::MediumSpringGreen( 0xFF00fa9A );
	const Color Color::MediumTurquoise  ( 0xFF48d1cC );
	const Color Color::MediumVioletRed  ( 0xFFc71585 );
	const Color Color::MidnightBlue  	( 0xFF191970 );
	const Color Color::MintCream  		( 0xFFf5fffa );
	const Color Color::MistyRose  		( 0xFFffe4e1 );
	const Color Color::Moccasin  		( 0xFFffe4b5 );
	const Color Color::NavajoWhite  	( 0xFFffdead );
	const Color Color::Navy  			( 0xFF000080 );
	const Color Color::OldLace  		( 0xFFfdf5e6 );
	const Color Color::Olive  			( 0xFF808000 );
	const Color Color::OliveDrab  		( 0xFF6b8e23 );
	const Color Color::Orange  			( 0xFFffa500 );
	const Color Color::OrangeRed  		( 0xFFff4500 );
	const Color Color::Orchid  			( 0xFFda70d6 );
	const Color Color::PaleGoldenrod  	( 0xFFeee8aa );
	const Color Color::PaleGreen  		( 0xFF98fb98 );
	const Color Color::PaleTurquoise  	( 0xFFafeeee );
	const Color Color::PaleVioletRed  	( 0xFFd87093 );
	const Color Color::PapayaWhip  		( 0xFFffefd5 );
	const Color Color::PeachPuff  		( 0xFFffdab9 );
	const Color Color::Peru  			( 0xFFcd853f );
	const Color Color::Pink  			( 0xFFffc0cb );
	const Color Color::Plum  			( 0xFFdda0dd );
	const Color Color::PowderBlue  		( 0xFFb0e0e6 );
	const Color Color::Purple  			( 0xFF800080 );
	const Color Color::Red  			( 0xFFff0000 );
	const Color Color::RosyBrown  		( 0xFFbc8f8f );
	const Color Color::RoyalBlue  		( 0xFF4169e1 );
	const Color Color::SaddleBrown 		( 0xFF8b4513 );
	const Color Color::Salmon  			( 0xFFfa8072 );
	const Color Color::SandyBrown  		( 0xFFf4a460 );
	const Color Color::SeaGreen  		( 0xFF2e8b57 );
	const Color Color::Seashell  		( 0xFFfff5ee );
	const Color Color::Sienna  			( 0xFFa0522d );
	const Color Color::Silver  			( 0xFFc0c0c0 );
	const Color Color::SkyBlue  		( 0xFF87ceeb );
	const Color Color::SlateBlue  		( 0xFF6a5acd );
	const Color Color::SlateGray  		( 0xFF708090 );
	const Color Color::SlateGrey  		( 0xFF708090 );
	const Color Color::Snow  			( 0xFFfffafa );
	const Color Color::SpringGreen 		( 0xFF00ff7f );
	const Color Color::SteelBlue  		( 0xFF4682b4 );
	const Color Color::Tan  			( 0xFFd2b48c );
	const Color Color::Teal  			( 0xFF008080 );
	const Color Color::Thistle  		( 0xFFd8bfd8 );
	const Color Color::Tomato  			( 0xFFff6347 );
	const Color Color::Turquoise  		( 0xFF40e0d0 );
	const Color Color::Violet  			( 0xFFee82ee );
	const Color Color::Wheat  			( 0xFFf5deb3 );
	const Color Color::White  			( 0xFFffffff );
	const Color Color::WhiteSmoke  		( 0xFFf5f5f5 );
	const Color Color::Yellow  			( 0xFFffff00 );
	const Color Color::YellowGreen 		( 0xFF9acd32 );
	
	
	//-------------------------------------------------------------------
	Color Color::operator+( const Color& k ) const
	{
		Color kNewColor;
		kNewColor.r = Util::limitUint8(((int)r) + k.r);
		kNewColor.g = Util::limitUint8(((int)g) + k.g);
		kNewColor.b = Util::limitUint8(((int)b) + k.b);
		kNewColor.a = Util::limitUint8(((int)a) + k.a);
		return kNewColor;
	}
	
	//-------------------------------------------------------------------
	Color Color::operator-( const Color& k ) const
	{
		Color kNewColor;
		kNewColor.r = Util::limitUint8(((int)r) - k.r);
		kNewColor.g = Util::limitUint8(((int)g) - k.g);
		kNewColor.b = Util::limitUint8(((int)b) - k.b);
		kNewColor.a = Util::limitUint8(((int)a) - k.a);
		return kNewColor;
	}
	
	//-------------------------------------------------------------------
	Color Color::operator*( float f ) const
	{
		Color kNewColor;
		kNewColor.r = (uint8_t)( (float)r * f );
		kNewColor.g = (uint8_t)( (float)g * f );
		kNewColor.b = (uint8_t)( (float)b * f );
		kNewColor.a = (uint8_t)( (float)a * f );
		return kNewColor;
	}
	
	//-------------------------------------------------------------------
	Color Color::operator*( const Color& k ) const
	{
		Color kNewColor;
		kNewColor.r = (uint8_t)(((int)r * (int)k.r )/255);
		kNewColor.g = (uint8_t)(((int)g * (int)k.g )/255);
		kNewColor.b = (uint8_t)(((int)b * (int)k.b )/255);
		kNewColor.a = (uint8_t)(((int)a * (int)k.a )/255);
		return kNewColor;
	}
	
	//____ setCMYK ___________________________________________________________________
	
	void Color::setCMYK( float c, float m, float y, float k, uint8_t alpha )
	{
		r = (uint8_t) (255*(1.f-c)*(1.f-k));
		g = (uint8_t) (255*(1.f-m)*(1.f-k));
		b = (uint8_t) (255*(1.f-y)*(1.f-k));
		a = alpha;
	}
	
	
	//____ getCMYK() _______________________________________________________________
	
	void Color::getCMYK( float* _c, float* _m, float* _y, float* _k )
	{
		float fr = r / 255.f;
		float fg = r / 255.f;
		float fb = r / 255.f;
	
		float k = 1.f - wg::max( fr, fg, fb );
		* _c = (1.f-fr-k) / (1.f-k);
		* _m = (1.f-fg-k) / (1.f-k);
		* _y = (1.f-fb-k) / (1.f-k);
		* _k = k;
	}
	
	//____ blend() ________________________________________________________________
	
	Color Color::mix( Color color1, Color color2, uint8_t balance )
	{
		Color col;
	
		col.r = color1.r + (uint8_t) (((int)color2.r) - ((int)color1.r)*balance)/255;
		col.g = color1.g + (uint8_t) (((int)color2.g) - ((int)color1.g)*balance)/255;
		col.b = color1.b + (uint8_t) (((int)color2.b) - ((int)color1.b)*balance)/255;
		col.a = color1.a + (uint8_t) (((int)color2.a) - ((int)color1.a)*balance)/255;
	
		return col;
	}
	
	//____ invert()_________________________________________________________________
	
	Color Color::invert( Color color, uint8_t grade )
	{		
		color.r = ((255-color.r)*grade + color.r*(255-grade))/255;
		color.g = ((255-color.g)*grade + color.g*(255-grade))/255;
		color.b = ((255-color.b)*grade + color.b*(255-grade))/255;
		return color;
	}
	
	//____ blend()___________________________________________________________________
	
	Color Color::blend( Color baseColor, Color blendColor, BlendMode operation )
	{
		switch( operation )
		{
			case BlendMode::Add:
				return baseColor + blendColor;
			case BlendMode::Blend:
				return mix(baseColor, Color(blendColor.r, blendColor.g, blendColor.b, baseColor.a), blendColor.a);
			case BlendMode::Undefined:
			case BlendMode::Ignore:
				return baseColor;
			case BlendMode::Invert:
				return invert( baseColor, blendColor.a );
			case BlendMode::Multiply:
				return baseColor * blendColor;
			case BlendMode::Subtract:
				return baseColor - blendColor;
			case BlendMode::Replace:
			default:
				return blendColor;
		}		
	}
	
	
	

} // namespace wg
