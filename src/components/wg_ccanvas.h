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

	class CCanvas;
	typedef	StrongComponentPtr<CCanvas>	CCanvas_p;
	typedef	WeakComponentPtr<CCanvas>	CCanvas_wp;


	//____ CCanvas _____________________________________________________________
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

	class CCanvas : public GeoComponent
	{
		friend class ICanvas;

	public:
		CCanvas(GeoComponent::Holder * pHolder);
		virtual ~CCanvas() {};

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________

		bool				setDevice(GfxDevice * pDevice);
		inline GfxDevice_p	device() const;

		bool				setFactory(SurfaceFactory * pFactory);
		inline SurfaceFactory_p	factory() const;

		void				setLostCallback(std::function<void(CCanvas*)> func);
		inline std::function<void(CCanvas*)>	lostCallback() const;

		void				clear();

		void				present();
		void				present(RectI pixelArea);

		//.____ Content _______________________________________________

		bool				setPixelFormat(PixelFormat format);
		inline PixelFormat	pixelFormat() const;

		bool				setSize(SizeI pixelSize);
		inline bool			isSizeFixed() const;
		inline SizeI		size() const;

		void				setBackColor(Color color);
		inline Color		backColor() const;

		inline Surface_wp	surface() const;

		//.____ Appearance _____________________________________________

		void				setPresentationScaling(SizePolicy2D policy);
		inline SizePolicy2D	presentationScaling() const;

		void				setOrigo(Origo origo);
		inline Origo		origo() const;


		//.____ Misc __________________________________________________

		inline CCanvas_p	ptr() { return CCanvas_p(this); }


	protected:

		void			_render(GfxDevice * pDevice, const Rect& _canvas);
		bool			_alphaTest(const Coord& ofs, int markOpacity);

		void			_regenSurface();
		Rect			_calcPresentationArea() const;

		void			_setComponentSize(Size sz);
		Size			_preferredSize() const;


		GfxDevice_p		m_pDevice;
		SurfaceFactory_p m_pFactory;
		Surface_p		m_pSurface;
		SizeI			m_fixedSize;			// Fixed size of surface in pixels if set.
		PixelFormat		m_pixelFormat			= PixelFormat::BGR_8;
		SizePolicy2D	m_presentationScaling	= SizePolicy2D::Original;
		Origo			m_origo					= Origo::NorthWest;
		Color			m_backColor				= Color::White;

		std::function<void(CCanvas*)>	m_surfaceLostCallback;

	};


	//____ device() ___________________________________________________________
	/**
	*	@brief	Gets the GfxDevice previously set.
	*
	**/

	GfxDevice_p CCanvas::device() const
	{
		return m_pDevice;
	}

	//____ factory() ___________________________________________________
	/**
	*	@brief	Gets the specificly set SurfaceFactory.
	*
	*	Gets the SurfaceFactory specificly set using setSurfaceFactory().
	*   This method will not return the SurfaceFactory implicitly provided by the GfxDevice.
	*
	*	@return SurfaceFactory specificly set using setSurfaceFactory() or nullptr if none.
	**/

	SurfaceFactory_p CCanvas::factory() const
	{
		return m_pFactory;
	}

	//____ lostCallback() ______________________________________________
	/**
	*	@brief	Gets the callback previously set.
	*
	**/

	std::function<void(CCanvas*)> CCanvas::lostCallback() const
	{
		return m_surfaceLostCallback;
	}

	//____ pixelFormat() ________________________________________________________
	/**
	*	@brief	Gets the pixel format of the surface.
	*
	*	Gets the pixel format of the surface. Please note that you will get a valid
	*   return value even if no surface exists due to lack of SurfaceFactory.
	*
	*	@return Pixel format of the surface, which is guaranteed to be one of BGR_8, BGRA_8, BGR_16
	*	or BGRA_16.
	**/

	PixelFormat CCanvas::pixelFormat() const
	{
		return m_pixelFormat;
	}

	//____ isSizeFixed() ______________________________________________________
	/**
	*	@brief Check if surface has a fixed size.
	*
	*	Check if surface has a fixed size.
	*
	*	@return True if surface has a fixed size.
	**/

	bool CCanvas::isSizeFixed() const
	{
		return m_fixedSize == SizeI(0, 0) ? false : true;
	}

	//____ size() _____________________________________________________________
	/**
	*	@brief Gets the dimensions of the surface.
	*
	*	Gets the dimensions of the surface in pixels, which might be different from the dimensions
	*	of this CCanvas component if a fixed size has been set.
	*
	*	@return SizeI of surface OR specified fixed size if no surface present and fixed
	*			size has been set OR {0,0} if neither.
	**/

	SizeI CCanvas::size() const
	{
		return m_pSurface ? m_pSurface->size() : m_fixedSize;
	}

	//____ backColor() ________________________________________________________
	/**
	*	@brief Gets the back color of the surface.
	*
	*	Gets the back color SPECIFIED for the surface, which the surface is filled with when generated. This
	*	might be of higher fidelity than what is actually used if the surface has lower color resolution than
	*	eight bits per channel. Also note that alpha values are ignored for surfaces without alpha channel.
	*
	*	@return	Back color as specified for the surface.
	**/
	Color CCanvas::backColor() const
	{
		return m_backColor;
	}

	//____ presentationScaling() ______________________________________________
	/**
	*	@brief Gets how a fixed size surface is scaled.
	*
	*	Gets how a fixed size surface is scaled to cover the CCanvas components area.
	**/

	SizePolicy2D CCanvas::presentationScaling() const
	{
		return m_presentationScaling;
	}

	//____ origo() ____________________________________________________________
	/**
	*	@brief Gets origo used for a fixed size surface
	*
	*	Gets the origo used to align the surface when it is of a different size than the CCanvas.
	*
	*	@return	Origo used for alignment of surface
	**/
	Origo CCanvas::origo() const
	{
		return m_origo;
	}

	//____ surface() ____________________________________________________________
	/**
	*	@brief	Gets a weakpointer to the surface
	*
	*	Gets a pointer to the Surface presented by this component. Please note
	*	that the pointer returned is a weakpointer and will turn to null when
	*	the surface is discared by CCanvas.
	*
	*	Converting the returned weakpointer to a strongpointer will preserve the
	*	Surface, but will be different from the one used by CCanvas after it has
	*	been discarded.
	*
	*	@return	Pointer to surface or nullptr if no surface present.
	**/

	Surface_wp CCanvas::surface() const
	{
		return m_pSurface.rawPtr();
	}


}

#endif //WG_CCANVAS_DOT_H

