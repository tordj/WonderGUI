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
#ifndef WG_RENDERLAYERCAPSULE_DOT_H
#define WG_RENDERLAYERCAPSULE_DOT_H
#pragma once

#include <wg_capsule.h>

namespace wg
{
	class RenderLayerCapsule;
	typedef	StrongPtr<RenderLayerCapsule>	RenderLayerCapsule_p;
	typedef	WeakPtr<RenderLayerCapsule>		RenderLayerCapsule_wp;

	//____ RenderLayerCapsule ______________________________________________________
	/**
	* @brief	A widget that sets the tint color and blend mode of a single child.
	*
	* The RenderLayerCapsule sets the tint color and blend mode, which is inherited by its
	* sub-widgets branch during the rendering process.
	*
	* This can for example be used to create fading effects when parts of the UI (dis)appears.
	*
	*/

	class RenderLayerCapsule : public Capsule
	{
	public:
		//.____ Creation __________________________________________

		static RenderLayerCapsule_p	create() { return RenderLayerCapsule_p(new RenderLayerCapsule()); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering _________________________________________________

		void	setRenderLayer(int layer);
		int		renderLayer() const { return m_renderLayer; }

	protected:
		RenderLayerCapsule();
		virtual ~RenderLayerCapsule();
		virtual Widget* _newOfMyType() const override { return new RenderLayerCapsule(); };

		void		_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void		_cloneContent(const Widget* _pOrg) override;

		int			m_renderLayer = -1;
	};


} // namespace wg
#endif //WG_SHADERCAPSULE_DOT_H
