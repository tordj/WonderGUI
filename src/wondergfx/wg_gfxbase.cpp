
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
#include <wg_gfxutil.h>


namespace wg
{
	int					GfxBase::s_gfxInitCounter = 0;
	GfxContext_p		GfxBase::s_pContext;

	int 				GfxBase::s_curveTab[c_nCurveTabEntries];

	namespace PixelTools
	{
		extern uint8_t* pConv_16_linear_to_8_sRGB;
	}



	//____ init() __________________________________________________________________

	bool GfxBase::init()
	{
		if( s_gfxInitCounter == 0 )
		{
			if( !GearBase::init() )
				return false;
			
			HiColor::_initTables();
			_genCurveTab();
			
			s_pContext = GfxContext::create();
			GearBase::s_pContext = s_pContext;
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
			throwError(ErrorLevel::SilentError, ErrorCode::IllegalCall, "Call to GfxBase::exit() ignored, not initialized or already exited.", nullptr, nullptr, __func__, __FILE__, __LINE__);
			return false;
		}
		
		s_gfxInitCounter = 0;
		s_pContext = nullptr;
		
		delete [] PixelTools::pConv_16_linear_to_8_sRGB;
		PixelTools::pConv_16_linear_to_8_sRGB = nullptr;

		return GearBase::exit();
	}

	//____ setContext() __________________________________________________________

	bool GfxBase::setContext( GfxContext * pContext )
	{
		if( s_pContext && s_pContext->typeInfo() != GfxContext::TYPEINFO )
		{
			throwError(ErrorLevel::Error, ErrorCode::FailedPrerequisite, "A higher level context such as GUIContext is already in place. Use setContext method of GfxBase or GUIBase (or equivalent) to replace it.", nullptr, nullptr, __func__, __FILE__, __LINE__);

			return false;
		}

		if( pContext && pContext->typeInfo() != GfxContext::TYPEINFO )
		{
			throwError(ErrorLevel::Error, ErrorCode::FailedPrerequisite, "A higher level context such as GUIContext can not be set using GfxBase::setContext(). Use setContext method of GUIBase (or equivalent) to set it.", nullptr, nullptr, __func__, __FILE__, __LINE__);

			return false;
		}
		
		s_pContext = pContext;
		GearBase::s_pContext = pContext;
		return true;
	}

	//____ _genCurveTab() ___________________________________________________________

	void GfxBase::_genCurveTab()
	{
		//		double factor = 3.14159265 / (2.0 * c_nCurveTabEntries);

		for (int i = 0; i < c_nCurveTabEntries; i++)
		{
			double y = 1.f - i / (double)c_nCurveTabEntries;
			s_curveTab[i] = (int)(Util::squareRoot(1.f - y*y)*65536.f);
		}
	}

} // namespace wg
