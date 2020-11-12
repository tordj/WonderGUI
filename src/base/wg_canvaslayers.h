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

#include <initializer_list>
#include <utility>

namespace wg
{

	class CanvasLayers;
	typedef	StrongPtr<CanvasLayers>	CanvasLayers_p;
	typedef	WeakPtr<CanvasLayers>	CanvasLayers_wp;

	//____ Class CanvasLayers ________________________________________________________

	class	CanvasLayers : public Object	
	{
	public:
		//.____ Creation __________________________________________

		static CanvasLayers_p	create(const std::initializer_list<std::pair<BlendMode, PixelFormat>>& extraLayers);

		//.____ Identification _________________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		inline int				size() const { return m_nbLayers; }

		PixelFormat				layerFormat(int layer) const;
		BlendMode				layerBlendMode(int layer) const;

		void					setDefaultLayer(int layer);
		inline int				defaultLayer() const { return m_defaultLayer;  }


		//.____ Internal ______________________________________________________

		inline PixelFormat _layerFormat(int i) const { return m_formats[i]; }
		inline BlendMode _layerBlendMode(int i) const { return m_blendModes[i]; }

		const static int	c_maxLayers = 16;

	protected:
		CanvasLayers(const std::initializer_list<std::pair<BlendMode, PixelFormat>>& extraLayers);


		int					m_nbLayers;
		int					m_defaultLayer = 0;
		PixelFormat			m_formats[c_maxLayers];
		BlendMode			m_blendModes[c_maxLayers];

	};



} // namespace wg
#endif //WG_CANVASLAYERS_DOT_H
