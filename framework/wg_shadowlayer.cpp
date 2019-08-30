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

#include <wg_shadowlayer.h>
#include <wg_eventhandler.h>
#include <wg_event.h>
#include <wg_gfxdevice.h>
#include <wg_surfacefactory.h>
#include <wg_patches.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_util.h>

#include <algorithm>

using namespace WgUtil;

static const char    c_widgetType[] = { "ShadowLayer" };
static const char    c_hookType[] = { "ShadowHook" };

//_____________________________________________________________________________
WgShadowLayer* WgShadowHook::Parent() const
{
    return m_pParent;
}


//_____________________________________________________________________________
const char * WgShadowHook::Type(void) const
{
    return ClassType();
}

//_____________________________________________________________________________
const char * WgShadowHook::ClassType()
{
    return c_hookType;
}

//_____________________________________________________________________________
void WgShadowHook::_requestResize()
{
}

//_____________________________________________________________________________
WgHook * WgShadowHook::_prevHook() const
{
    if (m_pParent->m_baseHook.Widget())
        return &m_pParent->m_baseHook;
    else
        return 0;
}

//_____________________________________________________________________________
WgHook * WgShadowHook::_nextHook() const
{
    return 0;
}

//_____________________________________________________________________________
WgContainer * WgShadowHook::_parent() const
{
    return m_pParent;
}




//____ Constructor ____________________________________________________________

WgShadowLayer::WgShadowLayer() : m_frontHook(this)
{
    _startReceiveTicks();
}

//____ Destructor _____________________________________________________________

WgShadowLayer::~WgShadowLayer()
{
    _stopReceiveTicks();
    delete m_pShadowSurface;
}

//____ Type() _________________________________________________________________

const char *WgShadowLayer::Type(void) const
{
    return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgShadowLayer::GetClass()
{
    return c_widgetType;
}

void WgShadowLayer::ClearShadows()
{
    
}

bool WgShadowLayer::AddShadow(WgWidget * pWidget, WgSkin * pShadow)
{
    if( !pWidget || !pShadow )
        return false;
    
    WgCoord pos;
    _descendantPos(pWidget, pos);
    
    WgRect geo = { pos - pShadow->ContentOfs(WgStateEnum::Normal, m_scale), pShadow->SizeForContent(pWidget->PixelSize(), m_scale) };
    
    m_shadows.push_back( WgShadow(pWidget,pShadow,geo));
    
    WgPatches patches;
    patches.Add(geo);
    
    // Remove portions of patches that are covered by opaque front widgets
    
    m_frontHook.Widget()->_onMaskPatches(patches, m_size, m_size, _getBlendMode());
    
    // Make request render calls
    
    for (const WgRect * pRect = patches.Begin(); pRect < patches.End(); pRect++)
        _requestRender(*pRect);
    
    return true;
}


void WgShadowLayer::RemoveShadow(int index)
{
    
}

void WgShadowLayer::RemoveShadow(WgWidget * pWidget)
{
    
}

void WgShadowLayer::SetSkin(const WgSkinPtr& pSkin)
{
    
}

//____ MatchingPixelHeight() _________________________________________________________________

int WgShadowLayer::MatchingPixelHeight( int width ) const
{
    int matchFront = 0;
    int matchBase = 0;
    
    WgSize padding = m_pSkin ? m_pSkin->ContentPadding(m_scale) : WgSize(0,0);
    width -= padding.w;
    
    if (m_baseHook.Widget() )
        matchBase = m_baseHook.Widget()->MatchingPixelHeight(width);
    if (m_frontHook.Widget() )
        matchFront = m_frontHook.Widget()->MatchingPixelHeight(width);
    
    return std::max(matchFront, matchBase) + padding.h;
}

//____ MatchingPixelWidth() _________________________________________________________________

int WgShadowLayer::MatchingPixelWidth( int height ) const
{
    int matchFront = 0;
    int matchBase = 0;
    
    WgSize padding = m_pSkin ? m_pSkin->ContentPadding(m_scale) : WgSize(0,0);
    height -= padding.h;
    
    if (m_baseHook.Widget() )
        matchBase = m_baseHook.Widget()->MatchingPixelWidth(height);
    if (m_frontHook.Widget() )
        matchFront = m_frontHook.Widget()->MatchingPixelWidth(height);
    
    return std::max(matchFront, matchBase) + padding.w;
}

//____ PreferredPixelSize() _________________________________________________________________

WgSize WgShadowLayer::PreferredPixelSize() const
{
    WgSize prefFront;
    WgSize prefBase;
    WgSize padding = m_pSkin ? m_pSkin->ContentPadding(m_scale) : WgSize(0,0);
    
    if (m_baseHook.Widget() )
        prefBase = m_baseHook.Widget()->PreferredPixelSize();
    if (m_frontHook.Widget() )
        prefFront = m_frontHook.Widget()->PreferredPixelSize();
    
    return WgSize::max(prefFront, prefBase) + padding;
}


void WgShadowLayer::_onRequestRender(const WgRect& rect, const WgLayerHook * pHook)
{
    
}


WgLayerHook * WgShadowLayer::_firstLayerHook() const
{
    
}

WgHook* WgShadowLayer::_firstHook() const
{
    
}

WgHook* WgShadowLayer::_lastHook() const
{
    
}

WgHook * WgShadowLayer::_firstHookWithGeo(WgRect& geo) const
{
}

WgHook * WgShadowLayer::_nextHookWithGeo(WgRect& geo, WgHook * pHook) const
{
}

WgHook * WgShadowLayer::_lastHookWithGeo(WgRect& geo) const
{
    
}

WgHook * WgShadowLayer::_prevHookWithGeo(WgRect& geo, WgHook * pHook) const
{
    
}

void WgShadowLayer::_replaceWidgetInHook(WgWidget * pNewWidget)
{
    
}


void WgShadowLayer::_onCloneContent(const WgWidget * _pOrg)
{
    
}

void WgShadowLayer::_onEvent(const WgEvent::Event * pEvent, WgEventHandler * pHandler)
{
    
}

void WgShadowLayer::_onNewSize(const WgSize& size)
{
    delete m_pShadowSurface;
    m_pShadowSurface = nullptr;
    
    if( m_pSkin )
        m_frontHook.m_geo = m_pSkin->ContentRect(size, m_state, m_scale);
    else
        m_frontHook.m_geo = { 0,0,size };

    if (m_frontHook.Widget())
        m_frontHook.Widget()->_onNewSize(m_frontHook.m_geo);

    WgLayer::_onNewSize(size);
}

void WgShadowLayer::_renderPatches(wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches)
{
    
}


//++++=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//____ _setSkin() _______________________________________________________________

void ShadowLayer::_setSkin( Skin * pSkin )
{
    //TODO: Update m_frontSlot.geo if content padding (for current state) has been affected.
    
    Layer::_setSkin(pSkin);
}

//____ _childRequestResize() ______________________________________________

void ShadowLayer::_childRequestResize(Slot * pSlot)
{
    _requestResize();
}

//____ _releaseChild() ____________________________________________________

void ShadowLayer::_releaseChild(Slot * pSlot)
{
    if (pSlot == &m_baseSlot)
        Layer::_releaseChild(pSlot);
    else
    {
        m_frontSlot.replaceWidget(this, nullptr);
        _requestRender( m_frontSlot.geo );
        _requestResize();
    }
}

//____ _beginLayerSlots() _______________________________________________

const LayerSlot * ShadowLayer::_beginLayerSlots() const
{
    return &m_frontSlot;
}

//____ _endLayerSlots() ________________________________________________

const LayerSlot * ShadowLayer::_endLayerSlots() const
{
    if( m_frontSlot.pWidget )
        return (&m_frontSlot) + 1;
    else
        return &m_frontSlot;
}

//____ _sizeOfLayerSlot() _______________________________________________

int ShadowLayer::_sizeOfLayerSlot() const
{
    return sizeof(LayerSlot);
}

//____ _onRequestRender() _______________________________________________

void ShadowLayer::_onRequestRender(const RectI& rect, const LayerSlot * pSlot)
{
    // Widgets from base slot can be masked by widgets from front slot.

    if (pSlot == &m_baseSlot)
    {
        Patches patches;
        patches.add(rect);

        // Remove portions of patches that are covered by opaque front widgets

        m_frontSlot.pWidget->_maskPatches(patches, m_frontSlot.geo, m_frontSlot.geo, _getBlendMode());

        // Make request render calls

        for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
            _requestRender(*pRect);
    }
    else
        _requestRender( rect + m_frontSlot.geo.pos() );
}

//____ _object() __________________________________________________________

Object * ShadowLayer::_object()
{
    return this;
}

//____ _didAddShadows() ____________________________________________________

void ShadowLayer::_didAddShadows(int nb)
{
    for (auto it = m_shadows.end() - nb; it != m_shadows.end(); it++)
    {
        CoordI pos;
        _descendantPos(it->widget(), pos);
        
//            RectI geo = it->shadow()->_contentRect( {pos, it->widget()->m_size}, StateEnum::Normal );

        Skin * pSkin = it->shadow();
        RectI geo = { pos - pSkin->_contentOfs(StateEnum::Normal), pSkin->_sizeForContent(it->widget()->m_size) };

        _setShadowGeo(&(*it), geo);

        Patches patches;
        patches.add(geo);
        
        // Remove portions of patches that are covered by opaque front widgets
        
        m_frontSlot.pWidget->_maskPatches(patches, m_size, m_size, _getBlendMode());
        
        // Make request render calls
        
        for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
            _requestRender(*pRect);
    }


}

//____ _willRemoveShadows() _______________________________________________

void ShadowLayer::_willRemoveShadows(int ofs, int nb)
{
    for (auto it = m_shadows.end() - nb; it != m_shadows.end(); it++)
    {
        RectI geo = _shadowGeo(&(*it));
        
        Patches patches;
        patches.add(geo);
        
        // Remove portions of patches that are covered by opaque front widgets
        
        m_frontSlot.pWidget->_maskPatches(patches, m_size, m_size, _getBlendMode());
        
        // Make request render calls
        
        for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
            _requestRender(*pRect);
    }
}

//____ _cloneContent() ____________________________________________________

void ShadowLayer::_cloneContent( const Widget * _pOrg )
{

}

//____ _receive() __________________________________________________________

void ShadowLayer::_receive( Msg * _pMsg )
{
    switch (_pMsg->type())
    {
        case MsgType::Tick:
        {
            //TODO: Should not be on tick, but on pre-render stage.
            //TODO: Needs to work with absent frontLayer;
            //TODO: Should check that shadow casting children are descendants of our frontLayer, not us.
            //TODO: Should use A8 surface for shadow once GlGfxDevice can handle it (write to A8).
            
            // Check for removed children and changes to geo that will affect shadows.
            
            Patches patches;

            for( auto it = m_shadows.begin() ; it < m_shadows.end() ; it++ )
            {
                Widget * pWidget = it->widget();
                Shadow * pShadow = &(*it);
                
                // Remove shadow for deleted widget.
                
                if( pWidget == nullptr )
                {
                    patches.add( _shadowGeo(pShadow) );
                    it = m_shadows.erase(it);
                }
                
                //
                
                CoordI pos;
                if( _descendantPos(pWidget, pos))
                {
                    // Widget is still our descendant, check
                    // so its geo has not changed.
                    
//                        RectI geo = {pos, pWidget->m_size };

                    Skin * pSkin = pShadow->shadow();
                    RectI geo = { pos - pSkin->_contentOfs(StateEnum::Normal), pSkin->_sizeForContent(pWidget->m_size) };


                    RectI oldGeo = _shadowGeo(pShadow);
                    if( geo != oldGeo )
                    {
                        patches.add(oldGeo);
                        patches.add(geo);
                        _setShadowGeo(pShadow, geo);
                    }
                }
                else
                {
                    // Widget is currently not a descendant of us,
                    // hide the shadow.
                    
                    RectI geo = _shadowGeo(pShadow);
                    if( !geo.isEmpty() )
                    {
                        patches.add(geo);
                        _setShadowGeo(pShadow, {0,0,0,0} );
                    }
                }
            }
            
            // Early out if there is nothing to update in shadow layer.
            
            if( patches.isEmpty() )
                break;
            
            // Mask foreground from shadow updates and request render on the remains.
            
            if( m_frontSlot.pWidget )
                m_frontSlot.pWidget->_maskPatches(patches, m_size, m_size, BlendMode::Blend);
            
            for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
                _requestRender(*pRect);

            break;
        }

        default:
            break;
    }

    Layer::_receive(_pMsg);
}



//____ _render() __________________________________________________________

void ShadowLayer::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
{
    RectI contentGeo = m_frontSlot.geo + _canvas.pos();

    // Generate masked patches for our skin, baseSlot widget, and shadow.

    ClipPopData clipPop;

    if (m_frontSlot.pWidget)
    {
        // Collect dirty patches from gfxDevice

        int nClipRects = pDevice->clipListSize();
        auto pClipRects = pDevice->clipList();

        Patches patches(nClipRects);

        for (int i = 0; i < nClipRects; i++)
            patches.push(pixelsToRaw(pClipRects[i]));

        m_frontSlot.pWidget->_maskPatches(patches, contentGeo, contentGeo, pDevice->blendMode());		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

        clipPop = patchesToClipList(pDevice, patches);
    }


    // If we have a skin, render it

    if (m_pSkin)
        m_pSkin->_render(pDevice, _canvas, m_state);
    
    // Render base slot widgets

    if (m_baseSlot.pWidget)
        m_baseSlot.pWidget->_render(pDevice, contentGeo, contentGeo);

    // Update shadow layer

    bool bFullSurfaceUpdate = !m_pShadowSurface;

    if (!m_pShadowSurface)
    {
        auto pSurfaceFactory = Base::activeContext()->surfaceFactory();

        if (pSurfaceFactory)
            m_pShadowSurface = pSurfaceFactory->createSurface(rawToPixels(m_size), PixelFormat::BGRA_8);
    }
    
    if( m_pShadowSurface )
    {
        auto oldCanvas = pDevice->canvas();
        auto oldBlendMode = pDevice->blendMode();

        pDevice->setCanvas(m_pShadowSurface, false);

        RectI        fullClipList;
        ClipPopData orgClipList;
        if (bFullSurfaceUpdate)
        {
            orgClipList = pushClipList(pDevice);
            fullClipList = { 0,0, rawToPixels(m_size) };
            pDevice->setClipList(1, &fullClipList);
        }
        
        pDevice->setBlendMode(BlendMode::Replace);
        pDevice->fill( {255,255,255,0} );

        pDevice->setBlendMode(BlendMode::Max);
        for( auto& shadow : m_shadows )
            shadow.shadow()->_render(pDevice, _shadowGeo(&shadow) + contentGeo.pos(), StateEnum::Normal);

        if (bFullSurfaceUpdate)
            popClipList(pDevice, orgClipList);

        pDevice->setCanvas(oldCanvas, false);

        // Render shadows

        pDevice->setBlendMode(BlendMode::Subtract);
        pDevice->setBlitSource(m_pShadowSurface);
        pDevice->blit( {0,0} );

        pDevice->setBlendMode(oldBlendMode);

    }

    //Render front slot widgets

    popClipList(pDevice, clipPop);

    if (m_frontSlot.pWidget)
        m_frontSlot.pWidget->_render(pDevice, contentGeo, contentGeo);

}


