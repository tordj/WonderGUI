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

#include <wg_canvascapsule.h>
#include <wg_gfxdevice.h>
#include <wg_base.h>
#include <wg_util.h>

namespace wg
{
	const TypeInfo CanvasCapsule::TYPEINFO = { "CanvasCapsule", &Capsule::TYPEINFO };

	using namespace Util;

	//____ constructor ____________________________________________________________

	CanvasCapsule::CanvasCapsule()
	{
	}

	//____ destructor _____________________________________________________________

	CanvasCapsule::~CanvasCapsule()
	{
	}

	//____ typeInfo() ____________________________________________________________

	const TypeInfo& CanvasCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSkin() ___________________________________________________________

	void CanvasCapsule::setSkin(Skin* pSkin)
	{
		if( !m_bScaleCanvas )
		{
			SizeSPX oldBorderSize = m_skin.contentBorderSize(m_scale);
			SizeSPX newBorderSize = pSkin ? pSkin->_contentBorderSize(m_scale) : SizeSPX();
			
			if( oldBorderSize != newBorderSize )
			{
				m_pCanvas = nullptr;
				m_canvasSize = _contentRect();
			}
		}
		
		Capsule::setSkin(pSkin);
	}


	//____ setTintColor() ______________________________________________________

	void CanvasCapsule::setTintColor(HiColor color, BlendMode mode)
	{
		if (color != m_tintColor || mode != m_tintMode)
		{
			m_tintMode = mode;
			m_tintColor = color;
			_requestRender();
		}
	}

	//____ setRenderMode() __________________________________________________________

	void CanvasCapsule::setRenderMode(BlendMode mode)
	{
		if (mode != m_renderMode)
		{
			m_renderMode = mode;
			_requestRender();
		}
	}

	//____ setSurfaceFactory() _________________________________________________

	void CanvasCapsule::setSurfaceFactory(SurfaceFactory* pFactory)
	{
		if (pFactory != m_pFactory)
		{
			m_pFactory = pFactory;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasFormat() _____________________________________________________

	void CanvasCapsule::setCanvasFormat(PixelFormat format)
	{
		if (format != m_canvasFormat)
		{
			m_canvasFormat = format;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasLayers() ___________________________________________________

	void CanvasCapsule::setCanvasLayers(CanvasLayers * pLayers)
	{
		if( pLayers != m_pCanvasLayers )
		{
			m_pCanvasLayers = pLayers;
			_requestRender();
		}
	}

	//____ setRenderLayer() ___________________________________________________

	void CanvasCapsule::setRenderLayer(int renderLayer)
	{
		if (renderLayer != m_renderLayer)
		{
			m_renderLayer = renderLayer;
			_requestRender();
		}
	}

	//____ setScaleCanvas() ______________________________________________________

	void CanvasCapsule::setScaleCanvas(bool bScale)
	{
		if( bScale != m_bScaleCanvas )
		{
			m_bScaleCanvas = bScale;
			_resizeCanvasAndChild();
		}
	}

	//____ _resizeCanvasAndChild() _______________________________________________

	void CanvasCapsule::_resizeCanvasAndChild()
	{
		SizeSPX newSize;
		if( m_bScaleCanvas )
		{
			if( slot.widget() )
				newSize = slot.widget()->_defaultSize(m_scale);
			else
				newSize.clear();
		}
		else
			newSize = _contentRect();

		if( newSize != m_canvasSize )
		{
			m_canvasSize = newSize;
			m_pCanvas = nullptr;
			if( slot.widget() )
				slot.widget()->_resize(newSize, m_scale);
		}
	}

	//____ _render() _____________________________________________________________

	void CanvasCapsule::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		m_skin.render(pDevice, _canvas, m_scale, m_state);

		RectSPX contentRect = _contentRect(_canvas);

		// Possibly regenerate the canvas

		if (!m_pCanvas)
		{
			SurfaceFactory* pFactory = m_pFactory ? m_pFactory : Base::defaultSurfaceFactory();
			if (!pFactory)
			{
				//TODO: Error handling!
				return;
			}

			SizeI pixelSize = m_canvasSize / 64;
			m_pCanvas = pFactory->createSurface( WGBP(Surface, _.size = pixelSize, _.format = m_canvasFormat, 
													  _.canvas = true, _.scale = m_scale ));
			m_patches.clear();
			m_patches.add(m_canvasSize);
		}

		// Render children into canvas surface
		
		pDevice->beginCanvasUpdate(m_pCanvas, m_patches.size(), m_patches.begin(), m_pCanvasLayers, m_renderLayer);
		
		RectSPX canvas = {0,0, m_canvasSize};
		slot._widget()->_render(pDevice, canvas, canvas);

		pDevice->endCanvasUpdate();

		m_patches.clear();

		// Blit our canvas

		int		rl = pDevice->renderLayer();
		BlendMode bm = pDevice->blendMode();
		HiColor c = pDevice->tintColor();	

		if (m_renderLayer != -1)
			pDevice->setRenderLayer(m_renderLayer);

		pDevice->setBlendMode(m_renderMode);

		if (m_tintColor != HiColor::Undefined)
		{
			HiColor newCol = HiColor::blend(c, m_tintColor, m_tintMode);
			pDevice->setTintColor(newCol);
		}

		pDevice->setBlitSource(m_pCanvas);
		
		if( m_bScaleCanvas )
		{
			pDevice->stretchBlit( _canvasWindow(contentRect) );
		}
		else
			pDevice->blit(contentRect);

		pDevice->setTintColor(c);
		pDevice->setBlendMode(bm);
		pDevice->setRenderLayer(rl);
	}

	//____ _resize() _____________________________________________________________

	void CanvasCapsule::_resize(const SizeSPX& size, int scale)
	{
		if( !m_bScaleCanvas )
		{
			m_pCanvas = nullptr;
			m_canvasSize = _contentRect(size);
			Capsule::_resize(size, scale);
		}
		else
		{
			m_size = size;
			m_scale = scale;
		}
		
		
	}

	//____ _releaseChild() _______________________________________________________
	
	void CanvasCapsule::_releaseChild( StaticSlot * pSlot )
	{
		Capsule::_releaseChild(pSlot);
		m_canvasSize.clear();
		m_pCanvas = nullptr;
	}

	//____ _replaceChild() _______________________________________________________

	void CanvasCapsule::_replaceChild(StaticSlot * pSlot, Widget * pWidget)
	{
		Capsule::_replaceChild(pSlot, pWidget);
		_resizeCanvasAndChild();
	}

	//____ _childRequestRender() _________________________________________________

	void CanvasCapsule::_childRequestRender(StaticSlot* pSlot)
	{
		m_patches.clear();
		m_patches.push(m_canvasSize);
		Capsule::_childRequestRender(pSlot);
	}

	void CanvasCapsule::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		m_patches.add(rect);

		RectSPX dirt;

		RectSPX contentRect = _contentRect();
		
		if( m_bScaleCanvas )
		{
			float scaleFactor = std::min( { 1.f, contentRect.w / (float) m_canvasSize.w, contentRect.h / (float) m_canvasSize.h } );
			dirt = alignUp(rect*scaleFactor);
			
			dirt += _canvasWindow(contentRect).pos();
		}
		else
			dirt = rect + contentRect.pos();

		_requestRender(dirt);
//		_requestRender();
	}

	//____ _childRequestResize() _________________________________________________

	void CanvasCapsule::_childRequestResize( StaticSlot * pSlot )
	{
		if( m_bScaleCanvas )
		{
			auto def = pSlot->widget()->_defaultSize(m_scale);
			if( def != m_canvasSize )
			{
				m_canvasSize = def;
				m_pCanvas = nullptr;
				pSlot->widget()->_resize(def, m_scale);
				_requestRender();
			}
		}
		else
			Capsule::_requestResize();
	}

					   
					   
	//____ _canvasWindow() _______________________________________________________

	RectSPX CanvasCapsule::_canvasWindow( RectSPX window )
	{
		float scaleFactor = std::min( { 1.f, window.w / (float) m_canvasSize.w, window.h / (float) m_canvasSize.h } );
		
		SizeSPX canvasInWindowSize = align(SizeSPX( m_canvasSize * scaleFactor ));
		
		return Util::placementToRect(m_placement, window.size(), canvasInWindowSize) + window.pos();
	}

}
