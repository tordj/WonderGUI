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
#ifndef WG_MODBITMAP_DOT_H
#define WG_MODBITMAP_DOT_H
#pragma once


#include <wg_modbitmapitem.h>

namespace wg
{
	class ModBitmap;
	typedef	StrongInterfacePtr<ModBitmap>	ModBitmap_p;
	typedef	WeakInterfacePtr<ModBitmap>		ModBitmap_wp;

	//____ ModBitmap __________________________________________________________

	class ModBitmap : public Interface
	{
	public:
		/** @private */

		ModBitmap(ModBitmapItem* pItem) : m_pItem(pItem) {}

		bool			setDevice(GfxDevice_p pDevice) { return m_pItem->setDevice(pDevice); }
		GfxDevice_p		device() const { return m_pItem->m_pDevice; }

		void			setBitmapLostCallback(std::function<void(ModBitmap*)> func) { m_pItem->setLostCallback(func); }
		std::function<void(ModBitmap*)>	bitmapLostCallback() const { return m_pItem->m_bitmapLostCallback; }

		bool			setPixelType(PixelType format) { return m_pItem->setPixelType(format); }
		PixelType		pixelType() const { return m_pItem->m_pixelType; }

		bool			setSize(Size sz) { return m_pItem->setBitmapSize(sz); }
		bool			isSizeFixed() const { return m_pItem->m_fixedSize == Size(0, 0) ? false : true; }
		Size			size() const { return m_pItem->m_pSurface ? m_pItem->m_pSurface->size() : m_pItem->m_fixedSize; }

		void			setBackColor(Color color) { m_pItem->setBackColor(color); }
		Color			backColor() const { return m_pItem->m_backColor; }

		void			setPresentationScaling(SizePolicy2D policy) { m_pItem->setPresentationScaling(policy); }
		SizePolicy2D	presentationScaling() const { return m_pItem->m_presentationScaling; }

		void			setOrigo(Origo origo) { m_pItem->setOrigo(origo); }
		Origo			origo() const { return m_pItem->m_origo; }

		void			present() { m_pItem->present(); }
		void			present(Rect area) { m_pItem->present(area); }

		Surface_wp		surface() const { return m_pItem->m_pSurface.rawPtr(); }

	protected:
		virtual Object * 	_object() const { return m_pItem->_object(); }

		ModBitmapItem * m_pItem;

	};
} // namespace wg

#endif //WG_MODBITMAP_DOT_H
