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

namespace wg 
{
	
	const Color Color::transparent		( 0x00FFFFFF );
	const Color Color::none				( 0x00000000 );
	
	const Color Color::aliceblue		( 0xFFF0F8FF );
	const Color Color::antiquewhite		( 0xFFFAEBD7 );
	const Color Color::aqua				( 0xFF00ffff );
	const Color Color::aquamarine		( 0xFF7fffd4 );
	const Color Color::azure			( 0xFFf0ffff );
	const Color Color::beige			( 0xFFf5f5dc );
	const Color Color::bisque  			( 0xFFffe4c4 );
	const Color Color::black	  		( 0xFF000000 );
	const Color Color::blanchedalmond 	( 0xFFffebcD );
	const Color Color::blue 	  		( 0xFF0000ff );
	const Color Color::blueviolet 		( 0xFF8a2be2 );
	const Color Color::brown   			( 0xFFa52a2a );
	const Color Color::burlywood  		( 0xFFdeb887 );
	const Color Color::cadetblue  		( 0xFF5f9ea0 );
	const Color Color::chartreuse 		( 0xFF7fff00 );
	const Color Color::chocolate  		( 0xFFd2691e );
	const Color Color::coral   			( 0xFFff7f50 );
	const Color Color::cornflowerblue 	( 0xFF6495eD );
	const Color Color::cornsilk 		( 0xFFfff8dc );
	const Color Color::crimson  		( 0xFFdc143c );
	const Color Color::cyan  			( 0xFF00ffff );
	const Color Color::darkblue  		( 0xFF00008b );
	const Color Color::darkcyan  		( 0xFF008b8b );
	const Color Color::darkgoldenrod  	( 0xFFb8860b );
	const Color Color::darkgray  		( 0xFFa9a9a9 );
	const Color Color::darkgrey  		( 0xFFa9a9a9 );
	const Color Color::darkgreen  		( 0xFF006400 );
	const Color Color::darkkhaki  		( 0xFFbdb76b );
	const Color Color::darkmagenta  	( 0xFF8b008b );
	const Color Color::darkolivegreen 	( 0xFF556b2F );
	const Color Color::darkorange  		( 0xFFff8c00 );
	const Color Color::darkorchid  		( 0xFF9932cc );
	const Color Color::darkred  		( 0xFF8b0000 );
	const Color Color::darksalmon  		( 0xFFe9967a );
	const Color Color::darkseagreen  	( 0xFF8fbc8f );
	const Color Color::darkslateblue  	( 0xFF483d8b );
	const Color Color::darkslategray  	( 0xFF2f4f4f );
	const Color Color::darkslategrey  	( 0xFF2f4f4f );
	const Color Color::darkturquoise  	( 0xFF00ced1 );
	const Color Color::darkviolet  		( 0xFF9400d3 );
	const Color Color::deeppink  		( 0xFFff1493 );
	const Color Color::deepskyblue 		( 0xFF00bfff );
	const Color Color::dimgray  		( 0xFF696969 );
	const Color Color::dimgrey  		( 0xFF696969 );
	const Color Color::dodgerblue  		( 0xFF1e90ff );
	const Color Color::firebrick  		( 0xFFb22222 );
	const Color Color::floralwhite 		( 0xFFfffaf0 );
	const Color Color::forestgreen 		( 0xFF228b22 );
	const Color Color::fuchsia  		( 0xFFff00ff );
	const Color Color::gainsboro  		( 0xFFdcdcdc );
	const Color Color::ghostwhite  		( 0xFFf8f8ff );
	const Color Color::gold  			( 0xFFffd700 );
	const Color Color::goldenrod  		( 0xFFdaa520 );
	const Color Color::gray  			( 0xFF808080 );
	const Color Color::grey  			( 0xFF808080 );
	const Color Color::green  			( 0xFF008000 );
	const Color Color::greenyellow 		( 0xFFadff2f );
	const Color Color::honeydew  		( 0xFFf0fff0 );
	const Color Color::hotpink  		( 0xFFff69b4 );
	const Color Color::indianred   		( 0xFFcd5c5c );
	const Color Color::indigo   		( 0xFF4b0082 );
	const Color Color::ivory  			( 0xFFfffff0 );
	const Color Color::khaki  			( 0xFFf0e68c );
	const Color Color::lavender  		( 0xFFe6e6fa );
	const Color Color::lavenderblush  	( 0xFFfff0f5 );
	const Color Color::lawngreen  		( 0xFF7cfc00 );
	const Color Color::lemonchiffon		( 0xFFfffacd );
	const Color Color::lightblue  		( 0xFFadd8e6 );
	const Color Color::lightcoral  		( 0xFFf08080 );
	const Color Color::lightcyan  		( 0xFFe0ffff );
	const Color Color::lightgoldenrodyellow	( 0xFFfAFAD2 );
	const Color Color::lightgray  		( 0xFFd3d3d3 );
	const Color Color::lightgrey  		( 0xFFd3d3d3 );
	const Color Color::lightgreen  		( 0xFF90ee90 );
	const Color Color::lightpink  		( 0xFFffb6c1 );
	const Color Color::lightsalmon  	( 0xFFffa07a );
	const Color Color::lightseagreen  	( 0xFF20b2aa );
	const Color Color::lightskyblue  	( 0xFF87cefa );
	const Color Color::lightslategray 	( 0xFF778899 );
	const Color Color::lightslategrey 	( 0xFF778899 );
	const Color Color::lightsteelblue 	( 0xFFb0c4dE );
	const Color Color::lightyellow  	( 0xFFffffe0 );
	const Color Color::lime  			( 0xFF00ff00 );
	const Color Color::limegreen  		( 0xFF32cd32 );
	const Color Color::linen  			( 0xFFfaf0e6 );
	const Color Color::magenta  		( 0xFFff00ff );
	const Color Color::maroon  			( 0xFF800000 );
	const Color Color::mediumaquamarine ( 0xFF66cdaA );
	const Color Color::mediumblue  		( 0xFF0000cd );
	const Color Color::mediumorchid		( 0xFFba55d3 );
	const Color Color::mediumpurple		( 0xFF9370d8 );
	const Color Color::mediumseagreen 	( 0xFF3cb371 );
	const Color Color::mediumslateblue  ( 0xFF7b68eE );
	const Color Color::mediumspringgreen( 0xFF00fa9A );
	const Color Color::mediumturquoise  ( 0xFF48d1cC );
	const Color Color::mediumvioletred  ( 0xFFc71585 );
	const Color Color::midnightblue  	( 0xFF191970 );
	const Color Color::mintcream  		( 0xFFf5fffa );
	const Color Color::mistyrose  		( 0xFFffe4e1 );
	const Color Color::moccasin  		( 0xFFffe4b5 );
	const Color Color::navajowhite  	( 0xFFffdead );
	const Color Color::navy  			( 0xFF000080 );
	const Color Color::oldlace  		( 0xFFfdf5e6 );
	const Color Color::olive  			( 0xFF808000 );
	const Color Color::olivedrab  		( 0xFF6b8e23 );
	const Color Color::orange  			( 0xFFffa500 );
	const Color Color::orangered  		( 0xFFff4500 );
	const Color Color::orchid  			( 0xFFda70d6 );
	const Color Color::palegoldenrod  	( 0xFFeee8aa );
	const Color Color::palegreen  		( 0xFF98fb98 );
	const Color Color::paleturquoise  	( 0xFFafeeee );
	const Color Color::palevioletred  	( 0xFFd87093 );
	const Color Color::papayawhip  		( 0xFFffefd5 );
	const Color Color::peachpuff  		( 0xFFffdab9 );
	const Color Color::peru  			( 0xFFcd853f );
	const Color Color::pink  			( 0xFFffc0cb );
	const Color Color::plum  			( 0xFFdda0dd );
	const Color Color::powderblue  		( 0xFFb0e0e6 );
	const Color Color::purple  			( 0xFF800080 );
	const Color Color::red  			( 0xFFff0000 );
	const Color Color::rosybrown  		( 0xFFbc8f8f );
	const Color Color::royalblue  		( 0xFF4169e1 );
	const Color Color::saddlebrown 		( 0xFF8b4513 );
	const Color Color::salmon  			( 0xFFfa8072 );
	const Color Color::sandybrown  		( 0xFFf4a460 );
	const Color Color::seagreen  		( 0xFF2e8b57 );
	const Color Color::seashell  		( 0xFFfff5ee );
	const Color Color::sienna  			( 0xFFa0522d );
	const Color Color::silver  			( 0xFFc0c0c0 );
	const Color Color::skyblue  		( 0xFF87ceeb );
	const Color Color::slateblue  		( 0xFF6a5acd );
	const Color Color::slategray  		( 0xFF708090 );
	const Color Color::slategrey  		( 0xFF708090 );
	const Color Color::snow  			( 0xFFfffafa );
	const Color Color::springgreen 		( 0xFF00ff7f );
	const Color Color::steelblue  		( 0xFF4682b4 );
	const Color Color::tan  			( 0xFFd2b48c );
	const Color Color::teal  			( 0xFF008080 );
	const Color Color::thistle  		( 0xFFd8bfd8 );
	const Color Color::tomato  			( 0xFFff6347 );
	const Color Color::turquoise  		( 0xFF40e0d0 );
	const Color Color::violet  			( 0xFFee82ee );
	const Color Color::wheat  			( 0xFFf5deb3 );
	const Color Color::white  			( 0xFFffffff );
	const Color Color::whitesmoke  		( 0xFFf5f5f5 );
	const Color Color::yellow  			( 0xFFffff00 );
	const Color Color::yellowgreen 		( 0xFF9acd32 );
	
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
	
	Color Color::blend( const Color& start, const Color& dest, float grade )
	{
		Color col;
	
		col.r = start.r + (uint8_t) (((int)dest.r) - ((int)start.r)*grade);
		col.g = start.g + (uint8_t) (((int)dest.g) - ((int)start.g)*grade);
		col.b = start.b + (uint8_t) (((int)dest.b) - ((int)start.b)*grade);
		col.a = start.a + (uint8_t) (((int)dest.a) - ((int)start.a)*grade);
	
		return col;
	}
	
	

} // namespace wg
