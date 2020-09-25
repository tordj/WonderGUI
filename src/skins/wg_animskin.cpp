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

#include <wg_animskin.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_msgrouter.h>
#include <wg_skinslot.h>

namespace wg
{

	using namespace Util;

	const TypeInfo AnimSkin::TYPEINFO = { "AnimSkin", &Skin::TYPEINFO };

	//____ constructor ________________________________________________________

	AnimSkin::AnimSkin()
	{
		m_routeId = Base::msgRouter()->addRoute(MsgType::Tick, [this](Msg* pMsg) { this->_update(static_cast<TickMsg*>(pMsg)->timediff()); });
	}

	//____ destructor _________________________________________________________

	AnimSkin::~AnimSkin()
	{
		Base::msgRouter()->deleteRoute(m_routeId);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& AnimSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _addSlot() _________________________________________________________

	void AnimSkin::_addSlot(SkinSlot* pSlot)
	{
		m_mySlots.push_back( pSlot );
		m_nSkinSlots++;
	}

	//____ _removeSlot() ______________________________________________________

	void AnimSkin::_removeSlot(SkinSlot* pSlot)
	{
		//TODO: We need something faster here...

		auto it = std::find(m_mySlots.begin(), m_mySlots.end(), pSlot);
		if (it != m_mySlots.end())
			m_mySlots.erase(it);

		m_nSkinSlots--;
	}

	//____ _requestRender() ___________________________________________________

	void AnimSkin::_requestRender()
	{
		for (auto pSlot : m_mySlots)
			pSlot->requestRender();
	}

}