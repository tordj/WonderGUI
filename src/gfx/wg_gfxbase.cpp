
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
	int					GfxBase::s_gfxInitCounter = 0;

	bool				GfxBase::s_bSRGB = true;

	SurfaceFactory_p	GfxBase::s_pDefaultSurfaceFactory;
	GfxDevice_p			GfxBase::s_pDefaultGfxDevice;

	BitmapCache_p		GfxBase::s_pDefaultBitmapCache;



	//____ init() __________________________________________________________________

	bool GfxBase::init( )
	{
		if( s_gfxInitCounter == 0 )
		{
			s_gfxInitCounter++;
			return GearBase::init();
		}
	}

	//____ exit() __________________________________________________________________

	bool GfxBase::exit()
	{
		if( s_gfxInitCounter > 1 )
		{
			s_gfxInitCounter--;
			return true;
		}
		
		if( s_gfxInitCounter <= 0 )
		{
			handleError(ErrorSeverity::SilentFail, ErrorCode::IllegalCall, "Call to GfxBase::exit() ignored, not initialized or already exited.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}
		
		s_gfxInitCounter = 0;

		s_pDefaultGfxDevice 		= nullptr;
		s_pDefaultSurfaceFactory 	= nullptr;

		if( s_pDefaultBitmapCache )
		{
			s_pDefaultBitmapCache->clear();
			s_pDefaultBitmapCache = nullptr;
		}
			
		return GearBase::exit();
	}

	//____ defaultBitmapCache() __________________________________________________

	BitmapCache_p GfxBase::defaultBitmapCache()
	{
		if( s_pDefaultBitmapCache == nullptr )
			s_pDefaultBitmapCache = BitmapCache::create(16*1024*1024);

		return s_pDefaultBitmapCache;
	}

	//____ setDefaultSurfaceFactory() ____________________________________________

	void GfxBase::setDefaultSurfaceFactory( SurfaceFactory * pFactory )
	{
		s_pDefaultSurfaceFactory = pFactory;
	}

	//____ setDefaultGfxDevice() _________________________________________________

	void GfxBase::setDefaultGfxDevice( GfxDevice * pDevice )
	{
		s_pDefaultGfxDevice = pDevice;
	}

	//____ setDefaultToSRGB() ____________________________________________________

	void GfxBase::setDefaultToSRGB( bool bSRGB )
	{
		s_bSRGB = bSRGB;
	}


} // namespace wg
