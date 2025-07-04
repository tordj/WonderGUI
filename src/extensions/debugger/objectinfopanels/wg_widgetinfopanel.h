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

#ifndef	WG_WIDGETINFOPANEL_DOT_H
#define WG_WIDGETINFOPANEL_DOT_H
#pragma once

#include <wg_tablepanel.h>
#include <wg_debugpanel.h>

namespace wg
{
	class WidgetInfoPanel;
	typedef	StrongPtr<WidgetInfoPanel>	WidgetInfoPanel_p;
	typedef	WeakPtr<WidgetInfoPanel>	WidgetInfoPanel_wp;



	class WidgetInfoPanel : public DebugPanel
	{
	public:

		//.____ Creation __________________________________________

		static WidgetInfoPanel_p		create( const Blueprint& blueprint, Widget * pWidget) { return WidgetInfoPanel_p(new WidgetInfoPanel(blueprint, pWidget) ); }

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


	protected:
		WidgetInfoPanel(const Blueprint& blueprint, Widget * pWidget );
		~WidgetInfoPanel() {}
	};

} // namespace wg
#endif //WG_WIDGETTINFOPANEL_DOT_H


