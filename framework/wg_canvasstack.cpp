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

#include <wg_canvasstack.h>
#include <wg_base.h>
#include <wg3_context.h>
#include <wg3_util.h>
#include <wg3_base.h>

	using namespace wg::Util;

static const char    c_widgetType[] = {"WgCanvasStack"};
static const char    c_customCapsuleType[] = {"WgCanvasStack::CustomCapsule"};


const wg::TypeInfo WgCanvasStack::CustomSkin::TYPEINFO = { "CanvasStack::CustomSkin", &wg::Skin::TYPEINFO };


//	const TypeInfo CanvasStack::TYPEINFO = { "CanvasStack", &Capsule::TYPEINFO };
//	const TypeInfo CanvasStack::CustomCapsule::TYPEINFO = { "CanvasStack::CustomCapsule", &Capsule::TYPEINFO };

	//____ constructor ____________________________________________________________

	WgCanvasStack::WgCanvasStack()
	{
	}

	//____ Destructor _____________________________________________________________

	WgCanvasStack::~WgCanvasStack()
	{
	}

    //____ Type() _________________________________________________________________

    const char * WgCanvasStack::Type( void ) const
    {
        return GetClass();
    }

    //____ GetClass() ____________________________________________________________

    const char * WgCanvasStack::GetClass()
    {
        return c_widgetType;
    }

//____ SetPreferredSize() _________________________________________________

	void WgCanvasStack::SetPreferredSize(WgSize preferred)
	{
		if (preferred != m_preferred)
		{
			m_preferred = preferred;
			_requestResize();
		}
	}

	//____ PreferredPixelSize() ____________________________________________________

	WgSize WgCanvasStack::PreferredPixelSize() const
	{
		return m_preferred.isEmpty() ? WgCapsule::PreferredPixelSize() : m_preferred*m_scale/WG_SCALE_BASE;
	}

	//____ MatchingPixelWidth() ____________________________________________________

	int WgCanvasStack::MatchingPixelWidth(int height) const
	{
		return m_preferred.isEmpty() ? WgCapsule::MatchingPixelWidth(height) : m_preferred.w*m_scale/WG_SCALE_BASE;
	}

	//____ MatchingPixelHeight() ___________________________________________________

	int WgCanvasStack::MatchingPixelHeight(int width) const
	{
		return m_preferred.isEmpty() ? WgCapsule::MatchingPixelHeight(width) : m_preferred.h*m_scale/WG_SCALE_BASE;
	}

	//____ SetCanvases() ______________________________________________________

	void WgCanvasStack::SetCanvases(int nbCanvases)
	{
		// Error checks and early outs

		if (nbCanvases < 0)
		{
			//TODO: Error handling!
			return;
		}

		_regenCanvases(nbCanvases, m_size - _contentPaddingSize());
	}

	//____ Canvas() ___________________________________________________________

	wg::Surface_p WgCanvasStack::Canvas(int idx) const
	{
		if (idx < 0 || idx >= (int)m_canvases.size())
		{
			//TODO: Error handling
			return nullptr;
		}

		return m_canvases[idx];
	}

	//____ ForceRedraw() ___________________________________________________________

	void WgCanvasStack::ForceRedraw()
	{
		m_patches.clear();
		m_patches.push(PixelSize() - _contentPaddingSize());
		_requestRender();
	}

	//____ SetCreateFunc() ____________________________________________________

	void WgCanvasStack::SetCreateFunc(const std::function<wg::Surface_p(int, WgSize)>& func)
	{
		m_createFunc = func;
	}

	//____ SetClearFunc() _____________________________________________________

	void WgCanvasStack::SetClearFunc(const std::function<void(wg::GfxDevice*, std::vector<wg::Surface_p>&)>& func)
	{
		m_clearFunc = func;
	}

	//____ SetCombineFunc() ___________________________________________________

	void WgCanvasStack::SetCombineFunc(const std::function<void(wg::GfxDevice*, wg::Surface*, std::vector<wg::Surface_p>&)>& func)
	{
		m_combineFunc = func;
	}

	//____ RedirectSkin() _____________________________________________________

	wg::Skin_p WgCanvasStack::RedirectSkin(wg::Skin * pSkin, int canvasIdx)
	{
		return new CustomSkin(this, pSkin, canvasIdx);
	}

	//____ RedirectWidget() ___________________________________________________

	WgCapsule* WgCanvasStack::RedirectWidget(WgWidget * pWidget, int canvasIdx)
	{
		auto p = new CustomCapsule(this, canvasIdx);
		p->SetChild(pWidget);
		return p;
	}

	//____ _renderPatches() __________________________________________________________

    void WgCanvasStack::_renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
    {
        m_bRendering = true;

        if (m_canvases.size() == 0)
        {
            WgCapsule::_renderPatches(pDevice, _canvas, _window, _pPatches);
        }
        else
        {
            if (m_pSkin)
                _renderSkin(m_pSkin, pDevice, m_state, _canvas, m_scale);

            pDevice->setClipList(m_patches.size(), m_patches.begin());
            _clearCanvases(pDevice);
            WgRect canvasRect(0,0, _canvas.size() - _contentPaddingSize());

            auto pOldCanvas = pDevice->canvas();
            pDevice->setCanvas(m_canvases[0],false);
            WgCapsule::_renderPatches(pDevice, canvasRect, canvasRect, &m_patches);
            pDevice->setClipList(m_patches.size(), m_patches.begin());
            pDevice->setCanvas(pOldCanvas,false);

            _combineCanvases(pDevice);

            pDevice->setClipList(_pPatches->size(), _pPatches->begin());

            pDevice->setBlitSource(m_pEndCanvas);
            pDevice->blit(_canvas.pos(), _canvas.size() );

            m_patches.clear();
        }
        m_bRendering = false;
    }

	//____ _onCloneContent() ____________________________________________________

	void WgCanvasStack::_onCloneContent(const WgWidget * _pOrg)
	{

	}

	//____ _onNewSize() __________________________________________________________

	void WgCanvasStack::_onNewSize(const WgSize& size)
	{
		if (size != m_size)
		{
            m_size = size;
			_regenCanvases((int)m_canvases.size(), size - _contentPaddingSize());
			WgCapsule::_onNewSize(size);
		}
	}

	//____ _regenCanvases() ___________________________________________________

	void WgCanvasStack::_regenCanvases( int nbCanvases, wg::SizeI canvasSize )
	{
		// Resize vector

		m_canvases.resize(nbCanvases);

		// Add new canvases if needed

		for (int i = 0; i < nbCanvases; i++)
		{
			if (!m_canvases[i] || m_canvases[i]->size() != canvasSize)
				m_canvases[i] = _createCanvas(i, canvasSize);
		}

		// Regen or delete endCanvas

		if (nbCanvases == 0)
			m_pEndCanvas = nullptr;
		else if (!m_pEndCanvas || m_pEndCanvas->size() != canvasSize)
			m_pEndCanvas = wg::Base::activeContext()->surfaceFactory()->createSurface(canvasSize);

		// Request render

		m_patches.clear();
		m_patches.push(canvasSize);
	}

	//____ _childRequestRender() ______________________________________________

    void WgCanvasStack::_onRenderRequested()
    {
		m_patches.clear();
		m_patches.push(m_size - _contentPaddingSize());
		WgCapsule::_onRenderRequested();
	}

    void WgCanvasStack::_onRenderRequested(const WgRect& rect)
	{
		m_patches.add(rect - _contentRect().pos());
		WgCapsule::_onRenderRequested(rect);
	}

	//____ _createCanvas() ____________________________________________________

	wg::Surface_p WgCanvasStack::_createCanvas(int canvasIdx, wg::SizeI size)
	{
		if (m_createFunc)
			return m_createFunc(canvasIdx, size);
		else
			return wg::Base::activeContext()->surfaceFactory()->createSurface(size);
	}

	//____ _clearCanvases() ___________________________________________________

	void WgCanvasStack::_clearCanvases(wg::GfxDevice * pDevice)
	{
		wg::BlendMode	oldBlendMode = pDevice->blendMode();
		wg::Surface_p	oldCanvas = pDevice->canvas();


        auto oldClipList = pDevice->clipList();
        auto oldClipListSize = pDevice->clipListSize();

        pDevice->setClipList(m_patches.size(), m_patches.begin());
		pDevice->setBlendMode(wg::BlendMode::Replace);

		if (m_clearFunc)
			m_clearFunc(pDevice, m_canvases);
		else
		{
			for (auto& pCanvas : m_canvases)
			{
				pDevice->setCanvas(pCanvas, false);
				pDevice->fill(wg::Color::Transparent);
			}
		}

		pDevice->setBlendMode(oldBlendMode);
		pDevice->setCanvas(oldCanvas,false);

        pDevice->setClipList(oldClipListSize, oldClipList);
	}

	//____ _combineCanvases() _________________________________________________

	void WgCanvasStack::_combineCanvases(wg::GfxDevice * pDevice)
	{
		wg::BlendMode	oldBlendMode = pDevice->blendMode();
		wg::Surface_p	oldCanvas = pDevice->canvas();

        auto oldClipList = pDevice->clipList();
        auto oldClipListSize = pDevice->clipListSize();

        pDevice->setClipList(m_patches.size(), m_patches.begin());
		pDevice->setBlendMode(wg::BlendMode::Replace);
		pDevice->setCanvas(m_pEndCanvas, false);

		if (m_combineFunc)
			m_combineFunc(pDevice, m_pEndCanvas, m_canvases);
		else
		{
			for (auto& pCanvas : m_canvases)
			{
				pDevice->setBlitSource(pCanvas);
				pDevice->blit({ 0,0 });
			}
		}

		pDevice->setBlendMode(oldBlendMode);
		pDevice->setCanvas(oldCanvas, false);
        pDevice->setClipList(oldClipListSize, oldClipList);
	}


WgCanvasStack::CustomSkin::CustomSkin(WgCanvasStack * pHolder, wg::Skin * pSkin, int canvasIdx)
    : m_pStack(pHolder), m_pSkin(pSkin), m_canvasIdx(canvasIdx)
{
    m_bOpaque = m_pSkin->isOpaque();
    m_bIgnoresFraction = m_pSkin->ignoresFraction();
    m_bContentShifting = m_pSkin->isContentShifting();
}

const wg::TypeInfo& WgCanvasStack::CustomSkin::typeInfo(void) const
{
		return TYPEINFO;
}

wg::Size WgCanvasStack::CustomSkin::minSize() const
{
    return m_pSkin->minSize();
}

wg::Size WgCanvasStack::CustomSkin::preferredSize() const
{
    return m_pSkin->preferredSize();
}

wg::Size WgCanvasStack::CustomSkin::sizeForContent(const wg::Size& contentSize) const
{
    return m_pSkin->sizeForContent(contentSize);
}

wg::Border WgCanvasStack::CustomSkin::contentPadding(wg::State state) const
{
    return m_pSkin->contentPadding(state);
}

wg::Size WgCanvasStack::CustomSkin::contentPaddingSize() const
{
    return m_pSkin->contentPaddingSize();
}

wg::Coord WgCanvasStack::CustomSkin::contentOfs(wg::State state) const
{
    return m_pSkin->contentOfs(state);
}

wg::Rect WgCanvasStack::CustomSkin::contentRect(const wg::Rect& canvas, wg::State state) const
{
    return m_pSkin->contentRect(canvas,state);
}

bool WgCanvasStack::CustomSkin::isOpaque(wg::State state) const
{
    return m_pSkin->isOpaque(state);

}

bool WgCanvasStack::CustomSkin::isOpaque(const wg::Rect& rect, const wg::Size& canvasSize, wg::State state) const
{
    return m_pSkin->isOpaque(rect,canvasSize,state);
}

bool WgCanvasStack::CustomSkin::isStateIdentical(wg::State state, wg::State comparedTo, float fraction) const
{
    return m_pSkin->isStateIdentical(state,comparedTo,fraction);
}

bool WgCanvasStack::CustomSkin::markTest(const wg::Coord& ofs, const wg::Rect& canvas, wg::State state, int opacityTreshold, float fraction) const
{
    return m_pSkin->markTest(ofs, canvas, state, opacityTreshold, fraction);
}

void WgCanvasStack::CustomSkin::render(wg::GfxDevice * pDevice, const wg::Rect& canvas, wg::State state, float fraction) const
{
    WgCanvasStack * pStack = m_pStack.GetRealPtr() ? static_cast<WgCanvasStack*>(m_pStack.GetRealPtr()) : nullptr;


    if (!pStack || !pStack->m_bRendering)
    {
        // Skin rendered from Widget not descendant to CanvasStack.
        // This is ok, we just do no redirection.

        m_pSkin->render(pDevice, canvas, state, fraction);
        return;
    }

    if (m_canvasIdx >= (int) pStack->m_canvases.size())
        return;												// The indexed canvas does not exist. Render nothing.

    auto oldCanvas = pDevice->canvas();
    pDevice->setCanvas(pStack->m_canvases[m_canvasIdx],false);
    m_pSkin->render(pDevice,canvas,state,fraction);
    pDevice->setCanvas(oldCanvas,false);
}

wg::Rect WgCanvasStack::CustomSkin::fractionChangeRect(const wg::Rect& canvas, wg::State state, float oldFraction, float newFraction) const
{
    return m_pSkin->fractionChangeRect(canvas,state,oldFraction,newFraction);
}



WgCanvasStack::CustomCapsule::CustomCapsule(WgCanvasStack * pHolder, int canvasIdx)
    : m_pStack(pHolder), m_canvasIdx(canvasIdx)
{
}


const char * WgCanvasStack::CustomCapsule::Type( void ) const
{
    return c_customCapsuleType;
}

void WgCanvasStack::CustomCapsule::_renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
    WgCanvasStack * pStack = m_pStack.GetRealPtr() ? static_cast<WgCanvasStack*>(m_pStack.GetRealPtr()) : nullptr;

    if (!pStack || !pStack->m_bRendering)
    {
        // Widget not descendant to CanvasStack.
        // This is ok, we just do no redirection.

        WgCapsule::_renderPatches(pDevice, _canvas, _window,_pPatches);
        return;
    }

    if (m_canvasIdx >= (int) pStack->m_canvases.size())
        return;												// The indexed canvas does not exist. Render nothing.

    auto oldCanvas = pDevice->canvas();
    pDevice->setCanvas(pStack->m_canvases[m_canvasIdx], false);
    WgCapsule::_renderPatches(pDevice, _canvas, _window,_pPatches);
    pDevice->setCanvas(oldCanvas, false);
}




