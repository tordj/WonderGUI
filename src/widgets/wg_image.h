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
		//.____ Creation __________________________________________

		static Image_p	create() { return Image_p(new Image()); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static Image_p		cast( Object * pObject );

		//.____ Appearance ________________________________________________________

		void				setImage( Surface * pSurface, const RectI& rect );	///< @brief Set image to display.
		void				setImage( Surface * pSurface );						///< @brief Set image to display.

		inline Surface_p	imageSurface() const;								///< @brief Get surface of image displayed.
		inline RectI		imageRect() const;									///< @brief Get area of surface rectangle of image displayed.

	protected:
		Image();
		virtual ~Image();
		virtual Widget* _newOfMyType() const override { return new Image(); };

		SizeI		_preferredSize() const override;
		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		bool		_alphaTest( const CoordI& ofs ) override;

	private:

		Surface_p	m_pSurface;
		RectI		m_rect;

	};


	Surface_p Image::imageSurface() const
	{
		return m_pSurface;
	}

	RectI Image::imageRect() const
	{
		return m_rect;
	}



} // namespace wg
#endif //WG_FILLER_DOT_H
