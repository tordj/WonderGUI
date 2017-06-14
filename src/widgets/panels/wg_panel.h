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
#include <wg_hideablechildren.h>


namespace wg 
{
	
	class Patches;
	
	class Panel;
	typedef	StrongPtr<Panel>	Panel_p;
	typedef	WeakPtr<Panel>		Panel_wp;
	
	

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

	class PanelSlotsHolder : public HideableChildrenHolder  /** @private */
	{
		virtual void	_repadSlots( PanelSlot * pSlot, int nb, Border padding ) = 0;
	};

	//____ PanelChildren ________________________________________________________

	template<class SlotType, class HolderType> class PanelChildren : public HideableChildren<SlotType,HolderType>
	{
	public:
		using		iterator = SlotIterator<SlotType>;
		using		ChildGroup<SlotType,HolderType>::m_pSlotArray;
		using		ChildGroup<SlotType,HolderType>::m_pHolder;
		
		PanelChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : HideableChildren<SlotType,HolderType>(pSlotArray, pHolder) {}
	
		bool		setPadding( int index, Border padding )
		{
			if( index < 0 || index >= m_pSlotArray->size() )
				return false;
				
			m_pHolder->_repadSlots( m_pSlotArray->slot(index), 1, padding );
			return true;
		}

		bool		setPadding( iterator it, Border padding )
		{
			//TODO: Assert

			m_pHolder->_repadSlots( it._slot(), 1, padding );
			return true;
		}


		Border		padding( int index ) const { return m_pSlotArray->slot(index)->padding; }
		Border		padding( iterator it ) const { return it._slot()->padding; }
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
