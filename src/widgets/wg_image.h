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


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

namespace wg 
{
	
	class Image;
	typedef	WgStrongPtr<Image,Widget_p>		Image_p;
	typedef	WgWeakPtr<Image,Widget_wp>		Image_wp;
	
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
		static Image_p	create() { return Image_p(new Image()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Image_p	cast( const Object_p& pObject );
		
	
		void				setImage( const Surface_p& pSurface, const Rect& rect );	///< @brief Set image to display.
		void				setImage( const Surface_p& pSurface );						///< @brief Set image to display.
	
		inline Surface_p	imageSurface() const;											///< @brief Get surface of image displayed.
		inline Rect		imageRect() const;												///< @brief Get area of surface rectangle of image displayed.
	
		Size				preferredSize() const;
	
	protected:
		Image();
		virtual ~Image();
		virtual Widget* _newOfMyType() const { return new Image(); };
	
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		bool	_onAlphaTest( const Coord& ofs, const Size& sz );
	
	private:
	
		Surface_p	m_pSurface;
		Rect			m_rect;
	
	};
	
	
	Surface_p Image::imageSurface() const
	{
		return m_pSurface;
	}
	
	Rect Image::imageRect() const
	{
		return m_rect;
	}
	
	

} // namespace wg
#endif //WG_FILLER_DOT_H
