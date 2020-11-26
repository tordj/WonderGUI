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


#include <wg_gradient.h>

namespace wg
{
	//____ constructor ________________________________________________________

	Gradient::Gradient(Origo start, HiColor startColor, HiColor endColor)
	{
		switch (start)
		{
		case Origo::Center:
			topLeft = topRight = bottomRight = bottomLeft = startColor;
			break;

		case Origo::NorthWest:
			topLeft = startColor;
			bottomRight = endColor;
			topRight = bottomLeft = HiColor::mix(startColor, endColor, 2048);
			break;

		case Origo::North:
			topLeft = topRight = startColor;
			bottomLeft = bottomRight = endColor;
			break;

		case Origo::NorthEast:
			topRight = startColor;
			bottomLeft = endColor;
			topLeft = bottomRight = HiColor::mix(startColor, endColor, 2048);
			break;

		case Origo::East:
			topRight = bottomRight = startColor;
			topLeft = bottomLeft = endColor;
			break;

		case Origo::SouthEast:
			bottomRight = startColor;
			topLeft = endColor;
			bottomLeft = topRight = HiColor::mix(startColor, endColor, 2048);
			break;

		case Origo::South:
			bottomLeft = bottomRight = startColor;
			topLeft = topRight = endColor;
			break;

		case Origo::SouthWest:
			bottomLeft = startColor;
			topRight = endColor;
			bottomRight = topLeft = HiColor::mix(startColor, endColor, 2048);
			break;

		case Origo::West:
			topLeft = bottomLeft = startColor;
			topRight = bottomRight = endColor;
			break;
		}

	}


}



