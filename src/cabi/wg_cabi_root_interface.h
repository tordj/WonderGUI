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

#ifndef	WG_CABI_ROOT_INTERFACE_DOT_H
#define	WG_CABI_ROOT_INTERFACE_DOT_H
#pragma once

#include <wg_c_types.h>
#include <wg_c_geo.h>


extern "C" {


	struct wg_cabi_root_incalls
	{
		int			structSize;
		void*		pCABIRoot;

		wg_spx		(*matchingHeight)(void* pCABIRoot, wg_spx width, int scale);
		wg_spx			(*matchingWidth)(void* pCABIRoot, wg_spx height, int scale);

		wg_sizeSPX		(*defaultSize)(void* pCABIRoot, int scale);
		wg_sizeSPX		(*minSize)(void* pCABIRoot, int scale);
		wg_sizeSPX		(*maxSize)(void* pCABIRoot, int scale);

		int				(*markTest)(void* pCABIRoot, wg_coordSPX ofs);

		void    		(*preRender)(void* pCABIRoot);
		void			(*render)(void* pCABIRoot, wg_obj pDevice, wg_rectSPX canvas, wg_rectSPX window);

		void			(*resize)(void* pCABIRoot, wg_sizeSPX size, int scale);
		void			(*setState)(void* pCABIRoot, wg_state state);
		void			(*receive)(void* pCABIRoot, wg_obj msg);

//		wg_string		(*tooltip)(void* pCABIRoot);
		wg_pointerStyle	(*pointerStyle)(void* pCABIRoot);

		void			(*setPointerPos)(void* pCABIRoot, wg_coordSPX pos, int64_t timestamp );
		void			(*setButtonState)(void* pCABIRoot, int button, int pressed, int64_t timestamp );
		void			(*setKeyState)(void* pCABIRoot, int nativeKeycode, int pressed, int64_t timestamp );
		void			(*putText)(void* pCABIRoot, const char * pUTF8String );

		void			(*onUpdate)(void* pCABIRoot, int microPassed, int64_t microsecTimestamp);
	};

	struct wg_cabi_root_outcalls
	{
		int			structSize;
		wg_obj		hostCapsule;

		void		(*requestRender)(wg_obj hostCapsule, wg_rectSPX rect);
		void		(*requestResize)(wg_obj	hostCapsule);
		int			(*isVisible)(wg_obj hostCapsule);
		wg_rectSPX	(*windowSection)(wg_obj hostCapsule);
		int			(*requestFocus)(wg_obj hostCapsule);
		int			(*releaseFocus)(wg_obj hostCapsule);
		int			(*requestPreRenderCall)(wg_obj hostCapsule);


		void		(*requestInView)(wg_obj hostCapsule, wg_rectSPX mustHaveArea, wg_rectSPX niceToHaveArea );

		void		(*connect)(wg_obj hostCapsule, wg_cabi_root_incalls* pInterface);
		void		(*disconnect)(wg_obj hostCapsule);
	};
}


#endif //WG_CABI_ROOT_INTERFACE_DOT_H
