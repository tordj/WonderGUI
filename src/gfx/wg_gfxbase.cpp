
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
#include <wg_pixeltools.h>
#include <wg_edgemaptools.h>


namespace wg
{
	int					GfxBase::s_gfxInitCounter = 0;

	bool				GfxBase::s_bSRGB = true;

	SurfaceFactory_p	GfxBase::s_pDefaultSurfaceFactory;
	EdgemapFactory_p	GfxBase::s_pDefaultEdgemapFactory;
	GfxDevice_p			GfxBase::s_pDefaultGfxDevice;

	BitmapCache_p		GfxBase::s_pDefaultBitmapCache;

	namespace PixelTools
	{
		extern uint8_t* pConv_16_linear_to_8_sRGB;
	}



	//____ init() __________________________________________________________________

	bool GfxBase::init( )
	{
		if( s_gfxInitCounter == 0 )
		{
			HiColor::_initTables();
			EdgemapTools::init();
			
			s_gfxInitCounter++;
			return GearBase::init();
		}

		s_gfxInitCounter++;
		return true;

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
			throwError(ErrorLevel::SilentError, ErrorCode::IllegalCall, "Call to GfxBase::exit() ignored, not initialized or already exited.", nullptr, &TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}
		
		s_gfxInitCounter = 0;

		s_pDefaultGfxDevice 		= nullptr;
		s_pDefaultSurfaceFactory 	= nullptr;
		s_pDefaultEdgemapFactory	= nullptr;

		EdgemapTools::exit();
		
		if( s_pDefaultBitmapCache )
		{
			s_pDefaultBitmapCache->clear();
			s_pDefaultBitmapCache = nullptr;
		}
		
		delete [] PixelTools::pConv_16_linear_to_8_sRGB;
		PixelTools::pConv_16_linear_to_8_sRGB = nullptr;

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

	//____ setDefaultEdgemapFactory() ____________________________________________

	void GfxBase::setDefaultEdgemapFactory(EdgemapFactory* pFactory)
	{
		s_pDefaultEdgemapFactory = pFactory;
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
