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
#ifndef WG_IMAGE_DOT_H
#define WG_IMAGE_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg
{

	class Image;
	typedef	StrongPtr<Image>		Image_p;
	typedef	WeakPtr<Image>		Image_wp;

	//____ Image __________________________________________________________
	/**
	 * @brief	Widget for displaying an image.
	 *
	 * Image is a widget that displays an image.
	 *
	 **/

	class Image : public Widget
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			Surface_p		image;
			Rect			imageRect;
			int				imageMarkAlpha = 1;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static Image_p	create() { return Image_p(new Image()); }
		static Image_p	create( const Blueprint& blueprint ) { return Image_p(new Image( blueprint )); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance ________________________________________________________

		void				setImage( Surface * pSurface, const Rect& rect );	///< @brief Set image to display.
		void				setImage( Surface * pSurface );						///< @brief Set image to display.

		inline Surface_p	imageSurface() const;								///< @brief Get surface of image displayed.
		inline Rect			imageRect() const;									///< @brief Get area of surface rectangle of image displayed.

		//.____ Behavior ______________________________________________________

		inline void			setImageMarkAlpha( int markAlpha );
		inline int			imageMarkAlpha() const;


		//.____ Internal ____________________________________________

		SizeSPX	_defaultSize(int scale) const override;


	protected:
		Image();
		Image( const Blueprint& bp );
		virtual ~Image();

		void		_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		bool		_alphaTest( const CoordSPX& ofs ) override;

	private:

		Surface_p	m_pSurface;
		Rect		m_rect;
		int			m_imageMarkAlpha = 1;
	};


	Surface_p Image::imageSurface() const
	{
		return m_pSurface;
	}

	Rect Image::imageRect() const
	{
		return m_rect;
	}

	void Image::setImageMarkAlpha(int markAlpha)
	{
		m_imageMarkAlpha = markAlpha;
	}

	int Image::imageMarkAlpha() const
	{
		return m_imageMarkAlpha;
	}


} // namespace wg
#endif //WG_IMAGE_DOT_H