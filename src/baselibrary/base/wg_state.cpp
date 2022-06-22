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
	const int State::s_stateToIndexTable[MaxValue + 1] = {	0, 1, 0, 0, 2, 3, 4, 5, 6, 7, 0, 0, 8, 9, 10, 11,
															0, 0, 0, 0, 12, 13, 0, 0, 0, 0, 0, 0, 14, 15, 0, 0,
															16, 0, 0, 0, 0, 0, 0, 0, 17 };

	const uint8_t State::s_indexToStateTable[IndexAmount] = { 0, 1, 4, 4 + 1, 4 + 2, 4 + 2 + 1, 8, 8 + 1, 8 + 4, 8 + 4 + 1, 8 + 4 + 2,
															  8 + 4 + 2 + 1, 16 + 4, 16 + 4 + 1, 16 + 8 + 4, 16 + 8 + 4 + 1, 32, 32 + 8 };


	const State State::Normal					= 0;
	const State State::Focused					= 1;
	const State State::Hovered					= 2;
	const State State::HoveredFocused			= 3;
	const State State::Pressed					= 4;
	const State State::PressedFocused			= 5;
	const State State::Selected					= 6;
	const State State::SelectedFocused			= 7;
	const State State::SelectedHovered			= 8;
	const State State::SelectedHoveredFocused	= 9;
	const State State::SelectedPressed			= 10;
	const State State::SelectedPressedFocused	= 11;
	const State State::Targeted					= 12;
	const State State::TargetedFocused			= 13;
	const State State::TargetedSelected			= 14;
	const State State::TargetedSelectedFocused	= 15;
	const State State::Disabled					= 16;
	const State State::DisabledSelected			= 17;

	
}
