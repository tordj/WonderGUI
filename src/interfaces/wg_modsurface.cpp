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

#include <wg_modsurface.h>

namespace wg
{
	//____ setDevice() ________________________________________________________
	/**
	*	@brief Sets the GfxDevice to be used for drawing operations
	*
	*	@param	pDevice		Pointer to the GfxDevice to be used to draw on the Surface.
	*
	*	The specified GfxDevice will be initialized to use the Surface of this component
	*	as its canvas and can be used to draw onto it. To draw onto the Surface you start
	*	with a call to the device's beginRender(), followed by the calls for your drawing operations and endRender().
	*	Finally you need to call the present() method of this ModSurface to make sure that what has been
	*	drawn is copied to the screen.
	*
	*	The device should be reserved for this ModSurface until replaced and not used for anything else. Calling this
	*	method is likely to discard your current Surface. You will need to specify a GfxDevice or SurfaceFactory for 
	*	a Surface to be generated and displayed.
	*
	*	@return	True if successful, False if device could not be set.
	**/

	bool ModSurface::setDevice(GfxDevice * pDevice) 
	{ 
		return m_pItem->setDevice(pDevice); 
	}

	//____ device() ___________________________________________________________
	/**
	*	@brief	Gets the GfxDevice previously set.
	*
	*	
	**/

	GfxDevice_p ModSurface::device() const 
	{ 
		return m_pItem->m_pDevice; 
	}

	//____ setSurfaceFactory() ________________________________________________
	/**
	*	@brief	Sets the SurfaceFactory used to create the Surface
	*
	*	@param	pFactory	Pointer to the SurfaceFactory to be used to create the Surface. If nullptr
	*						the default SurfaceFactory of the GfxDevice will be set.
	*
	*	Specifies the SurfaceFactory used to generate and regenerate the Surface. If different from the
	*   one already set, the current surface will be discarded and a new one created.
	*
	*	The SurfaceFactory only needs to be specified if you want a Surface of a different type than
	*   what is default for your specified GfxDevice or if you don't specify any GfxDevice.
	*
	*	@return	True if successfull. False if pFactory can't be used as a canvas by the current GfxDevice.
	**/

	bool ModSurface::setSurfaceFactory(SurfaceFactory * pFactory) 
	{ 
		return m_pItem->setSurfaceFactory(pFactory); 
	}

	//____ surfaceFactory() ___________________________________________________
	/**
	*	@brief	Gets the specificly set SurfaceFactory.
	*
	*	Gets the SurfaceFactory specificly set using setSurfaceFactory(). 
	*   This method will not return the SurfaceFactory implicitly provided by the GfxDevice.
	*
	*	@return SurfaceFactory specificly set using setSurfaceFactory() or nullptr if none.
	**/

	SurfaceFactory_p ModSurface::surfaceFactory() const 
	{ 
		return m_pItem->m_pFactory; 
	}

	//____ setSurfaceLostCallback() ___________________________________________
	/**
	*	@brief	Sets a callback for when the Surface has been discarded.
	*
	*	@param func		Function to be called. Nullptr is allowed and will simply remove
	*					previously set callback.
	*
	*	Sets a function that will be called when the Surface for this component has been discarded.
	*	The purpose is to redraw any graphics that has been lost.
	* 
	*	The new Surface has been created, filled with the background color and is ready to be
	*	drawn upon when the function is called.
	*  
	**/

	void ModSurface::setSurfaceLostCallback(std::function<void(ModSurface*)> func) 
	{ 
		m_pItem->setLostCallback(func); 
	}

	//____ surfaceLostCallback() ______________________________________________
	/**
	*	@brief	Gets the callback previously set.
	*
	**/

	std::function<void(ModSurface*)> ModSurface::surfaceLostCallback() const 
	{ 
		return m_pItem->m_surfaceLostCallback; 
	}

	//____ setPixelFormat() _____________________________________________________
	/**
	*	@brief Sets the pixel format for the surface.
	*
	*	@param	type	The pixel format to be used by the surface. This needs to
	*					be one of BGR_8, BGRA_8, BGR_16 or BGRA_16.
	*
	*	Sets the pixel format for the surface.
	*	Changing the pixel format will discard and recreate the surface, destroying all its content.
	*	Default pixel format for ModSurface is BGR_8, which means that no alpha channel is included.
	*
	*	@return True if pixel format was accepted, otherwise false.
	**/

	bool ModSurface::setPixelFormat(PixelFormat format) 
	{ 
		return m_pItem->setPixelFormat(format); 
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

	PixelFormat ModSurface::pixelFormat() const 
	{ 
		return m_pItem->m_pixelFormat; 
	}

	//____ setSize() __________________________________________________________
	/**
	*	@brief Sets a fixed size for the surface.
	*
	*	@param Size		The size for the surface, no smaller than {1,1} and no larger than
	*					allowed by the SurfaceFactory in use, or {0,0} to return the surface to dynamic
	*					resizing.
	*
	*	Sets a fixed size for the surface. When the surface has a fixed size it will not be discarded when
	*	resizing ModSurface. Instead it will be positioned and scaled within the ModSurface area according to the
	*	origo and presentation scaling settings.
	*
	*	Setting a fixed size different from the surfaces current size will discard the current surface.
	*
	*	@return		False if specified size was out of allowed range.
	**/

	bool ModSurface::setSize(Size sz) 
	{ 
		return m_pItem->setSurfaceSize(sz); 
	}

	//____ isSizeFixed() ______________________________________________________
	/**
	*	@brief Check if surface has a fixed size.
	*
	*	Check if surface has a fixed size.
	*
	*	@return True if surface has a fixed size.
	**/

	bool ModSurface::isSizeFixed() const 
	{ 
		return m_pItem->m_fixedSize == Size(0, 0) ? false : true; 
	}

	//____ size() _____________________________________________________________
	/**
	*	@brief Gets the dimensions of the surface.
	*
	*	Gets the dimensions of the surface, which might be different from the dimensions
	*	of this ModSurface component if a fixed size has been set.
	*
	*	@return Size of surface OR specified fixed size if no surface present and fixed 
	*			size has been set OR {0,0} if neither.
	**/

	Size ModSurface::size() const 
	{ 
		return m_pItem->m_pSurface ? m_pItem->m_pSurface->size() : m_pItem->m_fixedSize; 
	}

	//____ setBackColor() _____________________________________________________
	/**
	*	@brief Sets the back color of the surface.
	*
	*	@param Color	Background color for the surface.
	*
	*	Sets the back color of the the surface, which the surface is filled with when generated. The alpha
	*	value of the color will be respected IF THE SURFACE HAS AN ALPHA CHANNEL, allowing
	*	for transparent and semi-transparent background.
	*
	*	Default back color is White (0xFFFFFFFF). Changing the back color will discard the current surface.
	**/
	void ModSurface::setBackColor(Color color) 
	{ 
		m_pItem->setBackColor(color); 
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
	Color ModSurface::backColor() const 
	{ 
		return m_pItem->m_backColor; 
	}

	//____ setPresentationScaling() ___________________________________________
	/**
	*	@brief Sets how a fixed size surface is scaled.
	*
	*	@param	policy	Either Original, Stretch or Scale.
	*
	*	Specifies how a fixed size surface is stretched or scaled when the surface is 
	*	of a different size than the ModSurface component.
	*
	*	Default value is SizePolicy2D::Original.
	**/
	void ModSurface::setPresentationScaling(SizePolicy2D policy) 
	{ 
		m_pItem->setPresentationScaling(policy); 
	}

	//____ presentationScaling() ______________________________________________
	/**
	*	@brief Gets how a fixed size surface is scaled.
	*
	*	Gets how a fixed size surface is scaled to cover the ModSurface components area.
	**/

	SizePolicy2D ModSurface::presentationScaling() const 
	{ 
		return m_pItem->m_presentationScaling; 
	}

	//____ setOrigo() _________________________________________________________
	/**
	*	@brief Sets origo for a fixed size surface
	*
	*	Sets origo to be used to align the surface when it is of a different size than
	*	the ModSurface, which can only happen if it has been set to a fixed size.
	*
	**/

	void ModSurface::setOrigo(Origo origo) 
	{ 
		m_pItem->setOrigo(origo); 
	}

	//____ origo() ____________________________________________________________
	/**
	*	@brief Gets origo used for a fixed size surface
	*
	*	Gets the origo used to align the surface when it is of a different size than the ModSurface.
	*
	*	@return	Origo used for alignment of surface
	**/
	Origo ModSurface::origo() const 
	{ 
		return m_pItem->m_origo; 
	}

	//____ present() ____________________________________________________________
	/**
	*	@brief	Redraw the component with the content of the surface.
	*
	*	Marks the area of the component as dirty, forcing a redraw with the content
	*	of the surface during its next render update.
	*
	*	If only parts of the surface has been changed, you can use one or several calls to
	*	present(Rect) instead to increase performance.
	*
	**/

	void ModSurface::present()
	{ 
		m_pItem->present(); 
	}

	/**
	*	@brief	Redraw part of the component with the content of the surface.
	*
	*	@param	area	The area of the surface that has been modified and needs to be presented.
	*
	*	Marks the area of the component displaying the specified part of the surface as dirty,
	*	forcing a redraw with the content of the surface during its next render update.
	*
	**/
	void ModSurface::present(Rect area)
	{ 
		m_pItem->present(area); 
	}

	//____ surface() ____________________________________________________________
	/**
	*	@brief	Gets a weakpointer to the surface
	*
	*	Gets a pointer to the Surface presented by this component. Please note
	*	that the pointer returned is a weakpointer and will turn to null when
	*	the surface is discared by ModSurface.
	*
	*	Converting the returned weakpointer to a strongpointer will preserve the
	*	Surface, but will be different from the one used by ModSurface after it has
	*	been discarded.
	*
	*	@return	Pointer to surface or nullptr if no surface present.
	**/

	Surface_wp ModSurface::surface() const 
	{ 
		return m_pItem->m_pSurface.rawPtr(); 
	}

} // namespace wg

