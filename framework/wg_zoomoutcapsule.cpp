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

#include <wg_zoomoutcapsule.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_eventhandler.h>
#include <wg_patches.h>
#include <wg_surfacefactory.h>
#include <wg_payload.h>
#include <wg_image.h>
#include <wg_base.h>
#include <wg_sizecapsule.h>
#include <wg_shadercapsule.h>
#include <wg_util.h>
#include <wg_context.h>


static const char	c_widgetType[] = {"ZoomOutCapsule"};

//____ Constructor ____________________________________________________________

WgZoomOutCapsule::WgZoomOutCapsule()
{
}

//____ Destructor _____________________________________________________________

WgZoomOutCapsule::~WgZoomOutCapsule()
{
    delete m_pScreenshot;
}

//____ Type() _________________________________________________________________

const char * WgZoomOutCapsule::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgZoomOutCapsule::GetClass()
{
	return c_widgetType;
}

//____ SetSkin() ______________________________________________________________

void WgZoomOutCapsule::SetSkin(const WgSkinPtr& pSkin)
{
	if (pSkin != m_pSkin)
	{
		m_pSkin = pSkin;
		_requestResize();
		_requestRender();
	}
}

//____ SetOutlineSkin() ________________________________________________________

void WgZoomOutCapsule::SetOutlineSkin(const WgSkinPtr& pSkin)
{
    if (pSkin != m_pOutlineSkin)
    {
        m_pOutlineSkin = pSkin;
        if( m_bOutlineMode )
            _requestRender();
    }
}


//____ SetButtonSkin() ____________________________________________________________

void WgZoomOutCapsule::SetButtonSkin(const WgSkinPtr& pSkin)
{
        if( pSkin != m_pButtonSkin )
        {
            m_pButtonSkin = pSkin;
            _requestRender();
        }
}

//____ SetMinPointSize() __________________________________________________________

void WgZoomOutCapsule::SetMinPointSize( WgSize min )
{
    m_minPointSize = min;
    
    WgSize size = PixelSize();
    if( size.w < min.w*m_scale/WG_SCALE_BASE || size.h < min.h *m_scale/WG_SCALE_BASE )
        _requestResize();
    
}

//____ MinPixelSize() ___________________________________________________________

WgSize WgZoomOutCapsule::MinPixelSize() const
{
    return m_minPointSize * m_scale / WG_SCALE_BASE;
}


//____ PreferredPixelSize() ______________________________________________________

WgSize WgZoomOutCapsule::PreferredPixelSize() const
{
    WgSize preferred = WgCapsule::PreferredPixelSize() * m_outerZoom;
    
    WgSize minSize = m_minPointSize * m_scale / WG_SCALE_BASE;
    
    if( preferred.w < minSize.w)
        preferred.w = minSize.w;
    
    if(preferred.h < minSize.h)
        preferred.h = minSize.h;
    
    return preferred;
}

//____ MatchingPixelHeight() _____________________________________________________________

int WgZoomOutCapsule::MatchingPixelHeight(int pixelWidth) const
{
    int matching = WgCapsule::MatchingPixelHeight(pixelWidth) * m_outerZoom;
    
    int min = m_minPointSize.h * m_scale / WG_SCALE_BASE;
    
    if( matching < min )
        matching = min;
    
    return matching;
}

//____ MatchingPixelWidth() _____________________________________________________________

int WgZoomOutCapsule::MatchingPixelWidth(int pixelHeight) const
{
    int matching = WgCapsule::MatchingPixelWidth(pixelHeight) * m_outerZoom;
    
    int min = m_minPointSize.w * m_scale / WG_SCALE_BASE;
    
    if( matching < min )
        matching = min;
        
    return matching;
}

//____ SetInnerTransition() ______________________________________________________________

void WgZoomOutCapsule::SetInnerTransition(float factor)
{
    wg::limit(factor, 0.f, 1.f);
    
    m_innerTransitionFactor = factor;
    float zoom = m_minInnerZoom + (1.f-m_minInnerZoom)*WgUtil::ParametricBlendInOut(factor);
    
    if( zoom != m_innerZoom )
    {
        if( zoom == 1.f )
        {
            delete m_pScreenshot;
            m_pScreenshot = nullptr;
        }
        
        m_innerZoom = zoom;
        _requestRender();
    }
}

//____ SetInOutTransition() ___________________________________________________________

void WgZoomOutCapsule::SetInOutTransition(float factor )
{
    wg::limit(factor, 0.f, 1.f);
    
    if( factor != m_transitionFactor )
    {
        m_transitionFactor = factor;
    
        m_outerZoom = WgUtil::ParametricBlendInOut(factor);
        _requestResize();
    }
}

//____ SetOutlineMode() ___________________________________________________________

void WgZoomOutCapsule::SetOutlineMode(bool bOutline)
{
    if( bOutline != m_bOutlineMode )
    {
        m_bOutlineMode = bOutline;
        _requestRender();
    }
}

//____ SetTint() ________________________________________________________________

void WgZoomOutCapsule::SetTint( WgColor tint )
{
    if( tint != m_tint )
    {
        m_tint = tint;
        _requestRender();
    }
}

//____ ContentAreaPixels() ____________________________________________________________

WgRect WgZoomOutCapsule::ContentAreaPixels() const
{
        if(m_pScreenshot)
            return _screenshotArea(PixelSize());
        else
            return PixelSize();
}

//____ ContentAreaPoints() ____________________________________________________________

WgRect WgZoomOutCapsule::ContentAreaPoints() const
{
    WgRect r = ContentAreaPixels();
    
    return { r.x*WG_SCALE_BASE/m_scale, r.y*WG_SCALE_BASE/m_scale, r.w*WG_SCALE_BASE/m_scale, r.h*WG_SCALE_BASE/m_scale };
}


//____ FindWidget() ____________________________________________________________

WgWidget * WgZoomOutCapsule::FindWidget( const WgCoord& ofs, WgSearchMode mode )
{
    if( m_innerZoom == 1.f )
        return WgCapsule::FindWidget(ofs,mode);
    else
    {
        if( mode == WgSearchMode::Geometry || _onAlphaTest(ofs) )
            return this;

        return nullptr;
    }
}


//____ _onCloneContent() _______________________________________________________

void WgZoomOutCapsule::_onCloneContent( const WgWidget * _pOrg )
{
//	WgZoomOutCapsule * pOrg = (WgZoomOutCapsule*) _pOrg;

}

//____ _onEvent() ______________________________________________________________

void WgZoomOutCapsule::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
    if( m_innerZoom == 1.f )
    {
        WgCapsule::_onEvent(pEvent, pHandler);
        return;
    }
    
    switch( pEvent->Type() )
    {
        case WG_EVENT_MOUSEBUTTON_PRESS:
        {
           auto * pEv = static_cast<const WgEvent::MouseButtonPress*>(pEvent);
            
            if( pEv->Button() == 1 && m_buttonState.isHovered() )
            {
                m_bButtonPressed = true;
                _updateButtonState(PixelSize(), pEvent->PointerPixelPos() );
            }
        }
        break;

        case WG_EVENT_MOUSEBUTTON_RELEASE:
        {
            auto * pEv = static_cast<const WgEvent::MouseButtonRelease*>(pEvent);
            
            if( pEv->Button() == 1 )
            {
                m_bButtonPressed = true;
                if( m_buttonState.isPressed() )
                {
                    _updateButtonState(PixelSize(), pEvent->PointerPixelPos() );
                    pHandler->QueueEvent( new WgEvent::Selected(this) );
                }
            }
        }
        break;
            
        case WG_EVENT_MOUSE_ENTER:
        case WG_EVENT_MOUSE_MOVE:
        case WG_EVENT_MOUSE_LEAVE:
        {
            _updateButtonState(PixelSize(), pEvent->PointerPixelPos() );
        }
        break;
           
        case WG_EVENT_DROP_PICK:
        {
            auto * pEv = const_cast<WgEvent::DropPick*>(static_cast<const WgEvent::DropPick*>(pEvent));

            WgCoord pickOfs = pEv->pickOfs();
            
            WgRect canvas = m_pSkin ? m_pSkin->ContentRect(PixelSize(), m_state, m_scale) : WgRect(PixelSize());
            
            WgRect screenshotArea = _screenshotArea(canvas);
            
            if( screenshotArea.contains(pickOfs) )
            {
                pEv->setPayload(WgPayloadWidget::Create(m_hook.Widget()).GetRealPtr());
                
                // Generate a screenshot

                if( !m_pScreenshot && WgBase::Context()->pDevice )
                {
                    WgBase::Context()->pDevice->beginRender();
                    _regenScreenshot();
                    WgBase::Context()->pDevice->endRender();
                }
                
                auto pSurface = WgBase::Context()->pFactory->CreateSurface(m_pScreenshot->PixelSize(), WgPixelType::BGRA_8, wg::SurfaceFlag::Mipmapped );
                pSurface->CopyFrom( m_pScreenshot, {0,0} );

                auto pImage = new WgImage();
                pImage->SetImage(pSurface, true);

                auto pDragWidget = new WgSizeCapsule();
                pDragWidget->SetPreferredSize(screenshotArea.size()*WG_SCALE_BASE/m_scale);
                
                auto pShader = new WgShaderCapsule();
                pShader->SetColor( {255,255,255,64} );
                pShader->SetChild(pImage);

                pDragWidget->SetChild(pShader);
                pEv->setDragWidget( pDragWidget, screenshotArea.pos() - pickOfs  );
            }
            else
                return;             // Avoid WgWidget from setting default payload.
        }
        break;
            
//        case WG_EVENT_DROP_CANCEL:
//            SetOutlineMode(false);
//            break;
            
        default:
            break;
            
    }
    
    //TODO: Should swallow mouse button events
    
    WgCapsule::_onEvent(pEvent, pHandler);

}

//____ _updateButtonState() __________________________________________________

void WgZoomOutCapsule::_updateButtonState( WgSize gizmoCanvas, WgCoord pointerPos )
{
    WgRect r = _buttonArea(gizmoCanvas);
    WgState buttonState = WgStateEnum::Normal;
    
    if( r.contains(pointerPos) )
    {
        if( m_bButtonPressed )
            buttonState = WgStateEnum::Pressed;
        else
            buttonState = WgStateEnum::Hovered;
    }
    
    if( buttonState != m_buttonState )
    {
        m_buttonState = buttonState;
        _requestRender();
    }
}

//____ _regenScreenshot() _____________________________________________________

void WgZoomOutCapsule::_regenScreenshot()
{
    WgSurfaceFactory * pFactory = WgBase::Context()->pFactory;
    wg::GfxDevice * pDevice = WgBase::Context()->pDevice.rawPtr();

    if( m_pScreenshot )
        delete m_pScreenshot;

    if( !pDevice || !pFactory )
    {
        m_pScreenshot = nullptr;
        return;
    }
    
    WgSize sz = PixelSize();
    
    m_pScreenshot = pFactory->CreateSurface(sz, WgPixelType::BGRA_8, wg::SurfaceFlag::Mipmapped);
    m_pScreenshot->setScaleMode( WgScaleMode::Interpolate );
    m_pScreenshot->Fill(WgColor::Transparent);
    
    WgPatches patches;
    patches.Add(sz);
    
    auto pOldCanvas = pDevice->canvas();
    WgColor oldTint = pDevice->tintColor();
    WgBlendMode oldBlendMode = pDevice->blendMode();
    pDevice->setCanvas(m_pScreenshot->RealSurface());
    pDevice->setBlendMode(WgBlendMode::Blend);
    
    //            pDevice->SetTintColor( {oldTint.r, oldTint.g, oldTint.b, oldTint.a });
    m_hook.Widget()->_renderPatches(pDevice, sz, sz, &patches);
    
    pDevice->setCanvas(pOldCanvas);
    pDevice->setTintColor(oldTint);
    pDevice->setBlendMode(oldBlendMode);

}

//____ _onCollectPatches() ______________________________________________________

void WgZoomOutCapsule::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
    // Do the simple thing here
    
    container.Add( WgRect( geo, clip ) );
}

//____ _onMaskPatches() _________________________________________________________

void WgZoomOutCapsule::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
    if( m_innerZoom == 1.f )
        WgCapsule::_onMaskPatches( patches, geo, clip, blendMode );
    else
    {
        // Let's make this simple. We don't mask anything when zoomed out.
    }
}


//____ _renderPatches() __________________________________________________________

void WgZoomOutCapsule::_renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
    if( m_innerZoom == 1.f )
        WgCapsule::_renderPatches( pDevice, _canvas, _window, _pPatches );
    else
    {
        
        if( (m_pScreenshot == nullptr || _canvas.size() != m_pScreenshot->PixelSize()) && m_hook.Widget() )
        {
            _regenScreenshot();
        }
        
        WgColor oldTint;
        
        if( m_tint != WgColor::White )
        {
            oldTint = pDevice->tintColor();
            pDevice->setTintColor( oldTint * m_tint );
        }

        pDevice->setClipList(_pPatches->Size(), _pPatches->Begin());
        _onRender( pDevice, _canvas, _window );
        
        if( m_tint != WgColor::White )
            pDevice->setTintColor( oldTint );
    }
}


//____ _onRender() _____________________________________________________________

void WgZoomOutCapsule::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
    
//	WgCapsule::_onRender(pDevice, _canvas, _window);

    if( m_innerZoom < 1.f )
    {
        WgColor oldTint = pDevice->tintColor();
        
        pDevice->setTintColor({oldTint.r, oldTint.g, oldTint.b, uint8_t(oldTint.a*WgUtil::ParametricBlendInOut(1.f-m_innerTransitionFactor)) });

        if (m_pSkin)
            m_pSkin->Render(pDevice, m_state, _canvas, m_scale);

        WgRect canvas = m_pSkin ? m_pSkin->ContentRect(_canvas, m_state, m_scale) : _canvas;

        if( m_pButtonSkin )
            m_pButtonSkin->Render(pDevice, m_buttonState, _buttonArea(canvas), m_scale);

        pDevice->setTintColor(oldTint);


    
        if( m_bOutlineMode )
        {
            if( m_pOutlineSkin )
                m_pOutlineSkin->Render(pDevice, m_state, _screenshotArea(canvas), m_scale);
        }
        else if( m_pScreenshot )
        {
            pDevice->setBlitSource(m_pScreenshot->RealSurface());
            pDevice->stretchBlit(_screenshotArea(canvas));
        }
    }
}

//____ _onAlphaTest() ___________________________________________________________

bool WgZoomOutCapsule::_onAlphaTest( const WgCoord& ofs )
{
    WgSize sz = PixelSize();
    
    WgRect canvas = m_pSkin ? m_pSkin->ContentRect(sz, m_state, m_scale) : WgRect(sz);
    
	if( m_pScreenshot )
    {
        WgRect r = _screenshotArea(canvas);
        
        if( r.contains(ofs) )
        {

            //TODO: Figure out what is wrong and fix.
            
//            int alpha = m_pScreenshot->GetOpacity((ofs-r.pos())/m_zoomOutScale);
//            if( alpha >= m_markOpacity )
                return true;
        }
    }

    if( m_pButtonSkin )
    {
        WgRect r = _buttonArea(canvas);
        
        if( r.contains(ofs) )
        {
            int alpha = m_pButtonSkin->MarkTest(ofs - r.pos(), r.size(), m_state, m_markOpacity, m_scale);
            if( alpha >= m_markOpacity )
                return true;
        }
    }
    
    if( m_pSkin )
    {
        return m_pSkin->MarkTest(ofs, sz, m_state, m_markOpacity, m_scale);
    }

    return false;
}

//____ _setScale() ____________________________________________________________

void WgZoomOutCapsule::_setScale( int scale )
{
    WgCapsule::_setScale(scale);

    if( m_pScreenshot || m_pSkin )
        _requestResize();
}

//____ _screenshotArea() _____________________________________________________

WgRect WgZoomOutCapsule::_screenshotArea( WgRect canvas ) const
{
    WgSize sz;
    
    if( m_pScreenshot )
        sz = m_pScreenshot->PixelSize() * (m_innerZoom * m_outerZoom);
    else
        sz = { int(canvas.w*m_minInnerZoom), int(canvas.h*m_minInnerZoom) };
    
    
    return { canvas.x + (canvas.w-sz.w)/2, canvas.y + (canvas.h-sz.h)/2, sz.w, sz.h };
}

//____ _buttonArea() _____________________________________________________

WgRect WgZoomOutCapsule::_buttonArea( WgRect canvas ) const
{
    if( !m_pButtonSkin )
        return { 0, 0, 0, 0 };
    
    WgSize sz = m_pButtonSkin->PreferredSize(m_scale);
    
    return { canvas.pos(), sz };
}


