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

#include <wg_rootpanel.h>
#include <wg_base.h>
#include <wg_container.h>
#include <wg_boxskin.h>
#include <wg_inputhandler.h>
#include <wg_util.h>
#include <wg_internal.h>
#include <wg_context.h>

#include <new>


#include <wg_msgrouter.h>


namespace wg
{
	using namespace Util;

	const TypeInfo RootPanel::TYPEINFO = { "RootPanel", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	RootPanel_p	RootPanel::create() 
	{ 
		return RootPanel_p(new RootPanel()); 
	}

	RootPanel_p	RootPanel::create( Surface* pCanvas, GfxDevice* pDevice )
	{ 
		if (!pCanvas)
		{
			//TODO: Error handling!
			return nullptr;
		}

		return RootPanel_p(new RootPanel(pCanvas, pDevice)); 
	}

	RootPanel_p	RootPanel::create( CanvasRef ref, GfxDevice* pDevice )
	{
		if( ref < CanvasRef_min || ref > CanvasRef_max || ref == CanvasRef::None )
		{
			//TODO: Error handling!
			return nullptr;
		}

		if( pDevice == nullptr || pDevice->canvas(ref).size.isEmpty() )
		{
			//TODO: Error handling!
			return nullptr;
		}

		return RootPanel_p(new RootPanel(ref, pDevice));
	}


	//____ Constructor ____________________________________________________________

	RootPanel::RootPanel() : slot(this), m_skin(this)
	{
		m_bVisible = true;
		m_bHasGeo = false;
		m_bScaleSet = false;

		m_bDebugMode = false;

		BoxSkin::Blueprint bp;
		bp.color = HiColor(4096, 0, 0, 2048);
		bp.outline = 1;
		bp.outlineColor = HiColor(4096, 0, 0, 2048);
		
		BoxSkin::StateBP sbp;
		sbp.state = StateEnum::Focused;
		sbp.data.color = HiColor(4096, 0, 0, 2048);
		sbp.data.outlineColor = HiColor(4096, 0, 0, 4096);
		bp.states.push_back(sbp);

		m_pDebugOverlay = BoxSkin::create(bp);
		m_afterglowFrames = 4;
	}

	RootPanel::RootPanel(Surface* pSurface, GfxDevice * pGfxDevice ) : RootPanel()
	{
		m_canvas.ref = CanvasRef::None;
		m_canvas.pSurface = pSurface;
		m_canvas.size = pSurface->pixelSize()*64;
		m_canvas.scale = pSurface->scale();

		m_geo = m_canvas.size;
		m_pGfxDevice = pGfxDevice;
		m_scale = m_canvas.scale;
	}

	RootPanel::RootPanel(CanvasRef ref, GfxDevice* pGfxDevice) : RootPanel()
	{
		m_canvas = pGfxDevice->canvas(ref);

		m_geo = m_canvas.size;
		m_pGfxDevice = pGfxDevice;
		m_scale = m_canvas.scale;
	}


	//____ Destructor _____________________________________________________________

	RootPanel::~RootPanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& RootPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setGfxDevice() _________________________________________________________

	bool RootPanel::setGfxDevice(GfxDevice* pDevice)
	{
		m_pGfxDevice = pDevice;

		m_dirtyPatches.add( m_geo );
		return true;
	}

	//____ setCanvas() ________________________________________________________

	bool RootPanel::setCanvas(Surface* pSurface)
	{
		if (!pSurface)
		{
			return false;
		}

		int oldScale = m_scale;
		SizeSPX oldSize = m_geo.size();

		m_canvas.ref = CanvasRef::None;
		m_canvas.pSurface = pSurface;
		m_canvas.size = pSurface->pixelSize()*64;
		m_canvas.scale = pSurface->scale();

		if (!m_bScaleSet)
			m_scale = m_canvas.scale;

		if (!m_bHasGeo)
			m_geo = m_canvas.size;

		if ((m_scale != oldScale || oldSize != m_geo.size()) && slot._widget())
			OO(slot._widget())->_resize(m_geo.size(),m_scale);

		m_dirtyPatches.add(m_geo);
		return true;
	}

	bool RootPanel::setCanvas(CanvasRef canvasRef)
	{
		if( !m_pGfxDevice || m_pGfxDevice->canvas(canvasRef).size.isEmpty() )
		{
			//TODO: Error handling!
			return false;
		}

		int oldScale = m_scale;
		SizeSPX oldSize = m_geo.size();

		m_canvas = m_pGfxDevice->canvas(canvasRef);

		if (!m_bScaleSet)
			m_scale = m_canvas.scale;

		if (!m_bHasGeo)
			m_geo = m_canvas.size;

		if ((m_scale != oldScale || oldSize != m_geo.size()) && slot._widget())
			OO(slot._widget())->_resize(m_geo.size(), m_scale);

		m_dirtyPatches.add(m_geo);
		return true;
	}

	//____ setCanvasLayers() ___________________________________________________

	void RootPanel::setCanvasLayers( CanvasLayers * pLayers )
	{
		if( pLayers != m_pCanvasLayers )
		{
			m_pCanvasLayers = pLayers;
			m_dirtyPatches.add(m_geo);
		}
	}

	//____ setScale() __________________________________________________________

	bool RootPanel::setScale(int scale)
	{
		if (scale != m_scale)
		{
			m_scale = scale;
			m_bScaleSet = true;
			if (slot._widget())
			{
				OO(slot)._setSize(m_geo.size(),m_scale);
				m_dirtyPatches.add(m_geo);
			}
		}

		return true;
	}

	//____ clearScale() _________________________________________________________

	void RootPanel::clearScale()
	{
		if (m_bScaleSet)
		{
			m_bScaleSet = false;
			m_scale = m_canvas.scale;
			if (slot._widget())
			{
				OO(slot)._setSize(m_geo.size(), m_scale);
				m_dirtyPatches.add(m_geo);
			}
		}
	}


	//_____ setGeo() _____________________________________________________________

	bool RootPanel::setGeo(const Rect& _geo)
	{
		RectSPX geoSpx(m_canvas.size, align(ptsToSpx(_geo,m_scale)));

		m_bHasGeo = !_geo.isEmpty();

		if (m_geo != geoSpx)
		{
			m_geo = geoSpx;
			if (slot._widget())
				OO(slot)._setSize(m_geo.size());
		}

		return true;
	}

	//____ geo() __________________________________________________________________

	Rect RootPanel::geo() const
	{
		if (m_bHasGeo)
			return spxToPts(m_geo,m_scale);
		else
			return spxToPts(m_canvas.size,m_scale);
	}

	//____ _object() ____________________________________________________________

	Object* RootPanel::_object()
	{
		return this;
	}

	const Object* RootPanel::_object() const
	{
		return this;
	}

	//____ setVisible() ___________________________________________________________

	bool RootPanel::setVisible(bool bVisible)
	{
		if (bVisible != m_bVisible)
		{
			m_bVisible = bVisible;
			m_dirtyPatches.add(m_geo);
		}
		return true;
	}

	//____ setSkin() __________________________________________________________

	void RootPanel::setSkin(Skin* pSkin)
	{
		//TODO: Possibly resize child.

		m_skin.set(pSkin);
		m_dirtyPatches.add(m_geo);
	}


	//____ setDebugMode() ______________________________________________________

	void RootPanel::setDebugMode(bool onOff)
	{
		m_bDebugMode = onOff;

		// Make sure to clean up on the screen.

		if (m_bDebugMode == false)
		{
			for (auto it = m_afterglowRects.begin(); it != m_afterglowRects.end(); it++)
				m_dirtyPatches.add(&(*it));

			m_afterglowRects.clear();
		}
	}


	//____ setDebugOverlay() ____________________________________________________

	void RootPanel::setDebugOverlay(Skin* pOverlaySkin)
	{
		m_pDebugOverlay = pOverlaySkin;
	}

	//____ setDebugAfterglow() ____________________________________________________

	void RootPanel::setDebugAfterglow(int frames)
	{
		limit(frames, 0, 100);
		m_afterglowFrames = frames;
	}

	//____ render() _______________________________________________________________

	bool RootPanel::render()
	{
		return render(geo());
	}

	bool RootPanel::render(const Rect& clip)
	{
		if( !beginRender() )
			return false;

		if (!renderSection(clip))
		{
			endRender();
			return false;
		}

		if (!endRender())
			return false;

		return true;
	}

	//____ beginRender() __________________________________________________________

	bool RootPanel::beginRender()
	{
		GfxDevice* pGfxDevice = m_pGfxDevice ? m_pGfxDevice : Base::activeContext()->gfxDevice();

		if( !pGfxDevice || m_canvas.size.isEmpty() || !slot._widget() )
		{
			//TODO: Error handling!
			return false;						// No GFX-device or no widgets to render.
		}

		// Handle preRender calls.

		for (auto& pWidget : m_preRenderCalls)
			OO(pWidget)->_preRender();

		m_preRenderCalls.clear();

		// Handle debug overlays.

		if (m_bDebugMode)
		{
			// Remove from afterglow queue patches that are overlapped by our new dirty patches.

			for (std::deque<PatchesSPX>::iterator it = m_afterglowRects.begin(); it != m_afterglowRects.end(); ++it)
				it->sub(&m_dirtyPatches);

			// Add our new dirty patches to the top of the afterglow queue.

			m_afterglowRects.push_front(PatchesSPX());
			m_afterglowRects.front().add(&m_dirtyPatches);

			// Possibly remove overlays from the back, put them into dirty rects for re-render

			while ((int)m_afterglowRects.size() > m_afterglowFrames + 1)
			{
				m_dirtyPatches.add(&m_afterglowRects.back());
				m_afterglowRects.pop_back();
			}

			// Re-render graphics behind overlays that go from state FOCUSED to NORMAL

			if (m_afterglowRects.size() > 1)
			{
				m_dirtyPatches.add(&m_afterglowRects[1]);
			}
		}

		// Initialize GFX-device.

		return pGfxDevice->beginRender();
	}


	//____ renderSection() __________________________________________________________

	bool RootPanel::renderSection(const Rect& _clip)
	{
		GfxDevice* pGfxDevice = m_pGfxDevice ? m_pGfxDevice : Base::activeContext()->gfxDevice();

		if( !pGfxDevice || m_canvas.size.isEmpty() || !slot._widget() )
		{
			//TODO: Error handling!
			return false;						// Missing GfxDevice or Canvas or widgets to render.
		}
		
		// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)

		RectSPX geo = m_geo;
		RectSPX clip = RectSPX(align(ptsToSpx(_clip,m_scale)), geo);
		if( clip.w == 0 || clip.h == 0 )
			return false;						// Invalid rect area.

		// Nothing to render if we are hidden

		if (!m_bVisible)
			return true;						// Not an error, just hidden.

		// Copy and clip our dirty patches

		PatchesSPX dirtyPatches(m_dirtyPatches, clip);

		// Render the dirty patches recursively

		if (dirtyPatches.size() > 0)
		{
			int nRects = dirtyPatches.size();
			const RectSPX* pRects = dirtyPatches.begin();

			if (m_canvas.ref == CanvasRef::None )
				pGfxDevice->beginCanvasUpdate(m_canvas.pSurface, nRects, pRects, m_pCanvasLayers);
			else
				pGfxDevice->beginCanvasUpdate(m_canvas.ref, nRects, pRects, m_pCanvasLayers);

			m_skin.render(pGfxDevice, geo, m_scale, StateEnum::Normal);

			OO(slot._widget())->_render( pGfxDevice, geo, geo );

			pGfxDevice->endCanvasUpdate();
		}

		// Handle updated rect overlays

		if (m_bDebugMode && m_pDebugOverlay)
		{
			// Set clipping rectangle.

			if (m_canvas.ref == CanvasRef::None )
				pGfxDevice->beginCanvasUpdate(m_canvas.pSurface, 1, &clip, nullptr, 0);
			else
				pGfxDevice->beginCanvasUpdate(m_canvas.ref, 1, &clip, nullptr, 0);

			// Render our new overlays

			for (const RectSPX* pRect = m_afterglowRects[0].begin(); pRect != m_afterglowRects[0].end(); pRect++)
			{
				m_pDebugOverlay->_render(pGfxDevice, *pRect, m_scale, StateEnum::Focused);
			}

			// Render overlays that have turned into afterglow

			if (m_afterglowRects.size() > 1)
			{
				for (const RectSPX* pRect = m_afterglowRects[1].begin(); pRect != m_afterglowRects[1].end(); pRect++)
				{
					m_pDebugOverlay->_render(pGfxDevice, *pRect, m_scale, StateEnum::Normal);
				}
			}

			pGfxDevice->endCanvasUpdate();
		}

		return true;
	}

	//____ endRender() ____________________________________________________________

	bool RootPanel::endRender(void)
	{
		GfxDevice* pGfxDevice = m_pGfxDevice ? m_pGfxDevice : Base::activeContext()->gfxDevice();

		if( !pGfxDevice || m_canvas.size.isEmpty() || !slot._widget() )
		{
			//TODO: Error handling!
			return false;						// No GFX-device or no widgets to render.
		}
		
		// Turn dirty patches into update patches
		//TODO: Optimize by just making a swap.

		m_updatedPatches.clear();
		m_updatedPatches.add(&m_dirtyPatches);
		m_dirtyPatches.clear();

		return pGfxDevice->endRender();
	}


	//____ _findWidget() _____________________________________________________________

	Widget* RootPanel::_findWidget(const CoordSPX& ofs, SearchMode mode)
	{
		if (!m_geo.contains(ofs) || !slot._widget())
			return 0;

		if (slot._widget() && slot._widget()->isContainer())
			return OO(static_cast<Container*>(slot._widget()))->_findWidget(ofs, mode);

		return slot._widget();
	}


	//____ _focusedChild() ______________________________________________________

	Widget* RootPanel::_focusedChild() const
	{
		if (!m_pFocusedChild)
			return slot._widget();

		return m_pFocusedChild;
	}


	//____ _childPos() ________________________________________________________

	CoordSPX RootPanel::_childPos(const StaticSlot* pSlot) const
	{
		return m_geo.pos();
	}

	//____ _childGlobalPos() __________________________________________________

	CoordSPX RootPanel::_childGlobalPos(const StaticSlot* pSlot) const
	{
		return m_geo.pos();
	}

	//____ _isChildVisible() __________________________________________________

	bool RootPanel::_isChildVisible(const StaticSlot* pSlot) const
	{
		return true;
	}

	//____ _childWindowSection() __________________________________________________

	RectSPX RootPanel::_childWindowSection(const StaticSlot* pSlot) const
	{
		return m_geo;
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo& RootPanel::_slotTypeInfo(const StaticSlot* pSlot) const
	{
		return DynamicSlot::TYPEINFO;
	}

	//____ _container() __________________________________________________

	Container* RootPanel::_container()
	{
		return nullptr;
	}

	//____ _root() __________________________________________________

	RootPanel* RootPanel::_root()
	{
		return this;
	}

	//____ _scale() _______________________________________________

	int RootPanel::_scale() const
	{
		return m_scale;
	}

	//____ _childRequestRender() __________________________________________________

	void RootPanel::_childRequestRender(StaticSlot* pSlot)
	{
		if (m_bVisible)
			m_dirtyPatches.add(m_geo);
	}

	void RootPanel::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		if (m_bVisible)
			m_dirtyPatches.add(rect + m_geo.pos());
	}

	//____ _childRequestResize() __________________________________________________

	void RootPanel::_childRequestResize(StaticSlot* pSlot)
	{
		OO(slot)._setSize(m_geo.size());
	}

	//____ _childRequestFocus() __________________________________________________

	bool RootPanel::_childRequestFocus(StaticSlot* pSlot, Widget* pWidget)
	{
		if (pWidget == m_pFocusedChild.rawPtr())
			return true;

		Widget* pOldFocus = m_pFocusedChild.rawPtr();
		m_pFocusedChild = pWidget;
		return Base::inputHandler()->_focusChanged(this, pOldFocus, pWidget);
	}

	//____ _childReleaseFocus() __________________________________________________

	bool RootPanel::_childReleaseFocus(StaticSlot* pSlot, Widget* pWidget)
	{
		if (pWidget != m_pFocusedChild.rawPtr())
			return true;					// Never had focus, although widget seems to believe it.

		if (pWidget == slot._widget())
			return false;

		Widget* pOldFocus = m_pFocusedChild;
		m_pFocusedChild = slot._widget();
		return Base::inputHandler()->_focusChanged(this, pOldFocus, slot._widget());
	}

	//____ _childRequestInView() __________________________________________________

	void RootPanel::_childRequestInView(StaticSlot* pSlot)
	{
		// Do nothing, root ignores inView requests.
	}
	void RootPanel::_childRequestInView(StaticSlot* pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea)
	{
		// Do nothing, root ignores inView requests.
	}

	//____ _prevChild() __________________________________________________

	Widget* RootPanel::_prevChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	//____ _nextChild() __________________________________________________

	Widget* RootPanel::_nextChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void RootPanel::_releaseChild(StaticSlot* pSlot)
	{
		_replaceChild(pSlot, nullptr);
	}

	//____ _replaceChild() ____________________________________________________________

	void RootPanel::_replaceChild(StaticSlot* pSlot, Widget* pNewWidget)
	{
		RectSPX myGeo = m_geo;

		if (slot._widget())
			OO(slot._widget())->_collectPatches(m_dirtyPatches, myGeo, myGeo);

		OO(slot)._setWidget(pNewWidget);

		if (pNewWidget)
		{
			OO(pNewWidget)->_resize(myGeo.size(),m_scale);
			OO(pNewWidget)->_collectPatches(m_dirtyPatches, myGeo, myGeo);
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void RootPanel::_didAddSlots(StaticSlot* pSlot, int nb)
	{
		return;				// RootPanel doesn't support adding/moving/erasing slots
	}

	//____ _didMoveSlots() ____________________________________________________

	void RootPanel::_didMoveSlots(StaticSlot* pFrom, StaticSlot* pTo, int nb)
	{
		return;				// RootPanel doesn't support adding/moving/erasing slots
	}

	//____ _willEraseSlots() __________________________________________________

	void RootPanel::_willEraseSlots(StaticSlot* pSlot, int nb)
	{
		return;				// RootPanel doesn't support adding/moving/erasing slots
	}

	//____ _skinValue() _______________________________________________________

	float RootPanel::_skinValue(const SkinSlot* pSlot) const
	{
		return 1.f;
	}

	//____ _skinValue2() ______________________________________________________

	float RootPanel::_skinValue2(const SkinSlot* pSlot) const
	{
		return -1.f;
	}

	//____ _skinState() _______________________________________________________

	State RootPanel::_skinState(const SkinSlot* pSlot) const
	{
		return StateEnum::Normal;
	}

	//____ _skinSize() ________________________________________________________

	SizeSPX RootPanel::_skinSize(const SkinSlot* pSlot) const
	{
		return m_geo;
	}

	//____ _skinRequestRender() _______________________________________________

	void RootPanel::_skinRequestRender(const SkinSlot* pSlot)
	{
		m_dirtyPatches.clear();
		m_dirtyPatches.push(m_geo);
	}

	void RootPanel::_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect)
	{
		m_dirtyPatches.push(rect);
	}


} // namespace wg
