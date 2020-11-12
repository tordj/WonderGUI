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

#include <wg_context.h>
#include <wg_surfacefactory.h>
#include <wg_gfxdevice.h>

namespace wg
{
	const TypeInfo Context::TYPEINFO = { "Context", &Object::TYPEINFO };


	//____ typeInfo() _________________________________________________________

	const TypeInfo& Context::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setScale() _________________________________________________________

	bool Context::setScale(float scale)
	{
		if (scale < 0.25f || scale > 8.f || float(int(scale * 4)) != scale*4)
			return false;

		// Save scale

		m_scale = scale;
		return true;
	}

	//____ setGfxDevice() _____________________________________________________

	void Context::setGfxDevice(GfxDevice * pDevice)
	{
		m_pGfxDevice = pDevice;
	}

	//____ setSurfaceFactory() ________________________________________________

	void Context::setSurfaceFactory(SurfaceFactory * pSurfaceFactory)
	{
		m_pSurfaceFactory = pSurfaceFactory;
	}

	//____ setGammaCorrection() _______________________________________________

	void Context::setGammaCorrection(bool bOn)
	{
		m_bGammaCorrection = bOn;
	}

	//____ setCanvasLayers() __________________________________________________

	void Context::setCanvasLayers(CanvasLayers* pLayers)
	{
		m_pCanvasLayers = pLayers;
	}

}
