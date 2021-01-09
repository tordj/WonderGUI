/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_TOOLTIPLAYER_DOT_H
#define WG_TOOLTIPLAYER_DOT_H
#pragma once

#include <wg_layer.h>
#include <wg_cslot.h>

namespace wg
{


	class TooltipLayer;
	typedef	StrongPtr<TooltipLayer>	TooltipLayer_p;
	typedef	WeakPtr<TooltipLayer>	TooltipLayer_wp;

	//____ TooltipLayer ____________________________________________________________

	class TooltipLayer : public Layer
	{
	public:

		//

		struct Position
		{
			Origo	direction;
			Border	spacing;
			bool	bTooltipAroundPointer;
		};

		//.____ Creation __________________________________________

		static TooltipLayer_p	create() { return TooltipLayer_p(new TooltipLayer()); }

		//.____ Identification ________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Control _______________________________________________________

		void setTooltipGenerator( const std::function<Widget_p(Position& placement, const Widget* pHoveredWidget, const Border& widgetMargins )> func );
		void setTooltipPosition(Origo direction, Border spacing, bool bTooltipAroundPointer = true );

		void setDisplayTooltips(bool bDisplay);
		bool displayTooltips() const { return m_bDisplayTooltips;  }


	protected:
		TooltipLayer();
		virtual ~TooltipLayer();
		virtual Widget* _newOfMyType() const override { return new TooltipLayer(); };


		class Slot : public Layer::Slot
		{
			friend class TooltipLayer;
		public:
			const static TypeInfo	TYPEINFO;

		protected:

			Slot(SlotHolder* pHolder) : Layer::Slot(pHolder) {}
		};


		void			_updateTooltipGeo();

		void			_openTooltip();
		void			_closeTooltip();

		static Widget_p		_defaultTooltipGenerator(Position& placement, const Widget*, const Border&);

		// Overloaded from Widget

		void			_receive(Msg* pMsg) override;
		void			_update(int microPassed, int64_t microsecTimestamp) override;

		// Overloaded from Container

		const TypeInfo& _slotTypeInfo(const StaticSlot* pSlot) const override;

		Widget*			_findWidget(const Coord& ofs, SearchMode mode) override;

		void			_childRequestResize(StaticSlot* pSlot) override;


		// Overloaded from Layer

		const Layer::Slot*	_beginLayerSlots() const override;
		const Layer::Slot*	_endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		Slot		m_tooltipSlot;

		int			m_hoverMillisec		= 300;				// Number of millisec to hover before tooltip is displayed.
		Coord		m_hoverPos			= { -1,-1 };
		int			m_hoverCountdown	= 0;
		Widget*		m_pHoverWidget = nullptr;

		int			m_openTooltipMaxMovement = 8;			// How many points the pointer may move in any direction
															// from hoverPos once tooltip is displayed before it is closed.
		bool		m_bBlockReopen		= false;
		bool		m_bDisplayTooltips	= true;

		Position	m_defaultPosition = { Origo::SouthEast, {2,16,16,2}, true };
		Position	m_activePosition;

		std::function<Widget_p(Position& placement, const Widget* pHovered, const Border& margin)>	m_tooltipGenerator = _defaultTooltipGenerator;

	};

} // namespace wg
#endif //WG_POPUPLAYER_DOT_H