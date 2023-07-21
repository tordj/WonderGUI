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


	const State State::Normal					= State(0);
	const State State::Focused					= State(1);
	const State State::Hovered					= State(2);
	const State State::HoveredFocused			= State(3);
	const State State::Pressed					= State(4);
	const State State::PressedFocused			= State(5);
	const State State::Selected					= State(6);
	const State State::SelectedFocused			= State(7);
	const State State::SelectedHovered			= State(8);
	const State State::SelectedHoveredFocused	= State(9);
	const State State::SelectedPressed			= State(10);
	const State State::SelectedPressedFocused	= State(11);
	const State State::Targeted					= State(12);
	const State State::TargetedFocused			= State(13);
	const State State::TargetedSelected			= State(14);
	const State State::TargetedSelectedFocused	= State(15);
	const State State::Disabled					= State(16);
	const State State::DisabledSelected			= State(17);

	
}
