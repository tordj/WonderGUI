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

#ifndef	WG_GFXCONTEXT_DOT_H
#define	WG_GFXCONTEXT_DOT_H
#pragma once

#include <wg_gearcontext.h>
#include <wg_gfxdevice.h>
#include <wg_bitmapcache.h>


namespace wg
{
//	class BitmapCache;
//	typedef	StrongPtr<BitmapCache>	BitmapCache_p;

	class GfxContext;
	typedef	StrongPtr<GfxContext>	GfxContext_p;
	typedef	WeakPtr<GfxContext>		GfxContext_wp;

	class GfxContext : public GearContext
	{

	public:
		//.____ Creation __________________________________________

		static GfxContext_p	create() { return GfxContext_p(new GfxContext()); };

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content ___________________________________________________________
		
		BitmapCache_p		defaultBitmapCache();
		
		void				setDefaultSurfaceFactory( SurfaceFactory * pFactory );
		SurfaceFactory_p	defaultSurfaceFactory() const { return m_pDefaultSurfaceFactory; };

		void				setDefaultEdgemapFactory(EdgemapFactory* pFactory);
		EdgemapFactory_p	defaultEdgemapFactory() const { return m_pDefaultEdgemapFactory; };

		void				setDefaultGfxDevice( GfxDevice * pDevice );
		GfxDevice_p			defaultGfxDevice() const { return m_pDefaultGfxDevice; };

		void				setDefaultToSRGB( bool bSRGB );
		bool				defaultToSRGB() const { return m_bSRGB; }

	protected:
		GfxContext() {};
		virtual ~GfxContext();

		bool					m_bSRGB = true;
		
		SurfaceFactory_p		m_pDefaultSurfaceFactory;
		EdgemapFactory_p		m_pDefaultEdgemapFactory;

		GfxDevice_p				m_pDefaultGfxDevice;

		BitmapCache_p			m_pDefaultBitmapCache;
		
	};

} // namespace wg
#endif //WG_GFXCONTEXT_DOT_H
