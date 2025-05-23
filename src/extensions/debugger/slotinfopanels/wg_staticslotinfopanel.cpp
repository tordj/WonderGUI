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
#include "wg_staticslotinfopanel.h"
#include <wg_textdisplay.h>

namespace wg
{

	const TypeInfo StaticSlotInfoPanel::TYPEINFO = { "StaticSlotInfoPanel", &DebugPanel::TYPEINFO };


	//____ constructor _____________________________________________________________

	StaticSlotInfoPanel::StaticSlotInfoPanel(const Blueprint& blueprint, StaticSlot * pStaticSlot) : DebugPanel( blueprint )
	{
		auto pTable = TablePanel::create( WGOVR(blueprint.table, _.columns = 2, _.rows = 4 ));

		int row = 0;

		{
			auto pLabelX = TextDisplay::create(blueprint.listEntryLabel);
			pLabelX->display.setText("Relative X: ");

			auto pLabelY = TextDisplay::create(blueprint.listEntryLabel);
			pLabelY->display.setText("Relative Y: ");

			auto pLabelW = TextDisplay::create(blueprint.listEntryLabel);
			pLabelW->display.setText("Width: ");

			auto pLabelH = TextDisplay::create(blueprint.listEntryLabel);
			pLabelH->display.setText("Height: ");


			RectF geo = pStaticSlot->geo();

			auto pValueX = TextDisplay::create(blueprint.listEntryValue);
			pValueX->display.setText(std::to_string(geo.x));

			auto pValueY = TextDisplay::create(blueprint.listEntryValue);
			pValueY->display.setText(std::to_string(geo.y));

			auto pValueW = TextDisplay::create(blueprint.listEntryValue);
			pValueW->display.setText(std::to_string(geo.w));

			auto pValueH = TextDisplay::create(blueprint.listEntryValue);
			pValueH->display.setText(std::to_string(geo.h));


			pTable->slots[row][0] = pLabelX;
			pTable->slots[row][1] = pValueX;
			row++;

			pTable->slots[row][0] = pLabelY;
			pTable->slots[row][1] = pValueY;
			row++;

			pTable->slots[row][0] = pLabelW;
			pTable->slots[row][1] = pValueW;
			row++;

			pTable->slots[row][0] = pLabelH;
			pTable->slots[row][1] = pValueH;
			row++;
		}

		this->slot = pTable;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticSlotInfoPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}


} // namespace wg



