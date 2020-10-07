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

#include <wg_skinslot.h>
#include <wg_skinslotmanager.h>

namespace wg
{

	void SkinSlot::_setSkin(Skin* pSkin)
	{
		// Replace the skin

		if (m_pSkin)
			m_pSkin->_decUseCount();

		m_pSkin = pSkin;

		if (m_pSkin)
			m_pSkin->_incUseCount();

		// Check if we need a pocket and initialize or remove

		if (pSkin)
		{
			State state = m_pHolder->_skinState(this);

			auto animatedStates = pSkin->_animatedStates();

			if ((animatedStates.mask() & state.mask()) != 0)
				_initPocket();
			else
				_releasePocket();

		}
		else if (m_pPocket)
			_releasePocket();

	}

	void SkinSlot::_changeState(State newState, State oldState)
	{

	}

	bool SkinSlot::_update(int msPassed)
	{
		// Update possible state transition

		if (m_pPocket->transitionFromState != m_pPocket->transitionToState)
		{

		}
	}

	void SkinSlot::_initPocket()
	{
		if (!m_pPocket)
		{
			m_pPocket = SkinSlotManager::allocPocket();

		}
	}

	void SkinSlot::_releasePocket()
	{
		if (m_pPocket)
		{
			SkinSlotManager::freePocket(m_pPocket);
			m_pPocket = nullptr;
		}
	}


};