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

#include <wg_tablepanel.h>
#include <wg_dynamicslotvector.impl.h>
#include <wg_panel.impl.h>


namespace wg
{
	using namespace Util;

	template class DynamicSlotTable<TablePanelSlot>;
	template class Panel<TablePanelSlot>;

	const TypeInfo TablePanel::TYPEINFO = { "TablePanel", &Panel::TYPEINFO };
	const TypeInfo TablePanelSlot::TYPEINFO = { "TablePanelSlot", &DynamicSlot::TYPEINFO };



	//____ constructor ____________________________________________________________

	TablePanel::TablePanel()
	{
		m_bSiblingsOverlap = false;
	}

	//____ Destructor _____________________________________________________________

	TablePanel::~TablePanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TablePanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setAxis() ______________________________________________________

	void TablePanel::setAxis(Axis axis)
	{
		if (m_axis != axis)
		{
			m_axis = axis;
			_refreshGeometries();

		}
	}

	//____ setColumnLayout() _______________________________________________________

	void TablePanel::setColumnLayout(PackLayout* pLayout)
	{
		if (m_pColumnLayout.rawPtr() != pLayout)
		{
			m_pColumnLayout = pLayout;
			_refreshGeometries();

		}
	}

	//____ setRowLayout() _______________________________________________________

	void TablePanel::setRowLayout(PackLayout* pLayout)
	{
		if (m_pRowLayout.rawPtr() != pLayout)
		{
			m_pRowLayout = pLayout;
			_refreshGeometries();

		}
	}

	spx TablePanel::_matchingHeight(spx width, int scale) const
	{

	}

	spx TablePanel::_matchingWidth(spx height, int scale) const
	{

	}

	SizeSPX TablePanel::_defaultSize(int scale) const
	{
		if (scale != m_scale)
		{
			return _calcDefaultContentSize(scale) + m_skin.contentBorderSize(scale);
		}
		else
			return m_defaultContentSize + m_skin.contentBorderSize(scale);
	}



	void TablePanel::_refreshGeometries()
	{

	}


	SizeSPX TablePanel::_calcDefaultContentSize(int scale) const
	{
		if (scale == m_scale)
		{
			for( int row = 0 ; i < slots.nbRows)
		}
		else
		{

		}

	}

	void TablePanel::_resize(const SizeSPX& size, int scale)
	{

	}

	const TypeInfo& TablePanel::_slotTypeInfo(const StaticSlot* pSlot) const
	{
		return TablePanelSlot::TYPEINFO;
	}

	void TablePanel::_childRequestResize(StaticSlot* pSlot)
	{
		auto p = static_cast<TablePanelSlot*>(pSlot);

		auto size = p->_widget()->_defaultSize(m_scale);

		if (size != p->m_defaultSize)
		{
			p->m_defaultSize = size;
			m_defaultContentSize = _calcDefaultContentSize(m_scale);
		}

		_requestResize();
	}

	Widget* TablePanel::_firstChild() const
	{

	}

	Widget* TablePanel::_lastChild() const
	{

	}

	void  TablePanel::_firstSlotWithGeo(SlotWithGeo& package) const
	{

	}

	void  TablePanel::_nextSlotWithGeo(SlotWithGeo& package) const
	{

	}

	CoordSPX TablePanel::_childPos(const StaticSlot* pSlot) const
	{

	}

	void  TablePanel::_childRequestRender(StaticSlot* pSlot)
	{

	}

	void  TablePanel::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{

	}

	void  TablePanel::_childRequestResize(StaticSlot* pSlot)
	{

	}

	Widget* TablePanel::_prevChild(const StaticSlot* pSlot) const
	{

	}

	Widget* TablePanel::_nextChild(const StaticSlot* pSlot) const
	{

	}

	void TablePanel::_releaseChild(StaticSlot* pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots.erase(static_cast<TablePanelSlot*>(pSlot));

	}

	void TablePanel::_replaceChild(StaticSlot* pSlot, Widget* pNewWidget)
	{
		static_cast<TablePanelSlot*>(pSlot)->_setWidget(pNewWidget);
		_childRequestRender(pSlot);
		_childRequestResize(pSlot);
	}

	void TablePanel::_hideSlots(StaticSlot*, int nb)
	{

	}

	void TablePanel::_unhideSlots(StaticSlot*, int nb)
	{

	}

}
