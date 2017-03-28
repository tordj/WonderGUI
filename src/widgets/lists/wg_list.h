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

#ifndef	WG_LIST_DOT_H
#define	WG_LIST_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#include <wg_childgroup.h>

namespace wg 
{
	
	class List;
	typedef	StrongPtr<List,Container_p>		List_p;
	typedef	WeakPtr<List,Container_wp>		List_wp;
		
	//____ ListSlot ____________________________________________________________

	class ListSlot : public Slot
	{
	public:
		ListSlot() : bVisible(false) {}

		bool		bVisible;
	};

	//____ ListChildrenHolder ____________________________________________________________

	class ListChildrenHolder : public ChildGroupHolder
	{
		virtual void	_hideSlots(ListSlot * pSlot, int nb) = 0;
		virtual void	_unhideSlots(ListSlot * pSlot, int nb) = 0;
	};

	//____ ListChildren ________________________________________________________

	template<class SlotType, class HolderType> class ListChildren : public ChildGroup<SlotType, HolderType>
	{
	public:
		ListChildren(SlotArray<SlotType> * pSlotArray, HolderType * pHolder) : ChildGroup<SlotType, HolderType>(pSlotArray, pHolder) {}

		void	hide(int index)
		{
			if (index >= 0 || index < ChildGroup<SlotType, HolderType>::m_pSlotArray->size())
				ChildGroup<SlotType, HolderType>::m_pHolder->_hideSlots(ChildGroup<SlotType, HolderType>::m_pSlotArray->slot(index), 1);
		};

		void	unhide(int index)
		{
			if (index >= 0 || index < ChildGroup<SlotType, HolderType>::m_pSlotArray->size())
				ChildGroup<SlotType, HolderType>::m_pHolder->_unhideSlots(ChildGroup<SlotType, HolderType>::m_pSlotArray->slot(index), 1);
		};

		bool	isVisible(int index) 
		{ 
			if (index >= 0 || index < ChildGroup<SlotType, HolderType>::m_pSlotArray->size())
				return ChildGroup<SlotType, HolderType>::m_pSlotArray->slot(index)->bVisible;

			return false;
		}

		void	select(int index)
		{
			if (index >= 0 || index < ChildGroup<SlotType, HolderType>::m_pSlotArray->size())
			{
				ListSlot * pSlot = ChildGroup<SlotType, HolderType>::m_pSlotArray->slot(index);
				if( pSlot->bVisible)
					ChildGroup<SlotType, HolderType>::m_pHolder->_selectSlots(pSlot, 1);
			}
		}

		void	unselect(int index)
		{
			if (index >= 0 || index < ChildGroup<SlotType, HolderType>::m_pSlotArray->size())
			{
				ListSlot * pSlot = ChildGroup<SlotType, HolderType>::m_pSlotArray->slot(index);
				if (pSlot->bVisible)
					ChildGroup<SlotType, HolderType>::m_pHolder->_unselectSlots(pSlot, 1);
			}
		}

		bool	isVisible(int index)
		{
			if (index >= 0 || index < ChildGroup<SlotType, HolderType>::m_pSlotArray->size())
				return ChildGroup<SlotType, HolderType>::m_pSlotArray->slot(index)->pWidget->state().isSelected();

			return false;
		}


	};


	//____ List _________________________________________________________________
	
	class List : public Container
	{
		friend class ListHook;
	public:

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static List_p		cast( const Object_p& pObject );

		//.____ Appearance _________________________________________________

		virtual void		setEntrySkin( const Skin_p& pSkin );
		virtual bool		setEntrySkin( const Skin_p& pOddEntrySkin, const Skin_p& pEvenEntrySkin );
		Skin_p				oddEntrySkin() const { return m_pEntrySkin[0]; }
		Skin_p				evenEntrySkin() const { return m_pEntrySkin[1]; }
	
		virtual void		setLassoSkin( const Skin_p& pSkin );
		Skin_p				lassoSkin() const { return m_pLassoSkin; }

		//.____ Behavior ________________________________________________________

		virtual bool		setSelectMode( SelectMode mode );
		SelectMode			selectMode() const { return m_selectMode; }
	
	
	protected:
	
		List();
		virtual ~List();
	
		virtual void	_receive( const Msg_p& pMsg );

		void			_cloneContent( const Widget * _pOrg );
	
		virtual int		_selectSlots( ListSlot * pSlot, int nb, bool bPostMsg );
		virtual int		_unselectSlots(ListSlot * pSlot, int nb, bool bPostMsg);
		virtual int		_flipSelection( ListSlot * pSlot, int nb, bool bPostMsg );

		virtual ListSlot * _findEntry( const Coord& ofs ) = 0;
		virtual void	_getEntryGeo( Rect& geo, const ListSlot * pSlot ) const = 0;
	
		virtual Rect	_listArea() const = 0;										// Area for the entries (contentRect minus header).
		virtual Rect	_listWindow() const = 0;
		virtual Rect	_listCanvas() const = 0;
	
		virtual void	_onEntrySkinChanged( Size oldPadding, Size newPadding ) = 0;
		virtual void	_onLassoUpdated( const Rect& oldLasso, const Rect& newLasso ) = 0;


		SelectMode		m_selectMode;
		Skin_p			m_pEntrySkin[2];
		Skin_p			m_pLassoSkin;
		bool			m_bOpaqueEntries;
	
		Coord			m_lassoBegin;
		Coord			m_lassoEnd;
	
		int				m_focusedEntry;					// -1 = none
		int				m_hoveredEntry;					// -1 = none
	};
	
	
	

} // namespace wg
#endif //WG_LIST_DOT_H
