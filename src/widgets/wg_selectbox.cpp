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
#include <wg_base.h>
#include <wg_popuplayer.h>
#include <wg_inputhandler.h>
#include <wg_internal.h>
#include <wg_cdynamicvector.impl.h>
#include <wg_internal.h>

#include <algorithm>

namespace wg
{
	template class CDynamicVector<SelectBoxEntry>;

	using namespace Util;

	const TypeInfo SelectBox::TYPEINFO = { "SelectBox", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	SelectBox::SelectBox() : text(this), entries(this)
	{
		m_pListCanvas = new MySideCanvas(this);
	}

	//____ Destructor _____________________________________________________________

	SelectBox::~SelectBox()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SelectBox::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setEntrySkin() _____________________________________________________

	void SelectBox::setEntrySkin(Skin * pSkin)
	{
		Size oldPadding = m_entryContentPaddingSize;
		Size newPadding = pSkin ? pSkin->contentPaddingSize() : Size();

		m_pEntrySkin = pSkin;
		if (oldPadding != newPadding)
		{
			for (auto& entry : entries)
				entry.m_height += newPadding.h - oldPadding.h;

			m_listCanvasPreferredSize.w += newPadding.w - oldPadding.w;
			m_listCanvasPreferredSize.h += (newPadding.h - oldPadding.h) * entries.size();

			m_entryContentPaddingSize = newPadding;
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
		Skin* pOldSkin = m_pListCanvas->skin();

		Size oldPadding = pOldSkin ? pOldSkin->contentPaddingSize() : Size();
		Size newPadding = pSkin ? pSkin->contentPaddingSize() : Size();

		if (oldPadding != newPadding)
			m_listCanvasPreferredSize += newPadding - oldPadding;

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

	void SelectBox::selectEntry(const CDynamicVector<SelectBoxEntry>::iterator& it)
	{
		if (it < entries.begin() || it >= entries.end())
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidIterator, "Invalid iterator", _object(), TYPEINFO, __func__, __FILE__, __LINE__);
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
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::OutOfRange, "index out of range", _object(), TYPEINFO, __func__, __FILE__, __LINE__);
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

	//____ preferredSize() _____________________________________________________________

	Size SelectBox::preferredSize() const
	{
		return m_preferredSize;
	}

	//____ matchingHeight() ____________________________________________________________

	MU SelectBox::matchingHeight(MU width) const
	{
		if (width == m_size.w)
			return m_matchingHeight;
		else
		{
			Size contentPadding = _contentPaddingSize();

			MU contentWidth = width - contentPadding.w;
			MU contentHeight = 0;

			auto pMapper = _listTextMapper();
			for (auto& entry : entries)
			{
				contentHeight = std::max(contentHeight, pMapper->matchingHeight(&entry, contentWidth) );
			}
			return contentHeight + contentPadding.h;
		}
	}

	//____ _receive() _________________________________________________________________

	void SelectBox::_receive(Msg* pMsg)
	{
		Widget::_receive(pMsg);

		switch (pMsg->type())
		{
		case MsgType::PopupClosed:
		{
			m_bOpen = false;
			_setState(m_closeState);
			break;
		}

		case MsgType::MousePress:
		{
			if (static_cast<MouseButtonMsg*>(pMsg)->button() == MouseButton::Left)
			{
				if (m_bOpen)
				{
					_close();
				}
				else
				{
					_open();
					Base::inputHandler()->_yieldButtonEvents(MouseButton::Left, this, m_pListCanvas);
					m_bPressed = false;		// We have yielded our press...
				}
			}
			break;
		}

		default:
			break;
		}

		if (pMsg->isMouseButtonMsg() && static_cast<MouseButtonMsg*>(pMsg)->button() == MouseButton::Left)
			pMsg->swallow();

	}

	//____ _cloneContent() _______________________________________________________

	void SelectBox::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		SelectBox * pOrg = (SelectBox*) _pOrg;

	}

	//____ _render() _____________________________________________________________

	void SelectBox::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		OO(text)._render(pDevice, canvas);
	}

	//____ _resize() _______________________________________________________________

	void SelectBox::_resize(const Size& size)
	{
		m_size = size;
		m_matchingHeight = matchingHeight(size.w);
	}

	//____ _setState() ________________________________________________________

	void SelectBox::_setState(State state)
	{
		if (m_bOpen)
		{
			m_closeState = state;
			state.setPressed(true);			// Force pressed state when popup is open.
		}
		Widget::_setState(state);
		OO(text)._setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let OO(text).setState() return if rendering is needed)
	}

	//____ _open() ____________________________________________________________

	void SelectBox::_open()
	{
		auto pLayer = OO(_parent())->_getPopupLayer();
		if (pLayer && m_pListCanvas)
		{
			pLayer->popupSlots.pushFront(m_pListCanvas, this, globalGeo(), Origo::SouthWest, false);
			m_bOpen = true;
			m_closeState = m_state;
		}
	}

	//____ _close() ___________________________________________________________

	void SelectBox::_close()
	{
		auto pLayer = OO(_parent())->_getPopupLayer();
		if (pLayer && m_pListCanvas)
		{
			pLayer->popupSlots.pop(m_pListCanvas);
		}
	}

	//____ _updateListCanvasOpacity() ____________________________________________

	void SelectBox::_updateListCanvasOpacity()
	{
		bool bOpaque = false;

		Skin * pBgSkin = m_pListCanvas->m_pSkin;

		if (pBgSkin && pBgSkin->isOpaque())
			bOpaque = true;

		if (m_pEntrySkin && m_pEntrySkin->isOpaque() && (!pBgSkin || pBgSkin->contentPaddingSize().isEmpty()))
			bOpaque = true;

		m_pListCanvas->m_bOpaque = bOpaque;
	}


	//____ _requestRenderEntry() __________________________________________________

	void SelectBox::_requestRenderEntry(SelectBoxEntry* pEntry)
	{
		if (!m_pListCanvas->_parent())
			return;

		MU ofs = 0;
		for (auto p = &entries[0]; p != pEntry; p++)
			ofs += p->m_height;

		Rect rect = m_pListCanvas->_contentRect();
		rect.y += ofs;
		rect.h = pEntry->m_height;

		m_pListCanvas->_requestRender(rect);
	}

	//____ _markEntry() ________________________________________________________

	void SelectBox::_markEntry(int index)
	{
		if (index == m_markedEntryIndex)
			return;

		if (m_markedEntryIndex >= 0)
		{
			State oldState = entries[m_markedEntryIndex].m_state;
			entries[m_markedEntryIndex].m_state.setHovered(false);
			_listTextMapper()->onStateChanged(&entries[m_markedEntryIndex], entries[m_markedEntryIndex].m_state, oldState);
			_requestRenderEntry(&entries[m_markedEntryIndex]);
		}

		if (index >= 0)
		{
			State oldState = entries[index].m_state;
			entries[index].m_state.setHovered(true);
			_listTextMapper()->onStateChanged(&entries[index], entries[index].m_state, oldState);
			_requestRenderEntry(&entries[index]);
		}

		m_markedEntryIndex = index;
	}

	//____ _selectEntry() ________________________________________________________

	void SelectBox::_selectEntry(int index)
	{
		if (index == m_selectedEntryIndex)
			return;

		if (m_selectedEntryIndex >= 0)
		{
			State oldState = entries[m_selectedEntryIndex].m_state;
			entries[m_selectedEntryIndex].m_state.setSelected(false);
			_listTextMapper()->onStateChanged(&entries[m_selectedEntryIndex], entries[m_selectedEntryIndex].m_state, oldState);
			_requestRenderEntry(&entries[m_selectedEntryIndex]);
		}

		State oldState = entries[index].m_state;
		entries[index].m_state.setSelected(true);
		_listTextMapper()->onStateChanged(&entries[index], entries[index].m_state, oldState);
		_requestRenderEntry(&entries[index]);
		m_selectedEntryIndex = index;

		text.set(entries[index].m_string);
		_requestRender();

		Base::msgRouter()->post(SelectMsg::create(this));
	}

	//____ _findEntry() _____________________________________________________

	int SelectBox::_findEntry(const Coord& ofsInListPanel, Coord * pOfsOut)
	{
		Rect contentRect = m_pListCanvas->m_pSkin ? m_pListCanvas->_contentRect() : Rect(m_pListCanvas->m_size);

		if (contentRect.contains(ofsInListPanel))
		{
			Coord ofsInEntry = ofsInListPanel - contentRect.pos();
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

		Size entryPadding = m_pEntrySkin ? m_pEntrySkin->contentPaddingSize() : Size();
		Size boxPadding = m_pSkin ? m_pSkin->contentPaddingSize() : Size();
		Size listPadding = m_pListCanvas->m_pSkin ? m_pListCanvas->m_pSkin->contentPaddingSize() : Size();

		Size	oldPreferred		= m_preferredSize;
		int		oldMatchingHeight = m_matchingHeight;

		for (int i = 0; i < nb; i++)
		{
			// Set entry parent and mapper.

			pEntry->m_pParent = this;
			pMapper->addText(pEntry);

			// Update entry height and listCanvas matchingHeight

			MU entryHeight = pMapper->matchingHeight(pEntry, m_entryContentWidth) + entryPadding.h;

			pEntry->m_height = entryHeight;
			m_listCanvasMatchingHeight += entryHeight;

			// Update m_matchingHeight

			MU boxHeight = pMapper->matchingHeight(pEntry, (m_size.w - boxPadding.w) ) + boxPadding.h;

			if (boxHeight < m_matchingHeight)
				m_matchingHeight = boxHeight;

			// Update m_preferredSize and m_listCanvasPreferredSize

			Size contentPreferred = pMapper->preferredSize(pEntry);
			Size entryPreferred = contentPreferred + entryPadding;
			Size boxPreferred = contentPreferred + boxPadding;

			if (entryPreferred.w + listPadding.w > m_listCanvasPreferredSize.w)
				m_listCanvasPreferredSize.w = entryPreferred.w + listPadding.w;
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

	MU SelectBox::_sideCanvasMatchingHeight(const SideCanvas * pCanvas, MU width) const
	{
		if (width == m_pListCanvas->m_size.w)
			return m_listCanvasMatchingHeight;
		else
		{
			Size canvasPadding = m_pListCanvas->_contentPaddingSize();
			Size entryPadding = m_pEntrySkin ? m_pEntrySkin->contentPaddingSize() : Size();

			MU contentWidth = width - canvasPadding.w - entryPadding.w;

			MU height = canvasPadding.h + entryPadding.h * entries.size();

			auto pMapper = _listTextMapper();
			for (auto& entry : entries)
			{
				height += pMapper->matchingHeight(&entry, contentWidth);
			}
			return height;
		}
	}

	//____ _sideCanvasMatchingWidth() _________________________________________

	MU SelectBox::_sideCanvasMatchingWidth(const SideCanvas * pCanvas, MU height) const
	{
		return std::max(m_listCanvasPreferredSize.w, m_preferredSize.w);
	}

	//____ _sideCanvasPreferredSize() _________________________________________

	Size SelectBox::_sideCanvasPreferredSize(const SideCanvas * pCanvas) const
	{
		return { std::max(m_listCanvasPreferredSize.w, m_preferredSize.w), m_listCanvasPreferredSize.h };
	}

	//____ _sideCanvasRender() ________________________________________________
	
	void SelectBox::_sideCanvasRender(SideCanvas * _pWidget, GfxDevice * pDevice, const Rect& canvas, const Rect& window)
	{
		auto pWidget = static_cast<MySideCanvas*>(_pWidget);
		pWidget->Widget::_render(pDevice, canvas, window);

		Rect contentRect = pWidget->_contentRect(canvas);

		TextMapper* pTextMapper = _listTextMapper();

		Coord pos = contentRect.pos();
		for (auto& entry : entries )
		{
			Rect entryRect = { pos, contentRect.w, entry.m_height };
			if (m_pEntrySkin)
			{
				m_pEntrySkin->render(pDevice, entryRect, entry.m_state);
				entryRect = m_pEntrySkin->contentRect(entryRect, entry.m_state);
			}
			pTextMapper->render(&entry, pDevice, entryRect);
			pos.y += entry.m_height;
		}
	}

	//____ _sideCanvasRefresh() ________________________________________________

	void SelectBox::_sideCanvasRefresh(SideCanvas * pCanvas)
	{
		//TODO: Implement!
	}

	//____ _sideCanvasResize() _________________________________________________

	void SelectBox::_sideCanvasResize(SideCanvas * pCanvas, const Size& size)
	{
		TextMapper* pMapper = _listTextMapper();

		Size listCanvasPaddingSize = m_pListCanvas->m_pSkin ? m_pListCanvas->m_pSkin->contentPaddingSize() : Size();

		MU newContentWidth = size.w - listCanvasPaddingSize.w;
		MU matchingHeight = listCanvasPaddingSize.h;

		for (auto& entry : entries)
		{
			MU newEntryHeight = pMapper->matchingHeight(&entry, newContentWidth);
			pMapper->onResized(&entry, { newContentWidth, newEntryHeight }, { m_entryContentWidth,entry.m_height });
			newEntryHeight += m_entryContentPaddingSize.h;
			entry.m_height = newEntryHeight;
			matchingHeight += newEntryHeight;
		}

		m_entryContentWidth = newContentWidth;
		m_listCanvasMatchingHeight = matchingHeight;
		m_pListCanvas->Widget::_resize(size);
	}

	//____ _sideCanvasReceive() ________________________________________________

	void SelectBox::_sideCanvasReceive(SideCanvas * pCanvas, Msg * pMsg)
	{
		switch (pMsg->type())
		{
		case MsgType::MouseEnter:
		case MsgType::MouseMove:
		{
			Coord pos = pCanvas->toLocal(static_cast<InputMsg*>(pMsg)->pointerPos());
			_markEntry(_findEntry(pos));
			break;
		}

		case MsgType::MouseLeave:
			_markEntry(-1);
			break;

		case MsgType::MouseRelease:
		{
			if (m_markedEntryIndex != -1)
				_selectEntry(m_markedEntryIndex);
			break;
		}


		default:
			break;
		}
	}

	//____ _sideCanvasAlphaTest() _____________________________________________

	bool SelectBox::_sideCanvasAlphaTest(SideCanvas * pCanvas, const Coord& ofs)
	{
		// Test against SideCanvas widget itself

		if (m_pListCanvas->Widget::_alphaTest(ofs))
			return true;

		// Test against entries

		if (m_pEntrySkin)
		{
			Coord ofsInEntry;
			int entryIdx = _findEntry(ofs, &ofsInEntry);

			if (entryIdx >= 0)

			{
				Rect contentRect = m_pListCanvas->m_pSkin ? m_pListCanvas->_contentRect() : Rect(m_pListCanvas->m_size);
				Rect entryGeo = { 0,0,contentRect.w,entries[entryIdx].m_height };

				return m_pEntrySkin->markTest(ofsInEntry, entryGeo, entries[entryIdx].m_state, m_pListCanvas->m_markOpacity);
			}
		}

		return false;
	}

	//____ _sideCanvasSetSkin() _______________________________________________

	void SelectBox::_sideCanvasSetSkin(SideCanvas * pCanvas, Skin * pSkin)
	{
		m_pListCanvas->Widget::setSkin(pSkin);
		_updateListCanvasOpacity();
	}

	//____ _sideCanvasSetState() ______________________________________________

	void SelectBox::_sideCanvasSetState(SideCanvas * pCanvas, State state)
	{
		//TODO: Implement!
	}



	Size SelectBoxEntry::_textSize() const
	{
		return Size( m_pParent->m_entryContentWidth, m_height - m_pParent->m_entryContentPaddingSize.h );
	}

	State SelectBoxEntry::_textState() const
	{
		return m_state;
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

	void SelectBoxEntry::_mapperRequestRender(const Rect& rect)
	{
		m_pParent->_requestRenderEntry(this);			// Make it easy, entry is small anyway.
	}

	void SelectBoxEntry::_mapperRequestResize()
	{
		return;			// We ignore this, make sure to check size whenever we update text,style etc.
	}



} // namespace wg
