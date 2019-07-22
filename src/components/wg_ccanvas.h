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
#include <wg_component.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_color.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>


namespace wg
{
	class ICanvas;


	//____ CCanvas _____________________________________________________________

	class CCanvas : public Component	/** @private */
	{
		friend class ICanvas;

	public:
		CCanvas(ComponentHolder * pHolder, ICanvas * pInterface);
		virtual ~CCanvas() {};

		void			render(GfxDevice * pDevice, const Rect& _canvas);
		bool			alphaTest(const Coord& ofs, int markOpacity);


		void			regenSurface();
		Rect			calcPresentationArea() const;


		bool			setDevice(GfxDevice * pDevice);
		bool			setSurfaceFactory(SurfaceFactory * pFactory);
		void			setLostCallback(std::function<void(ICanvas*)> func);
		bool			setPixelFormat(PixelFormat format);
		bool			setSurfaceSize(Size sz);
		void			setComponentSize(Size sz);
		void			setBackColor(Color color);
		void			clear();
		void			setPresentationScaling(SizeQolicy2D policy);
		void			setOrigo(Origo origo);
		void			present();
		void			present(Rect area);
		Size			preferredSize() const;


	protected:
		GfxDevice_p		m_pDevice;
		SurfaceFactory_p m_pFactory;
		Surface_p		m_pSurface;
		Size			m_fixedSize;
		PixelFormat		m_pixelFormat			= PixelFormat::BGR_8;
		SizeQolicy2D	m_presentationScaling	= SizeQolicy2D::Original;
		Origo			m_origo					= Origo::NorthWest;
		Color			m_backColor				= Color::White;
		ICanvas *	m_pInterface;

		std::function<void(ICanvas*)>	m_surfaceLostCallback;

	};
}

#endif //WG_CCANVAS_DOT_H

