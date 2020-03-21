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
#include <wg_selectableslot.h>
#include <wg_cselectableslotvector.h>

namespace wg
{

	class List;
	typedef	StrongPtr<List>		List_p;
	typedef	WeakPtr<List>		List_wp;


	//____ List _________________________________________________________________

	/**
	* @brief	Base class for widgets providing lists of selectable entries.
	*
	* List is the base class for widgets that provide lists of selectable entries.
	*
	*/

	class List : public Container
	{
	public:

		//____ Slot ____________________________________________________________

		class Slot : public SelectableSlot
		{
			friend class List;
		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

		protected:
			Slot(SlotHolder * pHolder) : SelectableSlot(pHolder) {}
		};


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

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

		virtual void	_receive( Msg * pMsg ) override;

		void			_cloneContent( const Widget * _pOrg ) override;

		int				_selectSlot(Slot * pSlot, bool bPostMsg) { return _setSlotSelection(pSlot, _nextSlot(pSlot), true, bPostMsg); }
		int				_selectSlots(Slot * pBegin, Slot * pEnd, bool bPostMsg) { return _setSlotSelection(pBegin, pEnd, true, bPostMsg); }
		int				_unselectSlots(Slot * pBegin, Slot * pEnd, bool bPostMsg) { return _setSlotSelection(pBegin, pEnd, false, bPostMsg); }

		Widget*			_firstChild() const override;
		Widget*			_lastChild() const override;


		virtual int		_setSlotSelection(Slot * pBegin, Slot * pEnd, bool bSelect, bool bPostMsg);

		virtual int		_flipSelection( Slot * pBegin, Slot * pEnd, bool bPostMsg );

		virtual Slot * _findEntry( const CoordI& ofs ) = 0;
		virtual void	_getEntryGeo( RectI& geo, const Slot * pSlot ) const = 0;

		virtual RectI	_listArea() const = 0;										// Area for the entries (contentRect minus header).
		virtual RectI	_listWindow() const = 0;
		virtual RectI	_listCanvas() const = 0;

		virtual void	_onEntrySkinChanged( SizeI oldPadding, SizeI newPadding ) = 0;
		virtual void	_onLassoUpdated( const RectI& oldLasso, const RectI& newLasso ) = 0;


		virtual Slot * _beginSlots() const = 0;
		virtual Slot * _endSlots() const = 0;

		inline Slot * _nextSlot( Slot * pSlot ) const { return (Slot*) (((char*)pSlot)+m_sizeOfSlot); }
		inline const Slot * _nextSlot( const Slot * pSlot, int sizeOf ) const { return (const Slot*) (((char*)pSlot)+m_sizeOfSlot); }

		inline Slot * _prevSlot( Slot * pSlot ) const { return (Slot*) (((char*)pSlot)-m_sizeOfSlot); }
		inline const Slot * _prevSlot( const Slot * pSlot ) const { return (const Slot*) (((char*)pSlot)-m_sizeOfSlot); }

		int				m_sizeOfSlot;

		SelectMode		m_selectMode;
		Skin_p			m_pEntrySkin[2];
		Skin_p			m_pLassoSkin;
		bool			m_bOpaqueEntries;

		CoordI			m_lassoBegin;
		CoordI			m_lassoEnd;

		Widget_wp		m_pFocusedChild;
		Widget_wp		m_pHoveredChild;
	};




} // namespace wg
#endif //WG_LIST_DOT_H
