
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


#include <wg_gfxbase.h>
#include <wg_bitmapcache.h>

namespace wg
{
	const TypeInfo GfxContext::TYPEINFO = { "GfxContext", &GearContext::TYPEINFO };


	//____ destructor() _______________________________________________________

	GfxContext::~GfxContext()
	{
		if( m_pDefaultBitmapCache )
		{
			m_pDefaultBitmapCache->clear();
			m_pDefaultBitmapCache = nullptr;
		}
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxContext::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ defaultBitmapCache() __________________________________________________

	BitmapCache_p GfxContext::defaultBitmapCache()
	{
		if( m_pDefaultBitmapCache == nullptr )
			m_pDefaultBitmapCache = BitmapCache::create(16*1024*1024);

		return m_pDefaultBitmapCache;
	}

	//____ setDefaultSurfaceFactory() ____________________________________________

	void GfxContext::setDefaultSurfaceFactory( SurfaceFactory * pFactory )
	{
		m_pDefaultSurfaceFactory = pFactory;
	}

	//____ setDefaultEdgemapFactory() ____________________________________________

	void GfxContext::setDefaultEdgemapFactory(EdgemapFactory* pFactory)
	{
		m_pDefaultEdgemapFactory = pFactory;
	}

	//____ setDefaultGfxDevice() _________________________________________________

	void GfxContext::setDefaultGfxDevice( GfxDevice * pDevice )
	{
		m_pDefaultGfxDevice = pDevice;
	}

	//____ setDefaultToSRGB() ____________________________________________________

	void GfxContext::setDefaultToSRGB( bool bSRGB )
	{
		m_bSRGB = bSRGB;
	}

} // namespace wg
