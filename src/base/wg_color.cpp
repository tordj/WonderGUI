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
#include <wg_base.h>
#include <wg_context.h>

#include <algorithm>

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

	const HiColor HiColor::Transparent(0, 0, 0, 0);
	const HiColor HiColor::AliceBlue(4096, 3585, 3853, 4096);
	const HiColor HiColor::AntiqueWhite(4096, 3921, 3422, 2814);
	const HiColor HiColor::Aqua(4096, 0, 4096, 4096);
	const HiColor HiColor::Aquamarine(4096, 884, 4096, 2728);
	const HiColor HiColor::Azure(4096, 3585, 4096, 4096);
	const HiColor HiColor::Beige(4096, 3751, 3751, 2960);
	const HiColor HiColor::Bisque(4096, 4096, 3202, 2296);
	const HiColor HiColor::Black(4096, 0, 0, 0);
	const HiColor HiColor::BlanchedAlmond(4096, 4096, 3422, 2534);
	const HiColor HiColor::Blue(4096, 0, 0, 4096);
	const HiColor HiColor::BlueViolet(4096, 1061, 82, 3141);
	const HiColor HiColor::Brown(4096, 1572, 77, 77);
	const HiColor HiColor::Burlywood(4096, 3020, 1998, 1011);
	const HiColor HiColor::CadetBlue(4096, 467, 1429, 1469);
	const HiColor HiColor::Chartreuse(4096, 884, 4096, 0);
	const HiColor HiColor::Chocolate(4096, 2672, 582, 37);
	const HiColor HiColor::Coral(4096, 4096, 884, 320);
	const HiColor HiColor::CornflowerBlue(4096, 522, 1256, 3487);
	const HiColor HiColor::Cornsilk(4096, 4096, 3853, 2960);
	const HiColor HiColor::Crimson(4096, 2960, 15, 170);
	const HiColor HiColor::Cyan(4096, 0, 4096, 4096);
	const HiColor HiColor::DarkBlue(4096, 0, 0, 1078);
	const HiColor HiColor::DarkCyan(4096, 0, 1078, 1078);
	const HiColor HiColor::DarkGoldenrod(4096, 1998, 994, 4);
	const HiColor HiColor::DarkGray(4096, 1657, 1657, 1657);
	const HiColor HiColor::DarkGrey(4096, 1657, 1657, 1657);
	const HiColor HiColor::DarkGreen(4096, 0, 522, 0);
	const HiColor HiColor::DarkKhaki(4096, 2119, 1974, 606);
	const HiColor HiColor::DarkMagenta(4096, 1078, 0, 1078);
	const HiColor HiColor::DarkOliveGreen(4096, 365, 606, 99);
	const HiColor HiColor::DarkOrange(4096, 4096, 1095, 0);
	const HiColor HiColor::DarkOrchid(4096, 1331, 114, 2507);
	const HiColor HiColor::DarkRed(4096, 1078, 0, 0);
	const HiColor HiColor::DarkSalmon(4096, 3359, 1275, 809);
	const HiColor HiColor::DarkSeaGreen(4096, 1147, 2095, 1147);
	const HiColor HiColor::DarkSlateBlue(4096, 254, 176, 1078);
	const HiColor HiColor::DarkSlateGray(4096, 99, 311, 311);
	const HiColor HiColor::DarkSlateGrey(4096, 99, 311, 311);
	const HiColor HiColor::DarkTurquoise(4096, 0, 2561, 2644);
	const HiColor HiColor::DarkViolet(4096, 1238, 0, 2700);
	const HiColor HiColor::DeepPink(4096, 4096, 15, 1219);
	const HiColor HiColor::DeepSkyBlue(4096, 0, 2169, 4096);
	const HiColor HiColor::DimGray(4096, 582, 582, 582);
	const HiColor HiColor::DimGrey(4096, 582, 582, 582);
	const HiColor HiColor::DodgerBlue(4096, 37, 1165, 4096);
	const HiColor HiColor::FireBrick(4096, 1857, 49, 49);
	const HiColor HiColor::FloralWhite(4096, 4096, 3921, 3585);
	const HiColor HiColor::ForestGreen(4096, 49, 1078, 49);
	const HiColor HiColor::Fuchsia(4096, 4096, 0, 4096);
	const HiColor HiColor::Gainsboro(4096, 2960, 2960, 2960);
	const HiColor HiColor::GhostWhite(4096, 3853, 3853, 4096);
	const HiColor HiColor::Gold(4096, 4096, 2814, 0);
	const HiColor HiColor::Goldenrod(4096, 2901, 1572, 43);
	const HiColor HiColor::Gray(4096, 899, 899, 899);
	const HiColor HiColor::Grey(4096, 899, 899, 899);
	const HiColor HiColor::Green(4096, 0, 899, 0);
	const HiColor HiColor::GreenYellow(4096, 1745, 4096, 99);
	const HiColor HiColor::Honeydew(4096, 3585, 4096, 3585);
	const HiColor HiColor::HotPink(4096, 4096, 582, 1904);
	const HiColor HiColor::IndianRed(4096, 2534, 435, 435);
	const HiColor HiColor::Indigo(4096, 277, 0, 930);
	const HiColor HiColor::Ivory(4096, 4096, 4096, 3585);
	const HiColor HiColor::Khaki(4096, 3585, 3264, 1095);
	const HiColor HiColor::Lavender(4096, 3264, 3264, 3921);
	const HiColor HiColor::LavenderBlush(4096, 4096, 3585, 3751);
	const HiColor HiColor::LawnGreen(4096, 838, 3991, 0);
	const HiColor HiColor::LemonCiffon(4096, 4096, 3921, 2534);
	const HiColor HiColor::LightBlue(4096, 1745, 2843, 3264);
	const HiColor HiColor::LightCoral(4096, 3585, 899, 899);
	const HiColor HiColor::LightCyan(4096, 3080, 4096, 4096);
	const HiColor HiColor::LightGoldenrodYellow(4096, 3921, 3921, 2672);
	const HiColor HiColor::LightGray(4096, 2700, 2700, 2700);
	const HiColor HiColor::LightGrey(4096, 2700, 2700, 2700);
	const HiColor HiColor::LightGreen(4096, 1165, 3519, 1165);
	const HiColor HiColor::LightPink(4096, 4096, 1950, 2219);
	const HiColor HiColor::LightSalmon(4096, 4096, 1469, 809);
	const HiColor HiColor::LightSeaGreen(4096, 43, 1857, 1679);
	const HiColor HiColor::LightSkyBlue(4096, 1011, 2561, 3921);
	const HiColor HiColor::LightSlateGray(4096, 766, 1027, 1331);
	const HiColor HiColor::LightSlateGrey(4096, 766, 1027, 1331);
	const HiColor HiColor::LightSteelBlue(4096, 1812, 2296, 3020);
	const HiColor HiColor::LightYellow(4096, 4096, 4096, 3080);
	const HiColor HiColor::Lime(4096, 0, 4096, 0);
	const HiColor HiColor::LimeGreen(4096, 114, 2534, 114);
	const HiColor HiColor::Linen(4096, 3921, 3585, 3264);
	const HiColor HiColor::Magenta(4096, 4096, 0, 4096);
	const HiColor HiColor::Maroon(4096, 899, 0, 0);
	const HiColor HiColor::MediumAquamarine(4096, 546, 2534, 1679);
	const HiColor HiColor::MediumBlue(4096, 0, 0, 2534);
	const HiColor HiColor::MediumOrchid(4096, 2046, 365, 2700);
	const HiColor HiColor::MediumPurple(4096, 1219, 670, 2843);
	const HiColor HiColor::MediumSeaGreen(4096, 170, 1880, 684);
	const HiColor HiColor::MediumSlateBlue(4096, 824, 569, 3519);
	const HiColor HiColor::MediumSpringGreen(4096, 0, 3921, 1351);
	const HiColor HiColor::MediumTurquoise(4096, 254, 2644, 2507);
	const HiColor HiColor::MediumVioletRed(4096, 2374, 17, 978);
	const HiColor HiColor::MidnightBlue(4096, 25, 25, 670);
	const HiColor HiColor::MintCream(4096, 3751, 4096, 3921);
	const HiColor HiColor::MistyRose(4096, 4096, 3202, 3110);
	const HiColor HiColor::Moccasin(4096, 4096, 3202, 1927);
	const HiColor HiColor::NavajoWhite(4096, 4096, 3020, 1745);
	const HiColor HiColor::Navy(4096, 0, 0, 899);
	const HiColor HiColor::OldLace(4096, 4026, 3751, 3264);
	const HiColor HiColor::Olive(4096, 899, 899, 0);
	const HiColor HiColor::OliveDrab(4096, 606, 1130, 52);
	const HiColor HiColor::Orange(4096, 4096, 1572, 0);
	const HiColor HiColor::OrangeRed(4096, 4096, 231, 0);
	const HiColor HiColor::Orchid(4096, 2901, 670, 2785);
	const HiColor HiColor::PaleGoldenrod(4096, 3519, 3327, 1679);
	const HiColor HiColor::PaleGreen(4096, 1312, 3956, 1312);
	const HiColor HiColor::PaleTurquoise(4096, 1789, 3519, 3519);
	const HiColor HiColor::PaleVioletRed(4096, 2843, 670, 1219);
	const HiColor HiColor::PapayaWhip(4096, 4096, 3552, 2757);
	const HiColor HiColor::PeachPuff(4096, 4096, 2901, 2022);
	const HiColor HiColor::Peru(4096, 2534, 978, 189);
	const HiColor HiColor::Pink(4096, 4096, 2194, 2480);
	const HiColor HiColor::Plum(4096, 2990, 1469, 2990);
	const HiColor HiColor::PowderBlue(4096, 1812, 3080, 3264);
	const HiColor HiColor::Purple(4096, 899, 0, 899);
	const HiColor HiColor::Red(4096, 4096, 0, 0);
	const HiColor HiColor::RosyBrown(4096, 2095, 1147, 1147);
	const HiColor HiColor::RoyalBlue(4096, 202, 582, 3110);
	const HiColor HiColor::SaddleBrown(4096, 1078, 231, 14);
	const HiColor HiColor::Salmon(4096, 3921, 899, 697);
	const HiColor HiColor::SandyBrown(4096, 3717, 1551, 477);
	const HiColor HiColor::SeaGreen(4096, 95, 1078, 385);
	const HiColor HiColor::Seashell(4096, 4096, 3751, 3519);
	const HiColor HiColor::Sienna(4096, 1469, 338, 90);
	const HiColor HiColor::Silver(4096, 2194, 2194, 2194);
	const HiColor HiColor::SkyBlue(4096, 1011, 2561, 3422);
	const HiColor HiColor::SlateBlue(4096, 594, 414, 2534);
	const HiColor HiColor::SlateGray(4096, 670, 899, 1165);
	const HiColor HiColor::SlateGrey(4096, 670, 899, 1165);
	const HiColor HiColor::Snow(4096, 4096, 3921, 3921);
	const HiColor HiColor::SpringGreen(4096, 0, 4096, 884);
	const HiColor HiColor::SteelBlue(4096, 238, 930, 1904);
	const HiColor HiColor::Tan(4096, 2672, 1904, 1095);
	const HiColor HiColor::Teal(4096, 0, 899, 899);
	const HiColor HiColor::Thistle(4096, 2843, 2169, 2843);
	const HiColor HiColor::Tomato(4096, 4096, 511, 246);
	const HiColor HiColor::Turquoise(4096, 196, 3080, 2616);
	const HiColor HiColor::Violet(4096, 3519, 930, 3519);
	const HiColor HiColor::Wheat(4096, 3751, 3020, 1880);
	const HiColor HiColor::White(4096, 4096, 4096, 4096);
	const HiColor HiColor::WhiteSmoke(4096, 3751, 3751, 3751);
	const HiColor HiColor::Yellow(4096, 4096, 4096, 0);
	const HiColor HiColor::YellowGreen(4096, 1351, 2534, 114);



	int16_t		HiColor::unpackSRGBTab[256];
	int16_t		HiColor::unpackLinearTab[256];

	uint8_t		HiColor::packSRGBTab[4097];
	uint8_t		HiColor::packLinearTab[4097];


	//-------------------------------------------------------------------
	Color Color::operator+(const Color& k) const
	{
		Color kNewColor;
		kNewColor.r = Util::limitUint8(((int)r) + k.r);
		kNewColor.g = Util::limitUint8(((int)g) + k.g);
		kNewColor.b = Util::limitUint8(((int)b) + k.b);
		kNewColor.a = Util::limitUint8(((int)a) + k.a);
		return kNewColor;
	}

	//-------------------------------------------------------------------
	Color Color::operator-(const Color& k) const
	{
		Color kNewColor;
		kNewColor.r = Util::limitUint8(((int)r) - k.r);
		kNewColor.g = Util::limitUint8(((int)g) - k.g);
		kNewColor.b = Util::limitUint8(((int)b) - k.b);
		kNewColor.a = Util::limitUint8(((int)a) - k.a);
		return kNewColor;
	}

	//-------------------------------------------------------------------
	Color Color::operator*(float f) const
	{
		Color kNewColor;
		kNewColor.r = (uint8_t)((float)r * f);
		kNewColor.g = (uint8_t)((float)g * f);
		kNewColor.b = (uint8_t)((float)b * f);
		kNewColor.a = (uint8_t)((float)a * f);
		return kNewColor;
	}

	//-------------------------------------------------------------------
	Color Color::operator*(const Color& k) const
	{
		Color kNewColor;
		kNewColor.r = (uint8_t)(((int)r * (int)k.r) / 255);
		kNewColor.g = (uint8_t)(((int)g * (int)k.g) / 255);
		kNewColor.b = (uint8_t)(((int)b * (int)k.b) / 255);
		kNewColor.a = (uint8_t)(((int)a * (int)k.a) / 255);
		return kNewColor;
	}

/*
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
*/

	//____ constructor ________________________________________________________

	HiColor::HiColor(Color lowColor)
	{
		const int16_t* pUnpackTab = Base::activeContext()->gammaCorrection() ? unpackSRGBTab : unpackLinearTab;

		r = pUnpackTab[lowColor.r];
		g = pUnpackTab[lowColor.g];
		b = pUnpackTab[lowColor.b];
		a = unpackLinearTab[lowColor.a];
	}


	//____ blend() ________________________________________________________________

	HiColor HiColor::mix( HiColor color1, HiColor color2, int balance )
	{
		HiColor col;

		col.r = color1.r + ((int(color2.r) - int(color1.r)) * balance / 4096);
		col.g = color1.g + ((int(color2.g) - int(color1.g)) * balance / 4096);
		col.b = color1.b + ((int(color2.b) - int(color1.b)) * balance / 4096);
		col.a = color1.a + ((int(color2.a) - int(color1.a)) * balance / 4096);

		return col;
	}

	//____ invert()_________________________________________________________________

	HiColor HiColor::invert( HiColor color, int grade )
	{
		color.r = ((4096-color.r)*grade + color.r*(4096-grade))/4096;
		color.g = ((4096-color.g)*grade + color.g*(4096-grade))/4096;
		color.b = ((4096-color.b)*grade + color.b*(4096-grade))/4096;
		return color;
	}

	//____ min() ___________________________________________________________________

	HiColor HiColor::min(HiColor color1, HiColor color2)
	{
		return HiColor(std::min(color1.r, color2.r),std::min(color1.g,color2.g),std::min(color1.b,color2.b),std::min(color1.a,color2.a));
	}

	//____ max() ____________________________________________________________________

	HiColor HiColor::max(HiColor color1, HiColor color2)
	{
		return HiColor(std::max(color1.r, color2.r), std::max(color1.g, color2.g), std::max(color1.b, color2.b), std::max(color1.a, color2.a));
	}


	//____ blend()___________________________________________________________________

	HiColor HiColor::blend( HiColor baseColor, HiColor blendColor, BlendMode operation )
	{
		switch( operation )
		{
			case BlendMode::Add:
				return baseColor + blendColor;
			case BlendMode::Morph:
				return mix(baseColor, blendColor, 2048);
			case BlendMode::Blend:
				return mix(baseColor, HiColor(blendColor.r, blendColor.g, blendColor.b, baseColor.a), blendColor.a);
			case BlendMode::Undefined:
			case BlendMode::Ignore:
				return baseColor;
			case BlendMode::Invert:
				return invert( baseColor, blendColor.a );
			case BlendMode::Multiply:
				return baseColor * blendColor;
			case BlendMode::Subtract:
				return baseColor - blendColor;
			case BlendMode::Min:
				return min(baseColor, blendColor);
			case BlendMode::Max:
				return max(baseColor, blendColor);
			case BlendMode::Replace:
			default:
				return blendColor;
		}
	}

	//____ operator Colour() ___________________________________________________

	HiColor::operator Color() const
	{
		Color c;
		const uint8_t* pPackTab = Base::activeContext()->gammaCorrection() ? packSRGBTab : packLinearTab;
		return Color(pPackTab[r], pPackTab[g], pPackTab[b], packLinearTab[a]);
	}

	//-------------------------------------------------------------------
	HiColor HiColor::operator+(const HiColor& k) const
	{
		HiColor kNewColor;
		kNewColor.r = r + k.r;
		kNewColor.g = g + k.g;
		kNewColor.b = b + k.b;
		kNewColor.a = a + k.a;

		if (kNewColor.r > 4096) kNewColor.r = 4096;
		if (kNewColor.g > 4096) kNewColor.g = 4096;
		if (kNewColor.b > 4096) kNewColor.b = 4096;
		if (kNewColor.a > 4096) kNewColor.a = 4096;
		return kNewColor;
	}

	//-------------------------------------------------------------------
	HiColor HiColor::operator-(const HiColor& k) const
	{
		HiColor kNewColor;
		kNewColor.r = r - k.r;
		kNewColor.g = g - k.g;
		kNewColor.b = b - k.b;
		kNewColor.a = a - k.a;

		if (kNewColor.r < 4096) kNewColor.r = 0;
		if (kNewColor.g < 4096) kNewColor.g = 0;
		if (kNewColor.b < 4096) kNewColor.b = 0;
		if (kNewColor.a < 4096) kNewColor.a = 0;
		return kNewColor;
	}

	//-------------------------------------------------------------------
	HiColor HiColor::operator*(float f) const
	{
		HiColor kNewColor;
		kNewColor.r = (uint16_t)((float)r * f);
		kNewColor.g = (uint16_t)((float)g * f);
		kNewColor.b = (uint16_t)((float)b * f);
		kNewColor.a = (uint16_t)((float)a * f);

		if (kNewColor.r > 4096) kNewColor.r = 4096;
		if (kNewColor.g > 4096) kNewColor.g = 4096;
		if (kNewColor.b > 4096) kNewColor.b = 4096;
		if (kNewColor.a > 4096) kNewColor.a = 4096;

		return kNewColor;
	}

	//-------------------------------------------------------------------
	HiColor HiColor::operator*(const HiColor& k) const
	{
		HiColor kNewColor;
		kNewColor.r = (uint16_t)(((int)r * (int)k.r) / 4096);
		kNewColor.g = (uint16_t)(((int)g * (int)k.g) / 4096);
		kNewColor.b = (uint16_t)(((int)b * (int)k.b) / 4096);
		kNewColor.a = (uint16_t)(((int)a * (int)k.a) / 4096);
		return kNewColor;
	}

	//____ _initTables() ______________________________________________________

	void HiColor::_initTables()
	{
		float max = powf(255, 2.2f);

		for (int i = 0; i < 256; i++)
			HiColor::unpackSRGBTab[i] = int((powf(float(i), 2.2f) / max) * 4096 + 0.5f);

		for (int i = 0; i < 256; i++)
			HiColor::unpackLinearTab[i] = int(i / 255.f * 4096 + 0.5f);

		for (int i = 0; i <= 4096; i++)
			HiColor::packSRGBTab[i] = uint8_t(powf(i * max / 4096, 1 / 2.2f) + 0.5f);

		for (int i = 0; i <= 4096; i++)
			HiColor::packLinearTab[i] = uint8_t(i / 4096.f * 255.f + 0.5f);
	}


} // namespace wg
