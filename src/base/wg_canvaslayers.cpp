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

	CanvasLayers_p CanvasLayers::create(const std::initializer_list<std::pair<BlendMode, PixelFormat>>& extraLayers)
	{
		for (auto& layer : extraLayers)
		{
			PixelFormat format = layer.second;

			if (format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear)
			{
				Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Canvas layers can not be CLUT-based", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
				return false;
			}
		}

		return CanvasLayers_p(new CanvasLayers(extraLayers));
	}

	//____ constructor() ______________________________________________________

	CanvasLayers::CanvasLayers(const std::initializer_list<std::pair<BlendMode, PixelFormat>>& extraLayers)
	{
		m_nbLayers = extraLayers.size();

		int ofs = 0;
		for (auto& layer : extraLayers)
		{
			m_blendModes[ofs] = layer.first;
			m_formats[ofs] = layer.second;
			ofs++;
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
		if (layer < 0 || layer >= m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return PixelFormat::Unknown;
		}

		return m_formats[layer];
	}

	//____ layerBlendMode() ___________________________________________________

	BlendMode CanvasLayers::layerBlendMode(int layer) const
	{
		if (layer < 0 || layer >= m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return BlendMode::Undefined;
		}

		return m_blendModes[layer];
	}

	//___ setDefaultLayer() ___________________________________________________

	void CanvasLayers::setDefaultLayer(int layer)
	{
		if (layer < 0 || layer >= m_nbLayers)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Layer does not exist.", nullptr, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		m_defaultLayer = layer;
	}

}
