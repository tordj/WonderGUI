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

//____ SetFront() _____________________________________________________________

WgHook * WgShadowLayer::SetFront(WgWidget * pWidget)
{
	// Replace Widget

	WgWidget * pOldWidget = m_frontHook._releaseWidget();
	if (pOldWidget)
		delete pOldWidget;
	m_frontHook._attachWidget(pWidget);
	_onFrontChanged();
	return &m_baseHook;
}

//____ Front() ________________________________________________________________

WgWidget * WgShadowLayer::Front()
{
	return m_frontHook.Widget();
}

//____ DeleteFront() __________________________________________________________

bool WgShadowLayer::DeleteFront()
{
	WgWidget * pWidget = m_frontHook._releaseWidget();
	if (pWidget)
	{
		_onFrontChanged();
		delete pWidget;
		return true;
	}

	return false;
}

//____ ReleaseFront() _________________________________________________________

WgWidget * WgShadowLayer::ReleaseFront()
{
	WgWidget * pWidget = m_frontHook._releaseWidget();
	if (pWidget)
		_onFrontChanged();

	return pWidget;
}

//____ DeleteChild() __________________________________________________________

bool WgShadowLayer::DeleteChild(WgWidget * pWidget) 
{
	if (pWidget == m_baseHook.Widget())
		return DeleteBase();
	else if (pWidget == m_frontHook.Widget())
		return DeleteFront();

	return false; 
}

//____ ReleaseChild() _________________________________________________________

WgWidget * WgShadowLayer::ReleaseChild(WgWidget * pWidget) 
{ 
	if (pWidget == m_baseHook.Widget())
		return ReleaseBase();
	else if (pWidget == m_frontHook.Widget())
		return ReleaseFront();

	return nullptr;
}

//____ DeleteAllChildren() ____________________________________________________

bool WgShadowLayer::DeleteAllChildren() 
{ 
	return DeleteBase() || DeleteFront();
}

//____ ReleaseAllChildren() ___________________________________________________

bool WgShadowLayer::ReleaseAllChildren() 
{ 
	return ReleaseBase() || ReleaseFront();
}

//____ ClearShadows() _________________________________________________________

void WgShadowLayer::ClearShadows()
{
	_willRemoveShadows(0, (int) m_shadows.size());
	m_shadows.clear();
}

//____ AddShadow() ____________________________________________________________

bool WgShadowLayer::AddShadow(WgWidget * pWidget, const WgSkinPtr& pShadow)
{
    if( !pWidget || !pShadow )
        return false;
    
    WgCoord pos;
    _descendantPos(pWidget, pos);
    
    WgRect geo = { pos - pShadow->ContentOfs(WgStateEnum::Normal, m_scale), pShadow->SizeForContent(pWidget->PixelSize(), m_scale) };
    
    m_shadows.push_back( WgShadow(pWidget,pShadow.GetRealPtr(),geo));
    
    WgPatches patches;
    patches.Add(geo);
    
    // Remove portions of patches that are covered by opaque front widgets
    
    m_frontHook.Widget()->_onMaskPatches(patches, m_size, m_size, _getBlendMode());
    
    // Make request render calls
    
    for (const WgRect * pRect = patches.Begin(); pRect < patches.End(); pRect++)
        _requestRender(*pRect);
    
    return true;
}

//____ RemoveShadow() _________________________________________________________

void WgShadowLayer::RemoveShadow(int index)
{
	_willRemoveShadows(index, 1);
	m_shadows.erase(m_shadows.begin() + index);
}

void WgShadowLayer::RemoveShadow(WgWidget * pWidget)
{
	for (auto it = m_shadows.begin() ; it != m_shadows.end(); it++)
	{
		if (it->widget() == pWidget)
		{
			_willRemoveShadows(it - m_shadows.begin(), 1);
			m_shadows.erase(it);
			break;
		}
	}   
}

//____ SetSkin() ______________________________________________________________

void WgShadowLayer::SetSkin(const WgSkinPtr& pSkin)
{
	if (pSkin != m_pSkin)
	{
		m_pSkin = pSkin;
		_requestRender();
		_requestResize();
	}
}

//____ SetShadowTint() __________________________________________________________

void WgShadowLayer::SetShadowTint( uint8_t alpha )
{
    if( m_shadowTint != alpha )
    {
        m_shadowTint = alpha;
        _requestRenderShadows( 0, (int) m_shadows.size() );
    }
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

//____ _willRemoveShadows() ________________________________________________________________

void WgShadowLayer::_willRemoveShadows(int ofs, int nb)
{
    _requestRenderShadows(ofs,nb);
}

//____ _requestRenderShadows() ________________________________________________________

void WgShadowLayer::_requestRenderShadows(int ofs, int nb)
{
	for (auto p = &m_shadows[ofs]; p != &m_shadows[ofs + nb]; p++)
	{
		WgPatches patches;
		patches.Add(p->m_geo);

		// Remove portions of patches that are covered by opaque front widgets

		m_frontHook.Widget()->_onMaskPatches(patches, m_size, m_size, _getBlendMode());

		// Make request render calls

		for (const WgRect * pRect = patches.Begin(); pRect < patches.End(); pRect++)
			_requestRender(*pRect);
	}
}

//____ _firstLayerHook() _______________________________________________________________

WgLayerHook * WgShadowLayer::_firstLayerHook() const
{
	if (m_frontHook.Widget() )
		return (WgLayerHook*) &m_frontHook;

	return nullptr;
}

//____ _firstHook() ___________________________________________________________________

WgHook* WgShadowLayer::_firstHook() const
{
	if (m_baseHook.Widget())
		return (WgHook*)&m_baseHook;
	else if (m_frontHook.Widget())
		return (WgHook*)&m_frontHook;

	return nullptr;
}

//____ _lastHook() ____________________________________________________________________

WgHook* WgShadowLayer::_lastHook() const
{
	if (m_frontHook.Widget())
		return (WgHook*)&m_frontHook;
	else if (m_baseHook.Widget())
		return (WgHook*)&m_baseHook;

	return nullptr;
}

//____ _firstHookWithGeo() ____________________________________________________

WgHook * WgShadowLayer::_firstHookWithGeo(WgRect& geo) const
{
	if (m_baseHook.Widget())
	{
		geo = m_frontHook.m_geo;
		return (WgHook*)&m_baseHook;
	}
	else if (m_frontHook.Widget())
	{
		geo = m_frontHook.m_geo;
		return (WgHook*)&m_frontHook;
	}
	return nullptr;
}

//____ _nextHookWithGeo() _____________________________________________________

WgHook * WgShadowLayer::_nextHookWithGeo(WgRect& geo, WgHook * pHook) const
{
	if (pHook == &m_baseHook && m_frontHook.Widget())
	{
		geo = m_frontHook.m_geo;
		return (WgHook*)&m_frontHook;
	}

	return nullptr;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgShadowLayer::_lastHookWithGeo(WgRect& geo) const
{
	if (m_frontHook.Widget())
	{
		geo = m_frontHook.m_geo;
		return (WgHook*)&m_frontHook;
	}
	else if (m_baseHook.Widget())
	{
		geo = m_frontHook.m_geo;
		return (WgHook*)&m_baseHook;
	}

	return nullptr;
}

//____ _prevHookWithGeo() _____________________________________________________

WgHook * WgShadowLayer::_prevHookWithGeo(WgRect& geo, WgHook * pHook) const
{
	if (pHook == &m_frontHook && m_baseHook.Widget())
	{
		geo = m_frontHook.m_geo;
		return (WgHook*)&m_baseHook;
	}

	return nullptr;
}

//____ _onCloneContent() ______________________________________________________

void WgShadowLayer::_onCloneContent(const WgWidget * _pOrg)
{
    
}

//____ _onEvent() _____________________________________________________________

void WgShadowLayer::_onEvent(const WgEvent::Event * pEvent, WgEventHandler * pHandler)
{
	switch (pEvent->Type())
	{
		case WgEventType::WG_EVENT_TICK:
		{
			// Check for removed children and changes to geo that will affect shadows.

			WgPatches patches;

			for (auto it = m_shadows.begin(); it < m_shadows.end();)
			{
				WgWidget * pWidget = it->widget();
				WgShadow * pShadow = &(*it);

				// Remove shadow for deleted widget.

				if (pWidget == nullptr)
				{
					patches.Add(pShadow->m_geo);
					it = m_shadows.erase(it);
                    continue;
				}

				//

				WgCoord pos;
                WgRect oldGeo = pShadow->m_geo;
                WgSize widgetSize = pWidget->PixelSize();

                if( widgetSize.w > 0 && widgetSize.h > 0 && _descendantPos(pWidget, pos))
				{
					// Widget is still our descendant and visible, check
					// so its geo has not changed.

                    WgSkin * pSkin = pShadow->shadow();
                    WgRect geo = { pos - pSkin->ContentOfs(WgStateEnum::Normal,m_scale), pSkin->SizeForContent(widgetSize, m_scale) };

                    if (geo != oldGeo)
                    {
                        patches.Add(oldGeo);
                        patches.Add(geo);
                        pShadow->m_geo = geo;
                    }
				}
				else
				{
					// Widget is currently hidden or not a descendant of us,
					// hide the shadow.

					if (!oldGeo.isEmpty())
					{
						patches.Add(oldGeo);
                        pShadow->m_geo = {0,0,0,0};
					}
				}

                it++;       // Only increase iterator if widget wasn't deleted.
			}

			// Early out if there is nothing to update in shadow layer.

			if (patches.IsEmpty())
				break;

			// Mask foreground from shadow updates and request render on the remains.

			if (m_frontHook.Widget())
				m_frontHook.Widget()->_onMaskPatches(patches, m_size, m_size, WgBlendMode::Blend);

			for (const WgRect * pRect = patches.Begin(); pRect < patches.End(); pRect++)
				_requestRender(*pRect);

			break;
		}

		default:
			WgLayer::_onEvent(pEvent, pHandler);
			break;
	}
}

//____ _onNewSize() ___________________________________________________________

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

//____ _renderPatches() _______________________________________________________

void WgShadowLayer::_renderPatches(wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches)
{
	WgRect contentGeo = m_frontHook.m_geo + _canvas.pos();

	WgPatches patches;
	patches.Push(_pPatches);

	// Generate masked patches for our skin, baseSlot widget, and shadow.

	if (m_frontHook.Widget() )
		m_frontHook.Widget()->_onMaskPatches(patches, contentGeo, contentGeo, pDevice->blendMode());		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

	patches.Clip(_canvas);
	if (patches.IsEmpty())
		return;

	// If we have a skin, render it

	if (m_pSkin)
	{
		pDevice->setClipList(patches.Size(), patches.Begin());
		m_pSkin->Render(pDevice, m_state, _canvas, m_scale );
	}

	// Render base slot widgets

	if (m_baseHook.Widget() )
		m_baseHook.Widget()->_renderPatches(pDevice, contentGeo, contentGeo, &patches);

	// Update shadow layer

	bool bFullSurfaceUpdate = !m_pShadowSurface;

	if (!m_pShadowSurface)
	{
		auto pSurfaceFactory = WgBase::Context()->pFactory;

		if (pSurfaceFactory)
			m_pShadowSurface = pSurfaceFactory->CreateSurface(_canvas.size(), WgPixelType::BGRA_8);
	}

	if (m_pShadowSurface)
	{
		WgPatches   shadowPatches;

		if (bFullSurfaceUpdate)
			shadowPatches.Push({ 0,0, _canvas.size() });
		else
		{
			const WgRect * pPatches = patches.Begin();
			for (int i = 0; i < patches.Size(); i++)
				shadowPatches.Push(pPatches[i] - _canvas.pos());
		}

		auto oldBlendMode = pDevice->blendMode();
		auto oldTint = pDevice->tintColor();

		if (!shadowPatches.IsEmpty())
		{
			auto oldCanvas = pDevice->canvas();

			pDevice->setCanvas(m_pShadowSurface->RealSurface());

			pDevice->setClipList(shadowPatches.Size(), shadowPatches.Begin());
			pDevice->setTintColor(WgColor::White);
			pDevice->setBlendMode(WgBlendMode::Replace);
			pDevice->fill({ 0,0,0,0 });

			pDevice->setBlendMode(WgBlendMode::Max);

			for (auto& shadow : m_shadows)
				shadow.shadow()->Render(pDevice, WgStateEnum::Normal, shadow.m_geo, m_scale);


			pDevice->setCanvas(oldCanvas);
			pDevice->setClipList(patches.Size(), patches.Begin());
		}


		// Render shadows

        pDevice->setTintColor( { 255,255,255,m_shadowTint} );
		pDevice->setBlendMode(WgBlendMode::Blend);
		pDevice->setBlitSource( m_pShadowSurface->RealSurface() );
		pDevice->blit( _canvas.pos() );

//        for (auto& shadow : m_shadows)
//            pDevice->blit( shadow.m_geo.pos() + _canvas.pos(), shadow.m_geo );
        
        pDevice->setTintColor(oldTint);
		pDevice->setBlendMode(oldBlendMode);

	}

	//Render front slot widgets

	if (m_frontHook.Widget() )
		m_frontHook.Widget()->_renderPatches(pDevice, contentGeo, contentGeo, _pPatches);

}


//____ _onFrontChanced() ______________________________________________________

void WgShadowLayer::_onFrontChanged()
{
	_onRequestRender(WgRect(0, 0, m_size), 0);
	_requestResize();

	if (m_frontHook.Widget())
		m_frontHook.Widget()->_onNewSize(m_size);
}

