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
#pragma once

#include <wg_widget.h>
#include <wg_container.h>
#include <wg_skin.h>
#include <wg_hideablechildren.h>

namespace wg 
{
	
	class List;
	typedef	StrongPtr<List>		List_p;
	typedef	WeakPtr<List>		List_wp;
		
	//____ ListSlot ____________________________________________________________

	class ListSlot : public Slot		/** @private */
	{
	public:
		ListSlot() : bVisible(false) {}

		bool		bVisible;
	};

	//____ ListChildrenHolder ____________________________________________________________

	class ListChildrenHolder : public HideableChildrenHolder		/** @private */
	{
		virtual void	_hideSlots(ListSlot * pSlot, int nb) = 0;
		virtual void	_unhideSlots(ListSlot * pSlot, int nb) = 0;
	};

	//____ ListChildren ________________________________________________________

	template<class SlotType, class HolderType> class ListChildren : public HideableChildren<SlotType, HolderType>
	{
	public:
		using		iterator = SlotIterator<SlotType>;
		using		ChildGroup<SlotType,HolderType>::m_pSlotArray;
		using		ChildGroup<SlotType,HolderType>::m_pHolder;

		/** @private */

		ListChildren(SlotArray<SlotType> * pSlotArray, HolderType * pHolder) : HideableChildren<SlotType, HolderType>(pSlotArray, pHolder) {}

		//.____ Control _______________________________________________________

		void	select(int index)
		{
			//TODO: Assert

			ListSlot * pSlot = m_pSlotArray->slot(index);
			if( pSlot->bVisible)
				m_pHolder->_selectSlots(pSlot, 1);
		}

		void	select(iterator it)
		{
			//TODO: Assert

			ListSlot * pSlot = it._slot();
			if( pSlot->bVisible)
				m_pHolder->_selectSlots(pSlot, 1);
		}

		void	unselect(int index)
		{
			//TODO: Assert

			ListSlot * pSlot = m_pSlotArray->slot(index);
			if (pSlot->bVisible)
				m_pHolder->_unselectSlots(pSlot, 1);
		}

		void	unselect(iterator it)
		{
			//TODO: Assert

			ListSlot * pSlot = it._slot();
			if (pSlot->bVisible)
				m_pHolder->_unselectSlots(pSlot, 1);
		}


		bool	isSelected(int index)
		{
			//TODO: Assert
			
			return m_pSlotArray->slot(index)->pWidget->state().isSelected();
		}

		bool	isSelected(iterator it)
		{
			//TODO: Assert
			
			return it._slot()->pWidget->state().isSelected();
		}

	};


	//____ List _________________________________________________________________
	
	/**
	* @brief	Base class for widgets providing lists of selectable entries.
	* 
	* List is the base class for widgets that provide lists of selectable entries.
	* 
	*/ 

	class List : public Container, protected ListChildrenHolder
	{
	public:

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static List_p		cast( Object * pObject );

		//.____ Appearance _________________________________________________

		virtual void		setEntrySkin( Skin * pSkin );
		virtual bool		setEntrySkin( Skin * pOddEntrySkin, Skin * pEvenEntrySkin );
		Skin_p				oddEntrySkin() const { return m_pEntrySkin[0]; }
		Skin_p				evenEntrySkin() const { return m_pEntrySkin[1]; }
	
		virtual void		setLassoSkin( Skin * pSkin );
		Skin_p				lassoSkin() const { return m_pLassoSkin; }

		//.____ Behavior ________________________________________________________

		virtual bool		setSelectMode( SelectMode mode );
		SelectMode			selectMode() const { return m_selectMode; }
	
	
	protected:
	
		List();
		virtual ~List();
	
		virtual void	_receive( Msg * pMsg );

		void			_cloneContent( const Widget * _pOrg );
	
		int				_selectSlot(ListSlot * pSlot, bool bPostMsg) { return _setSlotSelection(pSlot, _nextSlot(pSlot), true, bPostMsg); }
		int				_selectSlots(ListSlot * pBegin, ListSlot * pEnd, bool bPostMsg) { return _setSlotSelection(pBegin, pEnd, true, bPostMsg); }
		int				_unselectSlots(ListSlot * pBegin, ListSlot * pEnd, bool bPostMsg) { return _setSlotSelection(pBegin, pEnd, false, bPostMsg); }

		Widget*			_firstChild() const;
		Widget*			_lastChild() const;


		virtual void	_didAddSlots(Slot * pSlot, int nb);
		virtual void	_willRemoveSlots(Slot * pSlot, int nb);
		virtual void	_hideSlots(ListSlot * pSlot, int nb);
		virtual void	_unhideSlots(ListSlot * pSlot, int nb);


		virtual int		_setSlotSelection(ListSlot * pBegin, ListSlot * pEnd, bool bSelect, bool bPostMsg);

		virtual int		_flipSelection( ListSlot * pBegin, ListSlot * pEnd, bool bPostMsg );

		virtual ListSlot * _findEntry( const Coord& ofs ) = 0;
		virtual void	_getEntryGeo( Rect& geo, const ListSlot * pSlot ) const = 0;
	
		virtual Rect	_listArea() const = 0;										// Area for the entries (contentRect minus header).
		virtual Rect	_listWindow() const = 0;
		virtual Rect	_listCanvas() const = 0;
	
		virtual void	_onEntrySkinChanged( Size oldPadding, Size newPadding ) = 0;
		virtual void	_onLassoUpdated( const Rect& oldLasso, const Rect& newLasso ) = 0;


		virtual ListSlot * _beginSlots() const = 0;
		virtual ListSlot * _endSlots() const = 0;

		inline ListSlot * _nextSlot( ListSlot * pSlot ) const { return (ListSlot*) (((char*)pSlot)+m_sizeOfSlot); }
		inline const ListSlot * _nextSlot( const ListSlot * pSlot, int sizeOf ) const { return (const ListSlot*) (((char*)pSlot)+m_sizeOfSlot); }

		inline ListSlot * _prevSlot( ListSlot * pSlot ) const { return (ListSlot*) (((char*)pSlot)-m_sizeOfSlot); }
		inline const ListSlot * _prevSlot( const ListSlot * pSlot ) const { return (const ListSlot*) (((char*)pSlot)-m_sizeOfSlot); }

		int				m_sizeOfSlot;

		SelectMode		m_selectMode;
		Skin_p			m_pEntrySkin[2];
		Skin_p			m_pLassoSkin;
		bool			m_bOpaqueEntries;
	
		Coord			m_lassoBegin;
		Coord			m_lassoEnd;
	
		Widget_wp		m_pFocusedChild;
		Widget_wp		m_pHoveredChild;
	};
	
	
	

} // namespace wg
#endif //WG_LIST_DOT_H
