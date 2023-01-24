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

#include <wg_cabi_base.h>
#include <wg_cabi_root.h>
#include <wg_cabi_root_incalls.h>
#include <wg_cabi_gfxdevice.h>
#include <wg_container.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_inputhandler.h>


namespace wg
{
	using namespace Util;

	const TypeInfo CABIRoot::TYPEINFO = { "CABIRoot", &Widget::TYPEINFO };

	//____ create() _______________________________________________________________

	CABIRoot_p CABIRoot::create()
	{
		return CABIRoot_p(new CABIRoot());
	}

	CABIRoot_p CABIRoot::create(wg_cabi_root_outcalls* pBridge)
	{
		return CABIRoot_p(new CABIRoot(pBridge));
	}


	//____ constructor ____________________________________________________________

	CABIRoot::CABIRoot() : slot(this)
	{
		std::memset(&m_cabi, 0, sizeof(m_cabi));
	}

	CABIRoot::CABIRoot(wg_cabi_root_outcalls* pBridge) : slot(this)
	{
		m_cabi = *pBridge;

		wg_cabi_root_incalls incalls = makeCABI_root_incalls(this);
		m_cabi.connect(m_cabi.hostCapsule, &incalls);
	}

	//____ Destructor _____________________________________________________________

	CABIRoot::~CABIRoot()
	{
		if (m_cabi.hostCapsule)
			m_cabi.disconnect(m_cabi.hostCapsule);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABIRoot::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//___ findWidget() ________________________________________________________

	Widget_p CABIRoot::findWidget(const Coord& _ofs, SearchMode mode)
	{
		CoordSPX ofs = Util::ptsToSpx(_ofs, m_scale);


		if (!(m_size.contains(ofs) || !slot._widget()))
			return nullptr;

		if (slot._widget() && slot._widget()->isContainer())
			return static_cast<Container*>(slot._widget())->_findWidget(ofs, mode);

		if (mode == SearchMode::Geometry || slot._widget()->_markTest(ofs))
			return slot._widget();

		return nullptr;
	}

	//____ addDirtyPatch() ____________________________________________________

	void CABIRoot::addDirtyPatch(const RectSPX& rect)
	{
		m_cabi.requestRender(m_cabi.hostCapsule, { rect.x,rect.y,rect.w,rect.h });
	}

	//____ addPreRenderCall() _________________________________________________

	bool CABIRoot::addPreRenderCall(Widget* pWidget)
	{
		bool canPreRender;

		if (m_preRenderCalls.empty())
			canPreRender = m_cabi.requestPreRenderCall(m_cabi.hostCapsule);
		else
			canPreRender = true;

		if (canPreRender)
			m_preRenderCalls.push_back(pWidget);

		return canPreRender;
	}


	//____ scale() ____________________________________________________________

	int CABIRoot::scale() const
	{
		return m_scale;
	}

	//____ geo() ______________________________________________________________

	Rect CABIRoot::geo() const
	{
		return Util::spxToPts(m_size,m_scale);
	}

	//____ _matchingHeight() __________________________________________________

	spx CABIRoot::_matchingHeight(spx width, int scale) const
	{
		if (slot.isEmpty())
			return 0;
		else
			return slot.widget()->_matchingHeight(width, scale);
	}

	//____ _matchingWidth() ___________________________________________________

	spx CABIRoot::_matchingWidth(spx height, int scale) const
	{
		if (slot.isEmpty())
			return 0;
		else
			return slot.widget()->_matchingWidth(height, scale);
	}

	//____ _defaultSize() _____________________________________________________

	SizeSPX CABIRoot::_defaultSize(int scale) const
	{
		if (slot.isEmpty())
			return SizeSPX();
		else
			return slot.widget()->_defaultSize(scale);
	}

	//____ _minSize() _________________________________________________________

	SizeSPX CABIRoot::_minSize(int scale) const
	{
		if (slot.isEmpty())
			return SizeSPX();
		else
			return slot.widget()->_minSize(scale);
	}

	//____ _maxSize() _________________________________________________________

	SizeSPX CABIRoot::_maxSize(int scale) const
	{
		if (slot.isEmpty())
			return SizeSPX();
		else
			return slot.widget()->_maxSize(scale);
	}

	//____ _markTest() ________________________________________________________

	bool CABIRoot::_markTest(const CoordSPX& ofs)
	{
		//TODO: Support skin with padding.

		return true;
	}

	//____ _preRender() _______________________________________________________

	void CABIRoot::_preRender()
	{
		for (auto& pWidget : m_preRenderCalls)
			pWidget->_preRender();

		m_preRenderCalls.clear();
	}

	//____ _render() __________________________________________________________

	void CABIRoot::_render(wg_obj device, const RectSPX& _canvas, const RectSPX& _window)
	{
		if (slot.isEmpty())
			return;

		wg_obj hFactory = CABI::gfxDevice->surfaceFactory(device);

		auto pFactory = CABISurfaceFactory::create(hFactory);
		auto pDevice = CABIGfxDevice::create(device, pFactory);

		slot._widget()->_render(pDevice, { _canvas.x, _canvas.y, _canvas.w, _canvas.h }, { _window.x, _window.y, _window.w, _window.h });
	}


	//____ _resize() __________________________________________________________

	void CABIRoot::_resize(const SizeSPX& size, int scale)
	{
		m_size = size;

		if (scale >= 0)
			m_scale = scale;

		if (!slot.isEmpty())
			slot._widget()->_resize(size, scale);
	}

	//____ _setState() ________________________________________________________

	void CABIRoot::_setState(State state)
	{
		State oldState = m_state;
		m_state = state;

		if (!slot.isEmpty())
		{
			if (oldState.isEnabled() != state.isEnabled())
				slot._widget()->setEnabled(state.isEnabled());

			if (oldState.isSelected() != state.isSelected())
			{
				bool bSelected = state.isSelected();

				State s = slot._widget()->state();
				s.setSelected(bSelected);
				slot._widget()->_setState(s);
			}
		}

		if( state.isFocused() != oldState.isFocused() )
		{
			if( m_pFocusedChild )
			{
				State s = m_pFocusedChild->state();
				s.setFocused(!s.isFocused());
				m_pFocusedChild->_setState(s);
				
				if( !state.isFocused() )
					m_pFocusedChild = nullptr;
			}
			
			Base::inputHandler()->setFocusedWindow(state.isFocused()?this:nullptr);
			
		}

	}

	//____ _receive() _________________________________________________________

	void CABIRoot::_receive(Msg* pMsg)
	{
		//TODO: Implement!!!
	}

	//____ _pointerStyle() ____________________________________________________

	PointerStyle CABIRoot::_pointerStyle() const
	{
		//TODO: Implement!!!

		return PointerStyle::Default;
	}

	//____ _setPointerPos() ___________________________________________________

	void CABIRoot::_setPointerPos( CoordSPX pos, int64_t timestamp )
	{
		Base::inputHandler()->setPointer(this, Util::spxToPts(pos, m_scale), timestamp );
	}

	//____ _setButtonState() __________________________________________________

	void CABIRoot::_setButtonState( int button, bool bPressed, int64_t timestamp )
	{
		Base::inputHandler()->setButton((MouseButton) button, bPressed, timestamp );
	}

	//____ _setKeyState() __________________________________________________

	void CABIRoot::_setKeyState( int nativeKeyCode, bool bPressed, int64_t timestamp )
	{
		Base::inputHandler()->setKey(nativeKeyCode, bPressed, timestamp );
	}

	//____ _putText() ____________________________________________________________

	void CABIRoot::_putText( const char * pUTF8Text )
	{
		Base::inputHandler()->putText(pUTF8Text);
	}

	//____ _update() __________________________________________________________

	void CABIRoot::_update(int microPassed, int64_t microsecTimestamp)
	{
		Base::msgRouter()->dispatch();
		Base::update(microsecTimestamp);
	}

	//____ _slotTypeInfo() ____________________________________________________

	const TypeInfo& CABIRoot::_slotTypeInfo(const StaticSlot* pSlot) const
	{
		return DynamicSlot::TYPEINFO;
	}

	//____ _container() _______________________________________________________

	Container* CABIRoot::_container()
	{
		return nullptr;
	}

	//____ _root() ____________________________________________________________

	Root* CABIRoot::_root()
	{
		return this;
	}

	//____ _scale() ___________________________________________________________

	int CABIRoot::_scale() const
	{
		return m_scale;
	}

	//____ _child() ___________________________________________________________
	
	Widget*	CABIRoot::_child() const
	{
		return slot._widget();
	}

	//____ _focusedChild() ____________________________________________________

	Widget* CABIRoot::_focusedChild() const
	{
		return m_pFocusedChild.rawPtr();
	}


	//____ _childPos() ________________________________________________________

	CoordSPX CABIRoot::_childPos(const StaticSlot* pSlot) const
	{
		return CoordSPX();
	}

	//____ _childGlobalPos() __________________________________________________

	CoordSPX CABIRoot::_childGlobalPos(const StaticSlot* pSlot) const
	{
		return CoordSPX();
	}

	//____ _isChildVisible() __________________________________________________

	bool CABIRoot::_isChildVisible(const StaticSlot* pSlot) const
	{
		return true;
	}

	//____ _childWindowSection() ______________________________________________

	RectSPX CABIRoot::_childWindowSection(const StaticSlot* pSlot) const
	{
		return m_size;
	}

	//____ _childRequestRender() ______________________________________________

	void CABIRoot::_childRequestRender(StaticSlot* pSlot)
	{
		if (m_cabi.hostCapsule)
			m_cabi.requestRender(m_cabi.hostCapsule, { 0,0,m_size.w,m_size.h });
	}

	void CABIRoot::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		if (m_cabi.hostCapsule)
			m_cabi.requestRender(m_cabi.hostCapsule, { rect.x,rect.y,rect.w,rect.h });
	}

	//____ _childRequestResize() ______________________________________________
	void CABIRoot::_childRequestResize(StaticSlot* pSlot)
	{
		if (m_cabi.hostCapsule)
			m_cabi.requestResize(m_cabi.hostCapsule);
	}

	//____ () ___________________________________________________
	bool CABIRoot::_childRequestFocus(StaticSlot* pSlot, Widget* pWidget)
	{
		if (!m_cabi.hostCapsule)
			return false;

		if( m_pFocusedChild )
		{
			State s = m_pFocusedChild->state();
			s.setFocused(false);
			m_pFocusedChild->_setState(s);

			m_pFocusedChild = pWidget;
			
			s = m_pFocusedChild->state();
			s.setFocused(true);
			m_pFocusedChild->_setState(s);
		}
		else
		{
			bool result = (bool) m_cabi.requestFocus(m_cabi.hostCapsule);
			if( result )
				m_pFocusedChild = pWidget;
			
			return result;
		}
	}

	//____ () ___________________________________________________
	bool CABIRoot::_childReleaseFocus(StaticSlot* pSlot, Widget* pWidget)
	{
		if (!m_cabi.hostCapsule)
			return false;

		bool result = (bool) m_cabi.releaseFocus(m_cabi.hostCapsule);
		
		return result;
	}

	//____ _childRequestInView() ___________________________________________________

	void CABIRoot::_childRequestInView(StaticSlot* pSlot)
	{
		if (m_cabi.hostCapsule)
			m_cabi.requestInView(m_cabi.hostCapsule, { 0,0,m_size.w,m_size.h }, { 0,0,m_size.w,m_size.h } );
	}

	void CABIRoot::_childRequestInView(StaticSlot* pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea)
	{
		if (m_cabi.hostCapsule)
			m_cabi.requestInView(m_cabi.hostCapsule, { mustHaveArea.x,mustHaveArea.y,mustHaveArea.w,mustHaveArea.h }, 
													{ niceToHaveArea.x, niceToHaveArea.y, niceToHaveArea.h, niceToHaveArea.w });
	}

	//____ _prevChild() _______________________________________________________

	Widget* CABIRoot::_prevChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget* CABIRoot::_nextChild(const StaticSlot* pSlot) const
	{
		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void CABIRoot::_releaseChild(StaticSlot* pSlot)
	{
		slot._setWidget(nullptr);

		if (m_cabi.hostCapsule)
		{
			m_cabi.requestRender(m_cabi.hostCapsule, { 0,0, m_size.w, m_size.h });
			m_cabi.requestResize(m_cabi.hostCapsule);
		}
	}

	//____ _replaceChild() ____________________________________________________

	void CABIRoot::_replaceChild(StaticSlot* pSlot, Widget* pWidget)
	{
		slot._setWidget(pWidget);

		if (pWidget)
			pWidget->_resize(m_size);

		if (m_cabi.hostCapsule)
		{
			m_cabi.requestRender(m_cabi.hostCapsule, { 0,0, m_size.w, m_size.h });
			m_cabi.requestResize(m_cabi.hostCapsule);
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void CABIRoot::_didAddSlots(StaticSlot* pSlot, int nb)
	{
		// Nothing to do.
	}

	//____ _didMoveSlots() ____________________________________________________

	void CABIRoot::_didMoveSlots(StaticSlot* pFrom, StaticSlot* pTo, int nb)
	{
		// Nothing to do.
	}

	//____ _willEraseSlots() __________________________________________________

	void CABIRoot::_willEraseSlots(StaticSlot* pSlot, int nb)
	{
		// Nothing to do.
	}

}
