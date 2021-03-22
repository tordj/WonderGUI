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

#include <wg_cshadows.h>

namespace wg
{
	const TypeInfo CShadows::TYPEINFO = { "CShadows", &Component::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CShadows::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ clear() ____________________________________________________________

	void CShadows::clear()
	{
		if (!m_pShadows->empty())
		{
			m_pHolder->_willRemoveShadows(0, (int) m_pShadows->size());
			m_pShadows->clear();
		}
	}

	//____ add() _______________________________________________________________

	bool CShadows::add(Widget * pWidget, Skin * pShadow)
	{
		if (pWidget == nullptr || pShadow == nullptr)
			return false;

		m_pShadows->emplace_back(pWidget,pShadow);
		m_pHolder->_didAddShadows(1);
		return true;
	}

	int CShadows::add(std::initializer_list <std::tuple<Widget*, Skin*>> shadows)
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

	void CShadows::remove(int index)
	{
		m_pHolder->_willRemoveShadows(index, 1);
		m_pShadows->erase(m_pShadows->begin() + index);
	}

	CShadows::iterator CShadows::remove(const_iterator pos)
	{
		m_pHolder->_willRemoveShadows(int(pos - m_pShadows->begin()), 1);
		return m_pShadows->erase(pos);
	}

	void CShadows::remove(int index, int amount)
	{
		m_pHolder->_willRemoveShadows(index, amount);
		m_pShadows->erase(m_pShadows->begin() + index, m_pShadows->begin() + index + amount);
	}

	CShadows::iterator CShadows::remove(const_iterator beg, const_iterator end)
	{
		m_pHolder->_willRemoveShadows(int(beg - m_pShadows->begin()), int(end-beg));
		return m_pShadows->erase(beg,end);
	}
}

