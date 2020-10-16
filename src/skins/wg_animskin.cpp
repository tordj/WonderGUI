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
#include <wg_cskinslot.h>
#include <wg_internal.h>

namespace wg
{

	using namespace Util;

	const TypeInfo AnimSkin::TYPEINFO = { "AnimSkin", &Skin::TYPEINFO };

	//____ constructor ________________________________________________________

	AnimSkin::AnimSkin()
	{
	}

	//____ destructor _________________________________________________________

	AnimSkin::~AnimSkin()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& AnimSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ _requestRender() ___________________________________________________
/*
	void AnimSkin::_requestRender()
	{
		if (m_pSuperSkin)
		{
			for (auto pSlot : m_mySlots)
			{
				Rect myGeo = OO(m_pSuperSkin)->_subSkinGeo(this, pSlot->size(), pSlot->state() );
				pSlot->requestRender(myGeo);
			}
		}
		else
		{
			for (auto pSlot : m_mySlots)
				pSlot->requestRender();
		}

	}
*/
}