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

#include <wg_scrollchart.h>
#include <wg_base.h>
#include <wg_gfxdevice.h>

#include <cstring>

namespace wg
{

    const TypeInfo ScrollChart::TYPEINFO = {"ScrollChart", &Widget::TYPEINFO};

    //____ constructor ____________________________________________________________

    ScrollChart::ScrollChart()
    {
    }

    //____ destructor _____________________________________________________________

    ScrollChart::~ScrollChart()
    {
    }

    //____ typeInfo() _________________________________________________________

    const TypeInfo& ScrollChart::typeInfo(void) const
    {
        return TYPEINFO;
    }



   	//____ _fullRefreshOfChart() _______________________________________

    void ScrollChart::_fullRefreshOfChart()
    {
        if(!m_pScrollSurface)
            return;

        SizeSPX wantedSize = m_size - _contentBorderSize(m_scale);

        if( m_pScrollSurface->pixelSize()*64 != wantedSize )
            m_pScrollSurface = nullptr;
        else
        {
            m_leftEdgeOfs = 0;
            m_scrollSurfaceDirtyRects[0] = wantedSize;
            m_nbScrollSurfaceDirtyRects = 1;
        }

    }

    //____ _renderCharts() ____________________________________________________

    void ScrollChart::_renderCharts(GfxDevice* pDevice, const RectSPX& canvas)
    {
        // Make sure we have a canvas

        if(m_pScrollSurface == nullptr)
        {
            m_pScrollSurface = pDevice->surfaceFactory()->createSurface( WGBP(Surface, _.size = canvas.size()/64, _.format = m_scrollSurfaceFormat, _.canvas = true) );

            m_leftEdgeOfs = 0;
            m_scrollSurfaceDirtyRects[0] = {0, 0, canvas.size()};
            m_nbScrollSurfaceDirtyRects = 1;
        }

        // Render the charts

        pDevice->beginCanvasUpdate(m_pScrollSurface, m_nbScrollSurfaceDirtyRects, m_scrollSurfaceDirtyRects);

        pDevice->setBlendMode(BlendMode::Replace);
        pDevice->fill(m_scrollSurfaceBgColor);
        pDevice->setBlendMode(BlendMode::Blend);

        _renderOnScrollSurface( m_leftEdgeOfs, m_leftEdgeTimestamp );

        pDevice->endCanvasUpdate();

        // Copy scroll surface to our canvas

        pDevice->setBlitSource(m_pScrollSurface);
        pDevice->blit({0, 0}, {m_leftEdgeOfs, 0, canvas.w - m_leftEdgeOfs, canvas.h});
        pDevice->blit({canvas.w - m_leftEdgeOfs,0}, {0,0,m_leftEdgeOfs,canvas.h});
    }

    	//____ _update() ____________________________________________________________

    void ScrollChart::_update(int microPassed, int64_t microsecTimestamp)
    {
        Chart::_update(microPassed, microsecTimestamp);

        if(!m_pScrollSurface)
        {
            m_leftEdgeTimestamp = microsecTimestamp - m_displayTime;
            _requestRender(_contentRect());
            return;
        }

        // Calc new m_leftEdgeTimestamp and m_leftEdgeOfs
        // Calc scrollSurface dirty rects.


        SizeSPX contentSize = m_size - _contentBorderSize(m_scale);


        int microsecPerPixel = m_displayTime / (contentSize.w / 64);

        int timePassed = microsecTimestamp - (m_leftEdgeTimestamp + m_displayTime);

        if(timePassed > m_displayTime)
        {
            m_leftEdgeTimestamp = microsecTimestamp - m_displayTime;
            m_scrollSurfaceDirtyRects[0] = contentSize;
            m_nbScrollSurfaceDirtyRects = 1;
            _requestRender(_contentRect());
        }
        else
        {
            int pixelsToScroll = (timePassed / microsecPerPixel);

            if(pixelsToScroll > 0)
            {
                m_leftEdgeTimestamp += pixelsToScroll * microsecPerPixel;

                m_leftEdgeOfs = (m_leftEdgeOfs + pixelsToScroll) % contentSize.w;

                RectSPX dirt = {m_leftEdgeOfs - pixelsToScroll * 64, 0, pixelsToScroll, contentSize.h};

                if(dirt.x < 0)
                {
                    m_scrollSurfaceDirtyRects[0] = {contentSize.w + dirt.x, 0, -dirt.x, dirt.h};
                    m_scrollSurfaceDirtyRects[1] = {0, 0, dirt.w + dirt.x, dirt.h};
                    m_nbScrollSurfaceDirtyRects = 2;
                }
                else
                {
                    m_scrollSurfaceDirtyRects[0] = dirt;
                    m_nbScrollSurfaceDirtyRects = 1;
                }

                _requestRender(_contentRect());
            }

        }

    }
