/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI PlotChartEntryics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI PlotChartEntryics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI PlotChartEntryics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include	<wg_plotchart.h>
#include	<wg_gfxdevice.h>
#include	<wg_base.h>

#include	<cstring>

namespace wg
{

	const TypeInfo PlotChart::TYPEINFO = { "PlotChart", &Widget::TYPEINFO };


	//____ constructor ____________________________________________________________

	PlotChart::PlotChart() : entries(this)
	{
	}

	//____ destructor _____________________________________________________________

	PlotChart::~PlotChart()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PlotChart::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _startedOrEndedTransition() __________________________________________

	void PlotChart::_startedOrEndedTransition()
	{
		bool bTransitioning = false;

		for(auto& graph : entries)
		{
			if (graph.m_pSampleTransition || graph.m_pColorTransition)
			{
				bTransitioning = true;
				break;
			}
		}

		if( bTransitioning != m_bTransitioning)
		{
			if (bTransitioning)
				_startReceiveUpdates();
			else
				_stopReceiveUpdates();

			m_bTransitioning = bTransitioning;
		}
	}

	//____ _didAddEntries() ___________________________________________________

	void PlotChart::_didAddEntries(PlotChartEntry* pEntry, int nb)
	{
		for (int i = 0; i < nb; i++)
			pEntry->m_pDisplay = this;

		_requestRenderChartArea();
	}

	//____ _didMoveEntries() __________________________________________________

	void PlotChart::_didMoveEntries(PlotChartEntry* pFrom, PlotChartEntry* pTo, int nb)
	{
		_requestRenderChartArea();
	}

	//____ _willEraseEntries() ________________________________________________

	void PlotChart::_willEraseEntries(PlotChartEntry* pEntry, int nb)
	{
		_requestRenderChartArea();
	}

	//____ _fullRefreshOfChart() ______________________________________________

	void PlotChart::_fullRefreshOfChart()
	{
		_requestRenderChartArea();
	}

	//____ _renderCharts() ____________________________________________________

	void PlotChart::_renderCharts(GfxDevice* pDevice, const RectSPX& canvas)
	{
		auto popData = Util::limitClipList(pDevice, canvas);

		for (auto& entry : entries)
		{
			if (entry.m_bVisible)
			{
				if (!entry.m_sprite)
				{
					spx size = (entry.m_radius + entry.m_outlineThickness) * 2 * m_scale;
					spx canvasSize = Util::alignUp(size);
					spx offset = (canvasSize - size) / 2;

					auto pSurface = pDevice->surfaceFactory()->createSurface(WGBP(Surface, _.format = PixelFormat::BGRA_8, _.size = { size / 64,size / 64 }, _.canvas = true));

					pDevice->beginCanvasUpdate(pSurface);
					pDevice->setBlendMode(BlendMode::Replace);
					pDevice->fill(HiColor::Transparent);
					pDevice->setBlendMode(BlendMode::Blend);
					pDevice->drawElipse({ offset, offset, canvasSize, canvasSize }, size, 
						entry.m_fillColor, entry.m_outlineThickness*m_scale, entry.m_outlineColor);
					pDevice->endCanvasUpdate();

					entry.m_sprite = pSurface;
				}

				SizeSPX center = entry.m_sprite->pixelSize() * 32;

				pDevice->setBlitSource(entry.m_sprite);

				if (!entry.m_tintGradient.isUndefined())
					pDevice->setTintGradient(canvas, entry.m_tintGradient);

				for (auto coord : entry.m_samples)
				{
					spx coordX = coord.x * canvas.w;
					spx coordY = (coord.y - m_displayCeiling) / (m_displayFloor - m_displayCeiling) * canvas.h;

					CoordSPX spxCoord = Util::align(CoordSPX( coordX - center.w, coordY - center.h ));
					pDevice->blit(spxCoord);
				}

				if (!entry.m_tintGradient.isUndefined())
					pDevice->clearTintGradient();
			}
		}

		Util::popClipList(pDevice, popData);
	}

	//____ _update() ____________________________________________________________

	void PlotChart::_update(int microPassed, int64_t microsecTimestamp)
	{
		Chart::_update(microPassed, microsecTimestamp);

		if (!m_bTransitioning)
			return;

		bool	transitionsActive = false;

		// Update all transitions

		for (auto& graph : entries)
		{
			if (graph.m_pSampleTransition)
			{
				int timestamp = graph.m_sampleTransitionProgress + microPassed;

				if( timestamp >= graph.m_pSampleTransition->duration() )
				{
					graph.m_sampleTransitionProgress = 0;
					graph.m_pSampleTransition = nullptr;
					graph.m_samples.swap( graph.m_endSamples );
				}
				else
				{
					graph.m_sampleTransitionProgress = timestamp;

					graph.m_pSampleTransition->snapshot(timestamp, (int) graph.m_samples.size(),
														graph.m_startSamples.data(),
														graph.m_endSamples.data(), 
														graph.m_samples.data());	

					transitionsActive = true;
				}
			}

			if (graph.m_pColorTransition)
			{
				int timestamp = graph.m_colorTransitionProgress + microPassed;

				if (timestamp >= graph.m_pColorTransition->duration())
				{
					graph.m_colorTransitionProgress = 0;
					graph.m_pColorTransition = nullptr;

					graph.m_fillColor = graph.m_endFillColor;
					graph.m_outlineColor = graph.m_endOutlineColor;
				}
				else
				{
					graph.m_colorTransitionProgress = timestamp;

					graph.m_fillColor = graph.m_pColorTransition->snapshot(timestamp, graph.m_startFillColor, graph.m_endFillColor);
					graph.m_outlineColor = graph.m_pColorTransition->snapshot(timestamp, graph.m_startOutlineColor, graph.m_endOutlineColor);

					transitionsActive = true;
				}
			}

			if (graph.m_pTintTransition)
			{
				int timestamp = graph.m_tintTransitionProgress + microPassed;

				if (timestamp >= graph.m_pTintTransition->duration())
				{
					graph.m_tintTransitionProgress = 0;
					graph.m_pTintTransition = nullptr;

					graph.m_tintGradient = graph.m_endTintGradient;
				}
				else
				{
					graph.m_tintTransitionProgress = timestamp;

					if (!graph.m_tintGradient.isUndefined())
					{
						graph.m_tintGradient.topLeft = graph.m_pColorTransition->snapshot(timestamp, graph.m_startTintGradient.topLeft, graph.m_endTintGradient.topLeft);
						graph.m_tintGradient.topRight = graph.m_pColorTransition->snapshot(timestamp, graph.m_startTintGradient.topRight, graph.m_endTintGradient.topRight);
						graph.m_tintGradient.bottomLeft = graph.m_pColorTransition->snapshot(timestamp, graph.m_startTintGradient.bottomLeft, graph.m_endTintGradient.bottomLeft);
						graph.m_tintGradient.bottomRight = graph.m_pColorTransition->snapshot(timestamp, graph.m_startTintGradient.bottomRight, graph.m_endTintGradient.bottomRight);
					}

					transitionsActive = true;
				}
			}

		}

		if (transitionsActive)
			_requestRenderChartArea();
		else
		{
			m_bTransitioning = false;
			_stopReceiveUpdates();
		}
	}

	//____ PlotChartEntry::PlotChartEntry() _______________________________________________________

	PlotChartEntry::PlotChartEntry(const Blueprint& bp)
	{
		m_fillColor = bp.color;
		m_tintGradient = bp.gradient;
		m_outlineColor = bp.outlineColor;
		m_outlineThickness = bp.outlineThickness;
		m_radius = bp.radius;
		m_bVisible = bp.visible;
	}

	//____ setColors() ________________________________________________________

	bool PlotChartEntry::setColors(HiColor fill, HiColor outline, ColorTransition* pTransition)
	{
		if( !fill.isValid() || !outline.isValid() )
			return false;

		if( fill == m_fillColor && outline == m_outlineColor )
			return true;

		if (pTransition)
		{
			m_pColorTransition = pTransition;
			m_colorTransitionProgress = 0;

			m_endFillColor = fill;
			m_endOutlineColor = outline;

			m_startFillColor = m_fillColor;
			m_startOutlineColor = m_outlineColor;

			m_pDisplay->_startedOrEndedTransition();
		}
		else
		{
			m_fillColor = fill;
			m_outlineColor = outline;

			m_pDisplay->_requestRenderChartArea();
		}

		return true;
	}

	//____ setSize() __________________________________________________________

	bool PlotChartEntry::setSize(pts radius, pts outlineThickness, ValueTransition* pTransition)
	{
		if (radius < 0 || outlineThickness < 0)
			return false;

		if (radius == m_radius && outlineThickness == m_outlineThickness)
			return true;

		if (pTransition)
		{
			m_pSizeTransition = pTransition;
			m_sizeTransitionProgress = 0;

			m_endRadius = radius;
			m_endOutlineThickness = outlineThickness;

			m_startRadius = m_radius;
			m_startOutlineThickness = m_outlineThickness;

			m_pDisplay->_startedOrEndedTransition();
		}
		else
		{
			m_radius = radius;
			m_outlineThickness = outlineThickness;

			m_pDisplay->_requestRenderChartArea();
		}

		return true;
	}



	//____ setTintGradients() _____________________________________________________

	bool PlotChartEntry::setTintGradient(Gradient tint, ColorTransition* pTransition)
	{
		if( !tint.isValid() )
			return false;

		if (tint == m_tintGradient)
			return true;

		if (pTransition)
		{
			m_pTintTransition = pTransition;
			m_tintTransitionProgress = 0;

			m_endTintGradient = tint;
			m_startTintGradient = m_tintGradient;


			m_pDisplay->_startedOrEndedTransition();
		}
		else
		{
			m_tintGradient = tint;

			m_pDisplay->_requestRenderChartArea();
		}

		return true;
	}


	//____ setSamples() _______________________________________________________

	void PlotChartEntry::setSamples(int nSamples, CoordF* pSamples)
	{
		if (nSamples == 0)
		{
			m_samples.clear();
		}
		else
		{
			m_samples.resize(nSamples);
			memcpy(m_samples.data(), pSamples, nSamples * sizeof(CoordF));
		}

		_endSampleTransition();

		m_pDisplay->_requestRenderChartArea();
	}

	//____ transitionSamples() ________________________________________________

	bool PlotChartEntry::transitionSamples(CoordTransition* pTransition, int nSamples, CoordF* pNewSamples)
	{
		if( nSamples != m_samples.size() )
			return false;

		m_pSampleTransition = pTransition;
		m_sampleTransitionProgress = 0;

		m_startSamples.resize(nSamples);
		memcpy(m_startSamples.data(), m_samples.data(), nSamples * sizeof(CoordF));

		m_endSamples.resize(nSamples);
		memcpy(m_endSamples.data(), pNewSamples, nSamples * sizeof(CoordF));

		m_pDisplay->_startedOrEndedTransition();

		return true;
	}

	//____ setVisible() _______________________________________________________

	void PlotChartEntry::setVisible(bool bVisible)
	{
		if( bVisible != m_bVisible )
		{
			m_bVisible = bVisible;
			m_pDisplay->_requestRenderChartArea();
		}
	}

	//____ samples() _______________________________________________________

	std::tuple<int, const CoordF*>  PlotChartEntry::samples() const 
	{ 
		return std::make_tuple((int)m_samples.size(), m_samples.data()); 
	}

	//____ _endSampleTransition() _____________________________________________

	void PlotChartEntry::_endSampleTransition()
	{
		m_pSampleTransition = nullptr;

		m_startSamples.resize(0);
		m_endSamples.resize(0);

		m_pDisplay->_startedOrEndedTransition();
	}

	//____ _endColorTransition() ______________________________________________

	void PlotChartEntry::_endColorTransition()
	{
		m_pColorTransition = nullptr;

		m_startFillColor = m_fillColor;
		m_endFillColor = m_fillColor;

		m_startOutlineColor = m_outlineColor;
		m_endOutlineColor = m_outlineColor;

		m_pDisplay->_startedOrEndedTransition();
	}

	//____ _endSizeTransition() _______________________________________________

	void PlotChartEntry::_endSizeTransition()
	{
		m_pSizeTransition = nullptr;
		m_startRadius = m_radius;
		m_startOutlineThickness = m_outlineThickness;

		m_pDisplay->_startedOrEndedTransition();
	}

	//____ _endTintTransition() _______________________________________________

	void PlotChartEntry::_endTintTransition()
	{
		m_pTintTransition = nullptr;
		m_startTintGradient = m_tintGradient;

		m_pDisplay->_startedOrEndedTransition();
	}



}
