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

#ifndef WG_TEXTLINK_DOT_H
#define WG_TEXTLINK_DOT_H

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

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

//____ WgTextLink _____________________________________________________________

class WgTextLink : public WgEmitter, private WgRefCounted
{
	friend class WgSmartPtr<WgTextLink>;
	friend class WgText;

public:
	void			SetClicked() { m_bClicked = true; }
	void			ClearClicked() { m_bClicked = false; }
	inline bool		Clicked() const { return m_bClicked; }

protected:
	WgTextLink() { m_bClicked = false; };


	// Called by text-class so we can respond to actions.

	void	OnAction( WgInput::UserAction action, int button_key );

	//

	bool			m_bClicked;
};

typedef	WgSmartPtr<WgTextLink>	WgTextLinkPtr;

#endif //WG_TEXTLINK_DOT_H
