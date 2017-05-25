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

#ifndef	WG_PANEL_DOT_H
#define	WG_PANEL_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_container.h>
#include <wg_skin.h>
#include <wg_slot.h>
#include <wg_childgroup.h>


namespace wg 
{
	
	class Patches;
	
	class Panel;
	typedef	StrongPtr<Panel>		Panel_p;
	typedef	WeakPtr<Panel>	Panel_wp;
	
	

	//____ PanelSlot ____________________________________________________________

	class PanelSlot : public Slot 	/** @private */
	{
	public:
		PanelSlot() : bVisible(false) {}

		Size		paddedPreferredSize() const { return pWidget->preferredSize() + padding; }
		Size		paddedMinSize() const { return pWidget->minSize() + padding; }
		Size		paddedMaxSize() const { return pWidget->maxSize() + padding; }
		int			paddedMatchingWidth( int paddedHeight ) const { return pWidget->matchingWidth( paddedHeight - padding.height() ) + padding.width(); }
		int			paddedMatchingHeight( int paddedWidth ) const { return pWidget->matchingHeight( paddedWidth - padding.width() ) + padding.height(); }
	
		bool		bVisible;
		Border		padding;
	};

	//____ PanelSlotsHolder ____________________________________________________________

	class PanelSlotsHolder : public ChildGroupHolder  /** @private */
	{
		virtual void	_hideSlots( PanelSlot * pSlot, int nb ) = 0;
		virtual void	_unhideSlots( PanelSlot * pSlot, int nb ) = 0;
		virtual void	_repadSlots( PanelSlot * pSlot, int nb, Border padding ) = 0;
	};

	//____ PanelChildren ________________________________________________________

	template<class SlotType, class HolderType> class PanelChildren : public ChildGroup<SlotType,HolderType>
	{
	public:
		PanelChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : ChildGroup<SlotType,HolderType>(pSlotArray, pHolder) {}

		bool	hide( int index ) 
		{
			if( index < 0 || index >= ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				return false;
				
			ChildGroup<SlotType,HolderType>::m_pHolder->_hideSlots( ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index), 1 );
			return true;
		};
		
		bool	unhide( int index )
		{
			if( index < 0 || index >= ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				return false;
				
			ChildGroup<SlotType,HolderType>::m_pHolder->_unhideSlots( ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index), 1 );
			return true;
		};

		bool		isVisible( int index ) 
		{ 
			if( index < 0 || index >= ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				return false;

			return ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index)->bVisible; 
		}
	
		bool		setPadding( int index, Border padding )
		{
			if( index < 0 || index >= ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				return false;
				
			ChildGroup<SlotType,HolderType>::m_pHolder->_repadSlots( ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index), 1, padding );
			return true;
		}

		Border		padding( int index ) const { return ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index)->padding; }
	};

	
	
	/**
	 * @brief	Base class for layout widgets.
	 *
	 * Base class for layout widgets.
	 */
	
	//____ Panel ________________________________________________________________
	
	class Panel : public Container
	{
	public:
		
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Panel_p		cast( Object * pObject );
		
		//.____ Behavior _______________________________________________________

		void		setTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
		bool		isTooltipGroup() const { return m_bTooltipGroup; }
	
		void		setMaskOp( MaskOp operation );
		MaskOp		maskOp() const { return m_maskOp; }
		
	
	protected:
		Panel();
		virtual ~Panel() {};
	
		bool			_isPanel() const;
		virtual void	_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );
		virtual void	_cloneContent( const Widget * _pOrg );
	
	
		bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
		MaskOp	m_maskOp;			// Specifies how container masks background.
	};
	
	
	

} // namespace wg
#endif //WG_PANEL_DOT_H
