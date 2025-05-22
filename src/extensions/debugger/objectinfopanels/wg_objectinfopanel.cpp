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
#include "wg_objectinfopanel.h"
#include <wg_textdisplay.h>

namespace wg
{

	const TypeInfo ObjectInfoPanel::TYPEINFO = { "ObjectInfoPanel", &DebugPanel::TYPEINFO };


	//____ constructor _____________________________________________________________

	ObjectInfoPanel::ObjectInfoPanel(const Blueprint& blueprint, Object * pObject) : DebugPanel( blueprint )
	{
		label.setText( Object::TYPEINFO.className );

		auto pTable = TablePanel::create( WGBP(TablePanel,
											   _.columns = 2,
											   _.rows = 2,
											   _.columnLayout = Base::defaultPackLayout()) );


		{
			auto pIdLabel = TextDisplay::create();
			pIdLabel->display.setText("refcount: ");

			auto pId = TextDisplay::create();
			pId->display.setText(std::to_string(pObject->refcount()));

			pTable->slots[0][0] = pIdLabel;
			pTable->slots[0][1] = pId;
		}

		{
			auto pIdLabel = TextDisplay::create();
			pIdLabel->display.setText("finalizer: ");

			auto pId = TextDisplay::create();
			pId->display.setText(std::to_string(pObject->finalizer()));

			pTable->slots[1][0] = pIdLabel;
			pTable->slots[1][1] = pId;
		}

		this->slot = pTable;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ObjectInfoPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}


} // namespace wg


