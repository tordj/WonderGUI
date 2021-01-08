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

#include <wg_renderlayercapsule.h>
#include <wg_gfxdevice.h>
#include <wg_patches.h>
#include <wg_internal.h>

namespace wg
{
	using namespace Util;

	const TypeInfo RenderLayerCapsule::TYPEINFO = { "RenderLayerCapsule", &Capsule::TYPEINFO };

	//____ constructor ____________________________________________________________

	RenderLayerCapsule::RenderLayerCapsule()
	{
	}

	//____ Destructor _____________________________________________________________

	RenderLayerCapsule::~RenderLayerCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& RenderLayerCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setRenderLayer() ______________________________________________________

	void RenderLayerCapsule::setRenderLayer(int layer)
	{
		if (layer != m_renderLayer)
		{
			m_renderLayer = layer;
			_requestRender();
		}
	}

	//____ _render() ________________________________________________________

	void RenderLayerCapsule::_render(GfxDevice* pDevice, const Rect& _canvas, const Rect& _window)
	{
		// Render our skin

		OO(skin)._render(pDevice, _canvas, m_state);

		if (!slot._widget())
			return;

		int oldLayer = pDevice->renderLayer();
		pDevice->setRenderLayer(m_renderLayer);

		Rect canvas = OO(skin)._contentRect(_canvas, m_state);
		OO(slot._widget())->_render(pDevice, canvas, canvas);


		pDevice->setRenderLayer(oldLayer);
	}

	//____ _cloneContent() _______________________________________________________

	void RenderLayerCapsule::_cloneContent(const Widget* _pOrg)
	{
		Capsule::_cloneContent(_pOrg);

		RenderLayerCapsule* pOrg = (RenderLayerCapsule*)_pOrg;

		m_renderLayer = pOrg->m_renderLayer;
	}

} // namespace wg
