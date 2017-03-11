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

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_SLOT_DOT_H
#	include <wg_slot.h>
#endif

#ifndef WG_CHILDGROUP_DOT_H
#	include <wg_childgroup.h>
#endif


namespace wg 
{
	
	class Patches;
	
	class Panel;
	typedef	StrongPtr<Panel,Container_p>		Panel_p;
	typedef	WeakPtr<Panel,Container_wp>	Panel_wp;
	
	class PanelHook;
	typedef	HookTypePtr<PanelHook,Hook_p>	PanelHook_p;
	

	//____ PanelSlot ____________________________________________________________
	
	class PanelSlot : public Slot
	{
	public:
		PanelSlot() : bVisible(false) {}

		Size		_paddedPreferredSize() const { return pWidget->preferredSize() + padding; }
		Size		_paddedMinSize() const { return pWidget->minSize() + padding; }
		Size		_paddedMaxSize() const { return pWidget->maxSize() + padding; }
		int			_paddedMatchingWidth( int paddedHeight ) const { return pWidget->matchingWidth( paddedHeight - padding.height() ) + padding.width(); }
		int			_paddedMatchingHeight( int paddedWidth ) const { return pWidget->matchingHeight( paddedWidth - padding.width() ) + padding.height(); }
		bool		_isVisible() const { return bVisible; }

	
		bool		bVisible;
		Border		padding;
	};

	//____ PanelChildrenHolder ____________________________________________________________

	class PanelSlotsHolder : public ChildGroupHolder
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

		void	hide( int index ) 
		{
			if( index >= 0 || index < ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				ChildGroup<SlotType,HolderType>::m_pHolder->_hideSlots( ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index), 1 );
		};
		
		void		unhide( int index )
		{
			if( index >= 0 || index < ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				ChildGroup<SlotType,HolderType>::m_pHolder->_unhideSlots( ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index), 1 );
		};

		bool		isVisible( int index ) { return ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index)->bVisible; }
	
		bool		setPadding( int index, Border padding )
		{
			if( index >= 0 || index < ChildGroup<SlotType,HolderType>::m_pSlotArray->size() )
				ChildGroup<SlotType,HolderType>::m_pHolder->_repadSlots( ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index), 1, padding );
		}

		Border		padding( int index ) const { return ChildGroup<SlotType,HolderType>::m_pSlotArray->slot(index)->padding; }
	};

	
	//____ PanelHook ____________________________________________________________
	
	class PanelHook : public Hook
	{
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static PanelHook_p	cast( const Hook_p& pInterface );
	
		PanelHook_p	prev() const { return static_cast<PanelHook*>(_prevHook()); }
		PanelHook_p	next() const { return static_cast<PanelHook*>(_nextHook()); }
		Panel_p		parent() const;
	
		virtual bool	setVisible( bool bVisible );
		bool			isVisible() { return m_bVisible; }
	
		virtual bool	setPadding( Border padding );
		Border		padding() const { return m_padding; }
	
	protected:
		PanelHook() : m_bVisible(true) {}
		 virtual ~PanelHook() {};
	
		Size		_paddedPreferredSize() const;
		Size		_paddedMinSize() const;
		Size		_paddedMaxSize() const;
		int			_paddedMatchingWidth( int paddedHeight ) const;
		int			_paddedMatchingHeight( int paddedWidth ) const;
		bool		_isVisible() const { return m_bVisible; }				// Overloading Hook::_isVisible().
	
		Size		_sizeFromPolicy( Size specifiedSize, SizePolicy widthPolicy, SizePolicy heightPolicy ) const;
	
		bool			m_bVisible;
		Border		m_padding;
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
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Panel_p	cast( const Object_p& pObject );
		
		void		setTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
		bool		isTooltipGroup() const { return m_bTooltipGroup; }
	
		void		setMaskOp( MaskOp operation );
		MaskOp	maskOp() const { return m_maskOp; }
		
		// Overloaded from IWidgets
	
	
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
