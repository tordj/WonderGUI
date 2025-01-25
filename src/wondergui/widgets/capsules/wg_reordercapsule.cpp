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
#include <wg_packpanel.h>

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
		m_dropCategory = 1;

		m_pTransition = CoordTransition::create(500000, TransitionCurve::EaseOut);

		m_pHoveredPosFiller = Filler::create( WGBP(Filler, _.skin = ColorSkin::create(Color::CornflowerBlue) ) );
		m_pPrevPosFiller = Filler::create(WGBP(Filler, _.skin = ColorSkin::create(Color::CornflowerBlue)));

		m_pHoveredPosFiller->setId(1);
		m_pHoveredPosFiller->setId(2);


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

				Container* pContainer = _ourContainer();

				// Find out which child we picked

				auto pickOfs = ptsToSpx( pMsg->pointerPos() - globalGeo().pos(), m_scale );

				Widget * pWidget = _findWidget(pickOfs, SearchMode::ActionTarget);

				while( pWidget != nullptr && pWidget != this && pWidget->parent() != pContainer )
					pWidget = pWidget->parent();

				//

				if( pWidget && pWidget != this )
				{
					auto pDataset = Dataset<Widget_wp>::create(pWidget);
					pMsg->setContent(DropType::Widget, m_dropCategory, pDataset);
					pMsg->setDragWidget(pWidget, {0,0} );		//TODO: Set pointer pos.

					m_pPicked = pWidget;

					auto pPackPanel = static_cast<PackPanel*>(pContainer);
					m_pickedPos = pPackPanel->slots.find(pWidget) - pPackPanel->slots.begin();

					m_hoveredPos = m_pickedPos;

					m_pHoveredPosFiller->setDefaultSize(pWidget->defaultSize() );
					static_cast<DynamicSlot*>(pWidget->_slot())->setWidget(m_pHoveredPosFiller);

					m_transitionProgress = m_pTransition->duration();

					m_pickState = PickState::Remaining;
					_startReceiveUpdates();
				}

				break;
			}

			case MsgType::DropCancel:
			{
				_endTransition();
				m_pickState = PickState::Canceled;
				break;
			}

			case MsgType::DropComplete:
			{
				_endTransition();
				break;
			}


			case MsgType::DropProbe:
			{
				auto pMsg = static_cast<DropProbeMsg*>(_pMsg);

				if (pMsg->dropType() == DropType::Widget && pMsg->category() == m_dropCategory)
				{
					pMsg->accept();
				}

				break;
			}

			case MsgType::DropEnter:
			case MsgType::DropMove:
			{
				auto pMsg = static_cast<DragNDropMsg*>(_pMsg);

				Coord hoverCoord = pMsg->pointerPos() - globalGeo().pos();

				Widget * pWidget = _findWidget( Util::ptsToSpx(hoverCoord, m_scale), SearchMode::ActionTarget);

				PackPanel* pOurContainer = static_cast<PackPanel*>(_ourContainer());


				while (pWidget && pWidget != this && pWidget->parent() != pOurContainer)
					pWidget = pWidget->parent();

				if (pWidget && pWidget->parent() == pOurContainer)
				{
					if (pWidget == m_pHoveredPosFiller)
						break;

					if (pWidget == m_pPrevPosFiller)
					{
						_markPosition(m_prevPos);
						break;
					}

					Rect markedGeo = pWidget->globalGeo();

					int index = pOurContainer->slots.index(pWidget);
					int compareIndex = index;

					if (m_hoveredPos >= 0 && m_hoveredPos < compareIndex)
						index--;

					if (m_prevPos >= 0 && m_prevPos < compareIndex)
						index--;

					if (pOurContainer->axis() == Axis::X)
					{
						if (pMsg->pointerPos().x > markedGeo.x + markedGeo.w / 2)
							index++;
					}

					_markPosition(index);

				}
				else
					_markPosition(m_pickedPos);


				break;
			}

			case MsgType::DropLeave:
				if( m_pickState != PickState::Completed)
					_markPosition(m_pickedPos);
				break;

			case MsgType::DropDeliver:
				auto pMsg = static_cast<DropDeliverMsg*>(_pMsg);

				pMsg->accept();

				_endTransition();
				m_pickState = PickState::Completed;
				break;

		}
	}

	//____ _endTransition() ______________________________________________________

	void ReorderCapsule::_endTransition()
	{
		m_transitionProgress = m_pTransition->duration();
		m_bTransitioning = false;
	}


	//____ _update() _____________________________________________________________

	void ReorderCapsule::_update(int microPassed, int64_t microsecTimestamp)
	{
		auto pPackPanel = static_cast<PackPanel*>(_ourContainer());

		if (m_bTransitioning)
		{
			int duration = m_pTransition->duration();

			m_transitionProgress += microPassed;
			if (m_transitionProgress >= duration)
			{
				m_transitionProgress = duration;
				m_bTransitioning = false;
			}

			Size targetSize = m_pPicked->defaultSize();
			Size startSize = m_transitionStartSize;

			int progress = std::max(0, m_transitionProgress);

			Coord res = m_pTransition->snapshot(progress, Coord{ startSize.w, startSize.h }, Coord{ targetSize.w, targetSize.h });
			Size newSize(res.x, res.y);

			Size complementarySize = targetSize;
			
			if (pPackPanel->axis() == Axis::X)
				complementarySize.w -= newSize.w;
			else
				complementarySize.h -= newSize.h;


			m_pHoveredPosFiller->setDefaultSize(newSize);
			m_pPrevPosFiller->setDefaultSize(complementarySize);
		}





		switch (m_pickState)
		{
		case PickState::Leaving:
		{


			break;
		}

		case PickState::Returning:
			break;

		case PickState::Canceled:
		{
			_markPosition(m_pickedPos);
			m_pickState = PickState::Finishing;
			m_dropPos = m_pickedPos;
			break;
		}

		case PickState::Completed:
		{
			m_pickState = PickState::Finishing;
			m_dropPos = m_hoveredPos;
			break;
		}

		case PickState::Finishing:
		{
			if (!m_bTransitioning)
			{
				// Remove fillers.

				m_pHoveredPosFiller->releaseFromParent();
				m_pPrevPosFiller->releaseFromParent();

				// Reinsert picked widget.

				pPackPanel->slots.insert(m_dropPos, m_pPicked);


				m_pPicked = nullptr;
				m_prevPos = -1;
				m_hoveredPos = -1;
				m_pickState = PickState::Unpicked;
				_stopReceiveUpdates();
			}
		}

		default:
			break;
		}

	}

	//____ _ourContainer() ________________________________________________________

	Container* ReorderCapsule::_ourContainer()
	{
		Widget* p = slot._widget();
		while (p->isInstanceOf(Capsule::TYPEINFO))
			p = static_cast<Capsule*>(p)->slot.widget();

		if (p->isContainer())
			return static_cast<Container*>(p);
		else
			return nullptr;
	}

	//____ _markPosition() _______________________________________________________

	void ReorderCapsule::_markPosition(int pos)
	{
		auto pPackPanel = static_cast<PackPanel*>(_ourContainer());

		if (pos == m_hoveredPos)
			return;											// No movement.

		std::swap(m_prevPos, m_hoveredPos);
		std::swap(m_pPrevPosFiller, m_pHoveredPosFiller);

		Size	startSize;

		if (pos == m_hoveredPos)
		{
			startSize = m_pHoveredPosFiller->defaultSize();
		}
		else
		{
			startSize = m_pPicked->defaultSize();

			if (pPackPanel->axis() == Axis::X)
				startSize.w = 0;
			else
				startSize.h = 0;

			int newHoveredPos = pos;

			if (m_prevPos >= 0 && m_prevPos < pos)
				pos++;

			if (m_hoveredPos >= 0 && m_hoveredPos < pos)
				pos++;

			m_hoveredPos = newHoveredPos;

			m_pHoveredPosFiller->setDefaultSize(startSize);

			pPackPanel->slots.insert(pos, m_pHoveredPosFiller);
		}

		m_transitionProgress =  -m_transitionDelay;
		m_bTransitioning = true;
		m_transitionStartSize = startSize;


	}



} // namespace wg
