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

#include <wg_selectbox.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_msgrouter.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const char SelectBox::CLASSNAME[] = {"SelectBox"};

	//____ Constructor ____________________________________________________________

	SelectBox::SelectBox() : text(this), entries(this)
	{
		m_pListCanvas = new MySideCanvas(this);
	}

	//____ Destructor _____________________________________________________________

	SelectBox::~SelectBox()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool SelectBox::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ________________________________________________________

	const char * SelectBox::className( void ) const
	{
		return CLASSNAME;
	}

	//____ setEntrySkin() _____________________________________________________

	void SelectBox::setEntrySkin(Skin * pSkin)
	{
		SizeI oldPadding = m_entryContentPadding.size();
		SizeI newPadding = pSkin ? pSkin->_contentPaddingSize() : SizeI(0,0);

		m_pEntrySkin = pSkin;
		if (oldPadding != newPadding)
		{
			m_entryContentPadding = pSkin ? pSkin->_contentPadding() : BorderI(0, 0);
			m_pListCanvas->_requestResize();
		}

		m_pListCanvas->_requestRender();
		_updateListCanvasOpacity();
	}

	//____ setEntryStyle() _____________________________________________________

	void SelectBox::setEntryStyle(TextStyle * pStyle)
	{
		if (m_pEntryStyle != pStyle)
		{
			m_pEntryStyle = pStyle;

			m_pListCanvas->_requestResize();
			m_pListCanvas->_requestRender();
		}
	}

	//____ setListSkin() _____________________________________________________

	void SelectBox::setListSkin(Skin * pSkin)
	{
		m_pListCanvas->setSkin(pSkin);
	}

	//____ setListTextMapper() ______________________________________________

	void SelectBox::setListTextMapper(TextMapper * pTextMapper)
	{
		m_pListTextMapper = pTextMapper;
		m_pListCanvas->_requestResize();
		m_pListCanvas->_requestRender();
	}


	//____ selectEntry() _____________________________________________________

	void SelectBox::selectEntry(const CVector<SelectBoxEntry>::iterator& it)
	{
		if (it < entries.begin() || it >= entries.end())
		{
			Base::handleError(ErrorCode::InvalidInterator, "Invalid iterator", _object(), CLASSNAME, __func__, __FILE__, __LINE__);
			return;
		}

		_selectEntry( it - entries.begin() );
	}

	//____ selectEntryById() __________________________________________________

	bool SelectBox::selectEntryById(int id)
	{
		for (auto& entry : entries)
		{
			if (entry.m_id == id)
			{
				int idx = &entry - &entries[0];
				_selectEntry(idx);
				return true;
			}
		}
		return false;
	}

	//____ selectEntryByIndex() _______________________________________________

	void SelectBox::selectEntryByIndex(int index)
	{
		if (index < 0 || index >= entries.size())
		{
			Base::handleError(ErrorCode::OutOfRange, "Index out of range", _object(), CLASSNAME, __func__, __FILE__, __LINE__);
			return;
		}

		_selectEntry(index);
	}

	//____ selectedEntryId() __________________________________________________

	int SelectBox::selectedEntryId() const
	{
		if (m_selectedEntryIndex >= 0)
			return entries[m_selectedEntryIndex].m_id;
		else
			return -1;
	}

	//____ receive() _________________________________________________________________

	void SelectBox::receive(Msg * pMsg)
	{

	}

	//____ _preferredSize() _____________________________________________________________

	SizeI SelectBox::_preferredSize() const
	{
		return m_preferredSize;
	}

	//____ _matchingHeight() ____________________________________________________________

	int SelectBox::_matchingHeight(int width) const
	{
		if (width == m_size.w)
			return m_matchingHeight;
		else
		{
			SizeI contentPadding = _contentPaddingSize();

			int contentWidth = width - contentPadding.w;
			int contentHeight = 0;

			auto pMapper = _listTextMapper();
			for (auto& entry : entries)
			{
				contentHeight = std::max(contentHeight, pMapper->matchingHeight(&entry, contentWidth) );
			}
			return contentHeight + contentPadding.h;
		}
	}

	//____ _cloneContent() _______________________________________________________

	void SelectBox::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		SelectBox * pOrg = (SelectBox*) _pOrg;

	}

	//____ _render() _____________________________________________________________

	void SelectBox::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectI canvas;
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		_text()._render(pDevice, canvas);
	}

	//____ _resize() _______________________________________________________________

	void SelectBox::_resize(const SizeI& size)
	{
		m_size = size;
		m_matchingHeight = _matchingHeight(size.w);
	}

	//____ _updateListCanvasOpacity() ____________________________________________

	void SelectBox::_updateListCanvasOpacity()
	{
		bool bOpaque = false;

		Skin * pBgSkin = m_pListCanvas->m_pSkin;

		if (pBgSkin && pBgSkin->isOpaque())
			bOpaque = true;

		if (m_pEntrySkin && m_pEntrySkin->isOpaque() && (!pBgSkin || pBgSkin->_contentPaddingSize().isEmpty()))
			bOpaque = true;

		m_pListCanvas->m_bOpaque = bOpaque;
	}


	//____ _requestRenderEntry() __________________________________________________

	void SelectBox::_requestRenderEntry(SelectBoxEntry* pEntry)
	{
		if (!m_pListCanvas->_parent())
			return;

		int ofs = 0;
		for (auto p = &entries[0]; p != pEntry; p++)
			ofs += p->m_height;

		RectI rect = m_pListCanvas->_contentRect();
		rect.y += ofs;
		rect.h = pEntry->m_height;

		m_pListCanvas->_requestRender(rect);
	}

	//____ _selectEntry() ________________________________________________________

	void SelectBox::_selectEntry(int index)
	{
		if (index == m_selectedEntryIndex)
			return;

		if (m_selectedEntryIndex >= 0)
			_requestRenderEntry(&entries[m_selectedEntryIndex]);

		_requestRenderEntry(&entries[index]);
		m_selectedEntryIndex = index;

		text.set(entries[index].m_string);
		_requestRender();

		Base::msgRouter()->post(SelectMsg::create(this));
	}

	//____ _entryState() __________________________________________________________

	State SelectBox::_entryState(int index)
	{
		State state;
		if (index == m_selectedEntryIndex)
			state.setSelected(true);
		if (index == m_hoveredEntryIndex)
			state.setHovered(true);

		return state;
	}

	//____ _findEntry() _____________________________________________________

	int SelectBox::_findEntry(const CoordI& ofsInListPanel, CoordI * pOfsOut)
	{
		RectI contentRect = m_pListCanvas->m_pSkin ? m_pListCanvas->_contentRect() : RectI(m_pListCanvas->m_size);

		if (contentRect.contains(ofsInListPanel))
		{
			CoordI ofsInEntry = ofsInListPanel - contentRect.pos();
			for (int i = 0; i < entries.size(); i++)
			{
				auto& entry = entries[i];

				if (ofsInEntry.y < entry.m_height)
				{
					if (pOfsOut)
						* pOfsOut = ofsInEntry;

					return i;
				}

				ofsInEntry.y -= entry.m_height;
			}
		}

		return -1;
	}

	//____ _didAddEntries() ______________________________________________________

	void SelectBox::_didAddEntries(SelectBoxEntry * pEntry, int nb)
	{
		auto pMapper = _listTextMapper();

		SizeI entryPadding = m_pEntrySkin ? m_pEntrySkin->_contentPaddingSize() : SizeI();
		SizeI boxPadding = m_pSkin ? m_pSkin->_contentPaddingSize() : SizeI();

		SizeI	oldPreferred		= m_preferredSize;
		int		oldMatchingHeight = m_matchingHeight;

		for (int i = 0; i < nb; i++)
		{
			// Set entry parent and mapper.

			pEntry->m_pParent = this;
			pMapper->addText(pEntry);

			// Update entry height and listCanvas matchingHeight

			int entryHeight = pixelsToRaw( pMapper->matchingHeight(pEntry, m_entryContentWidth) ) + entryPadding.h;

			pEntry->m_height = entryHeight;
			m_listCanvasMatchingHeight += entryHeight;

			// Update m_matchingHeight

			int boxHeight = pixelsToRaw(pMapper->matchingHeight(pEntry, m_size.w - boxPadding.w)) + boxPadding.h;

			if (boxHeight < m_matchingHeight)
				m_matchingHeight = boxHeight;

			// Update m_preferredSize and m_listCanvasPreferredSize

			SizeI contentPreferred = pixelsToRaw(pMapper->preferredSize(pEntry));
			SizeI entryPreferred = contentPreferred + entryPadding;
			SizeI boxPreferred = contentPreferred + boxPadding;

			if (entryPreferred.w > m_listCanvasPreferredSize.w)
				m_listCanvasPreferredSize.w = entryPreferred.w;
			m_listCanvasPreferredSize.h += entryPreferred.h;

			if (m_preferredSize.w < boxPreferred.w)
				m_preferredSize.w = boxPreferred.w;
			if (m_preferredSize.h < boxPreferred.h)
				m_preferredSize.h = boxPreferred.h;

			pEntry++;
		}

		// Check if we need to adjust preferredSize for the SelectBox.

		if (m_preferredSize != oldPreferred)
			_requestResize();

		// No use to requestRender here, added content is outside canvas content rect.

		m_pListCanvas->_requestResize();

	}

	//____ _didMoveEntries() __________________________________________________

	void SelectBox::_didMoveEntries(SelectBoxEntry * pFrom, SelectBoxEntry * pTo, int nb)
	{
		//TODO: Implement!!!
	}

	//____ _willEraseEntries() ________________________________________________

	void SelectBox::_willEraseEntries(SelectBoxEntry * pSlot, int nb)
	{
		//TODO: Implement!!!
	}

	//____ _sideCanvasMatchingHeight() ________________________________________

	int SelectBox::_sideCanvasMatchingHeight(const SideCanvas * pCanvas, int width) const
	{
		if (width == m_pListCanvas->m_size.w)
			return m_listCanvasMatchingHeight;
		else
		{
			SizeI canvasPadding = m_pListCanvas->_contentPaddingSize();
			SizeI entryPadding = m_pEntrySkin ? m_pEntrySkin->_contentPaddingSize() : SizeI();

			int contentWidth = width - canvasPadding.w - entryPadding.w;

			int height = canvasPadding.h + entryPadding.h * entries.size();

			auto pMapper = _listTextMapper();
			for (auto& entry : entries)
			{
				height += pMapper->matchingHeight(&entry, contentWidth);
			}
			return height;
		}
	}

	//____ _sideCanvasMatchingWidth() _________________________________________

	int SelectBox::_sideCanvasMatchingWidth(const SideCanvas * pCanvas, int height) const
	{
		return m_listCanvasPreferredSize.w;
	}

	//____ _sideCanvasPreferredSize() _________________________________________

	SizeI SelectBox::_sideCanvasPreferredSize(const SideCanvas * pCanvas) const
	{
		return m_listCanvasPreferredSize;
	}

	//____ _sideCanvasRender() ________________________________________________
	
	void SelectBox::_sideCanvasRender(SideCanvas * _pWidget, GfxDevice * pDevice, const RectI& canvas, const RectI& window)
	{
		auto pWidget = static_cast<MySideCanvas*>(_pWidget);
		pWidget->Widget::_render(pDevice, canvas, window);

		RectI contentRect = pWidget->_contentRect(canvas);




	}

	//____ _sideCanvasRefresh() ________________________________________________

	void SelectBox::_sideCanvasRefresh(SideCanvas * pCanvas)
	{
		//TODO: Implement!
	}

	//____ _sideCanvasResize() _________________________________________________

	void SelectBox::_sideCanvasResize(SideCanvas * pCanvas, const SizeI& size)
	{
		m_listCanvasMatchingHeight = m_pListCanvas->_matchingHeight(size.w);
		m_pListCanvas->Widget::_resize(size);
	}

	//____ _sideCanvasReceive() ________________________________________________

	void SelectBox::_sideCanvasReceive(SideCanvas * pCanvas, Msg * pMsg)
	{

	}

	//____ _sideCanvasAlphaTest() _____________________________________________

	bool SelectBox::_sideCanvasAlphaTest(SideCanvas * pCanvas, const CoordI& ofs)
	{
		// Test against SideCanvas widget itself

		if (m_pListCanvas->Widget::_alphaTest(ofs))
			return true;

		// Test against entries

		if (m_pEntrySkin)
		{
			CoordI ofsInEntry;
			int entryIdx = _findEntry(ofs, &ofsInEntry);

			if (entryIdx >= 0)
			{
				RectI contentRect = m_pListCanvas->m_pSkin ? m_pListCanvas->_contentRect() : RectI(m_pListCanvas->m_size);
				RectI entryGeo = { 0,0,contentRect.w,entries[entryIdx].m_height };

				return m_pEntrySkin->_markTest(ofsInEntry, entryGeo, _entryState(entryIdx), m_pListCanvas->m_markOpacity);
			}
		}

		return false;
	}

	//____ _sideCanvasSetSkin() _______________________________________________

	void SelectBox::_sideCanvasSetSkin(SideCanvas * pCanvas, Skin * pSkin)
	{
		m_pListCanvas->Widget::_setSkin(pSkin);
		_updateListCanvasOpacity();
	}

	//____ _sideCanvasSetState() ______________________________________________

	void SelectBox::_sideCanvasSetState(SideCanvas * pCanvas, State state)
	{
		//TODO: Implement!
	}



	SizeI SelectBoxEntry::_textSize() const
	{
		return rawToPixels( SizeI( m_pParent->m_entryContentWidth, m_height - m_pParent->m_entryContentPadding.height() ) );
	}

	State SelectBoxEntry::_textState() const
	{
		int myIndex = this - &m_pParent->entries[0];

		State s;
		if (myIndex == m_pParent->m_selectedEntryIndex)
			s = StateEnum::Selected;
		if (myIndex == m_pParent->m_hoveredEntryIndex)
			s.setHovered(true);

		return s;
	}

	TextStyle * SelectBoxEntry::_textStyle() const
	{
		return m_pParent->m_pEntryStyle ? m_pParent->m_pEntryStyle : Base::defaultStyle();
	}

	const Char * SelectBoxEntry::_textBegin() const
	{
		return m_string.chars();
	}

	int SelectBoxEntry::_textLength() const
	{
		return m_string.length();
	}

	bool SelectBoxEntry::_caretVisible() const
	{
		return false;
	}

	int SelectBoxEntry::_caretOffset() const
	{
		return 0;
	}

	std::tuple<int, int> SelectBoxEntry::_selectedText() const
	{
		return std::make_tuple(0, 0);
	}

	void SelectBoxEntry::_mapperRequestRender()
	{
		m_pParent->_requestRenderEntry(this);
	}

	void SelectBoxEntry::_mapperRequestRender(const RectI& rect)
	{
		m_pParent->_requestRenderEntry(this);			// Make it easy, entry is small anyway.
	}

	void SelectBoxEntry::_mapperRequestResize()
	{
		return;			// We ignore this, make sure to check size whenever we update text,style etc.
	}



} // namespace wg
