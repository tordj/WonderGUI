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

#include <wg_reordercapsule.h>
#include <wg_msg.h>
#include <wg_colorskin.h>
#include <wg_dataset.h>

namespace wg
{
	using namespace Util;

	const TypeInfo ReorderCapsule::TYPEINFO = { "ReorderCapsule", &Capsule::TYPEINFO };

	//____ create() ______________________________________________________________

	ReorderCapsule_p ReorderCapsule::create()
	{
		return ReorderCapsule_p(new ReorderCapsule());
	}

	ReorderCapsule_p ReorderCapsule::create(const Blueprint& blueprint)
	{
		return ReorderCapsule_p(new ReorderCapsule(blueprint));
	}

	//____ constructor ____________________________________________________________

	ReorderCapsule::ReorderCapsule()
	{
		m_bPickable = true;
		_init();
	}

	//____ _init() _______________________________________________________________

	void ReorderCapsule::_init()
	{
		m_pWithdrawFiller = Filler::create( WGBP(Filler, _.skin = ColorSkin::create(Color::BlueViolet) ) );
	}

	//____ destructor _____________________________________________________________

	ReorderCapsule::~ReorderCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ReorderCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _receive() ____________________________________________________________

	void ReorderCapsule::_receive(Msg* _pMsg)
	{
		switch( _pMsg->type())
		{
			case MsgType::DropPick:
			{
				auto pMsg = static_cast<DropPickMsg*>(_pMsg);

				// Find the panel inside us that we are affecting.
				// We allow for capsules between us and the panel.

				Widget * pContainer = slot._widget();
				while( pContainer->isInstanceOf(Capsule::TYPEINFO) )
					pContainer = static_cast<Capsule*>(pContainer)->slot.widget();

				// Find out which child we picked

				auto pickOfs = ptsToSpx( pMsg->pointerPos() - globalGeo().pos(), m_scale );

				Widget * pWidget = _findWidget(pickOfs, SearchMode::ActionTarget);

				while( pWidget != nullptr && pWidget != this && pWidget->parent() != pContainer )
					pWidget = pWidget->parent();

				//

				if( pWidget && pWidget != this )
				{
					auto pDataset = Dataset<Widget_wp>::create(pWidget);
					pMsg->setContent(DropType::Widget, 0, pDataset);
					pMsg->setDragWidget(pWidget, {0,0} );		//TODO: Set pointer pos.

					m_pPicked = pWidget;

					m_pWithdrawFiller->setDefaultSize(pWidget->size() );	//TODO: Doesn't take variations in scale into account.
					static_cast<DynamicSlot*>(pWidget->_slot())->setWidget(m_pWithdrawFiller);
				}

				break;
			}

			case MsgType::DropCancel:
			{
				static_cast<DynamicSlot*>(m_pWithdrawFiller->_slot())->setWidget(m_pPicked);				
				break;
			}

			case MsgType::DropComplete:
			{
				break;
			}

			case MsgType::DropProbe:
			{
				auto pMsg = static_cast<DropProbeMsg*>(_pMsg);

				auto pDataset = pMsg->dataset();



				break;
			}



			case MsgType::DropEnter:
				break;

			case MsgType::DropMove:
				break;

			case MsgType::DropLeave:
				break;

			case MsgType::DropDeliver:
				break;

		}
	}

	//____ _update() _____________________________________________________________

	void ReorderCapsule::_update(int microPassed, int64_t microsecTimestamp)
	{

	}


} // namespace wg
