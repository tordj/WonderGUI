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

#ifndef	WG_GFXBASE_DOT_H
#define	WG_GFXBASE_DOT_H
#pragma once

#include <assert.h>

#include <wg_gearbase.h>
#include <wg_gfxdevice.h>

namespace wg
{
	class BitmapCache;
	typedef	StrongPtr<BitmapCache>		BitmapCache_p;

	class SurfaceFactory;
	typedef	StrongPtr<SurfaceFactory>	SurfaceFactory_p;


	class GfxBase : public GearBase
	{
//		friend class Object_wp;
		friend class Object;

		friend class WeakPtrHub;

	public:

		//.____ Creation __________________________________________

		static bool init();
		static bool exit();

		//.____ Content _____________________________________________

		static BitmapCache_p		defaultBitmapCache();
		
		static void					setDefaultSurfaceFactory( SurfaceFactory * pFactory );
		static inline SurfaceFactory_p	defaultSurfaceFactory() { return s_pDefaultSurfaceFactory; };

		static void					setDefaultEdgemapFactory(EdgemapFactory* pFactory);
		static inline EdgemapFactory_p	defaultEdgemapFactory() { return s_pDefaultEdgemapFactory; };

		static void					setDefaultGfxDevice( GfxDevice * pDevice );
		static inline GfxDevice_p	defaultGfxDevice() { return s_pDefaultGfxDevice; };

		static void					setDefaultToSRGB( bool bSRGB );
		static inline bool			defaultToSRGB() { return s_bSRGB; }
		
		static constexpr int *		curveTab() { return s_curveTab; }
		static constexpr int		curveTabSize() { return c_nCurveTabEntries; }

		
	private:
		
		static void					_genCurveTab();

		static int					s_gfxInitCounter;
		
		static bool					s_bSRGB;
		
		static SurfaceFactory_p		s_pDefaultSurfaceFactory;
		static EdgemapFactory_p	s_pDefaultEdgemapFactory;

		static GfxDevice_p			s_pDefaultGfxDevice;

		static BitmapCache_p		s_pDefaultBitmapCache;

		const static int 			c_nCurveTabEntries = 1024;
		static int 					s_curveTab[c_nCurveTabEntries];

		
	};



} // namespace wg
#endif //WG_GFXBASE_DOT_H
