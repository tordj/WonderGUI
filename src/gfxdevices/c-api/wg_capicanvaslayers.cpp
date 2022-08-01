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

#include <wg_capicanvaslayers.h>
#include <wg_capigfxdevice.h>
#include <wg_base.h>

#include <wg_c_canvaslayers.h>

namespace wg
{
	const TypeInfo CAPICanvasLayers::TYPEINFO = { "CAPICanvasLayers", &CanvasLayers::TYPEINFO };


	//____ create() ___________________________________________________________

	CAPICanvasLayers_p CAPICanvasLayers::create(CAPIGfxDevice* pDevice, const Blueprint& bp)
	{
		// Sanity check

		if (bp.layers.size() > c_maxLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Number of layers exceeds maximum", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}

		for (auto& layer : bp.layers)
		{
			PixelFormat format = layer.format;

			if (format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear)
			{
				Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Canvas layers can not be CLUT-based", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
				return nullptr;
			}

			if (format == PixelFormat::Custom)
			{
				Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Canvas layers can not have custom pixel format", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
				return nullptr;
			}

		}

		if (bp.defaultLayer < 0 || bp.defaultLayer > bp.layers.size())
		{
			Base::handleError(ErrorSeverity::Critical, ErrorCode::InvalidParam, "Default layer is out of bounds.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}

		//

		return CAPICanvasLayers_p(new CAPICanvasLayers(pDevice,bp));
	}

	//____ constructor() ______________________________________________________

	CAPICanvasLayers::CAPICanvasLayers( CAPIGfxDevice * pDevice, const Blueprint& bp) : CanvasLayers(bp)
	{
		wg_canvasLayersBP cBP;

		memset(&cBP, 0, sizeof(wg_canvasLayersBP));

		cBP.defaultLayer = bp.defaultLayer;

		if (bp.canvasInitializer)
		{
			cBP.canvasInitializer = _callbackRouter;
			cBP.canvasInitializerPtr = this;
			cBP.canvasInitializerInt = 1;
		}

		if (bp.canvasFinalizer)
		{
			cBP.canvasFinalizer = _callbackRouter;
			cBP.canvasFinalizerPtr = this;
			cBP.canvasFinalizerInt = 2;
		}

		int nLayers = bp.layers.size();

		for (int i = 0; i < nLayers; i++)
		{
			cBP.layers[i].format = (wg_pixelFormat) bp.layers[i].format;

			if (bp.layers[i].canvasPreparer)
			{
				cBP.layers[i].canvasPreparer = _callbackRouter;
				cBP.layers[i].canvasPreparerPtr = this;
				cBP.layers[i].canvasPreparerInt = 3 + (i * 4);
			}

			if (bp.layers[i].initializer)
			{
				cBP.layers[i].initializer = _callbackRouter;
				cBP.layers[i].initializerPtr = this;
				cBP.layers[i].initializerInt = 3 + (i * 4)+1;
			}

			if (bp.layers[i].blender)
			{
				cBP.layers[i].blender = _callbackRouter;
				cBP.layers[i].blenderPtr = this;
				cBP.layers[i].blenderInt = 3 + (i * 4) + 2;
			}

			if (bp.layers[i].finalizer)
			{
				cBP.layers[i].finalizer = _callbackRouter;
				cBP.layers[i].finalizerPtr = this;
				cBP.layers[i].finalizerInt = 3 + (i * 4) + 3;
			}

		}

		m_cCanvasLayer = wg_createCanvasLayers(&cBP);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CAPICanvasLayers::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _callbackRouter() __________________________________________________

	void CAPICanvasLayers::_callbackRouter( wg_obj cDevice, void* _pMe, int funcNb )
	{
		CAPICanvasLayers* pMe = (CAPICanvasLayers*)_pMe;

		GfxDevice* pDevice = pMe->m_pDevice;

		if (funcNb == 1)
			pMe->m_canvasInitializer(pDevice);
		else if (funcNb == 2)
			pMe->m_canvasFinalizer(pDevice);
		else
		{
			auto& layer = pMe->m_layers[(funcNb - 3) / 4];

			int func = (funcNb - 3) % 4;

			switch (func)
			{
				case 0:
					layer.canvasPreparer(pDevice);
					break;
				case 1:
					layer.initializer(pDevice);
					break;
				case 2:
					layer.blender(pDevice);
					break;
				case 3:
					layer.finalizer(pDevice);
					break;

				default:
					break;
			}
		}
	}
}