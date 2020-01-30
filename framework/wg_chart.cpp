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

#include <wg_chart.h>
#include <wg_gfxdevice.h>
#include <wg_pen.h>
#include <wg_base.h>
#include <wg_texttool.h>
#include <wg_util.h>
#include <wg3_surfacefactory.h>

#include <algorithm>

static const char	c_widgetType[] = {"Chart"};

//____ Constructor ____________________________________________________________

WgChart::WgChart()
{
	m_defaultSize = WgSize(200,100);
	m_bOpaque = false;
	m_mode = WG_MODE_NORMAL;

	m_bDynamicValueRange = true;
	m_bDynamicSampleRange = true;

	m_topValue			= 1.f;
	m_bottomValue		= -1.f;

	m_firstSample		= 0.f;
	m_lastSample		= 0.f;

	m_sampleLabelStyle.alignment = WgOrigo::South;
	m_sampleLabelStyle.offset = { 0,0 };

	m_valueLabelStyle.alignment = WgOrigo::West;
	m_valueLabelStyle.offset = { 0,0 };

	m_waveIdCounter = 1;
	m_renderSectionWidth = 220;			// Keep a high value until we have upgraded to new GfxDevice with multi-clip support.
}

//____ Destructor _____________________________________________________________

WgChart::~WgChart()
{
}

//____ Type() _________________________________________________________________

const char * WgChart::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgChart::GetClass()
{
	return c_widgetType;
}

//____ SetPreferredChartSize() _______________________________________________________

void WgChart::SetPreferredChartSize( const WgSize& size )
{
	if( size != m_defaultSize )
	{
		m_defaultSize = size;
		_requestResize();
	}
}

//____ PreferredPixelSize() __________________________________________________________

WgSize WgChart::PreferredPixelSize() const
{
	WgSize sz = (m_defaultSize * m_scale) / WG_SCALE_BASE;

	sz += m_pixelPadding;
	if (m_pSkin)
		sz = _skinSizeForContent( m_pSkin, sz, m_scale);

	return sz;
}

//____ SetSkin() ______________________________________________________________

void WgChart::SetSkin(wg::Skin * pSkin)
{
    //TODO: Might need resampling if content padding has changed.
    
	m_pSkin = pSkin;
	_requestResize();		// Just in case
	_requestRenderInCache();
}

//____ SetCanvasPadding() _____________________________________________________

void WgChart::SetCanvasPadding(WgBorders padding)
{
	m_pointPadding = padding;
	WgBorders pixelPadding( (padding.top * m_scale) >> WG_SCALE_BINALS, (padding.right * m_scale) >> WG_SCALE_BINALS, 
							(padding.bottom * m_scale) >> WG_SCALE_BINALS, (padding.left * m_scale) >> WG_SCALE_BINALS );

	if (pixelPadding != m_pixelPadding)
	{
		m_pixelPadding = pixelPadding;
		_resampleAllWaves();
		_requestRenderInCache();
	}
}

//____ CanvasPadding() ________________________________________________________

WgBorders WgChart::CanvasPadding() const
{
	return m_pointPadding;
}

//____ ClearWaves() ___________________________________________________________

void WgChart::ClearWaves()
{
	m_waves.clear();
	_requestRenderInCache();
}

//____ AddWave() ______________________________________________________________

int WgChart::AddWave()
{
	m_waves.emplace_back();
	Wave&	w = m_waves.back();
	w.id = m_waveIdCounter++;
	return w.id;
}

//____ IsWaveDisplayed() ______________________________________________________

bool WgChart::IsWaveDisplayed(int waveId) const
{
	const Wave * p = _getWave(waveId);
	if (!p)
		return false;

	return !p->bHidden;
}

//____ HideWave() _____________________________________________________________

bool WgChart::HideWave(int waveId)
{
	Wave * p = _getWave(waveId);
	if (!p)
		return false;

	if (p->bHidden == false)
	{
		p->bHidden = true;

        if( waveId >= m_cacheFirst && waveId <= m_cacheLast )
            _requestRenderInCache();
        else
            _requestRender();
	}
	return true;
}

//____ UnhideWave() ___________________________________________________________

bool WgChart::UnhideWave(int waveId)
{
	Wave * p = _getWave(waveId);
	if (!p)
		return false;

	if (p->bHidden == true)
	{
		p->bHidden = false;
        if( waveId >= m_cacheFirst && waveId <= m_cacheLast )
            _requestRenderInCache();
        else
            _requestRender();
    }
	return true;
}

//____ HideAllWaves() _________________________________________________________

void WgChart::HideAllWaves()
{
	for (auto& wave : m_waves)
	{
		if (!wave.bHidden)
		{
			wave.bHidden = true;
			_requestRenderInCache();
		}
	}
}

//____ UnhideAllWaves() _______________________________________________________

void WgChart::UnhideAllWaves()
{
	for (auto& wave : m_waves)
	{
		if (wave.bHidden)
		{
			wave.bHidden = false;
			_requestRenderInCache();
		}
	}
}



//____ SetWaveStyle() _________________________________________________________

bool WgChart::SetWaveStyle(int waveId, WgColor frontFill, WgColor backFill, float topLineThickness, WgColor topLineColor, float bottomLineThickness, WgColor bottomLineColor)
{
	Wave * p = _getWave(waveId);
	if (!p)
		return false;

    // HACK!
    // This avoids the problem of top/bottom lines switching color when passing each other in all currently used cases.
    
    if( bottomLineThickness == 0.f )
        bottomLineColor = topLineColor;
    
    
	p->frontFill = frontFill;
	p->backFill = backFill;

	p->topLineThickness = topLineThickness;
	p->topLineColor = topLineColor;

	p->bottomLineThickness = bottomLineThickness;
	p->bottomLineColor = bottomLineColor;

    if( waveId >= m_cacheFirst && waveId <= m_cacheLast )
        _requestRenderInCache();
    else
        _requestRender();
	return true;
}

//____ SetWaveSamples() _______________________________________________________

bool WgChart::SetWaveSamples(int waveId, int firstSample, int nSamples, float * pTopBorderSamples, float * pBottomBorderSamples)
{
	return _setWaveSamples(waveId, firstSample, nSamples, pTopBorderSamples, pBottomBorderSamples, 0.f);
}

bool WgChart::SetWaveSamples(int waveId, int firstSample, int nSamples, float * pSamples, float floor)
{
	return _setWaveSamples(waveId, firstSample, nSamples, pSamples, nullptr, floor);
}


bool WgChart::_setWaveSamples(int waveId, int firstSample, int nSamples, float * pTopBorderSamples, float * pBottomBorderSamples, float defaultSample )
{
	Wave * pWave = _getWave(waveId);
	if (!pWave)
		return false;

	pWave->firstSample = firstSample;
	pWave->nSamples = nSamples;
	pWave->defaultSample = defaultSample;

	float max = -std::numeric_limits<float>::max();
    float min =  std::numeric_limits<float>::max();

	if (pTopBorderSamples)
	{
		pWave->orgTopSamples.resize(nSamples);
		for (int i = 0; i < nSamples; i++)
		{
			float sample = pTopBorderSamples[i];
			pWave->orgTopSamples[i] = sample;

			if (max < sample) max = sample;
			if (min > sample) min = sample;
		}
	}
	else
	{
		pWave->orgTopSamples.clear();
		max = defaultSample;
		min = defaultSample;
	}
	if (pBottomBorderSamples)
	{
		pWave->orgBottomSamples.resize(nSamples);
		for (int i = 0; i < nSamples; i++)
		{
			float sample = pBottomBorderSamples[i];
			pWave->orgBottomSamples[i] = sample;

			if (max < sample) max = sample;
			if (min > sample) min = sample;
		}
	}
	else
	{
		pWave->orgBottomSamples.clear();
		if (max < defaultSample) max = defaultSample;
		if (min > defaultSample) min = defaultSample;
	}

	pWave->maxSample = max;
	pWave->minSample = min;

	bool bResampleAll = _updateDynamics();
	if ( bResampleAll )
	{
		_resampleAllWaves();
		_requestRenderInCache();
	}
	else
		_resampleWave(pWave, true);

	return true;
}

//____ SetFixedValueRange() ________________________________________________________

bool WgChart::SetFixedValueRange(float topValue, float bottomValue)
{
	if (topValue == bottomValue)
		return false;

	if (m_bDynamicValueRange || topValue != m_topValue || bottomValue != m_bottomValue)
	{
		m_bDynamicValueRange = false;
		m_topValue = topValue;
		m_bottomValue = bottomValue;
		_resampleAllWaves();
		if (m_valueRangeResponder)
			m_valueRangeResponder(this, topValue, bottomValue);
		_requestRenderInCache();
	}

	return true;
}

//____ SetDynamicValueRange() ________________________________________________________

void WgChart::SetDynamicValueRange()
{
	if (!m_bDynamicValueRange)
	{
		m_bDynamicValueRange = true;
		_updateDynamics();
		_resampleAllWaves();
		_requestRenderInCache();
//		if (m_valueRangeResponder)
//			m_valueRangeResponder(this, m_topValue, m_bottomValue);
	}
}

//____ SetRenderSectionWidth() _________________________________________________________

bool WgChart::SetRenderSectionWidth( int pointWidth )
{
    if( pointWidth < 16 || pointWidth > 4096 )
        return false;
    
    m_renderSectionWidth = pointWidth;
    return true;
}


//____ SetFixedSampleRange() __________________________________________________

bool WgChart::SetFixedSampleRange(float firstSample, float lastSample)
{
	if (firstSample == lastSample)
		return false;

	if (m_bDynamicSampleRange || firstSample != m_firstSample || lastSample != m_lastSample)
	{

		m_bDynamicSampleRange = false;
		m_firstSample = firstSample;
		m_lastSample = lastSample;
		_resampleAllWaves();				//TODO: Optimize! Can skip if fractional start offset and width is same.
		if (m_sampleRangeResponder)
			m_sampleRangeResponder(this, firstSample, lastSample);
		_requestRenderInCache();
	}
	return true;
}

//____ NativeSampleRange() __________________________________________________

int WgChart::NativeSampleRange() const
{
    int width = PixelSize().w;
    
    if( m_pSkin )
        width -= _skinContentPadding( m_pSkin, m_scale).w;

    width -= m_pixelPadding.width();
    
    return width > 0 ? width + 1 : 0;
}


//____ SetDynamicSampleRange() ________________________________________________________

void WgChart::SetDynamicSampleRange()
{
	if (!m_bDynamicSampleRange)
	{
		m_bDynamicSampleRange = true;
		_updateDynamics();
		_resampleAllWaves();
		_requestRenderInCache();

//		if (m_sampleRangeResponder)
//			m_sampleRangeResponder(this, m_firstSample, m_lastValue);
	}
}

//____ SetSampleLabelStyle() __________________________________________________

void WgChart::SetSampleLabelStyle(WgOrigo alignment, WgCoord offset, wg::Skin * pSkin, const WgTextpropPtr& prop)
{
	m_sampleLabelStyle.alignment = alignment;
	m_sampleLabelStyle.offset = offset;
	m_sampleLabelStyle.pSkin = pSkin;
	m_sampleLabelStyle.pTextStyle = prop;
	_requestRender();
}

//____ SetSampleGridLines() ____________________________________________________

void WgChart::SetSampleGridLines(int nLines, GridLine * pLines)
{
	if (nLines == 0 || pLines == nullptr)
	{
		m_sampleGridLines.clear();
	}
	else
	{
		m_sampleGridLines.resize(nLines);

		for (int i = 0; i < nLines; i++)
			m_sampleGridLines[i] = pLines[i];
	}
	_requestRender();

}

//____ SetValueLabelStyle() ___________________________________________________

void WgChart::SetValueLabelStyle(WgOrigo alignment, WgCoord offset, wg::Skin * pSkin, const WgTextpropPtr& prop)
{
	m_valueLabelStyle.alignment = alignment;
	m_valueLabelStyle.offset = offset;
	m_valueLabelStyle.pSkin = pSkin;
	m_valueLabelStyle.pTextStyle = prop;
	_requestRender();
}

//____ SetValueGridLines() ____________________________________________________

void WgChart::SetValueGridLines(int nLines, GridLine * pLines)
{
	if (nLines == 0 || pLines == nullptr)
	{
		m_valueGridLines.clear();
	}
	else
	{
		m_valueGridLines.resize(nLines);

		for (int i = 0; i < nLines; i++)
			m_valueGridLines[i] = pLines[i];
	}
	_requestRender();
}

//____ SetResizeResponder() _____________________________________________________

void WgChart::SetResizeResponder(std::function<void(WgChart * pWidget, WgSize newSize)> func)
{
	m_resizeResponder = func;
	if (func)
		func(this, PixelSize());
}

//____ SetSampleRangeResponder() _____________________________________________________

void WgChart::SetSampleRangeResponder(std::function<void(WgChart * pWidget, float firstSample, float lastSample)> func)
{
	m_sampleRangeResponder = func;
	if (func)
		func(this, m_firstSample, m_lastSample);
}

//____ SetValueRangeResponder() _____________________________________________________

void WgChart::SetValueRangeResponder(std::function<void(WgChart * pWidget, float topValue, float bottomValue)> func)
{
	m_valueRangeResponder = func;
	if (func)
		func(this, m_topValue, m_bottomValue);
}

//____ SetBitmapCaching() ____________________________________________________________

bool WgChart::SetBitmapCaching( int firstWaveId, int lastWaveId, wg::SurfaceFactory * pFactory )
{
    if( firstWaveId < 1 || firstWaveId > lastWaveId || lastWaveId >= m_waveIdCounter || pFactory == nullptr )
        return false;
    
    m_cacheFirst = firstWaveId;
    m_cacheLast = lastWaveId;
    
    if( pFactory != m_pSurfaceFactory )
    {
        if( m_pCacheBitmap )
        {
            m_pCacheBitmap = nullptr;
            m_cacheDirt.Clear();
        }
        
        m_pSurfaceFactory = pFactory;
    }

    return true;
}

//____ ClearBitmapCaching() __________________________________________________________

void WgChart::ClearBitmapCaching()
{
    m_cacheFirst    = 0;
    m_cacheLast     = 0;
    
    if( m_pCacheBitmap )
        m_pCacheBitmap = nullptr;

    m_pSurfaceFactory = nullptr;
    m_cacheDirt.Clear();
}


//____ _onCloneContent() _______________________________________________________

void WgChart::_onCloneContent( const WgWidget * _pOrg )
{
	WgChart * pOrg = (WgChart*) _pOrg;

	m_defaultSize = pOrg->m_defaultSize;
}

//____ _renderPatches() ________________________________________________________

void WgChart::_renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
    if( m_cacheFirst > 0 )
    {
        if( !m_pCacheBitmap )
        {
            m_pCacheBitmap = m_pSurfaceFactory->createSurface( _canvas.size() );
            m_cacheDirt.Add( _canvas.size() );
        }
        
        if( !m_cacheDirt.IsEmpty() )
            _updateBitmapCache( pDevice );
    }
    
    WgWidget::_renderPatches( pDevice, _canvas, _window, _pPatches );
}

//____ _updateBitmapCache() ____________________________________________________

void WgChart::_updateBitmapCache( wg::GfxDevice * pDevice )
{
    auto pOldCanvas = pDevice->canvas();

    WgColor oldTint = pDevice->tintColor();
    WgBlendMode oldBlend = pDevice->blendMode();
    const WgRect * pOldClipList = pDevice->clipList();
    int     oldClipListSize = pDevice->clipListSize();
    
    pDevice->setCanvas(m_pCacheBitmap);
    pDevice->setBlendMode(WgBlendMode::Replace);
    pDevice->setTintColor(WgColor::White);

    pDevice->setClipList(m_cacheDirt.Size(), m_cacheDirt.Begin());
    pDevice->fill(WgColor::Transparent);

    pDevice->setBlendMode(WgBlendMode::Blend);

    WgRect canvas = m_pSkin ? _skinContentRect( m_pSkin, m_pCacheBitmap->size(), WgStateEnum::Normal, m_scale) : WgRect(0,0,m_pCacheBitmap->size());
    WgRect waveCanvas = canvas - m_pixelPadding;

 
    m_cacheDirt.Clip( { waveCanvas.x, canvas.y, waveCanvas.w, canvas.h } );

    for( int i = m_cacheFirst-1 ; i <= m_cacheLast-1 ; i++ )
        _renderWave( m_waves[i], pDevice, waveCanvas );

/*
    pDirt = m_cacheDirt.Begin();
    while( pDirt < m_cacheDirt.End() )
    {
        WgRect waveClip(*pDirt, { waveCanvas.x, canvas.y, waveCanvas.w, canvas.h });        // Samples stay within padding, values may stretch outside.
        for( int i = m_cacheFirst-1 ; i <= m_cacheLast-1 ; i++ )
            _renderWave( m_waves[i], pDevice, waveCanvas, waveClip );
        pDirt++;
    }
*/
    m_cacheDirt.Clear();
    pDevice->setCanvas(pOldCanvas);
    pDevice->setTintColor(oldTint);
    pDevice->setBlendMode(oldBlend);
    pDevice->setClipList(oldClipListSize, pOldClipList);
}

//____ _onRender() _____________________________________________________________

void WgChart::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
	WgWidget::_onRender(pDevice, _canvas, _window);
    
	WgRect canvas = m_pSkin ? _skinContentRect( m_pSkin, _canvas, WgStateEnum::Normal, m_scale) : _canvas;

	// Preparations for both grid and wave drawing

	WgRect waveCanvas = canvas - m_pixelPadding;
	float sampleScale = waveCanvas.w / (m_lastSample - m_firstSample);

	// Draw sample grid lines

	if (!m_sampleGridLines.empty())
	{
		for (auto& line : m_sampleGridLines)
		{
			int xOfs = waveCanvas.x + (int) ((line.pos - m_firstSample) * sampleScale);
			pDevice->drawLine({ xOfs, canvas.y }, WgDirection::Down, canvas.h, line.color, line.thickness * m_scale / WG_SCALE_BASE);

			if (!line.label.IsEmpty())
			{
				WgPen	pen(pDevice, _canvas);
				WgTextAttr attr;

				WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop());
				WgTextTool::AddPropAttributes(attr, m_sampleLabelStyle.pTextStyle);
				attr.size = attr.size * m_scale >> WG_SCALE_BINALS;
				pen.SetAttributes(attr);


				WgSize labelSize;
				labelSize.w = WgTextTool::lineWidth(nullptr, attr, WG_MODE_NORMAL, line.label.Chars());
				labelSize.h = pen.GetLineHeight();

				WgCoord textOfs;
				if (m_sampleLabelStyle.pSkin)
				{
					labelSize = _skinSizeForContent( m_sampleLabelStyle.pSkin, labelSize, m_scale);
					textOfs = _skinContentRect( m_sampleLabelStyle.pSkin, labelSize, WgStateEnum::Normal, m_scale ).pos();
				}

				WgCoord labelPos = _placeLabel({ xOfs,canvas.y+canvas.h }, m_sampleLabelStyle.alignment, m_sampleLabelStyle.offset, labelSize);

				if (m_sampleLabelStyle.pSkin)
					_renderSkin( m_sampleLabelStyle.pSkin, pDevice, WgStateEnum::Normal, { labelPos,labelSize }, m_scale);

				pen.SetPos(labelPos + textOfs);
                WgGfxDevice::PrintLine(pDevice, pen, attr, line.label.Chars());
			}
		}
	}

	// Draw value grid lines

	if (!m_valueGridLines.empty())
	{
		float top = std::min(m_topValue, m_bottomValue);
		float bottom = std::max(m_topValue, m_bottomValue);

		float mul = waveCanvas.h / (m_bottomValue - m_topValue);
		int	  startOfs = mul > 0 ? waveCanvas.y : waveCanvas.y + waveCanvas.h;

		for (auto& line : m_valueGridLines)
		{
			int yOfs = startOfs + (int)((line.pos - top) * mul); // +0.5f);
			pDevice->drawLine({ canvas.x, yOfs }, WgDirection::Right, canvas.w, line.color, line.thickness * m_scale / WG_SCALE_BASE );

			if (!line.label.IsEmpty())
			{
				WgPen	pen(pDevice, _canvas);
				WgTextAttr attr;

				WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop());
				WgTextTool::AddPropAttributes(attr, m_valueLabelStyle.pTextStyle);
				attr.size = attr.size * m_scale >> WG_SCALE_BINALS;
				pen.SetAttributes(attr);


				WgSize labelSize;
				labelSize.w = WgTextTool::lineWidth(nullptr, attr, WG_MODE_NORMAL, line.label.Chars());
				labelSize.h = pen.GetLineHeight();

				WgCoord textOfs;
				if (m_valueLabelStyle.pSkin)
				{
					labelSize = _skinSizeForContent( m_valueLabelStyle.pSkin, labelSize, m_scale);
					textOfs = _skinContentRect( m_valueLabelStyle.pSkin, labelSize, WgStateEnum::Normal, m_scale).pos();
				}

				WgCoord labelPos = _placeLabel({ canvas.x, yOfs }, m_valueLabelStyle.alignment, m_valueLabelStyle.offset, labelSize);

				if (m_valueLabelStyle.pSkin)
					_renderSkin( m_valueLabelStyle.pSkin, pDevice, WgStateEnum::Normal, { labelPos,labelSize }, m_scale);

				pen.SetPos(labelPos + textOfs);
                WgGfxDevice::PrintLine(pDevice, pen, attr, line.label.Chars());
			}
		}
	}

	// Render waves

    const WgRect * pOldClipList = pDevice->clipList();
    int     oldClipListSize = pDevice->clipListSize();
    
	WgRect waveClip( waveCanvas.x, canvas.y, waveCanvas.w, canvas.h );		// Samples stay within padding, values may stretch outside.
    
    int allocSize = oldClipListSize*sizeof(WgRect);
    WgRect * pRects = (WgRect*) WgBase::MemStackAlloc( allocSize );
    int nRects = 0;
    
    for( int i = 0 ; i < oldClipListSize ; i++ )
    {
        pRects[nRects] = WgRect(pOldClipList[i], waveClip );
        if( !pRects[nRects].isEmpty() )
            nRects++;
    }
    
    pDevice->setClipList(nRects, pRects);
    
    for( int i = 0 ; i < m_waves.size() ; i++ )
	{
        if( i == m_cacheFirst-1 )
        {
            pDevice->setBlitSource(m_pCacheBitmap);
            pDevice->blit( WgCoord(_canvas.x, _canvas.y), _canvas.size() );
            i = m_cacheLast-1;
        }
        else
            _renderWave( m_waves[i], pDevice, waveCanvas );
	}

    pDevice->setClipList(oldClipListSize, pOldClipList);
    WgBase::MemStackRelease(allocSize);
}

//____ _renderWave() __________________________________________________________

void WgChart::_renderWave( Wave& wave, wg::GfxDevice * pDevice, const WgRect& waveCanvas )
{
    if (wave.bHidden || wave.nSamples == 0)
       return;
    
    int xOfs = wave.resampledFirst;
    
    WgWaveLine top, bottom;
    
    top.color = wave.topLineColor;
    top.thickness = wave.topLineThickness*m_scale/WG_SCALE_BASE;
    top.length = (int) wave.resampledTop.size();
    top.pWave = wave.resampledTop.data();
    top.hold = wave.resampledDefault;
    
    bottom.color = wave.bottomLineColor;
    bottom.thickness = wave.bottomLineThickness*m_scale/WG_SCALE_BASE;
    bottom.length = (int)wave.resampledBottom.size();
    bottom.pWave = wave.resampledBottom.data();
    bottom.hold = wave.resampledDefault;
    
    int length = std::max(top.length, bottom.length)-1;
    
    if(length >= 1)
    {
        pDevice->drawWave(WgRect(waveCanvas.x + xOfs, waveCanvas.y, length, waveCanvas.h), &top, &bottom, wave.frontFill, wave.backFill);
//        pDevice->ClipDrawHorrWave(waveClip, WgCoord(waveCanvas.x + xOfs, waveCanvas.y), length, top, bottom, wave.frontFill, wave.backFill);
    }
}

//____ _placeLabel() __________________________________________________________

WgCoord	WgChart::_placeLabel(WgCoord startPoint, WgOrigo alignment, WgCoord labelOffset, WgSize labelSize) const
{
	return startPoint + (labelOffset*m_scale/WG_SCALE_BASE) -WgCoord(labelSize.w,labelSize.h) + WgUtil::OrigoToOfs(alignment, labelSize);
}

//____ _getWave() _____________________________________________________________

WgChart::Wave * WgChart::_getWave(int waveId)
{
	for (auto& wave : m_waves)
		if (wave.id == waveId)
			return &wave;
	return nullptr;
}

const WgChart::Wave * WgChart::_getWave(int waveId) const
{
	for (auto& wave : m_waves)
		if (wave.id == waveId)
			return &wave;
	return nullptr;
}



//____ _onAlphaTest() ___________________________________________________________

bool WgChart::_onAlphaTest( const WgCoord& ofs )
{
	//TODO: Implement?

	return true;
}


//____ _onNewSize() ___________________________________________________________

void WgChart::_onNewSize(const WgSize& size)
{
	if (m_resizeResponder)
		m_resizeResponder(this, size);

	_resampleAllWaves();
	WgWidget::_onNewSize(size);
}

//____ _setScale() ____________________________________________________________

void WgChart::_setScale(int scale)
{
	WgWidget::_setScale(scale);

	SetCanvasPadding(m_pointPadding);		// Update m_pixelPadding and resample if needed.
	_requestResize();
	_requestRenderInCache();
}


//____ _updateDynamics() ______________________________________________________

bool WgChart::_updateDynamics()
{
	bool bNeedRefresh = false;

	if (m_bDynamicSampleRange)
	{
		int first = INT_MAX, last = INT_MIN;

		for (auto& it : m_waves)
		{
			if (first > it.firstSample)
				first = it.firstSample;

			if (last < it.firstSample + it.nSamples)
				last = it.firstSample + it.nSamples;
		}

		if ((float)first != m_firstSample || (float)last != m_lastSample)
		{
			m_firstSample = (float) first;
			m_lastSample = (float) last;
			bNeedRefresh = true;
		}
	}

	if (m_bDynamicValueRange)
	{
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        
		for (auto& it : m_waves)
		{
			if (min > it.minSample)
				min = it.minSample;

			if (max < it.maxSample)
				max = it.maxSample;
		}

		if ( max != m_topValue || min != m_bottomValue)
		{
			m_topValue = max;
			m_bottomValue = min;
			bNeedRefresh = true;
		}
	}

	return bNeedRefresh;
}

//____ _resampleAllWaves() ____________________________________________________

void WgChart::_resampleAllWaves()
{
	for (auto& wave : m_waves)
		_resampleWave(&wave);
}

//____ _resampleWave() ________________________________________________________

void WgChart::_resampleWave(Wave * pWave, bool bRequestRenderOnChanges )
{
	WgSize	canvas = PixelSize();

	if (m_pSkin)
		canvas = _skinContentRect( m_pSkin, canvas, WgStateEnum::Normal, m_scale);


	canvas -= m_pixelPadding;

	float sampleScale = (m_lastSample - m_firstSample) / (canvas.w + 1);

	int nResampled = (int) (pWave->nSamples / sampleScale);
	if (nResampled < 0)
		nResampled = 0;

	float valueFactor = canvas.h / (m_bottomValue - m_topValue);

	float floor;
	float yOfs;
	if (valueFactor < 0)
	{
		floor = m_bottomValue;
		yOfs = canvas.h*256;
	}
	else
	{
		floor = m_topValue;
		yOfs = 0;
	}

	int newDefault = yOfs + (int)((pWave->defaultSample - floor) * valueFactor * 256);
	int newFirst = (int)((pWave->firstSample - m_firstSample) * sampleScale);


	int *	pNewTopSamples = nullptr;
	int *	pNewBottomSamples = nullptr;
	int		nbNewTopSamples = 0;
	int		nbNewBottomSamples = 0;

	if (pWave->orgTopSamples.size() > 0)
	{
		nbNewTopSamples = nResampled;

		if( bRequestRenderOnChanges )
			pNewTopSamples = (int*) WgBase::MemStackAlloc( nResampled * sizeof(int) );
		else
		{
			pWave->resampledTop.resize(nResampled);
			pNewTopSamples = &pWave->resampledTop[0];
		}

		if (nResampled == pWave->nSamples)
		{
			for (int i = 0; i < nResampled; i++)
				pNewTopSamples[i] = yOfs + (int)((pWave->orgTopSamples[i] - floor) * valueFactor * 256);
		}
		else
		{
			float stepFactor = (pWave->orgTopSamples.size() - 1) / (float) nResampled;

			for (int i = 0; i < nResampled; i++)
			{
				float sample = stepFactor*i;
				int ofs = (int)sample;
				int frac2 = ((int)(sample * 256)) & 0xFF;
				int frac1 = 256 - frac2;

				int val1 = (int)((pWave->orgTopSamples[ofs] - floor) * valueFactor * 256);
				int val2 = (int)((pWave->orgTopSamples[ofs+1] - floor) * valueFactor * 256);

				pNewTopSamples[i] = yOfs + ((val1*frac1 + val2*frac2) >> 8) ;
			}
		}
	}

	if (pWave->orgBottomSamples.size() > 0)
	{
		nbNewBottomSamples = nResampled;

		if (bRequestRenderOnChanges)
			pNewBottomSamples = (int*) WgBase::MemStackAlloc(nResampled * sizeof(int));
		else
		{
			pWave->resampledBottom.resize(nResampled);
			pNewBottomSamples = &pWave->resampledBottom[0];
		}

		if (nResampled == pWave->nSamples)
		{
			for (int i = 0; i < nResampled; i++)
				pNewBottomSamples[i] = yOfs + (int)((pWave->orgBottomSamples[i] - floor) * valueFactor * 256);
		}
		else
		{
			float stepFactor = (pWave->orgTopSamples.size() - 1) / (float)nResampled;

			for (int i = 0; i < nResampled; i++)
			{
				float sample = stepFactor*i;
				int ofs = (int)sample;
				int frac2 = ((int)(sample * 256)) & 0xFF;
				int frac1 = 256 - frac2;

				int val1 = (int)((pWave->orgBottomSamples[ofs] - floor) * valueFactor * 256);
				int val2 = (int)((pWave->orgBottomSamples[ofs + 1] - floor) * valueFactor * 256);

				pNewBottomSamples[i] = yOfs + ((val1*frac1 + val2*frac2) >> 8);
			}
		}
	}

	// Request render on the changes

	if (bRequestRenderOnChanges)
	{
		int begOrgSamples = pWave->resampledFirst;
		int begNewSamples = newFirst;
		float maxLineThickness = std::max(pWave->topLineThickness, pWave->bottomLineThickness)*m_scale / WG_SCALE_BASE;

        bool bInCache = (pWave >= _getWave(m_cacheFirst) && pWave < _getWave(m_cacheLast) );
        
		_requestRenderOnNewSamples(	begOrgSamples, pWave->resampledTop.size(), &pWave->resampledTop[0], pWave->resampledBottom.size(), &pWave->resampledBottom[0],
									begNewSamples, nbNewTopSamples, pNewTopSamples, nbNewBottomSamples, pNewBottomSamples,
									pWave->resampledDefault, newDefault, maxLineThickness, bInCache );

		// Replace old samples with new. Reversed order of Bottom/Top samples so that we free stack memory in right order.

		if( nbNewBottomSamples > 0 )
		{
			pWave->resampledBottom.resize(nResampled);
			memcpy(&pWave->resampledBottom[0], pNewBottomSamples, nbNewBottomSamples * sizeof(int));
			WgBase::MemStackRelease(nbNewBottomSamples * sizeof(int));
		}

		if( nbNewTopSamples > 0 )
		{
			pWave->resampledTop.resize(nResampled);
			memcpy(&pWave->resampledTop[0], pNewTopSamples, nbNewTopSamples * sizeof(int));
			WgBase::MemStackRelease(nbNewTopSamples * sizeof(int));
		}
	}

	// Finish up

	pWave->resampledDefault = newDefault;
	pWave->resampledFirst = newFirst;

	if (nbNewBottomSamples == 0)
		pWave->resampledBottom.clear();

	if (nbNewTopSamples == 0)
		pWave->resampledTop.clear();


}

//____ _requestRenderOnNewSamples() ___________________________________________

void WgChart::_requestRenderOnNewSamples(   int begOrgSamples, int nbOrgTopSamples, int * pOrgTopSamples, int nbOrgBottomSamples, int * pOrgBottomSamples,
											int begNewSamples, int nbNewTopSamples, int * pNewTopSamples, int nbNewBottomSamples, int * pNewBottomSamples,
											int orgDefaultSample, int newDefaultSample, float maxLineThickness, bool bInCache )
{
	// Calculate size of our sample canvas

	WgRect canvas = m_pSkin ? _skinContentRect( m_pSkin, PixelSize(), WgStateEnum::Normal, m_scale).size() : PixelSize();
	canvas -= m_pixelPadding;

	// Calculate needed margin (in pixels) for line thickness

	int margin = int(maxLineThickness) / 2 + 2;

	// Calculate what section we need to start in 


	int sectionBeg = (std::min(begOrgSamples,begNewSamples) / m_renderSectionWidth) * m_renderSectionWidth;



	while (sectionBeg < canvas.w)
	{
		int topDirtOfs = 0, topDirtHeight = 0;
		int bottomDirtOfs = 0, bottomDirtHeight = 0;

		int sectionWidth = std::min(m_renderSectionWidth, canvas.w - sectionBeg);

		int orgSampleOfs = sectionBeg - begOrgSamples - margin;
		int newSampleOfs = sectionBeg - begNewSamples - margin;
		int nSamples = m_renderSectionWidth + margin * 2;

		int min1, max1;
		int min2, max2;

		// Create dirty rect covering all old and new top samples

		if (nbOrgTopSamples > 0 || nbNewTopSamples > 0 || orgDefaultSample != newDefaultSample)
		{
			if (_lineFragmentMinMax(orgSampleOfs, nSamples, nbOrgTopSamples, pOrgTopSamples, orgDefaultSample, &min1, &max1))
			{
				topDirtOfs = min1 / 256 - margin;
				topDirtHeight = max1 / 256 + 2 + margin - topDirtOfs;
			}

			if (_lineFragmentMinMax(newSampleOfs, nSamples, nbNewTopSamples, pNewTopSamples, newDefaultSample, &min2, &max2))
			{
				if (min1 == max1 && min2 == max2 && min1 == min2)
				{
					// Totally flat line section with no movement, no need to render it

					topDirtOfs = 0;
					topDirtHeight = 0;
				}
				else
				{
					int newOfs = min2 / 256 - margin;
					int newHeight = max2 / 256 + 2 + margin - newOfs;

					if (newOfs < topDirtOfs)
					{
						topDirtHeight += topDirtOfs - newOfs;
						topDirtOfs = newOfs;
					}

					if (newOfs + newHeight > topDirtOfs + topDirtHeight)
						topDirtHeight = newOfs + newHeight - topDirtOfs;

				}
			}
		}

		// Calculate dirty height covering all old and new bottom samples

		if (nbOrgBottomSamples > 0 || nbNewBottomSamples > 0 || orgDefaultSample != newDefaultSample)
		{
			if (_lineFragmentMinMax(orgSampleOfs, nSamples, nbOrgBottomSamples, pOrgBottomSamples, orgDefaultSample, &min1, &max1))
			{
				bottomDirtOfs = min1 / 256 - margin;
				bottomDirtHeight = max1 / 256 + 2 + margin - bottomDirtOfs;
			}

			if (_lineFragmentMinMax(newSampleOfs, nSamples, nbNewBottomSamples, pNewBottomSamples, newDefaultSample, &min2, &max2))
			{
				if (min1 == max1 && min2 == max2 && min1 == min2)
				{
					// Totally flat line section with no movement, no need to render it

					bottomDirtOfs = 0;
					bottomDirtHeight = 0;
				}
				else
				{
					int newOfs = min2 / 256 - margin;
					int newHeight = max2 / 256 + 2 + margin - newOfs;

					if (newOfs < bottomDirtOfs)
					{
						bottomDirtHeight += bottomDirtOfs - newOfs;
						bottomDirtOfs = newOfs;
					}

					if (newOfs + newHeight > bottomDirtOfs + bottomDirtHeight)
						bottomDirtHeight = newOfs + newHeight - bottomDirtOfs;
				}
			}
		}

		// Do some clipping on height

		if (topDirtOfs >= canvas.h)
		{
			topDirtOfs = 0;
			topDirtHeight = 0;
		}

		if (topDirtOfs < 0)
		{
			topDirtHeight = std::max( topDirtHeight + topDirtOfs, 0 );
			topDirtOfs = 0;
		}

		if (topDirtOfs + topDirtHeight > canvas.h)
			topDirtHeight = canvas.h - topDirtOfs;

		if (bottomDirtOfs >= canvas.h)
		{
			bottomDirtOfs = 0;
			bottomDirtHeight = 0;
		}

		if (bottomDirtOfs < 0)
		{
			bottomDirtHeight = std::max(bottomDirtHeight + bottomDirtOfs, 0);
			bottomDirtOfs = 0;
		}

		if (bottomDirtOfs + bottomDirtHeight > canvas.h)
			bottomDirtHeight = canvas.h - bottomDirtOfs;


		// Generate dirty rect(s) and request render as needed

		if (bottomDirtOfs < topDirtOfs + topDirtHeight && bottomDirtOfs + bottomDirtHeight > topDirtOfs)
		{
			int dirtTop = std::min(topDirtOfs, bottomDirtOfs);
			int dirtHeight = std::max(topDirtOfs + topDirtHeight, bottomDirtOfs + bottomDirtHeight) - dirtTop;

			if (dirtHeight > 0)
            {
                WgRect rect( canvas.x + sectionBeg, canvas.y + dirtTop, sectionWidth, dirtHeight );
                
                if( bInCache )
                    _requestRenderInCache(rect);
                else
                    _requestRender(rect);
            }
		}
		else
		{
			if (topDirtHeight > 0)
            {
                WgRect rect( canvas.x + sectionBeg, canvas.y + topDirtOfs, sectionWidth, topDirtHeight );
                
                if( bInCache )
                    _requestRenderInCache(rect);
                else
                    _requestRender(rect);
            }

			if (bottomDirtHeight > 0)
            {
                WgRect rect( canvas.x + sectionBeg, canvas.y + bottomDirtOfs, sectionWidth, bottomDirtHeight );
                
                if( bInCache )
                    _requestRenderInCache(rect);
                else
                    _requestRender(rect);
            }
		}


		//

		sectionBeg += m_renderSectionWidth;
	}

}

//____ _requestRenderInCache() ___________________________________________________

void WgChart::_requestRenderInCache()
{
    if( m_pCacheBitmap )
        m_cacheDirt.Add( m_pCacheBitmap->size() );
    _requestRender();
}

void WgChart::_requestRenderInCache( const WgRect& rect )
{
    if( m_pCacheBitmap )
        m_cacheDirt.Add( rect );
    _requestRender(rect);
}


//____ _lineFragmentMinMax() ______________________________________________________

bool WgChart::_lineFragmentMinMax(int begin, int length, int nbSamples, int * pSamples, int defaultSample, int * wpMin, int * wpMax)
{
	int min, max;

	// Special case: Having 0 samples means that default sample is used.

	if (length == 0)
	{
		*wpMin = defaultSample;
		*wpMax = defaultSample;
		return true;
	}

	//

	if (begin < 0)
	{
		length += begin;
		begin = 0;
	}

	if (begin + length > nbSamples)
		length = nbSamples - begin;

	if (length <= 0)
		return false;

	max = min = pSamples[begin];
	length--;

	for (int i = begin+1; i < begin + length; i++)
	{
		int s = pSamples[i];
		min = std::min(s, min);
		max = std::max(s, max);
	}

	*wpMin = min;
	*wpMax = max;
	return true;
}
