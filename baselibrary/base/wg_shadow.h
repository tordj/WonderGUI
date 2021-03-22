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


#ifndef WG_SHADOW_DOT_H
#define WG_SHADOW_DOT_H
#pragma once

#include <wg_widget.h>

namespace wg
{

	class Shadow
	{
		friend class ShadowHolder;
	public:
		/** @private */
		Shadow(Widget* pWidget, Skin* pSkin) : m_pWidget(pWidget), m_pSkin(pSkin) {}

		Widget_p	widget() const { return m_pWidget.rawPtr(); }
		Skin_p		shadow() const { return m_pSkin; }

	private:
		Widget_wp	m_pWidget;
		Skin_p		m_pSkin;
		Rect		m_geo;
	};
}
#endif //WG_SHADOW_DOT_H
