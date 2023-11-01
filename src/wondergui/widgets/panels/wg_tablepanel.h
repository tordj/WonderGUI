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
#include <wg_dynamicslottable.h>

namespace wg
{
	class TablePanel;
	typedef	StrongPtr<TablePanel>		TablePanel_p;
	typedef	WeakPtr<TablePanel>			TablePanel_wp;


	//____ TablePanelSlot ____________________________________________________________

	class TablePanelSlot : public DynamicSlot
	{
		friend class TablePanel;
		template<class S> friend class SlotTable;
		friend class DynamicSlotTable<TablePanelSlot>;

	public:

	protected:
		TablePanelSlot(SlotHolder* pHolder) : DynamicSlot(pHolder) {}
		TablePanelSlot(TablePanelSlot&& o) = default;
		TablePanelSlot& operator=(TablePanelSlot&& o) = default;

		RectSPX		m_geo;
		SizeSPX		m_defaultSize;
	};

	//____ TablePanel _________________________________________________________

	class TablePanel : public Container
	{
	public:

		//.____ Creation __________________________________________

		static TablePanel_p	create() { return TablePanel_p(new TablePanel()); }
		static TablePanel_p	create(const Blueprint& blueprint) { return TablePanel_p(new TablePanel(blueprint)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis(Axis orientaiton);
		Axis			axis() const { return m_axis; }

		//.____ Behavior ________________________________________________________

		void			setColumnLayout(PackLayout* pLayout);
		PackLayout_p	columnLayout() const { return m_pColumnLayout; }

		void			setRowLayout(PackLayout* pLayout);
		PackLayout_p	rowLayout() const { return m_pRowLayout; }

		//.____ Misc ________________________________________________________________

		bool			setColumnWeight(int index, int amount, float weight);
		bool			setColumnWeight(int index, int amount, std::initializer_list<float> weights);

		bool			setRowWeight(int index, int amount, float weight);
		bool			setRowWeight(int index, int amount, std::initializer_list<float> weights);


		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale) const override;
		spx				_matchingWidth(spx height, int scale) const override;

		SizeSPX			_defaultSize(int scale) const override;

	protected:

		TablePanel();
		template< class BP> TablePanel(const BP& bp) : Panel(bp)
		{
		}

		virtual ~TablePanel();

		void		_refreshGeometries();
		SizeSPX		_calcDefaultContentSize(int scale) const;


		// Overloaded from Widget

		void		_resize(const SizeSPX& size, int scale) override;

		// Overloaded from Container

		const TypeInfo& _slotTypeInfo(const StaticSlot* pSlot) const override;

		Widget*		_firstChild() const override;
		Widget*		_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordSPX	_childPos(const StaticSlot* pSlot) const override;

		void		_childRequestRender(StaticSlot* pSlot) override;
		void		_childRequestRender(StaticSlot* pSlot, const RectSPX& rect) override;
		void		_childRequestResize(StaticSlot* pSlot) override;

		Widget*		_prevChild(const StaticSlot* pSlot) const override;
		Widget*		_nextChild(const StaticSlot* pSlot) const override;

		void		_releaseChild(StaticSlot* pSlot) override;
		void		_replaceChild(StaticSlot* pSlot, Widget* pNewChild) override;

		Axis					m_axis = Axis::Y;

		PackLayout_p			m_pColumnLayout;
		PackLayout_p			m_pRowLayout;

		std::vector<spx>		m_columnWidths;
		std::vector<spx>		m_rowHeights;

		SizeSPX					m_defaultContentSize;
	};

}
#endif //WG_TABLE_PANEL