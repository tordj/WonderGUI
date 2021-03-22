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

#include <wg_state.h>

namespace wg
{
	
	
	const uint8_t State::s_comboFilter[64] = { 	0,				// Normal 
											1, 				// Focused
											4+2, 			// Pressed -> Hovered + Pressed
											4+2+1,			// Pressed + Focused -> Hovered + Pressed + Focused
											4,				// Hovered
											4+1,			// Hovered + Focused
											4+2,			// Hovered + Pressed
											4+2+1,			// Hovered + Pressed + Focused
											8,				// Selected
											8+1,			// Selected + Focused
											8+4+2,			// Selected + Pressed -> Selected + Hovered + Pressed
											8+4+2+1,		// Selected + Pressed + Focused -> Selected + Hovered + Pressed + Focused
											8+4,			// Selected + Hovered
											8+4+1,			// Selected + Hovered + Focused
											8+4+2,			// Selected + Hovered + Pressed
											8+4+2+1,		// Selected + Hovered + Pressed + Focused
											16+4,			// Targeted -> Targeted + Hovered
											16+4+1,			// Targeted + Focused -> Targeted + Hovered + Focused
											16+4,			// Targeted + Pressed -> Targeted + Hovered
											16+4+1,			// Targeted + Pressed + Focused -> Targeted + Hovered + Focused
											16+4,			// Targeted + Hovered
											16+4+1,			// Targeted + Hovered + Focused
											16+4,			// Targeted + Hovered + Pressed -> Targeted + Hovered
											16+4+1,			// Targeted + Hovered + Pressed + Focused -> Targeted + Hovered + Focused
											
											
	
}