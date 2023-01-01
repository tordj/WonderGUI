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

#include <wg_cabi_root.h>
#include <wg_container.h>


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
	}

	//____ Destructor _____________________________________________________________

	CABIRoot::~CABIRoot()
	{
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
		
	void CABIRoot::addPreRenderCall(Widget* pWidget)
	{
		//TODO: Implement!!!
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

	bool CABIRoot::_markTest(const CoordSPX& ofs)
	{

	}

	void CABIRoot::_preRender()
	{

	}

	void CABIRoot::_render(wg_obj device, const RectSPX& _canvas, const RectSPX& _window)
	{
	}


	//____ _resize() __________________________________________________________

	void CABIRoot::_resize(const SizeSPX& size, int scale)
	{

	}

	//____ _setState() ________________________________________________________

	void CABIRoot::_setState(State state)
	{

	}

	//____ _receive() _________________________________________________________

	void CABIRoot::_receive(Msg* pMsg)
	{
		//TODO: Implement!!!
	}


