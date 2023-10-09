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

#include <wg_designoverlay.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_inputhandler.h>
#include <wg_boxskin.h>
#include <wg_packpanel.h>
#include <wg_numberdisplay.h>
#include <wg_textdisplay.h>
#include <wg_gfxdevice.h>

#include <wg_staticslotvector.impl.h>

#include <string>

namespace wg
{
	using namespace Util;

	template class StaticSlotVector<DesignOverlay::ToolboxSlot>;

	const TypeInfo DesignOverlay::TYPEINFO = { "DesignOverlay", &Overlay::TYPEINFO };
	const TypeInfo DesignOverlay::ToolboxSlot::TYPEINFO = { "DesignOverlay::ToolboxSlot", &Overlay::Slot::TYPEINFO };


	//____ constructor ____________________________________________________________

	DesignOverlay::DesignOverlay() : palettes(this)
	{
		// Create default skins

		m_pSelectionSkin = BoxSkin::create(1, Color::Transparent, Color::Red, 1);

		m_pToolboxSkin = BoxSkin::create({ 16,2,2,2 }, Color::White, Color::Yellow, { 16,2,2,2 } );

		// Add two default palettes

		m_pSlotToolbox = PackPanel::create();
		m_pSlotToolbox->setAxis(Axis::Y);

		m_pWidgetToolbox = PackPanel::create();
		m_pWidgetToolbox->setAxis(Axis::Y);

		palettes._pushBackEmpty(2);

		palettes[0]._setWidget(m_pSlotToolbox);
		palettes[0].m_bVisible = false;
		palettes[0].m_placement = Placement::South;

		palettes[1]._setWidget(m_pWidgetToolbox);
		palettes[1].m_bVisible = false;
		palettes[1].m_placement = Placement::East;
	}

	//____ Destructor _____________________________________________________________

	DesignOverlay::~DesignOverlay()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& DesignOverlay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setToolboxSkin() ___________________________________________________

	void DesignOverlay::setToolboxSkin(Skin * pSkin)
	{
		m_pToolboxSkin = pSkin;
		_requestRender();
	}

	//____ setSelectionSkin() _________________________________________________

	void DesignOverlay::setSelectionSkin(Skin * pSkin)
	{
		m_pSelectionSkin = pSkin;
		_requestRender();
	}

	//____ setEditMode() ______________________________________________________

	void DesignOverlay::setEditMode(bool bEditMode)
	{
		if (bEditMode != m_bEditMode)
		{
			m_bEditMode = bEditMode;
			_requestRender();
		}
	}

	//____ _matchingHeight() _______________________________________________________

	spx DesignOverlay::_matchingHeight(spx width, int scale) const
	{
		if (mainSlot._widget())
			return mainSlot._widget()->_matchingHeight(width, scale);
		else
			return Widget::_matchingHeight(width, scale);
	}

	//____ _matchingWidth() _______________________________________________________

	spx DesignOverlay::_matchingWidth(spx height, int scale) const
	{
		if (mainSlot._widget())
			return mainSlot._widget()->_matchingWidth(height,scale);
		else
			return Widget::_matchingWidth(height,scale);
	}

	//____ defaultSize() _____________________________________________________________

	SizeSPX DesignOverlay::_defaultSize(int scale) const
	{
		if (mainSlot._widget())
			return mainSlot._widget()->_defaultSize(scale);
		else
			return SizeSPX(1, 1);
	}

	//____ _findWidget() ____________________________________________________________

	Widget *  DesignOverlay::_findWidget(const CoordSPX& ofs, SearchMode mode)
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

	void DesignOverlay::_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window)
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
				m_pSelectionSkin->_render(pDevice, _selectionGeo(), m_scale, State::Normal);
		}

		for (auto& palette : palettes)
		{
			if (palette.m_bVisible)
			{
				RectSPX geo = palette.m_geo + _canvas.pos();

				if (m_pToolboxSkin)
					m_pToolboxSkin->_render(pDevice, geo + m_pToolboxSkin->_contentBorder(m_scale, State::Normal), m_scale, State::Normal);

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

	//____ _selectionGeo() ____________________________________________________

	RectSPX DesignOverlay::_selectionGeo() const
	{
		if (!m_pSelectedWidget)
			return RectSPX();

		RectSPX selectedGeo = m_pSelectedWidget->_globalGeo() - _globalPos();

		if (m_pSelectionSkin)
			selectedGeo += m_pSelectionSkin->_contentBorder(m_scale, State::Normal);

		return selectedGeo;
	}


	//____ _refreshRealGeo() __________________________________________________

	void DesignOverlay::_refreshRealGeo(ToolboxSlot * pSlot)
	{
		if (m_pSelectedWidget)
		{
			Placement placement = pSlot->m_placement;
			BorderSPX palettePadding = m_pToolboxSkin ? m_pToolboxSkin->_contentBorder(m_scale, State::Normal) : BorderSPX();

			SizeSPX wantedSize = pSlot->_widget()->_defaultSize(pSlot->_widget()->_scale()) + palettePadding;

			RectSPX selectedGeo = m_pSelectedWidget->_globalGeo() - _globalPos();
			selectedGeo += m_pSelectionSkin ? m_pSelectionSkin->_contentBorder(m_scale, State::Normal) : BorderSPX();
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

			if (pSlot->_widget()->_size() != childGeo.size())
				pSlot->_setSize(childGeo, m_scale);
		}
		else
		{
		}
	}

	//____ _selectWidget() ____________________________________________________

	void DesignOverlay::_selectWidget(Widget * pWidget)
	{
		if (pWidget == m_pSelectedWidget)
			return;

		// Update selectionSkin

		if (m_pSelectionSkin)
		{
			if (m_pSelectedWidget)
			{
				RectSPX geo = m_pSelectedWidget->_globalGeo() - _globalPos();
				geo += m_pSelectionSkin->_contentBorder(m_scale, State::Normal);
				_requestRender(geo);
			}

			m_pSelectedWidget = pWidget;

			if (m_pSelectedWidget)
			{
				RectSPX geo = m_pSelectedWidget->_globalGeo() - _globalPos();
				geo += m_pSelectionSkin->_contentBorder(m_scale, State::Normal);
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

			for (auto& palette : palettes)
			{
				palette.m_bVisible = true;
				_refreshRealGeo(&palette);
			}
		}
		else
		{
			BorderSPX palettePadding = m_pToolboxSkin ? m_pToolboxSkin->_contentBorder(m_scale, State::Normal) : BorderSPX();
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

	const TypeInfo&	DesignOverlay::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		if (pSlot == &mainSlot)
			return DynamicSlot::TYPEINFO;

		return ToolboxSlot::TYPEINFO;
	}

	//____ _childRequestResize() ______________________________________________

	void DesignOverlay::_childRequestResize( StaticSlot * pSlot )
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
		{
			auto p = static_cast<ToolboxSlot*>(pSlot);
			_refreshRealGeo( p );
		}
	}

	//____ _releaseChild() ____________________________________________________

	void DesignOverlay::_releaseChild(StaticSlot * pSlot)
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

	const Overlay::Slot * DesignOverlay::_beginOverlaySlots() const
	{
		return palettes._begin();
	}

	//____ _endOverlaySlots() ____________________________________________________

	const Overlay::Slot *  DesignOverlay::_endOverlaySlots() const
	{
		return palettes._end();
	}

	//____ _sizeOfOverlaySlot() __________________________________________________

	int DesignOverlay::_sizeOfOverlaySlot() const
	{
		return sizeof(ToolboxSlot);
	}


	//____ _resize() ___________________________________________________________

	void DesignOverlay::_resize( const SizeSPX& sz, int scale )
	{
		Overlay::_resize(sz, scale);

		// Refresh modal widgets geometry, their positions might have changed.

		for( auto pSlot = palettes._begin() ; pSlot != palettes._end() ; pSlot++ )
			_refreshRealGeo( pSlot );
	}

	//____ _receive() ______________________________________________________________

	void DesignOverlay::_receive( Msg * _pMsg )
	{
		if( !m_bEditMode )
			Overlay::_receive(_pMsg);


		switch( _pMsg->type() )
		{
			case MsgType::MousePress:
			{
				if (_pMsg->originalSourceRawPtr() == this)			// Make sure it wasn't a propagated press on widget in palette.
				{
					auto pMsg = static_cast<MousePressMsg*>(_pMsg);

					if (pMsg->button() == MouseButton::Left)
					{
						CoordSPX mousePos = pMsg->pointerSpxPos() - _globalPos();

						// Check for press on palette edge

						if (m_pToolboxSkin)
						{
							BorderSPX	contentBorder = m_pToolboxSkin->_contentBorder(m_scale, State::Normal);

							for (int i = 0; i < palettes.size(); i++)
							{
								if ((palettes[i].m_geo + contentBorder).contains(mousePos) )
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


			default:
				break;
		};

		Overlay::_receive(_pMsg);
	}
	
	//____ _createGenericSlotTool() ________________________________________

	Widget_p DesignOverlay::_createGenericSlotTool(const StaticSlot& slot)
	{
		auto pColumns = PackPanel::create();
		pColumns->setAxis(Axis::X);

		auto pHeaderColumn = PackPanel::create();
		pHeaderColumn->setAxis(Axis::Y);

		auto pValueColumn = PackPanel::create();
		pValueColumn->setAxis(Axis::Y);

		pColumns->slots << pHeaderColumn;
		pColumns->slots << pValueColumn;

		// Position

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("Position: ");

			CoordF pos = slot.pos();

			char buffer[256];
			sprintf(buffer, "%.2f, %.2f", pos.x, pos.y);

			auto pValue = TextDisplay::create();
			pValue->display.setText(buffer);

			pHeaderColumn->slots << pLabel;
			pValueColumn->slots << pValue;
		}

		// Size

		{
			auto pLabel = TextDisplay::create();
			pLabel->display.setText("Size: ");

			SizeF size = slot.size();

			char buffer[256];
			sprintf(buffer, "%.2f, %.2f", size.w, size.h);

			auto pValue = TextDisplay::create();
			pValue->display.setText(buffer);

			pHeaderColumn->slots << pLabel;
			pValueColumn->slots << pValue;
		}



		return pColumns;
	}

	//____ _createGenericWidgetTool() ______________________________________

	Widget_p DesignOverlay::_createGenericWidgetTool( Widget * pWidget )
	{
		auto pColumns = PackPanel::create();
		pColumns->setAxis(Axis::X);

		auto pHeaderColumn = PackPanel::create();
		pHeaderColumn->setAxis(Axis::Y);

		auto pValueColumn = PackPanel::create();
		pValueColumn->setAxis(Axis::Y);

		pColumns->slots << pHeaderColumn;
		pColumns->slots << pValueColumn;

		// ClassName

		{
			auto pNameLabel = TextDisplay::create();
			pNameLabel->display.setText("className: ");

			auto pName = TextDisplay::create();
			pName->display.setText(pWidget->typeInfo().className);

			pHeaderColumn->slots << pNameLabel;
			pValueColumn->slots << pName;
		}

		// Id

		{
//			char buffer[16];

			auto pIdLabel = TextDisplay::create();
			pIdLabel->display.setText("id: ");

			auto pId = TextDisplay::create();
			pId->display.setText(std::to_string(pWidget->id()));

			pHeaderColumn->slots << pIdLabel;
			pValueColumn->slots << pId;
		}

		return pColumns;
	}



} // namespace wg
