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

#include <wg_cabi_capsule.h>
#include <wg_gfxdevice.h>
#include <wg_msg.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo CABICapsule::TYPEINFO = { "CABICapsule", &Widget::TYPEINFO };

	//____ create() ___________________________________________________________

	CABICapsule_p CABICapsule::create()
	{
		return CABICapsule_p(new CABICapsule());
	}

	CABICapsule_p CABICapsule::create(const Blueprint& blueprint)
	{
		return CABICapsule_p(new CABICapsule(blueprint));
	}


	//____ constructor ________________________________________________________

	CABICapsule::CABICapsule()
	{
		std::memset(&m_cabi, 0, sizeof(m_cabi));
	}

	CABICapsule::CABICapsule(const Blueprint& bp) : CABICapsule()
	{
		_initFromBlueprint(bp);

	
	}

	//____ Destructor _________________________________________________________

	CABICapsule::~CABICapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABICapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ tooltip() __________________________________________________________

	String CABICapsule::tooltip() const
	{
		//TODO: Implement!!!

		return String();
	}

	//____ pointerStyle() _____________________________________________________

	PointerStyle CABICapsule::pointerStyle() const
	{
		if (m_cabi.pCABIRoot)
			return (PointerStyle)m_cabi.pointerStyle(m_cabi.pCABIRoot);
		else
			return m_pointerStyle;
	}

	//____ _defaultSize() _____________________________________________________

	SizeSPX CABICapsule::_defaultSize(int scale) const
	{
		if (m_cabi.pCABIRoot)
		{
			auto size = m_cabi.defaultSize(m_cabi.pCABIRoot, scale);
			return { size.w, size.h };
		}

		return SizeSPX();
	}

	//____ _minSize() _________________________________________________________

	SizeSPX CABICapsule::_minSize(int scale) const
	{
		if (m_cabi.pCABIRoot)
		{
			auto size = m_cabi.minSize(m_cabi.pCABIRoot, scale);
			return { size.w, size.h };
		}
		return SizeSPX();
	}

	//____ _maxSize() _________________________________________________________

	SizeSPX CABICapsule::_maxSize(int scale) const
	{
		if (m_cabi.pCABIRoot)
		{
			auto size = m_cabi.maxSize(m_cabi.pCABIRoot, scale);
			return { size.w, size.h };
		}

		return SizeSPX(spx_max,spx_max);
	}

	//____ _matchingHeight() __________________________________________________

	spx CABICapsule::_matchingHeight(spx width, int scale) const
	{
		if (m_cabi.pCABIRoot)
			return  m_cabi.matchingHeight(m_cabi.pCABIRoot, width, scale);

		return 0;
	}

	//____ _matchingWidth() ___________________________________________________

	spx CABICapsule::_matchingWidth(spx height, int scale) const
	{
		if (m_cabi.pCABIRoot)
			return  m_cabi.matchingWidth(m_cabi.pCABIRoot, height, scale);

		return 0;
	}

	//____ _preRender() _______________________________________________________

	void CABICapsule::_preRender()
	{
		if (m_cabi.pCABIRoot)
			m_cabi.preRender(m_cabi.pCABIRoot);
	}

	//____ _render() __________________________________________________________

	void CABICapsule::_render(GfxDevice* pDevice, const RectSPX& canvas, const RectSPX& window)
	{
		if (m_cabi.pCABIRoot)
			m_cabi.render(m_cabi.pCABIRoot, static_cast<Object*>(pDevice), {canvas.x,canvas.y,canvas.w,canvas.h}, { window.x,window.y,window.w,window.h });
	}

	//____ _resize() __________________________________________________________

	void CABICapsule::_resize(const SizeSPX& size, int scale)
	{
		if (m_cabi.pCABIRoot)
			m_cabi.resize(m_cabi.pCABIRoot, {size.w,size.h}, scale);
	}

	//____ _setState() ________________________________________________________

	void CABICapsule::_setState(State state)
	{
		if (m_cabi.pCABIRoot)
			m_cabi.setState(m_cabi.pCABIRoot, * (wg_state*) &state);
	}

	//____ _receive() _________________________________________________________

	void CABICapsule::_receive(Msg* pMsg)
	{
		if(!m_cabi.pCABIRoot)
		{
			Widget::_receive(pMsg);
			return;
		}

		switch( pMsg->type() )
		{
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			case MsgType::MouseLeave:
			{
				auto pMess = static_cast<InputMsg*>(pMsg);
				CoordSPX localPos = _toLocal( pMess->pointerSpxPos() );
				m_cabi.setPointerPos(m_cabi.pCABIRoot, { localPos.x, localPos.y }, pMess->timestamp() );
				
				break;
			}
				
			case MsgType::MousePress:
			{
				auto pMess = static_cast<MousePressMsg*>(pMsg);
				CoordSPX localPos = _toLocal( pMess->pointerSpxPos() );
				m_cabi.setButtonState(m_cabi.pCABIRoot, (int) pMess->button(), 1, pMess->timestamp() );
				break;
			}
				
			case MsgType::MouseRelease:
			{
				auto pMess = static_cast<MouseReleaseMsg*>(pMsg);
				CoordSPX localPos = _toLocal( pMess->pointerSpxPos() );
				m_cabi.setButtonState(m_cabi.pCABIRoot, (int) pMess->button(), 0, pMess->timestamp() );
				break;
			}

				
			default:
				break;
		}
	}

	//____ _update() __________________________________________________________

	void CABICapsule::_update(int microPassed, int64_t microsecTimestamp)
	{
		if(m_cabi.pCABIRoot)
			m_cabi.onUpdate(m_cabi.pCABIRoot, microPassed, microsecTimestamp);
	}


	//____ _alphaTest() _______________________________________________________

	bool CABICapsule::_alphaTest(const CoordSPX& ofs)
	{
		return true;
/*
		if (m_cabi.pCABIRoot)
			return m_cabi.alphaTest(m_cabi.pCABIRoot, { ofs.x,ofs.y } );

		return false;
*/
	}

	//____ _isRootVisible() ___________________________________________________

	bool CABICapsule::_isRootVisible() const
	{
		return _holder()->_isChildVisible(_slot());
	}

	//____ _rootWindowSection() _______________________________________________

	RectSPX CABICapsule::_rootWindowSection() const
	{
		auto rect = _windowSection();
		return { rect.x, rect.y, rect.w, rect.h };
	}

	//____ _rootRequestRender() _______________________________________________

	void CABICapsule::_rootRequestRender(const RectSPX& rect)
	{
		_requestRender();
	}

	//____ _rootRequestResize() _______________________________________________

	void CABICapsule::_rootRequestResize()
	{
		_requestResize();
	}

	//____ _rootRequestFocus() ________________________________________________

	bool CABICapsule::_rootRequestFocus()
	{
		return grabFocus();
	}

	//____ _rootReleaseFocus() ________________________________________________

	bool CABICapsule::_rootReleaseFocus()
	{
		return releaseFocus();
	}

	//____ _rootRequestInView() _______________________________________________

	void CABICapsule::_rootRequestInView(const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea)
	{
		_requestInView(mustHaveArea, niceToHaveArea);
	}

	//____ _connectRoot() _____________________________________________________

	void CABICapsule::_connectRoot(wg_cabi_root_incalls* pCalls)
	{
		m_cabi = *pCalls;
		_startReceiveUpdates();
	}

	//___ _disconnectRoot() ____________________________________________________

	void CABICapsule::_disconnectRoot()
	{
		_stopReceiveUpdates();
		std::memset(&m_cabi, 0, sizeof(m_cabi));
	}




} // namespace wg
