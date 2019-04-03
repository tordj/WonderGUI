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
		PackPanelSlot() : weight(1.f) {}
	
		float			weight;			// Weight for space allocation.
		Rect			geo;			// Real geo of child (no padding included).
		Size			preferredSize;	// Cached padded preferred size from the child.
	};
	


	class IPackPanelChildren;
	typedef	StrongInterfacePtr<IPackPanelChildren>	IPackPanelChildren_p;
	typedef	WeakInterfacePtr<IPackPanelChildren>	IPackPanelChildren_wp;
	
	
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

		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static PackPanel_p	cast( Object * pObject );
	
		//.____ Geometry ____________________________________________

		void			setOrientation( Orientation orientaiton );
		Orientation		orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }
			
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;

		//.____ Behavior ________________________________________________________

		void			setSizeBroker( SizeBroker * pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }

	
	protected:
		PackPanel();
		virtual ~PackPanel();
		virtual Widget* _newOfMyType() const { return new PackPanel(); };
	
	    // Overloaded from Widget
	    
		void			_setSize( const Size& size );
	 
	    
		// Overloaded from Container
		
		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const;


		// Overloaded from PackChildrenHolder

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_didMoveSlots(Slot * pFrom, Slot * pTo, int nb);
		void		_willRemoveSlots( Slot * pSlot, int nb );
		void		_hideSlots( Slot *, int nb );
		void		_unhideSlots( Slot *, int nb );
		void		_repadSlots( Slot *, int nb, Border padding );
		void		_repadSlots(Slot *, int nb, const Border * pPaddings);
		void		_reweightSlots(Slot * pSlot, int nb, float weight);
		void		_reweightSlots(Slot * pSlot, int nb, const float * pWeights);
		Object *	_object() { return this; }
		WidgetHolder *	_widgetHolder() { return this; }
		void		_refreshChildGeo() { _refreshChildGeo(true); }
		void		_refreshChildGeo(bool bRequestRender);

		// Overloaded from WidgetHolder

		Coord		_childPos( Slot * pSlot ) const;
		Size		_childSize( Slot * pSlot ) const;

		void		_childRequestRender( Slot * pSlot );
		void		_childRequestRender( Slot * pSlot, const Rect& rect );
		void		_childRequestResize( Slot * pSlot );

		Widget *	_prevChild( const Slot * pSlot ) const;
		Widget *	_nextChild( const Slot * pSlot ) const;

				
		//
		
		void		_hideChildren(PackPanelSlot * pSlot, int nb);
		void		_unhideChildren(PackPanelSlot * pSlot, int nb);

		void		_refreshAllWidgets();
		void		_updatePreferredSize();
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const;


		SlotArray<PackPanelSlot> m_children;
		
		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		Size			m_preferredContentSize;
	
	};
	
	

} // namespace wg
#endif //WG_PACKPANEL_DOT_H
