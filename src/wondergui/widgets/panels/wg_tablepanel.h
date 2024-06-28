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

#ifndef WG_TABLEPANEL_DOT_H
#define WG_TABLEPANEL_DOT_H
#pragma once


#include <wg_container.h>
#include <wg_slottable.h>
#include <wg_dynamicvector.h>

namespace wg
{

	class TablePanel;
	typedef	StrongPtr<TablePanel>	TablePanel_p;
	typedef	WeakPtr<TablePanel>		TablePanel_wp;


//____ TablePanelRow ______________________________________________________

	class TablePanelRow
	{
	public:
		
		struct Blueprint
		{
			bool	visible = true;
			float	weight = 1.f;
		};
	
		inline void		hide();
		inline void		unhide();

		inline void		setVisible(bool bVisible);
		inline bool		isVisible() const { return m_bVisible; }
		
		void			setWeight(float weight);
		inline float	weight() const { return m_weight; }
		
	protected:
		bool	m_bVisible = true;
		float	m_weight = 1.f;
		
		TablePanel *	m_pTable;
	};

	//____ TablePanelColumn ______________________________________________________

	class TablePanelColumn
	{
	public:

		struct Blueprint
		{
			bool	visible = true;
			float	weight = 1.f;
		};
		
		inline void		hide();
		inline void		unhide();

		inline void		setVisible(bool bVisible);
		inline bool		isVisible() const { return m_bVisible; }
		
		void			setWeight(float weight);
		inline float	weight() const { return m_weight; }
		
	protected:
		bool	m_bVisible = true;
		float	m_weight = 1.f;

		TablePanel *	m_pHolder;
	};



	//____ TablePanel _________________________________________________________

	class TablePanel : 	public Container, SlotTable::Holder, 
						protected DynamicVector<TablePanelRow>::Holder,
						protected DynamicVector<TablePanelColumn>::Holder
	{

		friend class TablePanelRow;
		friend class TablePanelColumn;
		
	public:

		//.____ Blueprint _____________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			disabled = false;
			bool			dropTarget = false;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::Undefined;
			MaskOp			maskOp = MaskOp::Recurse;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Undefined;
			bool			selectable = true;
			Skin_p			skin;
			bool			stickyFocus = false;
			bool			tabLock = false;
			String			tooltip;

		};
		
		//.____ Creation __________________________________________

		static TablePanel_p	create() { return TablePanel_p(new TablePanel()); }
		static TablePanel_p	create( const Blueprint& blueprint ) { return TablePanel_p(new TablePanel(blueprint)); }

		//.____ Components _______________________________________

		DynamicVector<TablePanelRow>	rows;
		DynamicVector<TablePanelColumn>	columns;
		SlotTable						slots;
		
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		
		//.____ Behavior ________________________________________________________

		void			setRowLayout(PackLayout* pLayout);
		PackLayout_p	rowLayout() const { return m_pRowLayout; }

		void			setColumnLayout(PackLayout* pLayout);
		PackLayout_p	columnLayout() const { return m_pColumnLayout; }

		void			setRowSpacing( pts before, pts between, pts after );
		void			setColumnSpacing( pts before, pts between, pts after );

		void			setRowSkins(  );

		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale) const override;
		spx				_matchingWidth(spx height, int scale) const override;

		SizeSPX			_defaultSize(int scale) const override;

		
	protected:
		TablePanel();
		template<class BP> TablePanel(const BP& bp) : Container(bp), rows(this), columns(this), slots(this,this)
		{
		}

		virtual ~TablePanel();

		// Overloaded from Container


		RectSPX			_slotGeo( const StaticSlot * pSlot ) const override;				///< Get the local position of a child.
		void			_childOverflowChanged( StaticSlot * pSlot, const BorderSPX& oldOverflow, const BorderSPX& newOverflow ) override;

		bool			_isChildVisible( const StaticSlot * pSlot ) const override;

		void			_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;
		void			_childRequestResize(StaticSlot * pSlot) override;

		Widget *		_prevChild(const StaticSlot * pSlot) const override;
		Widget *		_nextChild(const StaticSlot * pSlot) const override;

		void			_releaseChild(StaticSlot * pSlot) override;
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		Widget *		_firstChild() const override;
		Widget *		_lastChild() const override;

		void			_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void			_nextSlotWithGeo( SlotWithGeo& package ) const override;

		
		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		SizeSPX			_defaultSize(int scale) const override;
		SizeSPX			_minSize(int scale) const override;
		SizeSPX			_maxSize(int scale) const override;


		void			_resize( const SizeSPX& size, int scale ) override;
		
		// For vector holder
		
		void			_didAddEntries(TablePanelRow * pEntry, int nb) override;
		void			_didMoveEntries(TablePanelRow * pFrom, TablePanelRow * pTo, int nb) override;
		void			_willEraseEntries(TablePanelRow * pEntry, int nb) override;

		void			_didAddEntries(TablePanelColumn * pEntry, int nb) override;
		void			_didMoveEntries(TablePanelColumn * pFrom, TablePanelColumn * pTo, int nb) override;
		void			_willEraseEntries(TablePanelColumn * pEntry, int nb) override;

		//
		
		virtual void	_refreshSlots(int ofs, Axis axis, int nSlots) override;
		virtual Object*	_object() override;
		
		//
		
		void			_hideRows(TablePanelRow * pStart, int nb);
		void			_unhideRows(TablePanelRow * pStart, int nb);

		void			_hideColumns(TablePanelColumn * pStart, int nb);
		void			_unhideColumns(TablePanelColumn * pStart, int nb);

	};


//____ TablePanelRow::hide() __________________________________________________

void TablePanelRow::hide()
{
	m_pTable->_hideRows(this, 1);
}

//____ TablePanelRow::unhide() __________________________________________________

void TablePanelRow::unhide()
{
	m_pTable->_unhideRows(this, 1);
}

//____ TablePanelRow::setVisible() __________________________________________________

void TablePanelRow::setVisible(bool bVisible)
{
	if (bVisible)
		m_pTable->_unhideRows(this, 1);
	else
		m_pTable->_hideRows(this, 1);
}

//____ TablePanelColumn::hide() __________________________________________________

void TablePanelColumn::hide()
{
	m_pTable->_hideColumns(this, 1);
}

//____ TablePanelColumn::unhide() __________________________________________________

void TablePanelColumn::unhide()
{
	m_pTable->_unhideColumns(this, 1);
}

//____ TablePanelColumn::setVisible() __________________________________________________

void TablePanelColumn::setVisible(bool bVisible)
{
	if (bVisible)
		m_pTable->_unhideColumns(this, 1);
	else
		m_pTable->_hideColumns(this, 1);
}

} // namespace wg
#endif //WG_TABLEPANEL_DOT_H
