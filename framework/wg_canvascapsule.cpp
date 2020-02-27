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

#include <wg_canvascapsule.h>
#include <wg_gfxdevice.h>
#include <wg_eventhandler.h>

static const char    c_widgetType[] = {"CanvasCapsule"};

//____ Constructor ____________________________________________________________

WgCanvasCapsule::WgCanvasCapsule() : m_tintColor(0xFFFFFFFF), m_tintMode(WG_TINTMODE_OPAQUE), m_blendMode(WgBlendMode::Blend), m_pFactory(nullptr), m_pCanvas(nullptr),
m_fadeStartColor(0xFFFFFFFF), m_fadeEndColor(0xFFFFFFFF), m_fadeTime(0), m_fadeTimeCounter(0)
{
    m_bOpaque = false;
}

//____ Destructor _____________________________________________________________

WgCanvasCapsule::~WgCanvasCapsule()
{
}

//____ Type() _________________________________________________________________

const char * WgCanvasCapsule::Type( void ) const
{
    return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgCanvasCapsule::GetClass()
{
    return c_widgetType;
}

//____ SetSurfaceFactory() ____________________________________________________

void WgCanvasCapsule::SetSurfaceFactory(wg::SurfaceFactory * pFactory)
{
    if (pFactory == m_pFactory)
        return;

    if (m_pCanvas)
    {
        m_pCanvas = nullptr;
        _requestRender();
    }

    m_pFactory = pFactory;
}


//____ SetColor() ______________________________________________________________

void WgCanvasCapsule::SetColor( const WgColor& color)
{
    if (m_fadeEndColor != m_tintColor)
        _stopReceiveTicks();

    if( color != m_tintColor )
    {
        m_tintColor = color;
        _requestRender();
    }
}

//____ SetTintMode() ___________________________________________________________

void WgCanvasCapsule::SetTintMode( WgTintMode mode )
{
    if( mode != m_tintMode )
    {
        m_tintMode = mode;
        _requestRender();
    }
}

//____ SetBlendMode() __________________________________________________________

void WgCanvasCapsule::SetBlendMode( WgBlendMode mode )
{
    if( mode != m_blendMode )
    {
        m_blendMode = mode;
        _requestRender();
    }
}

//____ StartFade() ____________________________________________________________

void WgCanvasCapsule::StartFade(WgColor destination, int ms)
{
    if (destination == m_tintColor)
        return;

    m_fadeStartColor = m_tintColor;
    m_fadeEndColor = destination;
    m_fadeTimeCounter = 0;
    m_fadeTime = ms;

    _startReceiveTicks();
}

//____ StopFade() _____________________________________________________________

void WgCanvasCapsule::StopFade()
{
    m_fadeEndColor = m_tintColor;
    _stopReceiveTicks();
}

//____ FindWidget() ___________________________________________________________

WgWidget * WgCanvasCapsule::FindWidget(const WgCoord& ofs, WgSearchMode mode)
{
	if (mode == WgSearchMode::Geometry || m_tintColor.a > 0 || m_blendMode == WgBlendMode::Replace)
		return WgCapsule::FindWidget(ofs, mode);

	return nullptr;
}

//____ _onEvent() _____________________________________________________________

void WgCanvasCapsule::_onEvent(const WgEvent::Event * pEvent, WgEventHandler * pHandler)
{
    switch( pEvent->Type() )
    {
        case WG_EVENT_TICK:
        {
            const WgEvent::Tick * pTick = static_cast<const WgEvent::Tick*>(pEvent);

            int ms = pTick->Millisec();

            m_fadeTimeCounter += pTick->Millisec();

            WgColor newColor;
            if( m_fadeTimeCounter >= m_fadeTime )
                newColor = m_fadeEndColor;
            else
            {
                int r = ((int)m_fadeEndColor.r) - m_fadeStartColor.r;
                int g = ((int)m_fadeEndColor.g) - m_fadeStartColor.g;
                int b = ((int)m_fadeEndColor.b) - m_fadeStartColor.b;
                int a = ((int)m_fadeEndColor.a) - m_fadeStartColor.a;

                float progress = m_fadeTimeCounter / (float) m_fadeTime;

                newColor.r = m_fadeStartColor.r + (int) (r * progress);
                newColor.g = m_fadeStartColor.g + (int)(g * progress);
                newColor.b = m_fadeStartColor.b + (int)(b * progress);
                newColor.a = m_fadeStartColor.a + (int)(a * progress);
            }

            if (newColor != m_tintColor)
            {
                m_tintColor = newColor;
                _requestRender();

                if (newColor == m_fadeEndColor)
                    _stopReceiveTicks();
            }

        }
            break;

        default:
            WgCapsule::_onEvent(pEvent,pHandler);
            break;
    }

}



//____ _getBlendMode() _________________________________________________________

WgBlendMode WgCanvasCapsule::_getBlendMode() const
{
    return m_blendMode;
}

//____ _renderPatches() ________________________________________________________

void WgCanvasCapsule::_renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, wg::Patches * _pPatches )
{
    // Make sure we have children

    if (!m_hook.Widget())
        return;

    // Make sure we have a canvas

    if (!m_pCanvas)
    {
        if (!m_pFactory)
            return;                            // No SurfaceFactory set!

        WgSize maxSize = m_pFactory->maxSize();
        if (_canvas.w > maxSize.w || _canvas.h > maxSize.h)
            return;                            // Can't create a canvas of the required size!

        m_pCanvas = m_pFactory->createSurface(_canvas.size(), WgPixelType::BGRA_8);
        m_dirtyPatches.clear();
        m_dirtyPatches.add(_canvas.size());
    }

    // Go through dirty patches from screen canvas and update our back canvas where they overlap with our own

    wg::Patches renderStack;

    for (const WgRect * pScreenRect = _pPatches->begin(); pScreenRect != _pPatches->end(); pScreenRect++)
    {
        WgRect r( _canvas, *pScreenRect );

        if( r.w > 0 && r.h > 0 )
        {
            bool    bIntersected = false;
            r.x -= _canvas.x;
            r.y -= _canvas.y;

            for (const WgRect * pLocalDirt = m_dirtyPatches.begin(); pLocalDirt != m_dirtyPatches.end(); pLocalDirt++)
            {
                if (pLocalDirt->intersectsWith(r))
                {
                    renderStack.push(WgRect(*pLocalDirt,r));
                    bIntersected = true;
                }
            }
            
            if (bIntersected)
                m_dirtyPatches.sub(r);
        }
    }

    // Save old tint color and blend mode.

    WgBlendMode        oldBM;
    WgColor            oldTC;

    oldBM = pDevice->blendMode();
    oldTC = pDevice->tintColor();


    if (!renderStack.isEmpty())
    {
        auto pOldCanvas = pDevice->canvas();
        pDevice->setCanvas(m_pCanvas);

        pDevice->setBlendMode(WgBlendMode::Replace);
        pDevice->setTintColor(WgColor::White);

        pDevice->setClipList(renderStack.size(), renderStack.begin());
        pDevice->fill(WgColor::Transparent);

        pDevice->setBlendMode(WgBlendMode::Blend);

        m_hook.Widget()->_renderPatches(pDevice, _canvas.size(), _canvas.size(), &renderStack);
        pDevice->setCanvas(pOldCanvas);

    }

    // Set our tint color and blend mode for blitting from back canvas to screen.

    pDevice->setBlendMode(m_blendMode);

    if( m_tintMode == WG_TINTMODE_OPAQUE )
        pDevice->setTintColor(m_tintColor);
    else    // MULTIPLY
        pDevice->setTintColor(m_tintColor*oldTC);

    // Render patches

    WgWidget::_renderPatches(pDevice, _canvas, _canvas, _pPatches);

    // Reset old blend mode and tint color

    pDevice->setBlendMode(oldBM);
    pDevice->setTintColor(oldTC);
}

//____ _onRender() ____________________________________________________________

void WgCanvasCapsule::_onRender(wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window)
{
    // Make sure to render any skin we might have

    WgWidget::_onRender(pDevice, _canvas, _window);

    // Copy from our back canvas to the screen canvas

    pDevice->setBlitSource(m_pCanvas);
    pDevice->blit( WgCoord(_canvas.x, _canvas.y), { 0,0,_canvas.w,_canvas.h });
}


//____ _onCloneContent() _______________________________________________________

void WgCanvasCapsule::_onCloneContent( const WgWidget * _pOrg )
{
    WgCanvasCapsule * pOrg = (WgCanvasCapsule*) _pOrg;

    m_tintColor        = pOrg->m_tintColor;
    m_blendMode        = pOrg->m_blendMode;
    m_tintMode         = pOrg->m_tintMode;
}

//____ _onNewSize() ___________________________________________________________

void WgCanvasCapsule::_onNewSize(const WgSize& size)
{
    if (m_pCanvas && size != m_pCanvas->size() )
    {
        m_pCanvas = nullptr;
        _requestRender();
    }

    WgCapsule::_onNewSize(size);
}

//____ _onRenderRequested() ___________________________________________________

void WgCanvasCapsule::_onRenderRequested()
{
    m_dirtyPatches.clear();
    m_dirtyPatches.add(PixelSize());
    _requestRender();
}

void WgCanvasCapsule::_onRenderRequested(const WgRect& rect)
{
    m_dirtyPatches.add(rect);
    _requestRender(rect);
}

//____ _onCollectPatches() ____________________________________________________

void WgCanvasCapsule::_onCollectPatches( wg::Patches& container, const WgRect& geo, const WgRect& clip )
{
	if (m_tintColor.a > 0 || m_blendMode == WgBlendMode::Replace)
		container.add( WgRect( geo, clip ) );
}

//____ _onMaskPatches() _______________________________________________________

void WgCanvasCapsule::_onMaskPatches( wg::Patches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	//TODO: Support recursive masking.

	if( m_pCanvas && ((m_tintColor.a == 255 && m_pCanvas->pixelDescription()->A_bits == 0) || m_blendMode == WgBlendMode::Replace) )
		patches.sub(WgRect(geo, clip));

    return;
}
 
