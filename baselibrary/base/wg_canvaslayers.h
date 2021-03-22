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

#ifndef WG_CANVASLAYERS_DOT_H
#define WG_CANVASLAYERS_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_object.h>
#include <wg_color.h>

#include <initializer_list>
#include <utility>
#include <functional>

namespace wg
{
    class Surface;
    class GfxDevice;

	class CanvasLayers;
	typedef	StrongPtr<CanvasLayers>	CanvasLayers_p;
	typedef	WeakPtr<CanvasLayers>	CanvasLayers_wp;

	//____ Class CanvasLayers ________________________________________________________
	/**
	 * @class CanvasLayers
	 * @author Tord Jansson
	 * @date 22/01/21
	 * @file wg_canvaslayers.h
	 * @brief Specifies canvas layers and the operations on them.
	 * 
	 * CanvasLayers specifies layers 1 and above when performing multi-layer rendering.
	 * 
	 * Layer 0 (also called the base layer) is the surface set as canvas. The first specified layer
	 * in this CanvasLayer becomes layer 1. A maximum of 16 layers can be specified.
	 * 
	 * The surface of a layer is created on demand when the first widget or skin attempts to render to it
	 * (calls GfxDevice::setRenderLayer(layer-number)), thus there is no cost for specifying layers that
	 * are not used.
	 * 
	 * When the canvas is completed (when GfxDevice::endCanvasUpdate() is called), the generated
	 * surfaces are blended onto the base layer and destroyed, resulting in the canvas surface containing
	 * the composite image.
	 * 
	 * The format of the surface and the operation used to blend it onto the canvas surface can be specified.
	 * Several other methods can also be specified, dealing with initializing and finalizing the individual layer
	 * surfaces.
	 * 
	 * Using CanvasLayers allows for advanced rendering operations where shadows, glow, glass-effects etc
	 * can be inserted between backgrounds and foregrounds of the UI. It is also an easy way to bog down the user
	 * interface by allocating and blending large surfaces on the fly, using large amounts of memory, GPU and CPU.
	 * 
	 * CanvasLayers can also produce unexpected results when different widgets overlap. I.e. if an overlapping
	 * widget renders all its contents to a layer below the layer used by the widget behind it, 
	 * the content of the widget behind it could be rendered ontop anyway (if it really does or not depends on technical
	 * details of optimizations in the rendering pipeline we won't go into here, the point is you should be careful not to 
	 * mess with layers in a way that contradicts the layering of widgets).
	 * 
	 * It's a powerful tool, use it with caution.
	 */


	class	CanvasLayers : public Object	
	{
		friend class GfxDevice;
		
	public:
		//.____ Creation __________________________________________

		static CanvasLayers_p	create(const std::initializer_list<PixelFormat>& extraLayers);

		//.____ Identification _________________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline int				size() const { return m_nbLayers; }

		PixelFormat				layerFormat(int layer) const;

		void					setDefaultLayer(int layer);
		inline int				defaultLayer() const { return m_defaultLayer;  }

        void                    setLayerInitializer(int layer, const std::function<void(GfxDevice*)>& func);
        void                    setLayerFinalizer(int layer, const std::function<void(GfxDevice*)>& func);
        void                    setLayerBlender(int layer, const std::function<void(GfxDevice*)>& func);

        void                    setCanvasInitializer(const std::function<void(GfxDevice*)>& func);
        void                    setCanvasFinalizer(const std::function<void(GfxDevice*)>& func);
        
        void                    setCanvasModifier(int beforeLayer, const std::function<void(GfxDevice*)>& func);


	protected:
		CanvasLayers(const std::initializer_list<PixelFormat>& extraLayers);

		const static int	    c_maxLayers = 16;

		inline PixelFormat      _layerFormat(int i) const { return m_layers[i-1].format; }
        inline const std::function<void(GfxDevice * pDevice)>& _layerInitializer(int i) const { return m_layers[i-1].initializer; }
        inline const std::function<void(GfxDevice * pDevice)>& _layerFinalizer(int i) const { return m_layers[i-1].finalizer; }
        inline const std::function<void(GfxDevice * pDevice)>& _layerBlender(int i) const { return m_layers[i-1].blender; }

        inline const std::function<void(GfxDevice * pDevice)>& _canvasInitializer() const { return m_canvasInitializer; }
        inline const std::function<void(GfxDevice * pDevice)>& _canvasFinalizer() const { return m_canvasFinalizer; }

        inline const std::function<void(GfxDevice * pDevice)>& _canvasModifier(int i) const { return m_canvasModifiers[i-1]; }



        struct LayerSpec
        {
            PixelFormat format;
            
            std::function<void(GfxDevice * pDevice)>  initializer = nullptr;
            std::function<void(GfxDevice * pDevice)>  finalizer = nullptr;
            std::function<void(GfxDevice * pDevice)>  blender = nullptr;
        };

        int             m_defaultLayer = 0;                // 0 Means that the base layer (= canvas surface) is default.
        
        int             m_nbLayers;
        LayerSpec       m_layers[c_maxLayers];

        
        std::function<void(GfxDevice * pDevice)>  m_canvasInitializer = nullptr;
        std::function<void(GfxDevice * pDevice)>  m_canvasFinalizer = nullptr;

        std::function<void(GfxDevice * pDevice)>  m_canvasModifiers[c_maxLayers];
        
    };



} // namespace wg
#endif //WG_CANVASLAYERS_DOT_H
