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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H
#pragma once

#include <wg_sizebroker.h>
#include <wg_panel.h>
#include <wg_paddedslot.h>
#include <wg_iweightedchildren.h>

namespace wg
{

	class PackPanel;
	typedef	StrongPtr<PackPanel>		PackPanel_p;
	typedef	WeakPtr<PackPanel>			PackPanel_wp;


	//____ PackPanelSlot ____________________________________________________________

	class PackPanelSlot : public PaddedSlot		/** @private */
	{
	public:
		bool			bResizeRequired = false;
		float			weight = 1.f;				// Weight for space allocation.
		RectI			geo;						// Real geo of child (no padding included).
		SizeI			preferredSize;				// Cached padded preferred size from the child.
	};



//	class IPackPanelChildren;
//	typedef	StrongInterfacePtr<IPackPanelChildren>	IPackPanelChildren_p;
//	typedef	WeakInterfacePtr<IPackPanelChildren>	IPackPanelChildren_wp;


	/**
	 * @brief	A widget for arranging children horizontally or vertically.
	 *
	 * A widget for arranging children horizontally or vertically.
	 */

	class PackPanel : public Panel, protected WeightedChildrenHolder
	{

	public:

		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }

		//.____ Interfaces _______________________________________

		IWeightedChildren<PackPanelSlot,WeightedChildrenHolder>	children;

		//.____ Identification __________________________________________

		bool		isInstanceOf( const char * pClassName ) const override;
		const char *className( void ) const override;
		static const char	CLASSNAME[];
		static PackPanel_p	cast( Object * pObject );

		//.____ Geometry ____________________________________________

		void			setOrientation( Orientation orientaiton );
		Orientation		orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }

		//.____ Behavior ________________________________________________________

		void			setSizeBroker( SizeBroker * pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }


	protected:
		PackPanel();
		virtual ~PackPanel();
		virtual Widget* _newOfMyType() const  override { return new PackPanel(); };

		// Overloaded from Widget

		void			_resize( const SizeI& size ) override;

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;


		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;


		// Overloaded from PackChildrenHolder

		Slot *		_incSlot(Slot * pSlot) const override;
		Slot *		_decSlot(Slot * pSlot) const override;
		void		_didAddSlots( Slot * pSlot, int nb ) override;
		void		_didMoveSlots(Slot * pFrom, Slot * pTo, int nb) override;
		void		_willRemoveSlots( Slot * pSlot, int nb ) override;
		void		_hideSlots( Slot *, int nb ) override;
		void		_unhideSlots( Slot *, int nb ) override;
		void		_repadSlots( Slot *, int nb, BorderI padding ) override;
		void		_repadSlots(Slot *, int nb, const BorderI * pPaddings) override;
		void		_reweightSlots(Slot * pSlot, int nb, float weight) override;
		void		_reweightSlots(Slot * pSlot, int nb, const float * pWeights) override;
		Object *	_object() override { return this; }
		WidgetHolder *	_widgetHolder() override { return this; }
		void		_refreshChildGeo() override { _refreshChildGeo(true); }

		// Overloaded from WidgetHolder

		CoordI		_childPos( Slot * pSlot ) const override;
		SizeI		_childSize( Slot * pSlot ) const override;

		void		_childRequestRender( Slot * pSlot ) override;
		void		_childRequestRender( Slot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( Slot * pSlot ) override;

		Widget *	_prevChild( const Slot * pSlot ) const override;
		Widget *	_nextChild( const Slot * pSlot ) const override;

		void		_releaseChild(Slot * pSlot) override;

		//

		void		_refreshChildGeo(bool bRequestRender);

		void		_hideChildren(PackPanelSlot * pSlot, int nb);
		void		_unhideChildren(PackPanelSlot * pSlot, int nb);

		void		_refreshGeometries();
		SizeI		_calcPreferredSize();
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const;

		int			_setItemLengths(SizeBrokerItem * pItems, int nItems, int totalLength) const;
		int			_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const;


		SlotArray<PackPanelSlot> m_children;

		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		SizeI			m_preferredContentSize;

	};



} // namespace wg
#endif //WG_PACKPANEL_DOT_H
