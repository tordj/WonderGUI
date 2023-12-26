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

#include	<wg_chart.h>
#include	<wg_gfxdevice.h>
#include	<wg_base.h>

#include	<cstring>

namespace wg
{

	const TypeInfo Chart::TYPEINFO = { "Chart", &Widget::TYPEINFO };


	//____ constructor ____________________________________________________________

	Chart::Chart() : xLines(this), yLines(this),
		m_displaySkin(this), m_labelSkin(this)
	{
		m_displayCeiling = 0.f;
		m_displayFloor = 1.f;

		m_gridColor		= Color::DarkGray;
		m_gridThickness = 1;

		m_sideLabelPlacement = Placement::West;
		m_bottomLabelPlacement = Placement::South;

		m_sideLabelSpacing = 4;
		m_bottomLabelSpacing = 1;
	}

	//____ destructor _____________________________________________________________

	Chart::~Chart()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Chart::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setDisplayRange() __________________________________________________

	void Chart::setDisplayRange(float ceiling, float floor)
	{
		if (ceiling == m_displayCeiling && floor == m_displayFloor)
			return;

		m_displayCeiling = ceiling;
		m_displayFloor = floor;

		_fullRefreshOfChart();
	}

	//____ _render() ____________________________________________________________

	void Chart::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		RectSPX graphCanvas = m_graphCanvas + _canvas.pos();

		if (!m_displaySkin.isEmpty())
			m_displaySkin.render(pDevice, graphCanvas + m_displaySkin.contentBorder(m_scale, m_state), m_scale, m_state);

		float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
		float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

		// Render grid xLines

		{
			for (auto& line : xLines)
			{
				if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax)
				{
					float valueFactor = (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

					CoordSPX pos = graphCanvas.pos();
					pos.y += (line.m_value - m_displayCeiling) * valueFactor;

					HiColor color = line.m_color == HiColor::Undefined ? m_gridColor : line.m_color;
					pts thickness = line.m_thickness > 0 ? line.m_thickness : m_gridThickness;

					pDevice->drawLine(pos, Direction::Right, graphCanvas.w, color, thickness * m_scale);
				}
			}
		}

		// Render grid yLines

		{
			for (auto& line : yLines)
			{
				if (line.m_bVisible)
				{
					CoordSPX pos = graphCanvas.pos();
					pos.x += line.m_value * graphCanvas.w;

					HiColor color = line.m_color == HiColor::Undefined ? m_gridColor : line.m_color;
					pts thickness = line.m_thickness > 0 ? line.m_thickness : m_gridThickness;

					pDevice->drawLine(pos, Direction::Down, graphCanvas.h, color, thickness * m_scale);
				}
			}
		}


		// Render graphs

		_renderCharts(pDevice,graphCanvas);

		// Render labels

		for (auto& line : xLines)
		{
			if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax && !line.m_label.isEmpty())
			{
				RectSPX geo = line.m_labelGeo + _canvas.pos();

				SkinSlot* pSkin = line.m_labelSkin.isEmpty() ? &m_labelSkin : &line.m_labelSkin;

				if (!pSkin->isEmpty())
				{
					pSkin->render(pDevice, geo, m_scale, m_state, line.m_value);
					geo = pSkin->contentRect(geo, m_scale, m_state);
				}

				TextLayout* pLayout = line.m_pTextLayout ? line.m_pTextLayout : (m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout());

				pLayout->render(&line, pDevice, geo);
			}
		}

		for (auto& line : yLines)
		{
			if (line.m_bVisible && !line.m_label.isEmpty())
			{
				RectSPX geo = line.m_labelGeo + _canvas.pos();

				SkinSlot* pSkin = line.m_labelSkin.isEmpty() ? &m_labelSkin : &line.m_labelSkin;

				if (!pSkin->isEmpty())
				{
					pSkin->render(pDevice, geo, m_scale, m_state, line.m_value);
					geo = pSkin->contentRect(geo, m_scale, m_state);
				}

				TextLayout* pLayout = line.m_pTextLayout ? line.m_pTextLayout : (m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout());

				pLayout->render(&line, pDevice, geo);
			}
		}



	}



	//____ _didAddEntries() ___________________________________________________

	void Chart::_didAddEntries(GridLine* pEntry, int nb)
	{
		// Add new TextItems to TextLayout

		TextLayout* pDefaultLayout = m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout();

		for (int i = 0; i < nb; i++)
		{
			pEntry[i].m_pDisplay = this;
			
			TextLayout* pLayout = pEntry[i].m_pTextLayout ? pEntry[i].m_pTextLayout.rawPtr() : pDefaultLayout;
			pLayout->addText(&pEntry[i]);
		}

		//

		_recalcGraphCanvas();
		_repositionAllLabels();
		_requestRender();
	}

	//____ _didMoveEntries() __________________________________________________

	void Chart::_didMoveEntries(GridLine* pFrom, GridLine* pTo, int nb)
	{
		// Notify TextLayout of moved TextItems

		TextLayout* pDefaultLayout = m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout();

		for (int i = 0; i < nb; i++)
		{
			TextLayout* pLayout = pTo[i].m_pTextLayout ? pTo[i].m_pTextLayout.rawPtr() : pDefaultLayout;
			pLayout->didMoveText(&pFrom[i], &pTo[i]);
		}

		// Re-render in case 

		_requestRender();
	}

	//____ _willEraseEntries() ________________________________________________

	void Chart::_willEraseEntries(GridLine* pEntry, int nb)
	{
		// Remove TextItems from TextLayout

		TextLayout* pDefaultLayout = m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout();

		for (int i = 0; i < nb; i++)
		{
			TextLayout* pLayout = pEntry[i].m_pTextLayout ? pEntry[i].m_pTextLayout.rawPtr() : pDefaultLayout;
			pLayout->removeText(&pEntry[i]);
		}

		//

		if (_recalcGraphCanvas())
			_repositionAllLabels();

		_requestRender();
	}

	//____ _recalcGraphCanvas() _______________________________________________

	bool Chart::_recalcGraphCanvas()
	{
		RectSPX contentGeo = m_skin.isEmpty() ? RectSPX(0,0,m_size) : m_skin.contentRect(RectSPX(0,0,m_size),m_scale, m_state);

		BorderSPX margin;

		if (!m_displaySkin.isEmpty())
			margin = m_displaySkin.contentBorder(m_scale, m_state);



		// Space needed left of graph for left labels, we include all labels, including
		// the invisible and out of range ones se we don't need to adjust if they pop in.

		for (auto& line : xLines)
		{
			if (!line.m_label.isEmpty())
			{
				CoordSPX ofs = _sideLabelOffset(&line);
				ofs.x *= -1;

				if (ofs.x > margin.left)
					margin.left = ofs.x;
			}
		}

		// Space needed below graph for bottom labels, we include all labels, including
		// the invisible ones so we don't need to adjust when they are made visible.

		for (auto& line : yLines)
		{
			if (!line.m_label.isEmpty())
			{
				CoordSPX ofs = _bottomLabelOffset(&line);
				ofs.y += line.m_labelGeo.size().h;

				if (ofs.y > margin.bottom)
					margin.bottom = ofs.y;
			}
		}


		
		// Adjust top and bottom margins so no side labels sticks out above/below canvas.

		float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
		float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

		float valueFactor = (m_displayFloor - m_displayCeiling) * contentGeo.h;

		if (!xLines.isEmpty())
		{
			// Find top & bottom line with label.

			GridLine* pTop = nullptr;
			GridLine* pBottom = nullptr;

			for (auto& line : xLines)
			{
				if (line.m_bVisible && !line.m_label.isEmpty() && line.m_value >= rangeMin && line.m_value <= rangeMax )
				{
					if (pTop == nullptr || line.m_value > pTop->m_value)
						pTop = &line;

					if (pBottom == nullptr || line.m_value < pBottom->m_value)
						pBottom = &line;

				}
			}

			if (m_displayFloor > m_displayCeiling)
				std::swap(pTop, pBottom);

			if (pTop)		// If we have top we also have bottom.
			{
				spx topLabelOfs = _sideLabelOffset(pTop).y;
				spx bottomLabelOfs = _sideLabelOffset(pBottom).y + pBottom->m_labelGeo.h;


				bool bModified;
				do {
					bModified = false;

					spx canvasHeight = contentGeo.h - margin.top - margin.bottom;

					spx topLineOfs = (pTop->m_value - m_displayCeiling) * (m_displayFloor - m_displayCeiling) * canvasHeight;
					if (margin.top < -(topLineOfs + topLabelOfs) )
					{
						margin.top = -(topLineOfs + topLabelOfs);
						bModified = true;
					}

					spx bottomLineOfs = (pBottom->m_value - m_displayCeiling) * (m_displayFloor - m_displayCeiling) * canvasHeight;
					if (margin.bottom < (bottomLineOfs + bottomLabelOfs ) - canvasHeight )
					{
						margin.bottom = (bottomLineOfs + bottomLabelOfs) - canvasHeight;
						bModified = true;
					}

				} while (bModified == true);

			}
		}

		// Adjust side margins so no bottom labels sticks out left/right of canvas.

		if (!yLines.isEmpty())
		{
			// Find leftmost and rightmost line with label.

			GridLine* pLeftmost = nullptr;
			GridLine* pRightmost = nullptr;

			for (auto& line : yLines)
			{
				if (line.m_bVisible && !line.m_label.isEmpty() && line.m_value >= 0.f && line.m_value <= 1.f)
				{
					if (pLeftmost == nullptr || line.m_value < pLeftmost->m_value)
						pLeftmost = &line;

					if (pRightmost == nullptr || line.m_value > pRightmost->m_value)
						pRightmost = &line;

				}
			}

			if (pLeftmost)		// If we have leftmost we also have rightmost.
			{
				spx leftLabelOfs = _bottomLabelOffset(pLeftmost).x;
				spx rightLabelOfs = _bottomLabelOffset(pRightmost).x + pRightmost->m_labelGeo.w;

				bool bModified;
				do {
					bModified = false;

					spx canvasWidth = contentGeo.w - margin.left - margin.right;

					spx leftLineOfs = pLeftmost->m_value * canvasWidth;
					if (margin.left < -(leftLineOfs + leftLabelOfs))
					{
						margin.left = -(leftLineOfs + leftLabelOfs);
						bModified = true;
					}

					spx rightLineOfs = pRightmost->m_value * canvasWidth;
					if (margin.right < (rightLineOfs + rightLabelOfs) - canvasWidth)
					{
						margin.right = (rightLineOfs + rightLabelOfs) - canvasWidth;
						bModified = true;
					}

				} while (bModified == true);

			}
		}


		// Finish up

		RectSPX graphCanvas = contentGeo - margin;

		if (graphCanvas == m_graphCanvas)
			return false;

		m_graphCanvas = graphCanvas;
		return true;
	}

	//____ _repositionAllLabels() _____________________________________________

	void Chart::_repositionAllLabels()
	{
		float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
		float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

		float valueFactor = (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

		for (auto& line : xLines)
		{
			if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax)
			{
				CoordSPX pos = m_graphCanvas.pos();
				pos.y += (line.m_value - m_displayCeiling) * valueFactor;

				pos += _sideLabelOffset(&line);
				line.m_labelGeo.setPos(Util::align(pos));
			}
		}

		for (auto& line : yLines)
		{
			if (line.m_bVisible)
			{
				CoordSPX pos = m_graphCanvas.bottomLeft();
				pos.x += line.m_value * m_graphCanvas.w;

				pos += _bottomLabelOffset(&line); 
				line.m_labelGeo.setPos(Util::align(pos));
			}
		}
	}


	//____ _sideLabelOffset() ___________________________________________________

	CoordSPX Chart::_sideLabelOffset(GridLine* pLine)
	{
		spx spacing = Util::ptsToSpx(m_sideLabelSpacing, m_scale);

		Placement placement = pLine->m_labelPlacement != Placement::Undefined ? pLine->m_labelPlacement : m_sideLabelPlacement;
		CoordSPX pos = Util::placeRectAroundCoord(placement, { 0,0 }, pLine->m_labelGeo.size() + SizeSPX(2 * spacing, 0));
		pos += Util::ptsToSpx(pLine->m_labelAdjustment, m_scale);
		pos.x += spacing;
		return pos;
	}

	//____ _bottomLabelOffset() ___________________________________________________

	CoordSPX Chart::_bottomLabelOffset(GridLine* pLine)
	{
		spx spacing = Util::ptsToSpx(m_bottomLabelSpacing, m_scale);

		Placement placement = pLine->m_labelPlacement != Placement::Undefined ? pLine->m_labelPlacement : m_bottomLabelPlacement;
		CoordSPX pos = Util::placeRectAroundCoord(placement, { 0,0 }, pLine->m_labelGeo.size() + SizeSPX(0, 2 * spacing));
		pos += Util::ptsToSpx(pLine->m_labelAdjustment, m_scale);
		pos.y += spacing;
		return pos;
	}

	//____ _resize() __________________________________________________________

	void Chart::_resize(const SizeSPX& size, int scale)
	{
		int oldScale = m_scale;
		Widget::_resize(size, scale);

		// Resize labels if scale has changed

		if (scale != oldScale)
		{
			for (auto& gridLine : xLines)
				gridLine._resizeLabel(scale, oldScale);

			for (auto& gridLine : yLines)
				gridLine._resizeLabel(scale, oldScale);
		}


		_recalcGraphCanvas();
		_repositionAllLabels();

		_fullRefreshOfChart();
	}

	//____ _textSize() ________________________________________________________

	SizeSPX GridLine::_textSize() const
	{
		if (!m_labelSkin.isEmpty())
			return m_labelGeo.size() - m_labelSkin.contentBorderSize(m_pDisplay->_scale());
		else if( !m_pDisplay->m_labelSkin.isEmpty() )
			return m_labelGeo.size() - m_pDisplay->m_labelSkin.contentBorderSize(m_pDisplay->_scale());
		else
			return m_labelGeo.size();
	}

	//____ _textState() _______________________________________________________

	State GridLine::_textState() const
	{
		return m_pDisplay->m_state;
	}

	//____ _textStyle() _______________________________________________________

	TextStyle* GridLine::_textStyle() const
	{
		if (m_pTextStyle)
			return m_pTextStyle;
		else if (m_pDisplay->m_pTextStyle)
			return m_pDisplay->m_pTextStyle;

		return Base::defaultStyle();
	}

	//____ _textBegin() _______________________________________________________

	const Char* GridLine::_textBegin() const
	{
		return m_label.chars();
	}

	//____ _textLength() ______________________________________________________

	int GridLine::_textLength() const
	{
		return m_label.length();
	}

	//____ _caretVisible() ____________________________________________________

	bool GridLine::_caretVisible() const
	{
		return false;
	}

	//____ _caretOffset() _____________________________________________________

	int GridLine::_caretOffset() const
	{
		return 0;
	}

	//____ _selectedText() ____________________________________________________

	std::tuple<int, int> GridLine::_selectedText() const
	{
		return std::make_tuple(0, 0);
	}

	//____ _mapperRequestRender() _____________________________________________

	void GridLine::_mapperRequestRender()
	{
		RectSPX textGeo = m_labelGeo;

		if (!m_labelSkin.isEmpty() )
			textGeo -= m_labelSkin.contentBorder(m_pDisplay->m_scale, m_pDisplay->m_state);
		else if (!m_pDisplay->m_labelSkin.isEmpty())
			textGeo -= m_pDisplay->m_labelSkin.contentBorder(m_pDisplay->m_scale, m_pDisplay->m_state);

		m_pDisplay->_requestRender(textGeo);
	}

	void GridLine::_mapperRequestRender(const RectSPX& rect)
	{
		CoordSPX textPos = m_labelGeo.pos();

		if (!m_labelSkin.isEmpty())
			textPos += m_labelSkin.contentOfs(m_pDisplay->m_scale, m_pDisplay->m_state);
		else if (!m_pDisplay->m_labelSkin.isEmpty())
			textPos += m_pDisplay->m_labelSkin.contentOfs(m_pDisplay->m_scale, m_pDisplay->m_state);


		m_pDisplay->_requestRender(rect + textPos);
	}

	//____ _mapperRequestResize() _____________________________________________

	void GridLine::_mapperRequestResize()
	{
		_resizeLabel(m_pDisplay->m_scale, m_pDisplay->m_scale);
	}

	//____ _scale() ___________________________________________________________

	int GridLine::_scale() const
	{
		return m_pDisplay->m_scale;
	}

	//____ _resizeLabel() _____________________________________________________

	void GridLine::_resizeLabel(int scale, int oldScale)
	{
		TextLayout* pLayout			= m_pTextLayout ? m_pTextLayout : (m_pDisplay->m_pTextLayout ? m_pDisplay->m_pTextLayout : Base::defaultTextLayout() );
		SkinSlot *	pLabelSkin		= m_labelSkin.isEmpty() ? &m_pDisplay->m_labelSkin : &m_labelSkin;
		SizeSPX		oldLabelPadding;
		SizeSPX		newLabelPadding;

		if( !pLabelSkin->isEmpty() )
		{
			oldLabelPadding = pLabelSkin->contentBorderSize(oldScale);
			newLabelPadding = pLabelSkin->contentBorderSize(scale);
		}

		SizeSPX newSize = pLayout->defaultSize(this, scale);
		SizeSPX oldSize;
		
		if( !m_labelGeo.size().isEmpty() )
			oldSize = m_labelGeo.size() - oldLabelPadding;

		pLayout->onResized(this, newSize, oldSize, scale, oldScale);
		m_labelGeo.setSize(newSize + newLabelPadding);
	}

	//____ _skinValue() _______________________________________________________

	float GridLine::_skinValue(const SkinSlot* pSlot) const
	{
		return 1.f;
	}

	//____ _skinValue2() ______________________________________________________

	float GridLine::_skinValue2(const SkinSlot* pSlot) const
	{
		return -1.f;
	}

	//____ _skinState() _______________________________________________________

	State GridLine::_skinState(const SkinSlot* pSlot) const
	{
		return m_pDisplay->state();
	}

	//____ _skinSize() ________________________________________________________

	SizeSPX GridLine::_skinSize(const SkinSlot* pSlot) const
	{
		return m_labelGeo.size();
	}

	//____ _skinRequestRender() _______________________________________________

	void GridLine::_skinRequestRender(const SkinSlot* pSlot)
	{
		m_pDisplay->_requestRender(m_labelGeo);
	}

	void GridLine::_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect)
	{
		m_pDisplay->_requestRender(rect + m_labelGeo.pos());
	}
}
