/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_debugoverlay.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_inputhandler.h>
#include <wg_boxskin.h>
#include <wg_packpanel.h>
#include <wg_tablepanel.h>
#include <wg_numberdisplay.h>
#include <wg_textdisplay.h>
#include <wg_gfxdevice.h>
#include <wg_togglebutton.h>
#include <wg_togglegroup.h>
#include <wg_scrollpanel.h>

#include <wg_staticslotvector.impl.h>

#include <string>

namespace wg
{
	using namespace Util;

	template class StaticSlotVector<DebugOverlay::ToolboxSlot>;

	const TypeInfo DebugOverlay::TYPEINFO = { "DebugOverlay", &Overlay::TYPEINFO };
	const TypeInfo DebugOverlay::ToolboxSlot::TYPEINFO = { "DebugOverlay::ToolboxSlot", &Overlay::Slot::TYPEINFO };


	//____ constructor ____________________________________________________________

	DebugOverlay::DebugOverlay() : palettes(this)
	{
		// Create default skins

		m_pSelectionSkin = BoxSkin::create(1, Color::Transparent, Color::Red, 1);

		m_pToolboxSkin = BoxSkin::create({ 16,2,2,2 }, Color::White, Color::Yellow, { 16,2,2,2 } );

		// Add our default palettes

		m_pMainToolbox = PackPanel::create();
		m_pMainToolbox->setAxis(Axis::Y);

		m_pMainToolbox->slots << _createMainTool();


		Widget_p pMainToolbox;
		PackPanel_p	pMainContent;

		std::tie(pMainToolbox, pMainContent) = _createToolbox("Main Toolbox");


		m_pSlotToolbox = PackPanel::create();
		m_pSlotToolbox->setAxis(Axis::Y);

		m_pWidgetToolbox = PackPanel::create();
		m_pWidgetToolbox->setAxis(Axis::Y);

		pMainContent->slots.pushBack( {m_pSlotToolbox, m_pWidgetToolbox} );

		palettes._pushBackEmpty(1);

		palettes[0]._setWidget(pMainToolbox);
		palettes[0].m_bVisible = true;
		palettes[0].m_placement = Placement::East;

		_refreshRealGeo( &palettes[0] );


	}

	//____ Destructor _____________________________________________________________

	DebugOverlay::~DebugOverlay()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& DebugOverlay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setToolboxSkin() ___________________________________________________

	void DebugOverlay::setToolboxSkin(Skin * pSkin)
	{
		m_pToolboxSkin = pSkin;
		_requestRender();
	}

	//____ setSelectionSkin() _________________________________________________

	void DebugOverlay::setSelectionSkin(Skin * pSkin)
	{
		m_pSelectionSkin = pSkin;
		_requestRender();
	}

	//____ setEditMode() ______________________________________________________

	void DebugOverlay::setEditMode(bool bEditMode)
	{
		if (bEditMode != m_bEditMode)
		{
			m_bEditMode = bEditMode;
			_requestRender();
		}
	}

	//____ _matchingHeight() _______________________________________________________

	spx DebugOverlay::_matchingHeight(spx width, int scale) const
	{
		if (mainSlot._widget())
			return mainSlot._widget()->_matchingHeight(width, scale);
		else
			return Widget::_matchingHeight(width, scale);
	}

	//____ _matchingWidth() _______________________________________________________

	spx DebugOverlay::_matchingWidth(spx height, int scale) const
	{
		if (mainSlot._widget())
			return mainSlot._widget()->_matchingWidth(height,scale);
		else
			return Widget::_matchingWidth(height,scale);
	}

	//____ defaultSize() _____________________________________________________________

	SizeSPX DebugOverlay::_defaultSize(int scale) const
	{
		if (mainSlot._widget())
			return mainSlot._widget()->_defaultSize(scale);
		else
			return SizeSPX(1, 1);
	}

	//____ _findWidget() ____________________________________________________________

	Widget *  DebugOverlay::_findWidget(const CoordSPX& ofs, SearchMode mode)
	{
		if (m_bEditMode)
		{
			for (auto& slot : palettes)
				if (slot.m_geo.contains(ofs))
					return slot._widget();

			return this;
		}

		return Container::_findWidget(ofs, mode);
	}

	//____ _render() __________________________________________________________

	void DebugOverlay::_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		// Render skin

		m_skin.render(pDevice, _canvas, m_scale, m_state);

		RectSPX contentRect = m_skin.contentRect(_canvas, m_scale, m_state);

		if (!mainSlot.isEmpty())
		{
			mainSlot._widget()->_render(pDevice, contentRect, contentRect);
		}

		if (m_bEditMode)
		{
			if (m_pSelectedWidget)
				m_pSelectionSkin->_render(pDevice, _selectionGeo(), m_scale, State::Default);

			for (auto& palette : palettes)
			{
				if (palette.m_bVisible)
				{
					RectSPX geo = palette.m_geo + _canvas.pos();

					if (m_pToolboxSkin)
						m_pToolboxSkin->_render(pDevice, geo + m_pToolboxSkin->_contentBorder(m_scale, State::Default), m_scale, State::Default);

					RectSPX pxPaletteGeo = palette.m_geo;
					if (pDevice->clipBounds().isOverlapping(pxPaletteGeo))
					{
						ClipPopData popData = limitClipList(pDevice, palette.m_geo);
						palette._widget()->_render(pDevice, geo, geo);

						popClipList(pDevice, popData);
					}
				}
			}

		}

	}

	//____ _selectionGeo() ____________________________________________________

	RectSPX DebugOverlay::_selectionGeo() const
	{
		if (!m_pSelectedWidget)
			return RectSPX();

		RectSPX selectedGeo = _toLocal(m_pSelectedWidget->_globalGeo());

		if (m_pSelectionSkin)
			selectedGeo += m_pSelectionSkin->_contentBorder(m_scale, State::Default);

		return selectedGeo;
	}


	//____ _refreshRealGeo() __________________________________________________

	void DebugOverlay::_refreshRealGeo(ToolboxSlot * pSlot, bool bForceResize)
	{
		if (m_pSelectedWidget)
		{
			Placement placement = pSlot->m_placement;
			BorderSPX palettePadding = m_pToolboxSkin ? m_pToolboxSkin->_contentBorder(m_scale, State::Default) : BorderSPX();

			SizeSPX wantedSize = pSlot->_widget()->_defaultSize(pSlot->_widget()->_scale()) + palettePadding;

			RectSPX selectedGeo = _toLocal(m_pSelectedWidget->_globalGeo());
			selectedGeo += m_pSelectionSkin ? m_pSelectionSkin->_contentBorder(m_scale, State::Default) : BorderSPX();
			RectSPX surroundBox = selectedGeo + BorderSPX(wantedSize.h+4, wantedSize.w+4);

			CoordSPX ofs = placementToOfs(placement, surroundBox.size()) - placementToOfs(placement, wantedSize) + surroundBox.pos();
			ofs += pSlot->m_placementPos;

			RectSPX geo = align(RectSPX(ofs, wantedSize));

			if (geo.x < 0)
			{
				if (placement == Placement::West || placement == Placement::NorthWest || placement == Placement::SouthWest)
					geo.x = selectedGeo.right() + (selectedGeo.x - geo.x);	// Flipping to right side instead.
				else
					geo.x = 0;
			}
			else if (geo.right() > m_size.w)
			{
				if (placement == Placement::East || placement == Placement::NorthEast || placement == Placement::SouthEast)
					geo.x = selectedGeo.left() - (selectedGeo.right() - geo.right());	// Flipping to right side instead.
				else
					geo.x = m_size.w - geo.w;
			}

			if (geo.y < 0)
			{
				if (placement == Placement::North || placement == Placement::NorthWest || placement == Placement::NorthEast)
					geo.y = selectedGeo.bottom() + (selectedGeo.y - geo.y);	// Flipping to right side instead.
				else
					geo.y = 0;
			}
			else if (geo.bottom() > m_size.h)
			{
				if (placement == Placement::South || placement == Placement::SouthWest || placement == Placement::SouthEast)
					geo.y = selectedGeo.top() - (selectedGeo.bottom() - geo.bottom());	// Flipping to right side instead.
				else
					geo.y = m_size.h - geo.h;
			}

			if (geo.h > m_size.h)
				geo.h = m_size.h;


			RectSPX childGeo = geo - palettePadding;
			if (childGeo != pSlot->m_geo)
			{
				_requestRender(geo);
				_requestRender(pSlot->m_geo + palettePadding);

				pSlot->m_geo = childGeo;
			}

			if (pSlot->_widget()->_size() != childGeo.size() || bForceResize )
				pSlot->_setSize(childGeo, m_scale);
		}
		else
		{
		}
	}

	//____ _selectWidget() ____________________________________________________

	void DebugOverlay::_selectWidget(Widget * pWidget)
	{
		if (pWidget == m_pSelectedWidget)
			return;

		// Update selectionSkin

		if (m_pSelectionSkin)
		{
			if (m_pSelectedWidget)
			{
				RectSPX geo = _toLocal(m_pSelectedWidget->_globalGeo());
				geo += m_pSelectionSkin->_contentBorder(m_scale, State::Default);
				_requestRender(geo);
			}

			m_pSelectedWidget = pWidget;

			if (m_pSelectedWidget)
			{
				RectSPX geo = _toLocal(m_pSelectedWidget->_globalGeo());
				geo += m_pSelectionSkin->_contentBorder(m_scale, State::Default);
				_requestRender(geo);
			}
		}

		// Update palettes

		if (m_pSelectedWidget)
		{
			m_pSlotToolbox->slots.clear();
			m_pSlotToolbox->slots << _createGenericSlotTool(*m_pSelectedWidget->_slot());

			m_pWidgetToolbox->slots.clear();
			m_pWidgetToolbox->slots << _createGenericWidgetTool(m_pSelectedWidget);
/*
			for (auto& palette : palettes)
			{
				palette.m_bVisible = true;
				_refreshRealGeo(&palette);
			}
 */
		}
		else
		{
			BorderSPX palettePadding = m_pToolboxSkin ? m_pToolboxSkin->_contentBorder(m_scale, State::Default) : BorderSPX();
			for (auto& palette : palettes)
			{
				if (palette.m_bVisible)
				{
					_requestRender(palette.m_geo + palettePadding);
					palette.m_bVisible = false;
				}
			}
		}

		// 


	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	DebugOverlay::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		if (pSlot == &mainSlot)
			return DynamicSlot::TYPEINFO;

		return ToolboxSlot::TYPEINFO;
	}

	//____ _childRequestResize() ______________________________________________

	void DebugOverlay::_childRequestResize( StaticSlot * pSlot )
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
		{
			auto p = static_cast<ToolboxSlot*>(pSlot);
			_refreshRealGeo( p, true );
		}
	}

	//____ _releaseChild() ____________________________________________________

	void DebugOverlay::_releaseChild(StaticSlot * pSlot)
	{
		if (pSlot == &mainSlot )
			Overlay::_releaseChild(pSlot);
		else
		{
			auto p = static_cast<ToolboxSlot*>(pSlot);

			if (p->m_bVisible)
				_requestRender(p->m_geo);
			palettes._erase(p);
		}
	}

	//____ _beginOverlaySlots() __________________________________________________

	const Overlay::Slot * DebugOverlay::_beginOverlaySlots() const
	{
		return palettes._begin();
	}

	//____ _endOverlaySlots() ____________________________________________________

	const Overlay::Slot *  DebugOverlay::_endOverlaySlots() const
	{
		return palettes._end();
	}

	//____ _sizeOfOverlaySlot() __________________________________________________

	int DebugOverlay::_sizeOfOverlaySlot() const
	{
		return sizeof(ToolboxSlot);
	}


	//____ _resize() ___________________________________________________________

	void DebugOverlay::_resize( const SizeSPX& sz, int scale )
	{
		Overlay::_resize(sz, scale);

		// Refresh modal widgets geometry, their positions might have changed.

		for( auto pSlot = palettes._begin() ; pSlot != palettes._end() ; pSlot++ )
			_refreshRealGeo( pSlot, true );
	}

	//____ _receive() ______________________________________________________________

	void DebugOverlay::_receive( Msg * _pMsg )
	{
		// Check if it is our enable/disable toggle

		if( _pMsg->isInstanceOf( KeyMsg::TYPEINFO ) && static_cast<KeyMsg*>(_pMsg)->translatedKeyCode() == Key::Escape && (static_cast<KeyMsg*>(_pMsg)->modKeys() == ModKeys::StdCtrl || static_cast<KeyMsg*>(_pMsg)->modKeys() == ModKeys::MacCtrl) )
		{
			if( _pMsg->typeInfo() == KeyPressMsg::TYPEINFO )
				setEditMode(!m_bEditMode);

			_pMsg->swallow();
			Overlay::_receive(_pMsg);
			return;
		}

		//

		if( !m_bEditMode )
		{
			Overlay::_receive(_pMsg);
			return;
		}

		//

		switch( _pMsg->type() )
		{
			case MsgType::MousePress:
			{
				if (_pMsg->originalSourceRawPtr() == this)			// Make sure it wasn't a propagated press on widget in palette.
				{
					auto pMsg = static_cast<MousePressMsg*>(_pMsg);

					if (pMsg->button() == MouseButton::Left)
					{
						CoordSPX mousePos = _toLocal(pMsg->pointerSpxPos());

						// Check for press on palette edge

						if (m_pToolboxSkin)
						{
							BorderSPX	contentBorder = m_pToolboxSkin->_contentBorder(m_scale, State::Default);

							for (int i = 0; i < palettes.size(); i++)
							{
								if (palettes[i].m_bVisible && (palettes[i].m_geo + contentBorder).contains(mousePos) )
								{
									m_pressedToolbox = i;
									m_pressedToolboxStartOfs = palettes[i].m_placementPos;
									break;
								}
							}
						}

						// Check for selection of Widget if we didn't select a palette

						if (m_pressedToolbox == -1)
						{
							auto pContainer = wg_dynamic_cast<Container_p>(mainSlot._widget());
							if (pContainer)
							{
								Widget * pWidget = pContainer->_findWidget(mousePos, SearchMode::MarkPolicy);
								if (pWidget)
									_selectWidget(pWidget);
							}
						}
						pMsg->swallow();
					}
				}
				break;
			}

			case MsgType::MouseDrag:
			{
				if (m_pressedToolbox >= 0)
				{
					auto pMsg = static_cast<MouseDragMsg*>(_pMsg);

					palettes[m_pressedToolbox].m_placementPos = m_pressedToolboxStartOfs + pMsg->_draggedTotal();
					_refreshRealGeo(&palettes[m_pressedToolbox]);
				}

				break;
			}

			case MsgType::MouseRelease:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);

				if (pMsg->button() == MouseButton::Left)
					m_pressedToolbox = -1;

				break;
			}

			case MsgType::KeyPress:
			{
				auto pMsg = static_cast<KeyPressMsg*>(_pMsg);

				Key key = pMsg->translatedKeyCode();
				switch( key )
				{
					case Key::F1:
					case Key::F2:
					case Key::F3:
					case Key::F4:
					case Key::F5:
					{
						int paletteIdx = int(key) - int(Key::F1);

						if( palettes.size() > paletteIdx )
						{
							palettes[paletteIdx].m_bVisible = !palettes[paletteIdx].m_bVisible;
							_requestRender();

							// Make sure we don't keep dragging around an invisible box.

							m_pressedToolbox = -1;
						}
					}


					default:
						break;
				}

				break;
			}

			default:
				break;
		};

		Overlay::_receive(_pMsg);
	}

	//____ _createToolbox() ________________________________________

	std::tuple<Widget_p, PackPanel_p> DebugOverlay::_createToolbox( const char * pTitle )
	{
		auto pPanelSkin = BoxSkin::create( WGBP(BoxSkin,
												_.color = Color::Grey,
												_.outlineColor = Color::Black,
												_.outlineThickness = 1,
												_.padding = 4 ));

		auto pContentSkin = BoxSkin::create( WGBP(BoxSkin,
												  _.color = Color::White,
												  _.outlineColor = Color::Black,
												  _.outlineThickness = 1,
												  _.padding = 2));

		auto pMain = PackPanel::create( WGBP(PackPanel, _.axis = Axis::Y, _.skin = pPanelSkin) );

		auto pTitleDisplay = TextDisplay::create( WGBP(TextDisplay, _.display.text = pTitle ));

		auto pContentWindow = ScrollPanel::create( WGBP(ScrollPanel, _.skin = pContentSkin ));

		auto pContent = PackPanel::create( WGBP(PackPanel, _.axis = Axis::Y ));

		pContentWindow->slot = pContent;

		pMain->slots.pushBack( pTitleDisplay, WGBP(PackPanelSlot, _.weight = 0.f ));
		pMain->slots.pushBack( pContentWindow, WGBP(PackPanelSlot, _.weight = 1.f ));

		return std::make_tuple(pMain,pContent);
	}

	//____ _createMainTool() ________________________________________

	Widget_p DebugOverlay::_createMainTool()
	{
		auto pSection = PackPanel::create( WGBP(PackPanel, _.axis = Axis::X) );


		auto pPickButtonSkin = BoxSkin::create( WGBP(BoxSkin,
													 _.outlineColor = Color::Black,
													 _.outlineThickness = 1,
													 _.padding = 2,
													 _.states = { {State::Default, Color::Red}, {State::Selected, Color::Green}} ));


		auto pPickButton = ToggleButton::create( WGBP(ToggleButton, _.skin = pPickButtonSkin));

		pSection->slots << pPickButton;

		return pSection;
	}

	//____ _createGenericSlotTool() ________________________________________

	Widget_p DebugOverlay::_createGenericSlotTool(const StaticSlot& slot)
	{
		auto pTable = TablePanel::create( WGBP(TablePanel,
											   _.columns = 2,
											   _.rows = 5) );

		int row = 0;

		// Type

		{
			pTable->slots[row][0] = TextDisplay::create( WGBP(TextDisplay, _.display.text = "Slot Type:") );
			pTable->slots[row][0] = TextDisplay::create( WGBP(TextDisplay, _.display.text = slot.typeInfo().className ) );
			row++;
		}


		// Position

		{
			auto pLabelX = TextDisplay::create();
			pLabelX->display.setText("Relative X: ");

			auto pLabelY = TextDisplay::create();
			pLabelY->display.setText("Relative Y: ");

			auto pLabelW = TextDisplay::create();
			pLabelW->display.setText("Width: ");

			auto pLabelH = TextDisplay::create();
			pLabelH->display.setText("Height: ");


			RectF geo = slot.geo();

			auto pValueX = TextDisplay::create();
			pValueX->display.setText(std::to_string(geo.x));

			auto pValueY = TextDisplay::create();
			pValueY->display.setText(std::to_string(geo.y));

			auto pValueW = TextDisplay::create();
			pValueW->display.setText(std::to_string(geo.w));

			auto pValueH = TextDisplay::create();
			pValueH->display.setText(std::to_string(geo.h));


			pTable->slots[row][0] = pLabelX;
			pTable->slots[row][1] = pValueX;
			row++;

			pTable->slots[row][0] = pLabelY;
			pTable->slots[row][1] = pValueY;
			row++;

			pTable->slots[row][0] = pLabelW;
			pTable->slots[row][1] = pValueW;
			row++;

			pTable->slots[row][0] = pLabelH;
			pTable->slots[row][1] = pValueH;
			row++;

		}

		return pTable;
	}

	//____ _createGenericWidgetTool() ______________________________________

	Widget_p DebugOverlay::_createGenericWidgetTool( Widget * pWidget )
	{
		auto pTable = TablePanel::create( WGBP(TablePanel,
											   _.columns = 2,
											   _.rows = 2) );

		// ClassName

		{
			auto pNameLabel = TextDisplay::create();
			pNameLabel->display.setText("className: ");

			auto pName = TextDisplay::create();
			pName->display.setText(pWidget->typeInfo().className);

			pTable->slots[0][0] = pNameLabel;
			pTable->slots[0][1] = pName;
		}

		// Id

		{
			auto pIdLabel = TextDisplay::create();
			pIdLabel->display.setText("id: ");

			auto pId = TextDisplay::create();
			pId->display.setText(std::to_string(pWidget->id()));

			pTable->slots[1][0] = pIdLabel;
			pTable->slots[1][1] = pId;
		}

		return pTable;
	}



} // namespace wg
