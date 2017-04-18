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

namespace wg 
{
	
	class PackPanel;
	typedef	StrongPtr<PackPanel,Panel_p>		PackPanel_p;
	typedef	WeakPtr<PackPanel,Panel_wp>			PackPanel_wp;


	//____ PackPanelSlot ____________________________________________________________
	
	class PackPanelSlot : public PanelSlot
	{
	public:
		PackPanelSlot() : weight(1.f) {}
	
		float			weight;			// Weight for space allocation.
		Rect			geo;			// Real geo of child (no padding included).
		Size			preferredSize;	// Cached padded preferred size from the child.
	};
	
	
	class PackPanelChildren;
	typedef	StrongInterfacePtr<PackPanelChildren,Interface_p>	PackPanelChildren_p;
	typedef	WeakInterfacePtr<PackPanelChildren,Interface_wp>	PackPanelChildren_wp;
	
	//____ PackPanelChildren ________________________________________________________

	class PackPanelChildren : public PanelChildren<PackPanelSlot,PackPanel>
	{
	public:
		PackPanelChildren( SlotArray<PackPanelSlot> * pSlotArray, PackPanel * pHolder ) : PanelChildren<PackPanelSlot,PackPanel>(pSlotArray,pHolder) {}

		inline PackPanelChildren_p	ptr() { return PackPanelChildren_p(_object(),this); }

		bool		setWeight( int index, float weight );
		float		weight( int index ) const;
	};

	
	
	/**
	 * @brief	A widget for arranging children horizontally or vertically.
	 *
	 * A widget for arranging children horizontally or vertically.
	 */
	
	class PackPanel : public Panel, protected PanelSlotsHolder
	{
		friend class PackPanelChildren;
		friend class PanelChildren<PackPanelSlot,PackPanel>;
		friend class ChildGroup<PackPanelSlot,PackPanel>;
	
	public:

		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }
		
		//.____ Components _______________________________________

		PackPanelChildren	children;

		//.____ Identification __________________________________________

		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static PackPanel_p	cast( const Object_p& pObject );
	
		//.____ Geometry ____________________________________________

		void			setOrientation( Orientation orientaiton );
		Orientation		orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }
			
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;

		//.____ Behavior ________________________________________________________

		void			setSizeBroker( const SizeBroker_p& pBroker );
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

		void		_firstChildWithGeo( WidgetWithGeo& package ) const;
		void		_nextChildWithGeo( WidgetWithGeo& package ) const;


		// Overloaded from PanelSlotsHolder

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_willRemoveSlots( Slot * pSlot, int nb );
		void		_hideSlots( PanelSlot *, int nb );
		void		_unhideSlots( PanelSlot *, int nb );
		void		_repadSlots( PanelSlot *, int nb, Border padding );

		// Overloaded from WidgetHolder

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;

				
		//
		
		void		_hideChildren(PackPanelSlot * pSlot, int nb);
		void		_unhideChildren(PackPanelSlot * pSlot, int nb);

		void		_refreshAllWidgets();
		void		_refreshChildGeo();
		void		_updatePreferredSize();
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const;


		SlotArray<PackPanelSlot> m_children;
		
		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		Size			m_preferredSize;
	
	};
	
	

} // namespace wg
#endif //WG_PACKPANEL_DOT_H
