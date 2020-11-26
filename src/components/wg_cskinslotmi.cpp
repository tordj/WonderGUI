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

#include <wg_cskinslotmi.h>

namespace wg
{
	const TypeInfo CSkinSlotMI::TYPEINFO = { "CSkinSlotMI", &Component::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CSkinSlotMI::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ set() ______________________________________________________________

	void CSkinSlotMI::set(Skin* pSkin)
	{
		if (pSkin == m_pSkin)
			return;

		Skin_p pOldSkin = m_pSkin;

		// Replace the skin

		if (m_pSkin)
			m_pSkin->_decUseCount();

		m_pSkin = pSkin;

		if (m_pSkin)
			m_pSkin->_incUseCount();

		m_pHolder->_skinChanged(this, pSkin, pOldSkin);
	}

	//____ _addInstance() ________________________________________________________

	void CSkinSlotMI::_addInstance(SkinSlotPocket*& pPocket, void* instance, State state)
	{
		// Check if we need a pocket and initialize or remove

		if (m_pSkin && m_pSkin->animationLength(state))
		{
			_initPocket(pPocket, nullptr, state);
			pPocket->bAnimated = true;
		}
	}

	//____ _changeInstance() _____________________________________________________

	void CSkinSlotMI::_changeInstance(SkinSlotPocket*& pPocket, void* newInstance)
	{
		if (pPocket)
			pPocket->instance = newInstance;
	}

	//____ _stateChanged() _____________________________________________________

	void CSkinSlotMI::_stateChanged(SkinSlotPocket*& pPocket, void* instance, State newState, State oldState)
	{
		if (!m_pSkin || newState == oldState || m_pSkin->ignoresState() )
			return;

		// Update transitions based on state change.

		Bitmask<uint8_t> changedStatesMask = newState.mask() ^ oldState.mask();

		if (m_pSkin->transitioningStates() & changedStatesMask )
		{
			auto pTransitionTimes = m_pSkin->transitionTimes();

			// Check if this state change starts any new transition

			bool bStartTransition = false;
			for (int stateIndex = 0; stateIndex < StateBits_Nb; stateIndex++)
			{
				if (changedStatesMask.bit(stateIndex) && pTransitionTimes[stateIndex] != 0)
				{
					bStartTransition = true;
					break;
				}
			}

			// Update transitions

			if (bStartTransition)
			{
				// Make sure we have a pocket

				if (!pPocket)
					_initPocket(pPocket, instance, oldState);

				pPocket->transitionTo = newState.mask();

				// Step through statebits and update transition progress

				for (int i = 0; i < StateBits_Nb; i++)
				{
					if (changedStatesMask.bit(i))
					{
						if (pTransitionTimes[i] == 0)
							pPocket->transitionFrom.flipBit(i);
					}
				}
			}
		}

		// Check if our skin is animated in newState.

		if (m_pSkin->animationLength(newState))
		{
			if (!pPocket)
				_initPocket(pPocket, instance, newState);

			pPocket->bAnimated = true;
		}
		else
		{
			// Release pocket if there is no transition nor animation.

			if (pPocket && pPocket->transitionFrom == pPocket->transitionTo)
			{	
				_releasePocket(pPocket);
				pPocket = nullptr;
			}

		}

		// Check if we need to update skin right away due to state change.

		Rect	canvas		= m_pHolder->_skinInstanceSize(pPocket->instance);
		float	value1		= m_pHolder->_skinInstanceValue(pPocket->instance);
		float	value2		= m_pHolder->_skinInstanceValue2(pPocket->instance);

		int		animPos = pPocket ? pPocket->animationCounter : 0;
		float* pStateFractions = pPocket ? pPocket->fractionalState : nullptr;


		Rect dirtyRect = m_pSkin->dirtyRect(canvas, oldState, newState, value1, value1, value2, value2, animPos, animPos, pStateFractions, pStateFractions);
		if (!dirtyRect.isEmpty())
			m_pHolder->_skinInstanceRequestRender(pPocket->instance, dirtyRect);
	}

	//____ _valueChanged() _____________________________________________________

	void CSkinSlotMI::_valueChanged(SkinSlotPocket*& pPocket, void* instance, float newValue, float oldValue, float newValue2, float oldValue2)
	{
		if (!m_pSkin || m_pSkin->ignoresValue())
			return;

		Rect	canvas = m_pHolder->_skinInstanceSize(pPocket->instance);
		State	state = m_pHolder->_skinInstanceState(pPocket->instance);

		int		animPos = pPocket ? pPocket->animationCounter : 0;
		float*	pStateFractions = pPocket ? pPocket->fractionalState : nullptr;

		Rect dirtyRect = m_pSkin->dirtyRect(canvas, state, state, newValue, oldValue, newValue2, oldValue2, animPos, animPos, pStateFractions, pStateFractions);
		if (!dirtyRect.isEmpty())
			m_pHolder->_skinInstanceRequestRender(pPocket->instance, dirtyRect);
	}

	//____ _render() ___________________________________________________________

	void CSkinSlotMI::_render(SkinSlotPocket* pPocket, GfxDevice* pDevice, const Rect& canvas, State state, float value, float value2) const
	{
		if (m_pSkin)
		{
			int animPos = pPocket ? pPocket->animationCounter : 0;
			float* pFractionalState = pPocket ? pPocket->fractionalState : nullptr;

			m_pSkin->render(pDevice, canvas, state, value, value2, animPos, pFractionalState);
		}
	}

	//____ _update() __________________________________________________________

	bool CSkinSlotMI::_update(SkinSlotPocket* pPocket, int msPassed)
	{
		// Prepare our dirtyRect check

		int oldAnimPos = 0;
		int newAnimPos = 0;

		float* pOldStateFractions = nullptr;
		float* pNewStateFractions = nullptr;

		float oldFractionalState[StateBits_Nb];

		// Update possible state transition

		if (pPocket->transitionFrom != pPocket->transitionTo)
		{
			auto pTransitionTimes = m_pSkin->transitionTimes();

			for (int i = 0; i < StateBits_Nb; i++)
			{
				oldFractionalState[i] = pPocket->fractionalState[i];

				if (pPocket->transitionFrom.bit(i) != pPocket->transitionTo.bit(i) )
				{
					if (pPocket->transitionTo.bit(i))
					{
						pPocket->fractionalState[i] += float(msPassed) / pTransitionTimes[i];
						if (pPocket->fractionalState[i] >= 1.f)
						{
							pPocket->fractionalState[i] = 1.f;
							pPocket->transitionFrom.flipBit(i);
						}
					}
					else
					{
						pPocket->fractionalState[i] -= float(msPassed) / pTransitionTimes[i];
						if (pPocket->fractionalState[i] <= 0.f)
						{
							pPocket->fractionalState[i] = 0.f;
							pPocket->transitionFrom.flipBit(i);
						}
					}

				}
			}

			pOldStateFractions = oldFractionalState;
			pNewStateFractions = pPocket->fractionalState;
		}

		// Update animation

		if (pPocket->bAnimated)
		{
			oldAnimPos = pPocket->animationCounter;
			pPocket->animationCounter += msPassed;
			newAnimPos = pPocket->animationCounter;
		}

		// Request render if needed

		Rect	canvas = m_pHolder->_skinInstanceSize(pPocket->instance);
		State	state = m_pHolder->_skinInstanceState(pPocket->instance);
		float	value1 = m_pHolder->_skinInstanceValue(pPocket->instance);
		float	value2 = m_pHolder->_skinInstanceValue2(pPocket->instance);

		float	animPos = pPocket->animationCounter;
		float* pStateFractions = pPocket->fractionalState;

		Rect dirtyRect = m_pSkin->dirtyRect(canvas, state, state, value1, value1, value2, value2, newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
		if (!dirtyRect.isEmpty())
			m_pHolder->_skinInstanceRequestRender(this, dirtyRect);

		return (pPocket->bAnimated == false && pPocket->transitionFrom == pPocket->transitionTo);
	}

	//____ _initPocket() ______________________________________________________

	void CSkinSlotMI::_initPocket(SkinSlotPocket*& pPocket, void * instance, State state)
	{
		if (!pPocket)
		{
			pPocket = SkinSlotManager::allocPocket();
			pPocket->pHolder = this;
		}

		Bitmask<uint8_t> statemask = state.mask();

		pPocket->transitionFrom = statemask;
		pPocket->transitionTo = statemask;

		for (int i = 0; i < StateBits_Nb; i++)
			pPocket->fractionalState[i] = statemask.bit(i) ? 1.f : 0.f;

		pPocket->bAnimated = false;
		pPocket->animationCounter = 0;
		pPocket->instance = instance;
	}

	//____ _releasePocket() ___________________________________________________

	void CSkinSlotMI::_releasePocket(SkinSlotPocket*& pPocket)
	{
		if (pPocket)
		{
			SkinSlotManager::freePocket(pPocket);
			pPocket = nullptr;
		}
	}






};