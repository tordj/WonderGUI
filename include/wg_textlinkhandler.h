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

#ifndef WG_TEXTLINKHANDLER_DOT_H
#define WG_TEXTLINKHANDLER_DOT_H

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

#ifndef WG_EMITTER_DOT_H
#	include <wg_emitter.h>
#endif

#ifndef WG_INPUT_DOT_H
#	include <wg_input.h>
#endif

class WgText;

typedef	WgSmartPtr<class WgTextLinkHandler> WgTextLinkHandlerPtr;


//____ WgTextLinkHandler ______________________________________________________

class WgTextLinkHandler : public WgEmitter, public WgRefCounted
{
	friend class WgText;

public:
	static WgTextLinkHandlerPtr	Create() { return new WgTextLinkHandler(); }

private:
	WgTextLinkHandler() {}
	~WgTextLinkHandler() {}

	void	OnAction( WgInput::UserAction action, int button_key );
};


#endif //WG_TEXTLINKHANDLER_DOT_H
