
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

#include <wg_cabi_root_interface.h>

#include <wg_cabi_capsule.h>
#include <wg_cabi_base.h>

namespace wg
{

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

	int func_requestPreRenderCall(wg_obj hostCapsule)
	{
		return 	getCapsule(hostCapsule)->_requestPreRenderCall();
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

		calls.requestRender			= func_requestRender;
		calls.requestResize			= func_requestResize;
		calls.isVisible				= func_isVisible;
		calls.windowSection			= func_windowSection;
		calls.requestFocus			= func_requestFocus;
		calls.releaseFocus			= func_releaseFocus;
		calls.requestPreRenderCall	= func_requestPreRenderCall;
		calls.requestInView			= func_requestInView;
		calls.connect				= func_connect;
		calls.disconnect			= func_disconnect;

		return calls;
	}

} // namespace wg

