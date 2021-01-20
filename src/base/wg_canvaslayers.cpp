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

#include <wg_canvaslayers.h>
#include <wg_base.h>

namespace wg
{
	const TypeInfo CanvasLayers::TYPEINFO = { "CanvasLayers", &Object::TYPEINFO };

	//____ create() ___________________________________________________________

	CanvasLayers_p CanvasLayers::create(const std::initializer_list<PixelFormat>& extraLayers)
	{
		for (auto& layer : extraLayers)
		{
			PixelFormat format = layer;

			if (format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear)
			{
				Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Canvas layers can not be CLUT-based", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
				return nullptr;
			}
		}

		return CanvasLayers_p(new CanvasLayers(extraLayers));
	}

	//____ constructor() ______________________________________________________

	CanvasLayers::CanvasLayers(const std::initializer_list<PixelFormat>& extraLayers)
	{
		m_nbLayers = (int) extraLayers.size();

		int ofs = 0;
		for (auto& layer : extraLayers)
		{
			m_layers[ofs++].format = layer;
		}
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CanvasLayers::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ layerFormat() _____________________________________________________

	PixelFormat CanvasLayers::layerFormat(int layer) const
	{
		if (layer < 1 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return PixelFormat::Unknown;
		}

		return m_layers[layer-1].format;
	}


	//___ setDefaultLayer() ___________________________________________________

	void CanvasLayers::setDefaultLayer(int layer)
	{
		if (layer < 0 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		m_defaultLayer = layer;
	}

    void CanvasLayers::setLayerInitializer(int layer, const std::function<void(GfxDevice*)>& func)
    {
        m_layers[layer-1].initializer = func;
    }

    void CanvasLayers::setLayerFinalizer(int layer, const std::function<void(GfxDevice*)>& func)
    {
        m_layers[layer-1].finalizer = func;
    }

    void CanvasLayers::setLayerBlender(int layer, const std::function<void(GfxDevice*)>& func)
    {
        m_layers[layer-1].blender = func;
    }


    void CanvasLayers::setCanvasInitializer(const std::function<void(GfxDevice*)>& func)
    {
        m_canvasInitializer = func;
    }

    void CanvasLayers::setCanvasFinalizer(const std::function<void(GfxDevice*)>& func)
    {
        m_canvasFinalizer = func;
    }

    void CanvasLayers::setCanvasModifier(int layer, const std::function<void(GfxDevice*)>& func)
    {
        if (layer < 1 || layer > m_nbLayers)
        {
            Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
            return;
        }

        m_canvasModifiers[layer-1] = func;
    }

}
