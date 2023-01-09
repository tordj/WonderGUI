
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
#ifndef WG_CABIROOTINTERFACE_DOT_H
#define WG_CABIROOTINTERFACE_DOT_H
#pragma once

#include <wg_cabi_root_interface.h>

#include <wg_cabi_root.h>
#include <wg_cabi_capsule.h>
#include <wg_cabi_base.h>

namespace wg
{

	inline CABIRoot* getRoot(void * pRoot) {
		return static_cast<CABIRoot*>(reinterpret_cast<Object*>(pRoot));
	}



	static wg_spx func_matchingHeight(void* pCABIRoot, wg_spx width, int scale)
	{
		return (wg_spx) getRoot(pCABIRoot)->_matchingHeight(spx(width), scale);
	}

	static wg_spx func_matchingWidth(void* pCABIRoot, wg_spx height, int scale)
	{
		return (wg_spx) getRoot(pCABIRoot)->_matchingWidth(spx(height), scale);
	}

	static wg_sizeSPX func_defaultSize(void* pCABIRoot, int scale)
	{
		SizeSPX sz = getRoot(pCABIRoot)->_defaultSize(scale);
		return { sz.w, sz.h };
	}

	static wg_sizeSPX func_minSize(void* pCABIRoot, int scale)
	{
		SizeSPX sz = getRoot(pCABIRoot)->_minSize(scale);
		return { sz.w, sz.h };
	}

	static wg_sizeSPX func_maxSize(void* pCABIRoot, int scale)
	{
		SizeSPX sz = getRoot(pCABIRoot)->_maxSize(scale);
		return { sz.w, sz.h };
	}

	int	func_markTest(void* pCABIRoot, wg_coordSPX ofs)
	{
		return getRoot(pCABIRoot)->_markTest({ ofs.x, ofs.y });
	}

	void func_preRender(void* pCABIRoot)
	{
		getRoot(pCABIRoot)->_preRender();
	}

	void func_render(void* pCABIRoot, wg_obj device, wg_rectSPX canvas, wg_rectSPX window)
	{
		void * p = &CABI::object;

		getRoot(pCABIRoot)->_render(device, {canvas.x, canvas.y, canvas.w, canvas.h}, {window.x, window.y, window.w, window.h});
	}

	void func_resize(void* pCABIRoot, wg_sizeSPX size, int scale)
	{
		getRoot(pCABIRoot)->_resize({ size.w, size.h }, scale);
	}

	void func_setState(void* pCABIRoot, wg_state state)
	{
		getRoot(pCABIRoot)->_setState(*(State*)&state);
	}

	void func_receive(void* pCABIRoot, wg_obj msg)
	{

	}

	void func_setPointerPos(void* pCABIRoot, wg_coordSPX pos, int64_t timestamp )
	{
		getRoot(pCABIRoot)->_setPointerPos({pos.x,pos.y}, timestamp);
	}

	void func_setButtonState(void* pCABIRoot, int button, int pressed, int64_t timestamp )
	{
		getRoot(pCABIRoot)->_setButtonState(button, bool(pressed), timestamp);
	}


	void func_onUpdate(void* pCABIRoot, int microPassed, int64_t microsecTimestamp)
	{
		getRoot(pCABIRoot)->_update(microPassed,microsecTimestamp);
	}

	wg_pointerStyle func_pointerStyle(void* pCABIRoot)
	{
		return (wg_pointerStyle)getRoot(pCABIRoot)->_pointerStyle();
	}

	wg_cabi_root_incalls makeCABI_root_incalls(CABIRoot* pCABIRoot)
	{
		wg_cabi_root_incalls calls;

		calls.structSize		= sizeof(wg_cabi_root_incalls);
		calls.pCABIRoot			= static_cast<Object*>(pCABIRoot);
		calls.matchingHeight	= func_matchingHeight;
		calls.matchingWidth		= func_matchingWidth;
		calls.defaultSize		= func_defaultSize;
		calls.minSize			= func_minSize;
		calls.maxSize			= func_maxSize;
		calls.markTest			= func_markTest;
		calls.preRender			= func_preRender;
		calls.render			= func_render;
		calls.resize			= func_resize;
		calls.setState			= func_setState;
		calls.receive			= func_receive;
		calls.pointerStyle		= func_pointerStyle;
		calls.setPointerPos		= func_setPointerPos;
		calls.setButtonState	= func_setButtonState;
		calls.onUpdate			= func_onUpdate;

		return calls;
	}


} // namespace wg

#endif //WG_CABIROOTINTERFACE_DOT_H
