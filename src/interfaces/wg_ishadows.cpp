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

#include <wg_ishadows.h>

namespace wg
{
	//____ clear() ____________________________________________________________

	void IShadows::clear()
	{
		if (!m_pShadows->empty())
		{
			m_pHolder->_willRemoveShadows(0, m_pShadows->size());
			m_pShadows->clear();
		}
	}

	//____ add() _______________________________________________________________

	bool IShadows::add(Widget * pWidget, Skin * pShadow)
	{
		if (pWidget == nullptr || pShadow == nullptr)
			return false;

		m_pShadows->emplace_back(pWidget,pShadow);
		m_pHolder->_didAddShadows(1);
		return true;
	}

	int IShadows::add(std::initializer_list <std::tuple<Widget*, Skin*>> shadows)
	{
		int nAdded = 0;

		for (auto& entry : shadows)
		{
			Widget * pWidget = std::get<0>(entry);
			Skin * pShadow = std::get<1>(entry);

			if (pWidget && pShadow)
			{
				m_pShadows->emplace_back(pWidget, pShadow);
				nAdded++;
			}
		}

		if( nAdded > 0 )
			m_pHolder->_didAddShadows(nAdded);

		return nAdded;
	}

	//____ remove() ____________________________________________________________

	void IShadows::remove(int index)
	{
		m_pHolder->_willRemoveShadows(index, 1);
		m_pShadows->erase(m_pShadows->begin() + index);
	}

	IShadows::iterator IShadows::remove(const_iterator pos)
	{
		m_pHolder->_willRemoveShadows(pos - m_pShadows->begin(), 1);
		return m_pShadows->erase(pos);
	}

	void IShadows::remove(int index, int amount)
	{
		m_pHolder->_willRemoveShadows(index, amount);
		m_pShadows->erase(m_pShadows->begin() + index, m_pShadows->begin() + index + amount);
	}

	IShadows::iterator IShadows::remove(const_iterator beg, const_iterator end)
	{
		m_pHolder->_willRemoveShadows(beg - m_pShadows->begin(), end-beg);
		return m_pShadows->erase(beg,end);
	}
}

