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
#include <wg_gfxcontext.h>


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

		static bool init( GfxContext * pContext = nullptr );
		static bool exit();

		//.____ Content _____________________________________________
		
		static GfxContext_p			context() { assert(s_pContext); return s_pContext; }
		static bool					setContext( GfxContext * pContext );
		
		static constexpr int *		curveTab() { return s_curveTab; }
		static constexpr int		curveTabSize() { return c_nCurveTabEntries; }

	protected:
		static GfxContext_p			s_pContext;

	private:
		
		static void					_genCurveTab();

		static int					s_gfxInitCounter;
		
		const static int 			c_nCurveTabEntries = 1024;
		static int 					s_curveTab[c_nCurveTabEntries];

		
	};



} // namespace wg
#endif //WG_GFXBASE_DOT_H
