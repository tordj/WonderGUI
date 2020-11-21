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

	RootPanel_p	RootPanel::create( const SizeI& pixelSize, GfxDevice* pDevice )
	{ 
		return RootPanel_p(new RootPanel(pixelSize, pDevice)); 
	}


	//____ constructor ____________________________________________________________

	RootPanel::RootPanel() : slot(this), skin(this)
	{
		m_bVisible = true;
		m_bHasGeo = false;

		m_bDebugMode = false;

		BoxSkin_p pDebugOverlay = BoxSkin::create(1, Color(255, 0, 0, 128), Color(255, 0, 0, 128));
		pDebugOverlay->setColors(StateEnum::Focused, Color(255, 0, 0, 128), Color(255, 0, 0, 255));
		m_pDebugOverlay = pDebugOverlay;
		m_afterglowFrames = 4;
	}

	RootPanel::RootPanel(Surface* pCanvas, GfxDevice * pGfxDevice ) : RootPanel()
	{
		m_geo = pCanvas->size();
		m_canvasSize = pCanvas->size();
		m_pGfxDevice = pGfxDevice;
		m_pCanvas = pCanvas;
	}

	RootPanel::RootPanel(const SizeI& pixelSize, GfxDevice* pGfxDevice) : RootPanel()
	{
		m_geo = pixelSize;
		m_canvasSize = pixelSize;
		m_pGfxDevice = pGfxDevice;
		m_pCanvas = nullptr;
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

		m_dirtyPatches.add( geo() );
		return true;
	}

	//____ setCanvas() ________________________________________________________

	bool RootPanel::setCanvas(Surface* pCanvas)
	{
		if (!pCanvas)
		{
			return false;
		}

		m_pCanvas = pCanvas;
		m_canvasSize = pCanvas->size();

		if (!m_bHasGeo && slot._widget())
			OO(slot._widget())->_resize(Size::fromPX(pCanvas->size()));

		m_dirtyPatches.add(geo());
		return true;
	}

	bool RootPanel::setCanvas(const SizeI& pixelSize)
	{
		m_pCanvas = nullptr;
		m_canvasSize = pixelSize;

		if (!m_bHasGeo && slot._widget())
			OO(slot._widget())->_resize(Size::fromPX(pixelSize));

		m_dirtyPatches.add(geo());
		return true;
	}


	//_____ setGeo() _____________________________________________________________

	bool RootPanel::setGeo(const Rect& _geo)
	{
		if (m_geo == _geo)
			return true;

		m_geo = _geo.aligned();

		m_bHasGeo = !_geo.isEmpty();

		if (slot._widget())
			OO(slot)._setSize(geo().size());

		return true;
	}

	//____ geo() __________________________________________________________________

	Rect RootPanel::geo() const
	{
		if (m_bHasGeo)
			return m_geo;
		else
			return Size::fromPX(m_canvasSize);
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
			m_dirtyPatches.add(geo());
		}
		return true;
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
		if( !m_pGfxDevice || (!m_pCanvas && m_canvasSize.isEmpty()) || !slot._widget() )
			return false;						// No GFX-device or no widgets to render.

		// Handle preRender calls.

		for (auto& pWidget : m_preRenderCalls)
			OO(pWidget)->_preRender();

		m_preRenderCalls.clear();

		// Handle debug overlays.

		if (m_bDebugMode)
		{
			// Remove from afterglow queue patches that are overlapped by our new dirty patches.

			for (std::deque<Patches>::iterator it = m_afterglowRects.begin(); it != m_afterglowRects.end(); ++it)
				it->sub(&m_dirtyPatches);

			// Add our new dirty patches to the top of the afterglow queue.

			m_afterglowRects.push_front(Patches());
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

		return m_pGfxDevice->beginRender();
	}


	//____ renderSection() __________________________________________________________

	bool RootPanel::renderSection(const Rect& _clip)
	{
		if( !m_pGfxDevice || (!m_pCanvas && m_canvasSize.isEmpty()) || !slot._widget() )
			return false;						// Missing GfxDevice or Canvas or widgets to render.

		// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)

		Rect geo = this->geo();
		Rect clip = Rect(_clip, geo).aligned();
		if( clip.w == 0 || clip.h == 0 )
			return false;						// Invalid rect area.

		// Nothing to render if we are hidden

		if (!m_bVisible)
			return true;						// Not an error, just hidden.

		// Copy and clip our dirty patches

		Patches dirtyPatches(m_dirtyPatches, clip);

		// Render the dirty patches recursively

		if (dirtyPatches.size() > 0)
		{
			int nRects = dirtyPatches.size();
			const Rect* pRects = dirtyPatches.begin();

			int allocSize = nRects * sizeof(RectI);

			RectI* pNewRects = (RectI*)Base::memStackAlloc(allocSize);

			for (int i = 0; i < nRects; i++)
				pNewRects[i] = pRects[i].px();

			if (m_pCanvas)
				m_pGfxDevice->beginCanvasUpdate(m_pCanvas, nRects, pNewRects);
			else
				m_pGfxDevice->beginCanvasUpdate(m_canvasSize, nRects, pNewRects);

			OO(skin)._render(m_pGfxDevice, geo, StateEnum::Normal);

			OO(slot._widget())->_render( m_pGfxDevice, geo, geo );

			m_pGfxDevice->endCanvasUpdate();

			Base::memStackRelease(allocSize);
		}

		// Handle updated rect overlays

		if (m_bDebugMode && m_pDebugOverlay)
		{
			// Set clipping rectangle.

			RectI myClip = clip.px();

			if (m_pCanvas)
				m_pGfxDevice->beginCanvasUpdate(m_pCanvas, 1, &myClip, 0);
			else
				m_pGfxDevice->beginCanvasUpdate(m_canvasSize, 1, &myClip, 0);

			// Render our new overlays

			for (const Rect* pRect = m_afterglowRects[0].begin(); pRect != m_afterglowRects[0].end(); pRect++)
			{
				m_pDebugOverlay->render(m_pGfxDevice, *pRect, StateEnum::Focused);
			}

			// Render overlays that have turned into afterglow

			if (m_afterglowRects.size() > 1)
			{
				for (const Rect* pRect = m_afterglowRects[1].begin(); pRect != m_afterglowRects[1].end(); pRect++)
				{
					m_pDebugOverlay->render(m_pGfxDevice, *pRect, StateEnum::Normal);
				}
			}

			m_pGfxDevice->endCanvasUpdate();
		}

		return true;
	}

	//____ endRender() ____________________________________________________________

	bool RootPanel::endRender(void)
	{
		if( !m_pGfxDevice || (!m_pCanvas && m_canvasSize.isEmpty()) || !slot._widget() )
			return false;						// No GFX-device or no widgets to render.

		// Turn dirty patches into update patches
		//TODO: Optimize by just making a swap.

		m_updatedPatches.clear();
		m_updatedPatches.add(&m_dirtyPatches);
		m_dirtyPatches.clear();

		return m_pGfxDevice->endRender();
	}


	//____ _findWidget() _____________________________________________________________

	Widget* RootPanel::_findWidget(const Coord& ofs, SearchMode mode)
	{
		if (!geo().contains(ofs) || !slot._widget())
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

	Coord RootPanel::_childPos(const StaticSlot* pSlot) const
	{
		return geo().pos();
	}

	//____ _childGlobalPos() __________________________________________________

	Coord RootPanel::_childGlobalPos(const StaticSlot* pSlot) const
	{
		return geo().pos();
	}

	//____ _isChildVisible() __________________________________________________

	bool RootPanel::_isChildVisible(const StaticSlot* pSlot) const
	{
		return true;
	}

	//____ _childWindowSection() __________________________________________________

	Rect RootPanel::_childWindowSection(const StaticSlot* pSlot) const
	{
		return geo();
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

	//____ _childRequestRender() __________________________________________________

	void RootPanel::_childRequestRender(StaticSlot* pSlot)
	{
		if (m_bVisible)
			m_dirtyPatches.add(geo());
	}

	void RootPanel::_childRequestRender(StaticSlot* pSlot, const Rect& rect)
	{
		if (m_bVisible)
			m_dirtyPatches.add(Rect(geo().pos() + rect.pos(), rect.size()));
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
	void RootPanel::_childRequestInView(StaticSlot* pSlot, const Rect& mustHaveArea, const Rect& niceToHaveArea)
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
		Rect myGeo = geo();

		if (slot._widget())
			OO(slot._widget())->_collectPatches(m_dirtyPatches, myGeo, myGeo);

		OO(slot)._setWidget(pNewWidget);

		if (pNewWidget)
		{
			OO(pNewWidget)->_resize(myGeo.size());
			OO(pNewWidget)->_collectPatches(m_dirtyPatches, myGeo, myGeo);
		}
	}

	//____ _selectSlots() _____________________________________________________

	void RootPanel::_selectSlots(StaticSlot* pSlot, int nb)
	{
		return;				// RootPanel doesn't support selection
	}

	//____ _unselectSlots() ___________________________________________________

	void RootPanel::_unselectSlots(StaticSlot* pSlot, int nb)
	{
		return;				// RootPanel doesn't support selection
	}

	//____ _repadSlots() ______________________________________________________

	void RootPanel::_repadSlots(StaticSlot* pSlot, int nb, Border padding)
	{
		return;				// RootPanel doesn't support padding
	}

	void RootPanel::_repadSlots(StaticSlot* pSlot, int nb, const Border* pPadding)
	{
		return;				// RootPanel doesn't support padding
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

	//____ _hideSlots() _______________________________________________________

	void RootPanel::_hideSlots(StaticSlot* pSlot, int nb)
	{
		return;				// RootPanel doesn't support hiding
	}

	//____ _unhideSlots() _____________________________________________________

	void RootPanel::_unhideSlots(StaticSlot* pSlot, int nb)
	{
		return;				// RootPanel doesn't support hiding
	}

	//____ _componentState() __________________________________________________

	State RootPanel::_componentState(const GeoComponent* pComponent) const
	{
		return StateEnum::Normal;
	}

	//____ _componentPos() ____________________________________________________

	Coord RootPanel::_componentPos(const GeoComponent* pComponent) const
	{
		return Coord();
	}

	//____ _componentSize() ___________________________________________________

	Size RootPanel::_componentSize(const GeoComponent* pComponent) const
	{
		return m_geo;
	}

	//____ _componentGeo() ____________________________________________________

	Rect RootPanel::_componentGeo(const GeoComponent* pComponent) const
	{
		return m_geo;
	}

	//____ _globalComponentPos() ______________________________________________

	Coord RootPanel::_globalComponentPos(const GeoComponent* pComponent) const
	{
		return m_geo;
	}

	//____ _globalComponentGeo() ______________________________________________

	Rect RootPanel::_globalComponentGeo(const GeoComponent* pComponent) const
	{
		return m_geo;
	}

	//____ _componentRequestRender() __________________________________________

	void RootPanel::_componentRequestRender(const GeoComponent* pComponent)
	{
		m_dirtyPatches.clear();
		m_dirtyPatches.push(m_geo);
	}

	void RootPanel::_componentRequestRender(const GeoComponent* pComponent, const Rect& rect)
	{
		m_dirtyPatches.push(rect);
	}

	//____ _componentRequestResize() __________________________________________

	void RootPanel::_componentRequestResize(const GeoComponent* pComponent)
	{
	}

	//____ _componentRequestFocus() __________________________________________

	void RootPanel::_componentRequestFocus(const GeoComponent* pComponent)
	{
	}

	//____ _componentRequestInView() __________________________________________

	void RootPanel::_componentRequestInView(const GeoComponent* pComponent)
	{
	}

	void RootPanel::_componentRequestInView(const GeoComponent* pComponent, const Rect& mustHave, const Rect& niceToHave)
	{
	}

	//____ _retrieveComponentNotif() __________________________________________

	void RootPanel::_receiveComponentNotif(GeoComponent* pComponent, ComponentNotif notification, int value, void* pData)
	{
	}

	//____ _skinChanged() _____________________________________________________

	void RootPanel::_skinChanged(const CSkinSlot* pSlot, Skin* pNewSkin, Skin* pOldSkin)
	{
		m_dirtyPatches.clear();
		m_dirtyPatches.push(m_geo);
	}

	//____ _skinValue() _______________________________________________________

	float RootPanel::_skinValue(const CSkinSlot* pSlot) const
	{
		return 1.f;
	}

	//____ _skinValue2() ______________________________________________________

	float RootPanel::_skinValue2(const CSkinSlot* pSlot) const
	{
		return -1.f;
	}


} // namespace wg
