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
#ifndef WG_ICANVAS_DOT_H
#define WG_ICANVAS_DOT_H
#pragma once


#include <wg_ccanvas.h>

namespace wg
{
	class ICanvas;
	typedef	StrongInterfacePtr<ICanvas>	ICanvas_p;
	typedef	WeakInterfacePtr<ICanvas>	ICanvas_wp;

	//____ ICanvas __________________________________________________________
	/**
	* @brief Component displaying a surface on which rendering operations can be performed.
	*
	* The normal way to draw onto a CCanvas is to set a GfxDevice (reserved for
	* this CCanvas instance) through setDevice() and call its beginRender(), followed by
	* various render methods and endRender(). You can also retrieve a pointer to the surface,
	* lock it and modify the pixels directly. Either way you should call one of
	* the present() methods afterwards to make sure that the content of the canvas is
	* copied to the screen.
	*
	* The surface is replaced and all its content lost if its properties such as size and
	* PixelFormat is changed. A callback can be set to alert you when this happens, in which case
	* you should redraw the content.
	*
	* The size of the surface can be specified. If it is not specified, it will have the same size
	* as the CCanvas component and be replaced each time the size of the component is changed.
	*
	**/


	class ICanvas : public Interface
	{
	public:
		/** @private */

		ICanvas(CCanvas* pComponent) : m_pComponent(pComponent) {}

		bool			setDevice(GfxDevice * pDevice);
		GfxDevice_p		device() const;

		bool			setSurfaceFactory(SurfaceFactory * pFactory);
		SurfaceFactory_p	surfaceFactory() const;

		void			setSurfaceLostCallback(std::function<void(ICanvas*)> func);
		std::function<void(ICanvas*)>	surfaceLostCallback() const;

		bool			setPixelFormat(PixelFormat format);
		PixelFormat		pixelFormat() const;

		bool			setSize(Size sz);
		bool			isSizeFixed() const;
		Size			size() const;

		void			setBackColor(Color color);
		Color			backColor() const;

		void			setPresentationScaling(SizeQolicy2D policy);
		SizeQolicy2D	presentationScaling() const;

		void			setOrigo(Origo origo);
		Origo			origo() const;

		void			present();
		void			present(Rect area);

		Surface_wp		surface() const;

	protected:
		virtual Object * 	_object() const { return m_pComponent->_object(); }

		CCanvas * m_pComponent;

	};
} // namespace wg

#endif //WG_ICANVAS_DOT_H
