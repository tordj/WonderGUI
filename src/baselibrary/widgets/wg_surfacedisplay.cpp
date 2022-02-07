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

#include <wg_surfacedisplay.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

namespace wg
{
	using namespace Util;

	const TypeInfo SurfaceDisplay::TYPEINFO = { "SurfaceDisplay", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	SurfaceDisplay::SurfaceDisplay()
	{
	}

	SurfaceDisplay::SurfaceDisplay(const Blueprint& bp)
	{
		_initFromBlueprint(bp);

		m_placement = bp.surfacePlacement;
		m_offset = bp.surfaceOffset;

		if (bp.zoomToFit)
		{
			m_bZoomToFit = true;
		}
		else 
		{
			m_bZoomToFit = false;
			m_zoom = bp.zoom;

			m_minZoom = bp.minUserZoom;
			m_maxZoom = bp.maxUserZoom;
		}

		if (bp.surface)
			setSurface(bp.surface);
	}


	//____ Destructor _____________________________________________________________

	SurfaceDisplay::~SurfaceDisplay()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SurfaceDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSurface() _______________________________________________________

	void SurfaceDisplay::setSurface(Surface* pSurface)
	{
		if (pSurface == m_pSurface)
			return;

		if (m_pSurface)
			m_pSurface->removeObserver(m_surfaceObserverId);

		m_pSurface = pSurface;

		_adjustZoom();
		_adjustOffset();

		if (pSurface)
			m_surfaceObserverId = pSurface->addObserver([this](int nRects, const RectSPX* pRects) 
				{
					this->_surfaceModified(nRects, pRects);
				});


		_requestRender();
	}

	//____ setZoomToFit() _____________________________________________________

	void SurfaceDisplay::setZoomToFit(bool activate)
	{
		if (activate != m_bZoomToFit)
		{
			m_bZoomToFit = activate;
			if (_adjustZoom())
				_requestRender();
		}
	}

	//____ setZoom() __________________________________________________________

	void SurfaceDisplay::setZoom(float factor)
	{
		m_bZoomToFit = false;
		if( factor != m_zoom )
		{
			m_zoom = factor;
			_adjustOffset();
			_requestResize();
			_requestRender();
		}
	}

	//____ setOffset() ________________________________________________________

	void SurfaceDisplay::setOffset(Coord offset)
	{
		m_offset = offset;
		if( _adjustOffset() )
			_requestRender();
	}

	//____ setUserZoomable() __________________________________________________

	void SurfaceDisplay::setUserZoomable(float min, float max)
	{
		m_bZoomToFit = false;
		m_minZoom = min;
		m_maxZoom = max;
	}

	//____ setSurfacePlacement() ______________________________________________

	void SurfaceDisplay::setSurfacePlacement(Placement placement)
	{
		m_placement = placement;
		if (_adjustOffset())
			_requestRender();
	}


	//____ _preferredSize() ___________________________________________________

	SizeSPX SurfaceDisplay::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);
		return _surfaceDisplayRect(scale) + m_skin.contentPaddingSize(scale);
	}

	//____ _matchingHeight() __________________________________________________

	spx SurfaceDisplay::_matchingHeight(spx width, int scale) const
	{
		scale = _fixScale(scale);

		auto padding = m_skin.contentPaddingSize(scale);
		auto wantedSize = _surfaceDisplayRect(scale);

		spx height;
		if (m_bZoomToFit)
		{
			width -= padding.w;
			height = align(wantedSize.h * width / wantedSize.w);
		}
		else
			height = wantedSize.h;


		height += padding.h;
		return height;
	}

	//____ _matchingWidth() ___________________________________________________

	spx SurfaceDisplay::_matchingWidth(spx height, int scale) const
	{
		scale = _fixScale(scale);

		auto padding = m_skin.contentPaddingSize(scale);
		auto wantedSize = _surfaceDisplayRect(scale);

		spx width;
		if (m_bZoomToFit)
		{
			height -= padding.h;
			width = align(wantedSize.w * height / wantedSize.h);
		}
		else
			width = wantedSize.w;

		width += padding.w;
		return width;
	}

	//____ _resize() __________________________________________________________

	void SurfaceDisplay::_resize(const SizeSPX& size, int scale)
	{
		Widget::_resize(size, scale);

		_adjustZoom();
		_adjustOffset();
	}

	//____ _render() __________________________________________________________

	void SurfaceDisplay::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		if (!m_pSurface)
		{
			Widget::_render(pDevice, _canvas, _window);
			return;
		}

		RectSPX canvas;
		ClipPopData clipPop;

		if (!m_skin.isEmpty())
		{
			m_skin.render(pDevice, _canvas, m_scale, m_state);
			canvas = _contentRect(_canvas);

			clipPop = limitClipList(pDevice, canvas);
		}
		else
			canvas = _canvas;

		RectSPX destRect = _surfaceDisplayRect(m_scale) + canvas.pos();


		pDevice->setBlitSource(m_pSurface);
		pDevice->stretchBlit( destRect );

		popClipList(pDevice, clipPop);
	}

	//____ _alphaTest() _______________________________________________________

	bool SurfaceDisplay::_alphaTest(const CoordSPX& ofs)
	{
		//TODO: Implement!!!

		return true;
	}

	//____ _surfaceModified() _________________________________________________

	void SurfaceDisplay::_surfaceModified(int nRects, const RectSPX* pRects)
	{
		auto contentRect = _contentRect();

		for (int i = 0; i < nRects; i++)
		{
			auto rectInCanvas = alignUp(_surfaceRectToCanvasRect(pRects[i]));
			_requestRender(RectSPX::getUnion(contentRect, rectInCanvas));
		}
	}

	//____ _surfaceRectToCanvasRect() _________________________________________

	RectSPX SurfaceDisplay::_surfaceRectToCanvasRect(const RectSPX& surfaceRect)
	{
		auto surfSize = m_pSurface->pixelSize();

		RectF relativeSurfaceRect = RectF(surfaceRect);
		relativeSurfaceRect /= SizeF(surfSize) * 64;

		RectSPX canvasRect = _surfaceDisplayRect(m_scale);

		canvasRect.x += canvasRect.w * relativeSurfaceRect.x;
		canvasRect.y += canvasRect.h * relativeSurfaceRect.y;
		canvasRect.w *= relativeSurfaceRect.w;
		canvasRect.h *= relativeSurfaceRect.h;

		return canvasRect;
	}

	//____ _surfaceDisplayRect() ______________________________________________

	RectSPX SurfaceDisplay::_surfaceDisplayRect(int scale) const
	{
		if (m_pSurface)
		{
			Size surfSize = m_pSurface->pointSize();

			SizeSPX size = align(ptsToSpx(surfSize * m_zoom, scale));
			CoordSPX pos = align( CoordSPX( m_size.w/2 - (size.w/2 + m_offset.x * size.w / surfSize.w), m_size.h/2 -(size.h/2 + m_offset.y * size.h / surfSize.h) ) );

			return { pos, size };
		}

		return SizeSPX();
	}

	//____ _adjustZoom() ______________________________________________________

	bool SurfaceDisplay::_adjustZoom()
	{
		if (m_bZoomToFit && m_pSurface)
		{
			RectSPX canvas = _contentRect();
			SizeSPX destRectSize = _surfaceDisplayRect(m_scale);

			float widthScaling = canvas.w / (float)destRectSize.w;
			float heightScaling = canvas.h / (float)destRectSize.h;

			float zoom = std::min(widthScaling, heightScaling);
			if (zoom != m_zoom)
			{
				m_zoom = zoom;
				return true;
			}
		}

		return false;
	}


	//____ _adjustOffset() ____________________________________________________

	bool SurfaceDisplay::_adjustOffset()
	{
		auto dispRect = _surfaceDisplayRect(m_scale);

		CoordSPX pos;

		if (dispRect.w < m_size.w)
		{
			switch (m_placement)
			{
				case Placement::West:
				case Placement::NorthWest:
				case Placement::SouthWest:
					pos.x = 0;
					break;

				case Placement::North:
				case Placement::Center:
				case Placement::South:
					pos.x = align((m_size.w - dispRect.w) / 2);
					break;

				case Placement::East:
				case Placement::NorthEast:
				case Placement::SouthEast:
					pos.x = m_size.w - dispRect.w;
					break;
			}
		}
		else
		{
			pos.x = dispRect.x;

			if (pos.x > 0)
				pos.x = 0;
			else if (pos.x + dispRect.w < m_size.w)
				pos.x = m_size.w - dispRect.w;
		}

		if (dispRect.h < m_size.h)
		{
			switch (m_placement)
			{
			case Placement::West:
			case Placement::NorthWest:
			case Placement::SouthWest:
				pos.y = 0;
				break;

			case Placement::North:
			case Placement::Center:
			case Placement::South:
				pos.y = align((m_size.h - dispRect.h) / 2);
				break;

			case Placement::East:
			case Placement::NorthEast:
			case Placement::SouthEast:
				pos.y = m_size.h - dispRect.h;
				break;
			}
		}
		else
		{
			pos.y = dispRect.y;

			if (pos.y > 0)
				pos.y = 0;
			else if (pos.y + dispRect.h < m_size.h)
				pos.y = m_size.h - dispRect.h;
		}

		if (pos != dispRect.pos())
		{
			if (pos.x != dispRect.x)
				m_offset.x = -(m_pSurface->pointWidth() * pos.x / dispRect.w);

			if (pos.y != dispRect.y)
				m_offset.y = -(m_pSurface->pointHeight() * pos.y / dispRect.h);

			return true;		// Offset modified
		}

		return false;			// Offset not modified
	}


} // namespace wg
