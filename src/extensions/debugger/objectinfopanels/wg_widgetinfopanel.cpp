
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
#include "wg_widgetinfopanel.h"
#include <wg_textdisplay.h>

namespace wg
{

	const TypeInfo WidgetInfoPanel::TYPEINFO = { "WidgetInfoPanel", &DebugPanel::TYPEINFO };


	//____ constructor _____________________________________________________________

	WidgetInfoPanel::WidgetInfoPanel(const Blueprint& blueprint, Widget * pWidget) : DebugPanel( blueprint )
	{
		label.setText( Widget::TYPEINFO.className );

		auto pTable = TablePanel::create( WGBP(TablePanel,
											   _.columns = 2,
											   _.rows = 22,
											   _.columnLayout = Base::defaultPackLayout()) );


		int row = 0;

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("id: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->id()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}


		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("size W: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->size().w));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("size H: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->size().h));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("scale: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->scale()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("baggage: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->baggage()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("skin: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(int64_t(pWidget->skin().rawPtr())));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("tooltip: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(pWidget->tooltip());

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("pointer style: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(toString(pWidget->pointerStyle()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("mark policy: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(toString(pWidget->markPolicy()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("is pickable: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->isPickable()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("is pickhandle: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->isPickHandle()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("pick category: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->pickCategory()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("is drop target: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->isDropTarget()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("is tab locked: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->isTabLocked()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("is selectable: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->isSelectable()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("receiving updates: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->m_receivingUpdateCounter));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("has sticky focus: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->hasStickyFocus()));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;
		}

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("has overflow: ");

			auto pValue = TextDisplay::create();
			pValue->display.setText(std::to_string(pWidget->m_bOverflow));

			pTable->slots[row][0] = pLabel;
			pTable->slots[row][1] = pValue;
			row++;

			if( m_bOverflow )
			{
				BorderSPX overflow = pWidget->_overflow();

				{
					auto pLabel = TextDisplay::create();
					pLabel->display.setText("overflow top: ");

					auto pValue = TextDisplay::create();
					pValue->display.setText(std::to_string(overflow.top));

					pTable->slots[row][0] = pLabel;
					pTable->slots[row][1] = pValue;
					row++;
				}

				{
					auto pLabel = TextDisplay::create();
					pLabel->display.setText("overflow right: ");

					auto pValue = TextDisplay::create();
					pValue->display.setText(std::to_string(overflow.right));

					pTable->slots[row][0] = pLabel;
					pTable->slots[row][1] = pValue;
					row++;
				}

				{
					auto pLabel = TextDisplay::create();
					pLabel->display.setText("overflow bottom: ");

					auto pValue = TextDisplay::create();
					pValue->display.setText(std::to_string(overflow.bottom));

					pTable->slots[row][0] = pLabel;
					pTable->slots[row][1] = pValue;
					row++;
				}

				{
					auto pLabel = TextDisplay::create();
					pLabel->display.setText("overflow left: ");

					auto pValue = TextDisplay::create();
					pValue->display.setText(std::to_string(overflow.left));

					pTable->slots[row][0] = pLabel;
					pTable->slots[row][1] = pValue;
					row++;
				}

			}



		}





		this->slot = pTable;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& WidgetInfoPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}


} // namespace wg


