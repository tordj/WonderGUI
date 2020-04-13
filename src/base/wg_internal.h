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
#ifndef WG_INTERNAL_DOT_H
#define WG_INTERNAL_DOT_H
#pragma once

#include <wg_container.h>

namespace wg
{

	class OWidget : public Widget
	{
	public:
		using Widget::_resize;
		using Widget::_collectPatches;
		using Widget::_maskPatches;
		using Widget::_render;
		using Widget::_parent;
		using Widget::_slot;
		using Widget::_windowPadding;
		using Widget::_preRender;
		using Widget::_setState;
	};

	class OContainer : public Container
	{
	public:
		using Widget::_resize;
		using Widget::_collectPatches;
		using Widget::_maskPatches;
		using Widget::_render;
		using Widget::_parent;
		using Widget::_slot;
		using Widget::_windowPadding;
		using Widget::_preRender;
		using Widget::_setState;


		using Container::_findWidget;
		using Container::_getPopupLayer;
		using Container::_getModalLayer;
	};

	class OSlot : public StaticSlot
	{
	public:
		using StaticSlot::_widget;
		using StaticSlot::_setWidget;
		using StaticSlot::_setSize;

	};

	class OGeoComponent : public GeoComponent
	{
	public:
	};


	inline OWidget *			OO(Widget* pWidget) { return reinterpret_cast<OWidget*>(pWidget); }
	inline const OWidget *		OO(const Widget* pWidget) { return reinterpret_cast<const OWidget*>(pWidget); }

	inline OContainer *			OO(Container* pContainer) { return reinterpret_cast<OContainer*>(pContainer); }
	inline const OContainer *	OO(const Container* pContainer) { return reinterpret_cast<const OContainer*>(pContainer); }

	inline OSlot *				OO(StaticSlot* pSlot) { return reinterpret_cast<OSlot*>(pSlot); }
	inline const OSlot *		OO(const StaticSlot* pSlot) { return reinterpret_cast<const OSlot*>(pSlot); }

	inline OSlot&				OO(StaticSlot& slot) { return reinterpret_cast<OSlot&>(slot); }
	inline const OSlot&			OO(const StaticSlot& slot) { return reinterpret_cast<const OSlot&>(slot); }

	inline OGeoComponent&		OO(GeoComponent& slot) { return reinterpret_cast<OGeoComponent&>(slot); }
	inline const OGeoComponent&	OO(const GeoComponent& slot) { return reinterpret_cast<const OGeoComponent&>(slot); }


}

#endif //WG_INTERNAL_DOT_H
