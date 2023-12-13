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



				pDevice->setBlitSource(pSurface);
				pDevice->stretchBlit(_canvasWindow(dest) );
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
