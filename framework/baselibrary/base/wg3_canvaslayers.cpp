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

#include <wg3_canvaslayers.h>
#include <wg3_base.h>

namespace wg
{
	const TypeInfo CanvasLayers::TYPEINFO = { "CanvasLayers", &Object::TYPEINFO };

	//____ create() ___________________________________________________________
	/**
	 * @brief Creates a new CanvasLayer object with the layers specified.
	 * 
	 * Creates a new CanvasLayer object with layers of the formats specified. 
	 * 
	 * The layers are specified through a list of PixelFormats. The first value
	 * specifies the pixel format of layer 1, which is the first layer to be
	 * blended onto the canvas surface (which is layer 0) and thus the bottom
	 * layer, bar the canvas surface itself.
	 * 
	 * 
	 * 
	 * @param extraLayers - The PixelFormat for each layer, starting with layer 1 which is
	 * 						the bottom layer (bar the canvas surface itself, which is layer 0)
	 *						and moving upwards.
	 * 
	 * @return The created CanvasLayer object or nullptr if something went wrong.
	 */


	CanvasLayers_p CanvasLayers::create(const std::initializer_list<PixelFormat>& extraLayers)
	{
		// Sanity check
		
		if( extraLayers.size() > c_maxLayers )
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Number of layers exceeds maximum", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}
		
		for (auto& layer : extraLayers)
		{
			PixelFormat format = layer;

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

		//

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
	/**
	 * @brief Get PixelFormat of specificed layer.
	 * 
	 * Gets the PixelFormat of the specified layer.
	 * 
	 * @param layer		The layer we want to ask the format of. Please note that layer numbering starts on 1, not 0.
	 * 
	 * @return The PixelFormat of the specified layer.
	 */

	PixelFormat CanvasLayers::layerFormat(int layer) const
	{
		if (layer < 1 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return PixelFormat::Undefined;
		}

		return m_layers[layer-1].format;
	}


	//___ setDefaultLayer() ___________________________________________________
	/**
	 * @brief  Set default layer.
	 * 
	 * Sets the default layer, which is the layer unto which all widgets and skins are rendered unless otherwise specified. 
	 * 
	 * This value is initialized to 0.
	 * 
	 * @param layer		The layer to be set as default. 0 = Base layer, 1 = First specified layer in this CanvasLayer specification.
	 */

	void CanvasLayers::setDefaultLayer(int layer)
	{
		if (layer < 0 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		m_defaultLayer = layer;
	}

	//____ setLayerInitializer() _______________________________________________
	/**
	 * @brief Set a lambda function for clearing or initializing a layers surface.
	 * 
	 * Set a lambda function for clearing or initializing a layers surface.
	 * 
	 * @param layer		The layer for which we specify the initializer. Please note that layer numbering starts on 1, not 0.
	 * @param func		A lambda function to be used to initializing the layer after it is created.
	 * 
	 * By default a layer surface is filled with transparency when created, but that will be replaced by this function if specified.
	 * Use this function to fill the surface with any content you like before widgets or skins render into it.
	 * 
	 * When the function is called, it receives one parameter which is the GfxDevice to use. This has been prepared
	 * for operation in the following way:
	 * 
	 * - RenderTarget is set to the surface for the layer.
	 * - ClipList is set to only update patches that might be used.
	 * - BlendMode is unspecified, you need to set it!
	 * - TintColor and TintGradients are cleared.
	 * 
	 * BlendMode, TintColor and ClipList are automatically saved and restored. Any other settings you change
	 * (like BlitSource) will have to be saved and restored within the lambda.
	 * 
	 */

    void CanvasLayers::setLayerInitializer(int layer, const std::function<void(GfxDevice*)>& func)
    {
		if (layer < 1 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

        m_layers[layer-1].initializer = func;
    }

	//____ setLayerFinalizer() _________________________________________________
	/**
	 * @brief Set a lambda function to finalize the layers surface.
	 * 
	 * Set a lambda function to finalize the layers surface before it is blended onto the base layer (the specified canvas surface). 
	 * 
	 * This finalization step is purely optional, it just provides the option to modify the surface content before blending it onto the base layer.
	 * 
	 * @param layer		The layer for which we specify the finalizer. Please note that layer numbering starts on 1, not 0.
	 * @param func		A lambda function to be used to finalize the layer after it is created.
	 * 
	 * When the function is called, it receives one parameter which is the GfxDevice to use. This has been prepared
	 * for operation in the following way:
	 * 
	 * - RenderTarget is set to the surface for the layer.
	 * - ClipList is set to only update patches that might be used.
	 * - BlendMode is set to Blend.
	 * - TintColor and TintGradients are cleared.
	 * 
	 * No GfxDevice settings are saved or restored. Any settings you change will have to be saved and restored within the lambda.
	 * 
	 */

    void CanvasLayers::setLayerFinalizer(int layer, const std::function<void(GfxDevice*)>& func)
    {
		if (layer < 1 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

        m_layers[layer-1].finalizer = func;
    }

	//____ setLayerBlender() ____________________________________________________
	/**
	 * @brief Set the lambda function for blending a layer onto base layer.
	 * 
	 * Sets the lambda function for blending the specified layer onto the base layer (the specified canvas surface). 
	 * 
	 * By default the layer is blended onto the canvas surface using the BlendMode::Blend operation. If a blend function
	 * is specified, that is used instead.
	 * 
	 * 
	 * @param layer		The layer for which we specify the blend function. Please note that layer numbering starts on 1, not 0.
	 * @param func		A lambda function to be used to blend the layer onto the canvas surface.
	 * 
	 * When the function is called, it receives one parameter which is the GfxDevice to use. This has been prepared
	 * for operation in the following way:
	 * 
	 * - RenderTarget is set to the canvas surface (base layer).
	 * - Blit source is set to the layer to be blended.
	 * - ClipList is set to that provided to beginCanvasUpdate().
	 * - BlendMode is set to Blend.
	 * - TintColor and TintGradients are cleared.
	 * 
	 * No GfxDevice settings are saved or restored. Any settings you change will have to be saved and restored within the lambda.
	 * 
	 */

    void CanvasLayers::setLayerBlender(int layer, const std::function<void(GfxDevice*)>& func)
    {
		if (layer < 1 || layer > m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

        m_layers[layer-1].blender = func;
    }

	//____ setCanvasInitializer() ______________________________________________
	/**
	 * @brief Set a lambda function to initialize the canvas.
	 * 
	 * Sets a lambda function to initialize the canvas surface. This is called from GfxDevice::beginCanvasUpdate() and is
	 * an optional step.
	 * 
	 * @param func		A lambda function to be called when canvas surface is initialized.
	 * 
	 * When the function is called, it receives one parameter which is the GfxDevice to use. This has been prepared
	 * for operation in the following way:
	 * 
	 * - Render target is set to the canvas surface (base layer).
	 * - ClipList is set to that provided to beginCanvasUpdate().
	 * - BlendMode is set to Blend.
	 * - TintColor and TintGradients are cleared.
	 * 
	 * No GfxDevice settings are saved or restored. Any settings you change will have to be saved and restored within the lambda.
	 * 
	 */

    void CanvasLayers::setCanvasInitializer(const std::function<void(GfxDevice*)>& func)
    {
        m_canvasInitializer = func;
    }

	//____ setCanvasFinalizer() ________________________________________________
	/**
	 * @brief Set a lambda function to finalize the canvas.
	 * 
	 * Sets a lambda function to finalize the canvas surface. This is called from GfxDevice::endCanvasUpdate() and is
	 * an optional step.
	 * 
	 * @param func		A lambda function to be called when canvas surface is finalized.
	 * 
	 * When the function is called, it receives one parameter which is the GfxDevice to use. This has been prepared
	 * for operation in the following way:
	 * 
	 * - Render target is set to the canvas surface (base layer).
	 * - ClipList is set to that provided to beginCanvasUpdate().
	 * - BlendMode is set to Blend.
	 * - TintColor and TintGradients are cleared.
	 * 
	 * No GfxDevice settings are saved or restored. Any settings you change will have to be saved and restored within the lambda.
	 * 
	 */

    void CanvasLayers::setCanvasFinalizer(const std::function<void(GfxDevice*)>& func)
    {
        m_canvasFinalizer = func;
    }

	//____ setCanvasModifier() _________________________________________________
	/**
	 * @brief Set a lambda function to modify canvas before layer is blended onto it.
	 * 
	 * Sets a lambda function to modify the canvas surface before the specified layer is blended onto it. This is
	 * an optional step which provides you the oportunity to modify canvas content before next layer is blended onto it.
	 * 
	 * 
	 * @param layer		The layer before which the modifier should be called. Please note that layer numbering starts on 1, not 0.
	 * @param func		A lambda function to be used to modify content of the canvas surface.
	 * 
	 * When the function is called, it receives one parameter which is the GfxDevice to use. This has been prepared
	 * for operation in the following way:
	 * 
	 * - Render target is set to the canvas surface (base layer).
	 * - ClipList is set to that provided to GfxDevice::beginCanvasUpdate().
	 * - BlendMode is set to Blend.
	 * - TintColor and TintGradients are cleared.
	 * 
	 * No GfxDevice settings are saved or restored. Any settings you change will have to be saved and restored within the lambda.
	 * 
	 */

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