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
#ifndef WG_CCANVAS_DOT_H
#define WG_CCANVAS_DOT_H
#pragma once

#include <functional>

#include <wg_types.h>
#include <wg_geocomponent.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_color.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>


namespace wg
{
	class ICanvas;


	//____ CCanvas _____________________________________________________________

	class CCanvas : public GeoComponent	/** @private */
	{
		friend class ICanvas;

	public:
		CCanvas(GeoComponent::Holder * pHolder, ICanvas * pInterface);
		virtual ~CCanvas() {};

		void			render(GfxDevice * pDevice, const RectI& _canvas);
		bool			alphaTest(const CoordI& ofs, int markOpacity);


		void			regenSurface();
		RectI			calcPresentationArea() const;


		bool			setDevice(GfxDevice * pDevice);
		bool			setSurfaceFactory(SurfaceFactory * pFactory);
		void			setLostCallback(std::function<void(ICanvas*)> func);
		bool			setPixelFormat(PixelFormat format);
		bool			setSurfaceSize(SizeI szPixels);
		void			setComponentSize(SizeI sz);
		void			setBackColor(Color color);
		void			clear();
		void			setPresentationScaling(SizePolicy2D policy);
		void			setOrigo(Origo origo);
		void			present();
		void			present(RectI area);
		SizeI			preferredSize() const;


	protected:
		GfxDevice_p		m_pDevice;
		SurfaceFactory_p m_pFactory;
		Surface_p		m_pSurface;
		SizeI			m_fixedSize;			// Fixed size of surface in pixels if set.
		PixelFormat		m_pixelFormat			= PixelFormat::BGR_8;
		SizePolicy2D	m_presentationScaling	= SizePolicy2D::Original;
		Origo			m_origo					= Origo::NorthWest;
		Color			m_backColor				= Color::White;
		ICanvas *	m_pComponent;

		std::function<void(ICanvas*)>	m_surfaceLostCallback;

	};
}

#endif //WG_CCANVAS_DOT_H

