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
#ifndef WG_MODBITMAPITEM_DOT_H
#define WG_MODBITMAPITEM_DOT_H
#pragma once

#include <functional>

#include <wg_types.h>
#include <wg_item.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_color.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>


namespace wg
{
	class ModBitmap;


	//____ ModBitmapItem _____________________________________________________________

	class ModBitmapItem : public Item	/** @private */
	{
		friend class ModBitmap;

	public:
		ModBitmapItem(ItemHolder * pHolder, ModBitmap * pInterface);
		virtual ~ModBitmapItem() {};

		void		render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip);
		bool		alphaTest(const Coord& ofs, int markOpacity);


		void			regenSurface();
		Rect			calcPresentationArea() const;


		bool			setDevice(GfxDevice * pDevice);
		bool			setSurfaceFactory(SurfaceFactory * pFactory);
		void			setLostCallback(std::function<void(ModBitmap*)> func);
		bool			setPixelType(PixelType format);
		bool			setBitmapSize(Size sz);
		void			setItemSize(Size sz);
		void			setBackColor(Color color);
		void			clear();
		void			setPresentationScaling(SizePolicy2D policy);
		void			setOrigo(Origo origo);
		void			present();
		void			present(Rect area);
		Size			preferredSize() const;


	protected:
		GfxDevice_p		m_pDevice;
		SurfaceFactory_p m_pFactory;
		Surface_p		m_pSurface;
		Size			m_fixedSize;
		PixelType		m_pixelType				= PixelType::BGR_8;
		SizePolicy2D	m_presentationScaling	= SizePolicy2D::Default;
		Origo			m_origo					= Origo::NorthWest;
		Color			m_backColor				= Color::White;
		ModBitmap *		m_pInterface;

		std::function<void(ModBitmap*)>	m_bitmapLostCallback;

	};
}

#endif //WG_MODBITMAPITEM_DOT_H

