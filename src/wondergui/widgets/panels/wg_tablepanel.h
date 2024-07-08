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
		friend class TablePanel;
	public:
		struct Blueprint
		{
			bool	visible = true;
			float	weight = 1.f;
		};
	
		TablePanelRow() {};
		TablePanelRow( const Blueprint& bp ) : m_bVisible(bp.visible), m_weight(bp.weight) {}
		
		inline void		hide();
		inline void		unhide();

		inline void		setVisible(bool bVisible);
		inline bool		isVisible() const { return m_bVisible; }
		
		inline void		setWeight(float weight);
		inline float	weight() const { return m_weight; }
		
	protected:
		
		struct Cache
		{
			spx		defaultHeight;
			spx		minHeight;
			spx		maxHeight;
			spx		heightForColumnWidth;		// Height needed for current column widths.
		};

		TablePanel*		m_pTable;
		bool			m_bVisible = true;
		bool			m_bModified = false;
		float			m_weight = 1.f;
		spx				m_height;
		Cache			m_cache;

	};

	//____ TablePanelColumn ______________________________________________________

	class TablePanelColumn
	{
		friend class TablePanel;
	public:

		struct Blueprint
		{
			bool	visible = true;
			float	weight = 1.f;
		};

		TablePanelColumn() {};
		TablePanelColumn( const Blueprint& bp ) : m_bVisible(bp.visible), m_weight(bp.weight) {}
		
		inline void		hide();
		inline void		unhide();

		inline void		setVisible(bool bVisible);
		inline bool		isVisible() const { return m_bVisible; }
		
		inline void		setWeight(float weight);
		inline float	weight() const { return m_weight; }
		
	protected:

		struct Cache
		{
			spx		defaultWidth;
			spx		minWidth;
			spx		maxWidth;
		};

		bool			m_bVisible = true;
		bool			m_bModified = false;
		float			m_weight = 1.f;

		TablePanel *	m_pTable;
		spx				m_width;
		
		Cache			m_cache;
	};



	//____ TablePanel _________________________________________________________

	class TablePanel : 	public Container, SlotTable::Holder, 
						protected DynamicVector<TablePanelRow>::Holder,
						protected DynamicVector<TablePanelColumn>::Holder
	{

		friend class TablePanelRow;
		friend class TablePanelColumn;
		
	public:

		using		row_iterator = DynamicVector<TablePanelRow>::iterator;
		using		column_iterator = DynamicVector<TablePanelColumn>::iterator;

		
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

		
		//.____ Appearance _____________________________________________________
		
		void			hideRows(int index, int amount);
		void			hideRows(row_iterator beg, row_iterator end);

		void			unhideRows(int index, int amount);
		void			unhideRows(row_iterator beg, row_iterator end);

		void			hideColumns(int index, int amount);
		void			hideColumns(column_iterator beg, column_iterator end);

		void			setRowWeight(int index, int amount, float weight);
		void			setRowWeight(row_iterator  beg, row_iterator end, float weight);
		void			setRowWeight(int index, int amount, std::initializer_list<float> weights);
		void			setRowWeight(row_iterator beg, row_iterator end, std::initializer_list<float> weights);
		
		void			unhideColumns(int index, int amount);
		void			unhideColumns(column_iterator beg, column_iterator end);

		void			setColumnWeight(int index, int amount, float weight);
		void			setColumnWeight(column_iterator  beg, column_iterator end, float weight);
		void			setColumnWeight(int index, int amount, std::initializer_list<float> weights);
		void			setColumnWeight(column_iterator beg, column_iterator end, std::initializer_list<float> weights);
		
		//.____ Behavior ________________________________________________________

		void			setRowLayout(PackLayout* pLayout);
		PackLayout_p	rowLayout() const { return m_pLayoutY; }

		void			setColumnLayout(PackLayout* pLayout);
		PackLayout_p	columnLayout() const { return m_pLayoutX; }

		void			setRowSpacing( pts between ) { setRowSpacing(0,between,0); }
		void			setRowSpacing( pts before, pts between, pts after );

		void			setColumnSpacing( pts between ) { setColumnSpacing(0,between,0); }
		void			setColumnSpacing( pts before, pts between, pts after );

		void			setRowSkins( Skin * pSkin1, Skin * pSkin2 = nullptr );

		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale) const override;
		spx				_matchingWidth(spx height, int scale) const override;

		SizeSPX			_defaultSize(int scale) const override;
		SizeSPX			_minSize(int scale) const override;
		SizeSPX			_maxSize(int scale) const override;

		
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

		void			_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void			_resize( const SizeSPX& size, int scale ) override;
		
		// For DynamicVector holder
		
		void			_didAddEntries(TablePanelRow * pEntry, int nb) override;
		void			_didMoveEntries(TablePanelRow * pFrom, TablePanelRow * pTo, int nb) override;
		void			_willEraseEntries(TablePanelRow * pEntry, int nb) override;

		void			_didAddEntries(TablePanelColumn * pEntry, int nb) override;
		void			_didMoveEntries(TablePanelColumn * pFrom, TablePanelColumn * pTo, int nb) override;
		void			_willEraseEntries(TablePanelColumn * pEntry, int nb) override;
		
		// For TableSlot holder
		
		void			_refreshSlots(int ofs, Axis axis, int nSlots) override;
		Object*			_object() override;
		
		//
		
		void			_hideRows(TablePanelRow * pStart, int nb);
		void			_unhideRows(TablePanelRow * pStart, int nb);

		void			_hideColumns(TablePanelColumn * pStart, int nb);
		void			_unhideColumns(TablePanelColumn * pStart, int nb);

		void			_reweightRows(TablePanelRow * pEntry, int nb, float weight);
		void			_reweightRows(TablePanelRow * pEntry, int nb, const float * pWeights);

		void			_reweightColumns(TablePanelColumn * pEntry, int nb, float weight);
		void			_reweightColumns(TablePanelColumn * pEntry, int nb, const float * pWeights);

		//
		
		bool			_refreshColumns();
		bool			_refreshRows();
		
		bool			_refreshRowHeightForColumnWidth();
		
		void			_updateAllChildSizes();
		void			_updateModifiedChildSizes();
		
		bool			_refreshRowCache( int row, TablePanelRow::Cache& cache, int scale ) const;
		bool			_refreshColumnCache( int column, TablePanelColumn::Cache& cache, int scale ) const;
		
		SizeSPX			_sumOfPadding(int scale) const;

		void			_updateMinDefaultSize();
		
		SizeSPX			m_minSize;
		SizeSPX			m_defaultSize;
		
		PackLayout_p	m_pLayoutX;
		PackLayout_p	m_pLayoutY;
		
		pts				m_spacingX[3] = {0,0,0};
		pts				m_spacingY[3] = {0,0,0};
		
		Skin_p			m_pRowSkins[2];

		int				m_nVisibleColumns = 0;
		int				m_nVisibleRows = 0;

		
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

//____ TablePanelRow::setWeight() __________________________________________________

void TablePanelRow::setWeight(float weight)
{
	if (weight != m_weight)
		m_pTable->_reweightRows(this, 1, weight);
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

//____ TablePanelColumn::setWeight() __________________________________________________

void TablePanelColumn::setWeight(float weight)
{
	if (weight != m_weight)
		m_pTable->_reweightColumns(this, 1, weight);
}

} // namespace wg
#endif //WG_TABLEPANEL_DOT_H
