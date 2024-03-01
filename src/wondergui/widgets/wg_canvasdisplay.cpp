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

#include <wg_canvasdisplay.h>
#include <wg_canvascapsule.h>
#include <wg_gfxdevice.h>

namespace wg
{
	using namespace Util;

	const TypeInfo CanvasDisplay::TYPEINFO = { "CanvasDisplay", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	CanvasDisplay::CanvasDisplay()
	{
		m_defaultSize = Size(-1,-1);
	}

	//____ destructor _____________________________________________________________

	CanvasDisplay::~CanvasDisplay()
	{
		if (m_pCanvas)
			m_pCanvas->_removeSideDisplay(this);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CanvasDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setCanvas() ________________________________________________________

	void CanvasDisplay::setCanvas(CanvasCapsule* pCanvas)
	{
		if (pCanvas == m_pCanvas)
			return;

		if (m_pCanvas)
		{
			m_pCanvas->_removeSideDisplay(this);
		}

		m_pCanvas = pCanvas;

		SizeSPX newSize;
		if (pCanvas)
		{
			pCanvas->_addSideDisplay(this);
			newSize = pCanvas->_canvasSize();
		}

		_requestRender();

		if (newSize != m_canvasSize)
		{
			m_canvasSize = newSize;
			_requestResize();
		}
	}


	//____ setDefaultSize() _______________________________________________________

	void CanvasDisplay::setDefaultSize( const Size& size )
	{
		if( size != m_defaultSize )
		{
			m_defaultSize = size;
			_requestResize();
		}
	}

	//____ setTintColor() ______________________________________________________

	void CanvasDisplay::setTintColor(HiColor color, ColorTransition* pTransition)
	{
		if (color != m_tintColor)
		{
			if (pTransition)
			{
				if (!m_pTintColorTransition)
					_startReceiveUpdates();

				m_pTintColorTransition = pTransition;
				m_tintColorTransitionProgress = 0;
				m_startTintColor = m_tintColor;
				m_endTintColor = color;
			}
			else
			{
				if (m_pTintColorTransition)
					_stopReceiveUpdates();

				m_pTintColorTransition = nullptr;
				m_tintColor = color;
				_requestRender();
			}
		}
	}

	//____ setTintGradient() __________________________________________________

	void CanvasDisplay::setTintGradient(const Gradient& gradient, ColorTransition* pTransition)
	{
		if (gradient != m_gradient)
		{
			if (pTransition)
			{
				if (!m_pGradientTransition)
					_startReceiveUpdates();

				m_pGradientTransition = pTransition;
				m_gradientTransitionProgress = 0;
				m_startGradient = m_gradient;
				m_endGradient = gradient;
			}
			else
			{
				if (m_pGradientTransition)
					_stopReceiveUpdates();

				m_pGradientTransition = nullptr;
				m_gradient = gradient;
				_requestRender();
			}
			
		}
	}

	//____ setBlendMode() __________________________________________________________

	void CanvasDisplay::setBlendMode(BlendMode mode)
	{
		if (mode != m_blendMode)
		{
			m_blendMode = mode;
			_requestRender();
		}
	}

	//____ defaultSize() __________________________________________________________

	SizeSPX CanvasDisplay::_defaultSize(int scale) const
	{
		if (m_defaultSize.w >= 0 && m_defaultSize.h >= 0)
		{
			return SizeSPX(m_defaultSize*scale);
		}
		else if (m_pCanvas)
		{
			SizeSPX size = m_pCanvas->_canvasSize() / 64 * scale;
			size += m_skin.contentBorderSize(scale);
			return size;
		}
		else
			return Widget::_defaultSize(scale);
	}

	//____ _update() __________________________________________________________

	void CanvasDisplay::_update(int microPassed, int64_t microsecTimestamp)
	{
		if (m_pTintColorTransition)
		{
			int timestamp = m_tintColorTransitionProgress + microPassed;

			if (timestamp >= m_pTintColorTransition->duration())
			{
				m_tintColorTransitionProgress = 0;
				m_pTintColorTransition = nullptr;
				m_tintColor = m_endTintColor;

				if (m_tintColor != m_endTintColor)
				{
					m_tintColor = m_endTintColor;
					_requestRender();
				}

				_stopReceiveUpdates();
			}
			else
			{
				m_tintColorTransitionProgress = timestamp;
				HiColor color = m_pTintColorTransition->snapshot(timestamp, m_startTintColor, m_endTintColor);

				if (color != m_tintColor)
				{
					m_tintColor = color;
					_requestRender();
				}
			}
		}

		if (m_pGradientTransition)
		{
			int timestamp = m_gradientTransitionProgress + microPassed;

			if (timestamp >= m_pGradientTransition->duration())
			{
				m_gradientTransitionProgress = 0;
				m_pGradientTransition = nullptr;

				if (m_gradient != m_endGradient)
				{
					m_gradient = m_endGradient;
					_requestRender();
				}

				_stopReceiveUpdates();
			}
			else
			{
				m_gradientTransitionProgress = timestamp;

				Gradient gradient;

				gradient.topLeft = m_pGradientTransition->snapshot(timestamp, m_startGradient.topLeft, m_endGradient.topLeft);
				gradient.topRight = m_pGradientTransition->snapshot(timestamp, m_startGradient.topRight, m_endGradient.topRight);
				gradient.bottomLeft = m_pGradientTransition->snapshot(timestamp, m_startGradient.bottomLeft, m_endGradient.bottomLeft);
				gradient.bottomRight = m_pGradientTransition->snapshot(timestamp, m_startGradient.bottomRight, m_endGradient.bottomRight);

				if (gradient != m_gradient)
				{
					m_gradient = gradient;
					_requestRender();
				}
			}
		}
	}


	//____ render() ___________________________________________________________

	void CanvasDisplay::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		if (m_pCanvas)
		{
			auto dest = _contentRect(_canvas);

			if ( dest.w > 0 && dest.h > 0)
			{
				auto pSurface = m_pCanvas->_renderCanvas(pDevice);

				auto canvasArea = _canvasWindow(dest);
				
				// Blit our canvas

//				int		rl = pDevice->renderLayer();
				BlendMode bm = pDevice->blendMode();
				HiColor c = pDevice->tintColor();

//				if (m_renderLayer != -1)
//					pDevice->setRenderLayer(m_renderLayer);

				pDevice->setBlendMode(m_blendMode);

				pDevice->setTintColor(m_tintColor);

				if (m_gradient.isValid() )
					pDevice->setTintGradient(canvasArea, m_gradient);

				pDevice->setBlitSource(pSurface);
				pDevice->stretchBlit(canvasArea);

				if (m_gradient.isValid())
					pDevice->clearTintGradient();

				pDevice->setTintColor(c);
				pDevice->setBlendMode(bm);
//				pDevice->setRenderLayer(rl);
			}
		}
	}

	//____ _canvasReset() _____________________________________________________

	void CanvasDisplay::_canvasReset(SizeSPX canvasSize)
	{
		_requestRender();
		if (m_defaultSize.w < 0 && m_defaultSize.h < 0 && canvasSize != m_canvasSize)
		{
			m_canvasSize = canvasSize;
			_requestResize();
		}
	}

	//____ _canvasDirtyRect() _________________________________________________

	void CanvasDisplay::_canvasDirtyRect(const RectSPX rect)
	{
		RectSPX contentRect = _contentRect();
		RectSPX dirt;

		float scaleFactor = std::min({ 1.f, contentRect.w / (float)m_canvasSize.w, contentRect.h / (float)m_canvasSize.h });
		dirt = alignUp(rect * scaleFactor);
		dirt += _canvasWindow(contentRect).pos();

		_requestRender(dirt);
	}

	//____ _canvasWindow() _______________________________________________________

	RectSPX CanvasDisplay::_canvasWindow(RectSPX window) const
	{
		float scaleFactor = std::min({ 1.f, window.w / (float)m_canvasSize.w, window.h / (float)m_canvasSize.h });

		SizeSPX canvasInWindowSize = align(SizeSPX(m_canvasSize * scaleFactor));

		return Util::placementToRect(m_placement, window.size(), canvasInWindowSize) + window.pos();
	}




} // namespace wg
