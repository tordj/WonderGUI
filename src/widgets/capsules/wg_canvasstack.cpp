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
#include <wg_context.h>

namespace wg
{
	using namespace Util;

	const TypeInfo CanvasStack::TYPEINFO = { "CanvasStack", &Capsule::TYPEINFO };
	const TypeInfo CanvasStack::CustomSkin::TYPEINFO = { "CanvasStack::CustomSkin", &Skin::TYPEINFO };
	const TypeInfo CanvasStack::CustomCapsule::TYPEINFO = { "CanvasStack::CustomCapsule", &Capsule::TYPEINFO };

	//____ constructor ____________________________________________________________

	CanvasStack::CanvasStack()
	{
	}

	//____ Destructor _____________________________________________________________

	CanvasStack::~CanvasStack()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CanvasStack::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPreferredSize() _________________________________________________

	void CanvasStack::setPreferredSize(Size preferred)
	{
		if (preferred != m_preferred)
		{
			m_preferred = preferred;
			_requestResize();
		}
	}

	//____ preferredSize() ____________________________________________________

	Size CanvasStack::preferredSize() const
	{
		return m_preferred.isEmpty() ? Capsule::preferredSize() : m_preferred;
	}

	//____ matchingWidth() ____________________________________________________

	MU CanvasStack::matchingWidth(MU height) const
	{
		return m_preferred.isEmpty() ? Capsule::matchingWidth(height) : m_preferred.w;
	}

	//____ matchingHeight() ___________________________________________________

	MU CanvasStack::matchingHeight(MU width) const
	{
		return m_preferred.isEmpty() ? Capsule::matchingHeight(width) : m_preferred.h;
	}


	//____ setCanvases() ______________________________________________________

	void CanvasStack::setCanvases(int nbCanvases)
	{
		// Error checks and early outs

		if (nbCanvases < 0)
		{
			//TODO: Error handling!
			return;
		}

		_regenCanvases(nbCanvases, m_size - _contentPaddingSize());
	}

	//____ canvas() ___________________________________________________________

	Surface_p CanvasStack::canvas(int idx) const
	{
		if (idx < 0 || idx >= (int)m_canvases.size())
		{
			//TODO: Error handling
			return nullptr;
		}

		return m_canvases[idx];
	}

	//____ forceRedraw() ___________________________________________________________

	void CanvasStack::forceRedraw()
	{
		m_patches.clear();
		m_patches.push(m_size - _contentPaddingSize());
		_requestRender();
	}

	//____ setCreateFunc() ____________________________________________________

	void CanvasStack::setCreateFunc(const std::function<Surface_p(int, SizeI)>& func)
	{
		m_createFunc = func;
	}

	//____ setClearFunc() _____________________________________________________

	void CanvasStack::setClearFunc(const std::function<void(GfxDevice*, std::vector<Surface_p>&)>& func)
	{
		m_clearFunc = func;
	}

	//____ setCombineFunc() ___________________________________________________

	void CanvasStack::setCombineFunc(const std::function<void(GfxDevice*, Surface*, std::vector<Surface_p>&)>& func)
	{
		m_combineFunc = func;
	}

	//____ redirectSkin() _____________________________________________________

	Skin_p CanvasStack::redirectSkin(Skin * pSkin, int canvasIdx)
	{
		return new CustomSkin(this, pSkin, canvasIdx);
	}

	//____ redirectWidget() ___________________________________________________

	Capsule_p CanvasStack::redirectWidget(Widget * pWidget, int canvasIdx)
	{
		Capsule_p p = new CustomCapsule(this, canvasIdx);
		p->slot.setWidget(pWidget);
		return p;
	}

	//____ _render() __________________________________________________________

	void CanvasStack::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window)
	{
		m_bRendering = true;

		if (m_pSkin)
			m_pSkin->render(pDevice, _canvas, m_state);

		if (m_canvases.size() == 0)
		{
			Capsule::_render(pDevice, _canvas, _window);
		}
		else
		{
			auto clipPop = Util::patchesToClipList(pDevice, m_patches);

			_clearCanvases(pDevice);
			Rect canvasRect(0,0, _canvas.size() - _contentPaddingSize());

			auto pOldCanvas = pDevice->canvas();
			pDevice->setCanvas(m_canvases[0],false);
			Capsule::_render(pDevice, canvasRect, canvasRect);
			pDevice->setCanvas(pOldCanvas,false);

			_combineCanvases(pDevice);

			Util::popClipList(pDevice,clipPop);

			pDevice->setBlitSource(m_pEndCanvas);
			pDevice->blit(_canvas.pos(), _canvas.size() );

			m_patches.clear();
		}
		m_bRendering = false;
	}

	//____ _cloneContent() ____________________________________________________

	void CanvasStack::_cloneContent(const Widget * _pOrg)
	{

	}

	//____ _resize() __________________________________________________________

	void CanvasStack::_resize(const Size& size)
	{
		if (size != m_size)
		{
			_regenCanvases(m_canvases.size(), size - _contentPaddingSize());
			Capsule::_resize(size);
		}
	}

	//____ _regenCanvases() ___________________________________________________

	void CanvasStack::_regenCanvases( int nbCanvases, SizeI canvasSize )
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
			m_pEndCanvas = Base::activeContext()->surfaceFactory()->createSurface(canvasSize);

		// Request render

		m_patches.clear();
		m_patches.push(canvasSize);
	}

	//____ _childRequestRender() ______________________________________________

	void CanvasStack::_childRequestRender(StaticSlot * pSlot)
	{
		m_patches.clear();
		m_patches.push(m_size - _contentPaddingSize());
		Capsule::_childRequestRender(pSlot);
	}

	void CanvasStack::_childRequestRender(StaticSlot * pSlot, const Rect& rect)
	{
		m_patches.add(rect - _contentRect().pos());
		Capsule::_childRequestRender(pSlot, rect);
	}

	//____ _createCanvas() ____________________________________________________

	Surface_p CanvasStack::_createCanvas(int canvasIdx, SizeI size)
	{
		if (m_createFunc)
			return m_createFunc(canvasIdx, size);
		else
			return Base::activeContext()->surfaceFactory()->createSurface(size);
	}

	//____ _clearCanvases() ___________________________________________________

	void CanvasStack::_clearCanvases(GfxDevice * pDevice)
	{
		BlendMode	oldBlendMode = pDevice->blendMode();
		Surface_p	oldCanvas = pDevice->canvas();

		auto clipPopData = Util::patchesToClipList(pDevice, m_patches);

		pDevice->setBlendMode(BlendMode::Replace);

		if (m_clearFunc)
			m_clearFunc(pDevice, m_canvases);
		else
		{
			for (auto& pCanvas : m_canvases)
			{
				pDevice->setCanvas(pCanvas, false);
				pDevice->fill(Color::Transparent);
			}
		}

		pDevice->setBlendMode(oldBlendMode);
		pDevice->setCanvas(oldCanvas,false);

		Util::popClipList(pDevice, clipPopData);
	}

	//____ _combineCanvases() _________________________________________________

	void CanvasStack::_combineCanvases(GfxDevice * pDevice)
	{
		BlendMode	oldBlendMode = pDevice->blendMode();
		Surface_p	oldCanvas = pDevice->canvas();

		auto clipPopData = Util::patchesToClipList(pDevice, m_patches);
		pDevice->setBlendMode(BlendMode::Replace);
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
		Util::popClipList(pDevice, clipPopData);
	}


	CanvasStack::CustomSkin::CustomSkin(CanvasStack * pHolder, Skin * pSkin, int canvasIdx)
		: m_pStack(pHolder), m_pSkin(pSkin), m_canvasIdx(canvasIdx)
	{
	}

	const TypeInfo& CanvasStack::CustomSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	Size CanvasStack::CustomSkin::minSize() const
	{
		return m_pSkin->minSize();
	}

	Size CanvasStack::CustomSkin::preferredSize() const
	{
		return m_pSkin->preferredSize();
	}

	Size CanvasStack::CustomSkin::sizeForContent(const Size& contentSize) const
	{
		return m_pSkin->sizeForContent(contentSize);
	}

	Border CanvasStack::CustomSkin::contentPadding() const
	{
		return m_pSkin->contentPadding();
	}

	Size CanvasStack::CustomSkin::contentPaddingSize() const
	{
		return m_pSkin->contentPaddingSize();
	}

	Coord CanvasStack::CustomSkin::contentOfs(State state) const
	{
		return m_pSkin->contentOfs(state);
	}

	Rect CanvasStack::CustomSkin::contentRect(const Rect& canvas, State state) const
	{
		return m_pSkin->contentRect(canvas,state);
	}

	bool CanvasStack::CustomSkin::isOpaque() const
	{
		return m_pSkin->isOpaque();
	}

	bool CanvasStack::CustomSkin::isOpaque(State state) const
	{
		return m_pSkin->isOpaque(state);

	}

	bool CanvasStack::CustomSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return m_pSkin->isOpaque(rect,canvasSize,state);
	}

	bool CanvasStack::CustomSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		return m_pSkin->isStateIdentical(state,comparedTo,fraction);
	}

	bool CanvasStack::CustomSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		return m_pSkin->markTest(ofs, canvas, state, opacityTreshold, fraction);
	}

	void CanvasStack::CustomSkin::render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction) const
	{
		if (!m_pStack || !m_pStack->m_bRendering)
		{
			// Skin rendered from Widget not descendant to CanvasStack.
			// This is ok, we just do no redirection.

			m_pSkin->render(pDevice, canvas, state, fraction);
			return;
		}

		if (m_canvasIdx >= (int) m_pStack->m_canvases.size())
			return;												// The indexed canvas does not exist. Render nothing.

		auto oldCanvas = pDevice->canvas();
		pDevice->setCanvas(m_pStack->m_canvases[m_canvasIdx],false);
		m_pSkin->render(pDevice,canvas,state,fraction);
		pDevice->setCanvas(oldCanvas,false);
	}

	bool CanvasStack::CustomSkin::ignoresFraction() const
	{
		return m_pSkin->ignoresFraction();
	}

	Rect CanvasStack::CustomSkin::fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const
	{
		return m_pSkin->fractionChangeRect(canvas,state,oldFraction,newFraction);
	}


	const TypeInfo&	CanvasStack::CustomCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	CanvasStack::CustomCapsule::CustomCapsule(CanvasStack * pHolder, int canvasIdx)
		: m_pStack(pHolder), m_canvasIdx(canvasIdx)
	{
	}

	void CanvasStack::CustomCapsule::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window)
	{
		if (!m_pStack || !m_pStack->m_bRendering)
		{
			// Widget not descendant to CanvasStack.
			// This is ok, we just do no redirection.

			Capsule::_render(pDevice, _canvas, _window);
			return;
		}

		if (m_canvasIdx >= (int) m_pStack->m_canvases.size())
			return;												// The indexed canvas does not exist. Render nothing.

		auto oldCanvas = pDevice->canvas();
		pDevice->setCanvas(m_pStack->m_canvases[m_canvasIdx], false);
		Capsule::_render(pDevice, _canvas, _window);
		pDevice->setCanvas(oldCanvas, false);
	}




} // namespace wg
