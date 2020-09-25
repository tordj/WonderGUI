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
#ifndef WG_ANIMSKIN_DOT_H
#define WG_ANIMSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_object.h>

#include <utility>
#include <vector>

namespace wg
{

	class AnimSkin;
	typedef	StrongPtr<AnimSkin>	AnimSkin_p;
	typedef	WeakPtr<AnimSkin>		AnimSkin_wp;

	class AnimSkin : public Skin
	{
	public:
		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


	protected:
		AnimSkin();
		~AnimSkin();

		void	_addSlot(SkinSlot* pSlot) override;
		void	_removeSlot(SkinSlot* pSlot) override;

		void	_requestRender();

		virtual void	_update(int ms) = 0;

		RouteId		m_routeId;

		std::vector<SkinSlot*>	m_mySlots;

	};


} // namespace wg
#endif //WG_ANIMSKIN_DOT_H
