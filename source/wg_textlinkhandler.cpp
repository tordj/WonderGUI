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

#include <wg_textlinkhandler.h>
#include <wg_input.h>
#include <wg_signals.h>

using namespace WgSignal;


//____ OnAction() _____________________________________________________________

void WgTextLinkHandler::OnAction( WgInput::UserAction action, int button_key )
{
	switch( action )
	{
		case WgInput::POINTER_ENTER:
			Emit( PointerEnter() );
			break;
		case WgInput::POINTER_OVER:
			Emit( PointerOver() );
			break;
		case WgInput::POINTER_EXIT:
			Emit( PointerExit() );
			break;
		case WgInput::BUTTON_PRESS:
			Emit( ButtonPress(button_key) );
			break;
		case WgInput::BUTTON_DOWN:
			Emit( ButtonDown(button_key) );
			break;
		case WgInput::BUTTON_REPEAT:
			Emit( ButtonRepeat(button_key) );
			break;
		case WgInput::BUTTON_RELEASE:
     		Emit( ButtonRelease(button_key) );
			break;
		case WgInput::BUTTON_RELEASE_OUTSIDE:
     		Emit( ButtonReleaseOutside(button_key) );
			break;
		case WgInput::BUTTON_CLICK:
     		Emit( ButtonClick(button_key) );
			break;
		case WgInput::BUTTON_DOUBLECLICK:
     		Emit( ButtonDoubleClick(button_key) );
			break;
	}
}

