/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI ScrollChartEntryics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI ScrollChartEntryics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI ScrollChartEntryics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_base.h>
#include <wg_gfxdevice.h>
#include <wg_areascrollchart.h>

#include <cstring>

namespace wg
{

	const TypeInfo AreaScrollChart::TYPEINFO = {"AreaScrollChart", &Widget::TYPEINFO};

	//____ constructor ____________________________________________________________

	AreaScrollChart::AreaScrollChart() : entries(this)
	{
	}

	//____ destructor _____________________________________________________________

	AreaScrollChart::~AreaScrollChart()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& AreaScrollChart::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _update() ____________________________________________________________

	void AreaScrollChart::_update(int microPassed, int64_t microsecTimestamp)
	{
		ScrollChart::_update(microPassed, microsecTimestamp);
	}

	//____ _didAddEntries() ___________________________________________________

	void AreaScrollChart::_didAddEntries(AreaScrollChartEntry* pEntry, int nb)
	{
		_requestFullRedraw();
	}

	//____ _didMoveEntries() __________________________________________________

	void AreaScrollChart::_didMoveEntries(AreaScrollChartEntry* pFrom, AreaScrollChartEntry* pTo, int nb)
	{
		_requestFullRedraw();
	}

	//____ _willEraseEntries() ________________________________________________

	void AreaScrollChart::_willEraseEntries(AreaScrollChartEntry* pEntry, int nb)
	{
		_requestFullRedraw();
	}

	//____ _updateWaveformEdge() _________________________________________________

	void AreaScrollChart::_updateWaveformEdge(Waveform* pWaveform, uint64_t beginUS, int pixelIncUS, bool bTopEdge, AreaScrollChartEntry::SampleSet* pSamples)
	{
		// TODO: Better interpolation, especially when shrinking.

		int sampleIdx = bTopEdge ? 0 : 1;

        int wfSamples = pWaveform->nbSamples();

		spx * pConverted = (spx*) Base::memStackAlloc(wfSamples * sizeof(spx));

		SizeI	canvas = m_chartCanvas.size() / 64;

		float valueFactor = m_chartCanvas.h / (m_displayFloor - m_displayCeiling);

		uint64_t	pixelUS = beginUS;
		
		for (int i = 0; i < wfSamples; i++)
		{
			while( pSamples[1].timestamp < pixelUS )
				pSamples++;

			float frac1 = int(pixelUS - pSamples[0].timestamp) / float(pSamples[1].timestamp - pSamples[0].timestamp);
			float frac2 = 1.f - frac2;

			float val1 = (pSamples[0].samples[sampleIdx] - m_displayCeiling) * valueFactor;
			float val2 = (pSamples[1].samples[sampleIdx] - m_displayCeiling) * valueFactor;

			pConverted[i] = int(val1 * frac1 + val2 * frac2);
			
			pixelUS += pixelIncUS;
		}

		if (bTopEdge)
			pWaveform->setSamples(0, wfSamples, pConverted, nullptr);
		else
			pWaveform->setSamples(0, wfSamples, nullptr, pConverted);

		Base::memStackFree(wfSamples * sizeof(spx));
	}


	//____ _renderOnScrollSurface() ___________________________________________

	void AreaScrollChart::_renderOnScrollSurface( GfxDevice * pDevice, SizeSPX canvasSize, spx rightEdgeOfs, int64_t rightEdgeTimestamp, spx dirtLen )
	{
		
		int microsecPerPixel = m_displayTime / (canvasSize.w / 64);

//		int64_t leftEdgeTimestamp = rightEdgeTimestamp - dirtLen*microsecPerPixel;	// Left edge of dirty area, not whole window.
		
		
		
		
		for( auto& entry : entries )
		{
			// Calculate needed margin left and right of our window.
			
			pts maxThickness = std::max(entry.m_topOutlineThickness,entry.m_bottomOutlineThickness);
			
			int pixelMargin = (((maxThickness * m_scale) / 2) + 63) / 64;

			uint64_t firstEdgeTimestamp = rightEdgeTimestamp - (dirtLen+pixelMargin)*microsecPerPixel;
			uint64_t lastEdgeTimestamp = rightEdgeTimestamp + pixelMargin*microsecPerPixel;
			
			// Fill in with default samples if missing up to our lastEdgeTimestamp.
			
			if( entry.m_samples.empty() )
				_initEntrySamples(&entry);
			else if( entry.m_samples.back().timestamp < lastEdgeTimestamp )
			{
                AreaScrollChartEntry::SampleSet spl;
				spl.timestamp = lastEdgeTimestamp;
				spl.samples[0] = entry.m_defaultTopSample;
				spl.samples[1] = entry.m_defaultBottomSample;

				entry.m_samples.push_back(spl);
			}

			
			// Create our waveform
			
			SizeI waveformSize = { dirtLen/64 + pixelMargin*2, canvasSize.h / 64 };

			int edgesNeeded = dirtLen + 1;
 
			auto pWaveform = Waveform::create(WGBP(Waveform,
				_.size = waveformSize,
				_.bottomOutlineThickness = entry.m_bottomOutlineThickness*m_scale,
				_.color = entry.m_fillColor,
				_.gradient = entry.m_fillGradient,
				_.origo = SampleOrigo::Top,
				_.outlineColor = entry.m_outlineColor,
				_.outlineGradient = entry.m_outlineGradient,
				_.topOutlineThickness = entry.m_topOutlineThickness*m_scale
			) );
			
			
			// Find first sample to deal with
						
			auto pSample = entry.m_samples.begin();

			while( pSample->timestamp < firstEdgeTimestamp )
				pSample++;
			
			// 
			
			_updateWaveformEdge(pWaveform, firstEdgeTimestamp, microsecPerPixel, true, &(*pSample));
			_updateWaveformEdge(pWaveform, firstEdgeTimestamp, microsecPerPixel, false, &(*pSample));

			auto pEdgemap = pWaveform->refresh();
		
			
			
			
		}
		
		
		
		// Debug code
/*
		spx ofs = rightEdgeOfs;

		
		int timePerPixel = m_displayTime / (canvasSize.w/64);
		
		int descendPerPixel = 64;
		
		int64_t startPixel = rightEdgeTimestamp / timePerPixel;
		
		spx height = (startPixel*64) % canvasSize.h;
		
		
		int dirtWidth = dirtLen / 64;
		
		int memSize = dirtWidth*sizeof(CoordSPX);
		int memSize2 = dirtWidth*sizeof(HiColor);

		CoordSPX * p = (CoordSPX*) Base::memStackAlloc(memSize);
		HiColor * pCol = (HiColor*) Base::memStackAlloc(memSize2);
				
		for( int i = 0 ; i < dirtWidth ; i++ )
		{
			ofs -= 64;

			if( ofs < 0 )
				ofs += canvasSize.w;

			height -= 64;
			
			if( height < 0 )
				height += canvasSize.h;
			
			
			p[i].x = ofs;
			p[i].y = height;
			
			pCol[i] = Color::Red;
		}
		
		pDevice->plotPixels(dirtWidth, p, pCol );
		
		Base::memStackFree(memSize2);
		Base::memStackFree(memSize);
*/
		
		
	}

	//____ _startedOrEndedTransition() ________________________________________

	void AreaScrollChart::_startedOrEndedTransition()
	{

	}

	//____ _entryVisibilityChanged() __________________________________________

	void AreaScrollChart::_entryVisibilityChanged(AreaScrollChartEntry* pAreaChartEntry)
	{
		_requestFullRedraw();
	}

	//____ _initEntrySamples() ________________________________________________

	void AreaScrollChart::_initEntrySamples(AreaScrollChartEntry* pEntry)
	{
		AreaScrollChartEntry::SampleSet spl;
		spl.timestamp = m_latestTimestamp - 1000000000;
		spl.samples[0] = pEntry->m_defaultTopSample;
		spl.samples[1] = pEntry->m_defaultBottomSample;

		pEntry->m_samples.push_back(spl);
		
		spl.timestamp = m_latestTimestamp - m_latency;
		pEntry->m_samples.push_back(spl);
	}


	//____ AreaScrollChartEntry::constructor ___________________________________

	AreaScrollChartEntry::AreaScrollChartEntry()
	{
	}

	AreaScrollChartEntry::AreaScrollChartEntry(const AreaScrollChartEntry::Blueprint& bp)
	{
		m_bottomOutlineThickness 	= bp.bottomOutlineThickness;
		m_fillColor					= bp.color;
		m_fillGradient				= bp.gradient;
		m_outlineColor				= bp.outlineColor;
		m_outlineGradient			= bp.outlineGradient;
		m_topOutlineThickness		= bp.topOutlineThickness;
		m_bVisible					= bp.visible;
		
		m_defaultBottomSample		= bp.defaultBottomSample;
		m_defaultTopSample			= bp.defaultTopSample;
	}

	//____ AreaScrollChartEntry::addSamples() _________________________________

	void AreaScrollChartEntry::addSamples(int nbSamples, int sampleRate, const float* pTopSamples, const float * pBottomSamples, float rateTweak )
	{
		
		if( m_samples.empty() )
			m_pDisplay->_initEntrySamples(this);
		
		//
		
		float usPerSample = 1000000 / float(sampleRate);

		uint64_t timestamp = m_samples.back().timestamp;

		if( timestamp + nbSamples * usPerSample < m_pDisplay->m_latestTimestamp )
			usPerSample *= (1.f + rateTweak);
		else
			usPerSample *= (1.f - rateTweak);

		// Fill in the samples
		
		int offset = m_samples.size();
		m_samples.reserve(offset + nbSamples);
		
		for( int i = 0 ; i < nbSamples ; i++ )
			m_samples[offset+i].timestamp = timestamp + int64_t((i+1)*usPerSample);
		
		if( pTopSamples && pBottomSamples )
		{
			for( int i = 0 ; i < nbSamples ; i++ )
			{
				m_samples[offset+i].samples[0] = * pTopSamples++;
				m_samples[offset+i].samples[1] = * pBottomSamples++;
			}
		}
		else if( pTopSamples )
		{
			for( int i = 0 ; i < nbSamples ; i++ )
			{
				m_samples[offset+i].samples[0] = * pTopSamples++;
				m_samples[offset+i].samples[1] = m_defaultBottomSample;
			}
		}
		else if( pBottomSamples )
		{
			for( int i = 0 ; i < nbSamples ; i++ )
			{
				m_samples[offset+i].samples[0] = m_defaultTopSample;
				m_samples[offset+i].samples[1] = * pBottomSamples++;
			}
		}
		else
		{
			for( int i = 0 ; i < nbSamples ; i++ )
			{
				m_samples[offset+i].samples[0] = m_defaultTopSample;
				m_samples[offset+i].samples[1] = m_defaultBottomSample;
			}
		}
	}

	//____ AreaScrollChartEntry::setDefaultSample() _________________________________

	void AreaScrollChartEntry::setDefaultSample(float topSample, float bottomSample)
	{
		m_defaultTopSample = topSample;
		m_defaultBottomSample = bottomSample;
	}

	//____ AreaScrollChartEntry::setColors() _________________________________

	bool AreaScrollChartEntry::setColors(HiColor fill, HiColor outline, ColorTransition* pTransition)
	{
		if (!fill.isValid() || !outline.isValid())
			return false;

		if (!m_fillGradient.isUndefined() || !m_outlineGradient.isUndefined())
		{
			m_fillGradient = Gradient::Undefined;
			m_outlineGradient = Gradient::Undefined;

			m_pDisplay->_requestFullRedraw();
		}

		if (fill == m_fillColor && outline == m_outlineColor)
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

			m_pDisplay->_requestFullRedraw();
		}

		return true;
	}

	//____ AreaScrollChartEntry::setGradients() _________________________________

	bool AreaScrollChartEntry::setGradients(Gradient fill, Gradient outline, ColorTransition* pTransition)
	{
		if (!fill.isValid() || !outline.isValid())
			return false;

		if (fill == m_fillGradient && outline == m_outlineGradient)
			return true;

		if (pTransition)
		{
			m_pColorTransition = pTransition;
			m_colorTransitionProgress = 0;

			m_endFillGradient = fill;
			m_endOutlineGradient = outline;

			m_startFillGradient = m_fillGradient;
			m_startOutlineGradient = m_outlineGradient;

			m_pDisplay->_startedOrEndedTransition();
		}
		else
		{
			m_fillGradient = fill;
			m_outlineGradient = outline;

			m_pDisplay->_requestFullRedraw();
		}

		return true;
	}

	//____ AreaScrollChartEntry::setVisible() _________________________________

	void AreaScrollChartEntry::setVisible(bool bVisible)
	{
		if (bVisible != m_bVisible)
		{
			m_bVisible = bVisible;
			m_pDisplay->_entryVisibilityChanged(this);
		}
	}

	//____ AreaScrollChartEntry::_endColorTransition() ______________________________________________

	void AreaScrollChartEntry::_endColorTransition()
	{
		m_pColorTransition = nullptr;

		m_startFillColor = m_fillColor;
		m_endFillColor = m_fillColor;

		m_startOutlineColor = m_outlineColor;
		m_endOutlineColor = m_outlineColor;

		m_pDisplay->_startedOrEndedTransition();
	}


} // namespace wg
