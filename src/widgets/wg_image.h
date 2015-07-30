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

class WgImage;
typedef	WgStrongPtr<WgImage,WgWidget_p>		WgImage_p;
typedef	WgWeakPtr<WgImage,WgWidget_wp>		WgImage_wp;

//____ WgImage __________________________________________________________
/**
 * @brief	Widget for displaying an image.
 *
 * WgImage is a widget that displays an image.
 *
 **/

class WgImage : public WgWidget
{
public:
	static WgImage_p	create() { return WgImage_p(new WgImage()); }

	bool				isInstanceOf( const char * pClassName ) const;
	const char *		className( void ) const;
	static const char	CLASSNAME[];
	static WgImage_p	cast( const WgObject_p& pObject );
	

	void				SetImage( const WgSurface_p& pSurface, const WgRect& rect );	///< @brief Set image to display.
	void				SetImage( const WgSurface_p& pSurface );						///< @brief Set image to display.

	inline WgSurface_p	ImageSurface() const;											///< @brief Get surface of image displayed.
	inline WgRect		ImageRect() const;												///< @brief Get area of surface rectangle of image displayed.

	WgSize				preferredSize() const;

protected:
	WgImage();
	virtual ~WgImage();
	virtual WgWidget* _newOfMyType() const { return new WgImage(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );

private:

	WgSurface_p	m_pSurface;
	WgRect			m_rect;

};


WgSurface_p WgImage::ImageSurface() const
{
	return m_pSurface;
}

WgRect WgImage::ImageRect() const
{
	return m_rect;
}


#endif //WG_FILLER_DOT_H
