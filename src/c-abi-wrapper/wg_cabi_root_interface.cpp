
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

namespace wg
{



	static wg_spx func_matchingHeight(void* pCABIRoot, wg_spx width, int scale)
	{
		return (wg_spx) static_cast<CABIRoot*>(pCABIRoot)->_matchingHeight(spx(width), scale);
	}

	static wg_spx func_matchingWidth(void* pCABIRoot, wg_spx height, int scale)
	{
		return (wg_spx) static_cast<CABIRoot*>(pCABIRoot)->_matchingWidth(spx(height), scale);
	}

	static wg_sizeSPX func_defaultSize(void* pCABIRoot, int scale)
	{
		SizeSPX sz = static_cast<CABIRoot*>(pCABIRoot)->_defaultSize(scale);
		return { sz.w, sz.h };
	}

	static wg_sizeSPX func_minSize(void* pCABIRoot, int scale)
	{
		SizeSPX sz = static_cast<CABIRoot*>(pCABIRoot)->_minSize(scale);
		return { sz.w, sz.h };
	}

	static wg_sizeSPX func_maxSize(void* pCABIRoot, int scale)
	{
		SizeSPX sz = static_cast<CABIRoot*>(pCABIRoot)->_maxSize(scale);
		return { sz.w, sz.h };
	}

	int	func_markTest(void* pCABIRoot, wg_coordSPX ofs)
	{
		return static_cast<CABIRoot*>(pCABIRoot)->_markTest({ ofs.x, ofs.y });
	}

	void func_preRender(void* pCABIRoot)
	{
		static_cast<CABIRoot*>(pCABIRoot)->_preRender();
	}

	void func_render(void* pCABIRoot, wg_obj device, wg_rectSPX canvas, wg_rectSPX window)
	{
		static_cast<CABIRoot*>(pCABIRoot)->_render(device, {canvas.x, canvas.y, canvas.w, canvas.h}, {window.x, window.y, window.w, window.h});
	}

	void func_resize(void* pCABIRoot, wg_sizeSPX size, int scale)
	{
		static_cast<CABIRoot*>(pCABIRoot)->_resize({ size.w, size.h }, scale);
	}

	void func_setState(void* pCABIRoot, wg_state state)
	{
		static_cast<CABIRoot*>(pCABIRoot)->_setState(*(State*)&state);
	}

	void func_receive(void* pCABIRoot, wg_obj msg)
	{

	}

	wg_cabi_root_incalls makeCABI_root_incalls(CABIRoot* pCABIRoot)
	{
		wg_cabi_root_incalls calls;

		calls.structSize		= sizeof(wg_cabi_root_incalls);
		calls.pCABIRoot			= pCABIRoot;
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

		return calls;
	}


	inline CABICapsule* getCapsule(wg_obj obj) {
		return static_cast<CABICapsule*>(reinterpret_cast<Object*>(obj));
	}

	void func_requestRender(wg_obj hostCapsule, wg_rectSPX rect)
	{
		getCapsule(hostCapsule)->_rootRequestRender({ rect.x, rect.y, rect.w, rect.h });		
	}

	void func_requestResize(wg_obj	hostCapsule)
	{
		getCapsule(hostCapsule)->_rootRequestResize();
	}

	int func_isVisible(wg_obj hostCapsule)
	{
		return getCapsule(hostCapsule)->_isRootVisible();
	}

	wg_rectSPX func_windowSection(wg_obj hostCapsule)
	{
		auto rect = getCapsule(hostCapsule)->_rootWindowSection();

		return { rect.x, rect.y, rect.w, rect.h };
	}

	int func_requestFocus(wg_obj hostCapsule)
	{
		return getCapsule(hostCapsule)->_rootRequestFocus();
	}

	int func_releaseFocus(wg_obj hostCapsule)
	{
		return getCapsule(hostCapsule)->_rootRequestFocus();
	}

	void func_requestInView(wg_obj hostCapsule, wg_rectSPX mustHaveArea, wg_rectSPX niceToHaveArea)
	{
		return getCapsule(hostCapsule)->_rootRequestInView( {mustHaveArea.x, mustHaveArea.y, mustHaveArea.w, mustHaveArea.h},
															{niceToHaveArea.x, niceToHaveArea.y, niceToHaveArea.w, niceToHaveArea.h} );
	}

	void func_connect(wg_obj hostCapsule, wg_cabi_root_incalls* pInterface)
	{
		return getCapsule(hostCapsule)->_connectRoot(pInterface);
	}

	void func_disconnect(wg_obj hostCapsule)
	{
		return getCapsule(hostCapsule)->_disconnectRoot();
	}

	wg_cabi_root_outcalls makeCABI_root_outcalls(CABICapsule* pCABICapsule)
	{
		wg_cabi_root_outcalls	calls;

		calls.structSize = sizeof(wg_cabi_root_outcalls);
		calls.hostCapsule = static_cast<Object*>(pCABICapsule);

		calls.requestRender = func_requestRender;
		calls.requestResize = func_requestResize;
		calls.isVisible		= func_isVisible;
		calls.windowSection = func_windowSection;
		calls.requestFocus	= func_requestFocus;
		calls.releaseFocus	= func_releaseFocus;
		calls.requestInView = func_requestInView;
		calls.connect		= func_connect;
		calls.disconnect	= func_disconnect;

		return calls;
	}
} // namespace wg

#endif //WG_CABIROOTINTERFACE_DOT_H
