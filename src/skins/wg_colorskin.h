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
#ifndef WG_COLORSKIN_DOT_H
#define WG_COLORSKIN_DOT_H

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

class WgColorSkin;

typedef	WgStrongPtr<WgColorSkin,WgSkin_p>	WgColorSkin_p;


class WgColorSkin : public WgSkin
{
public:
	static WgColorSkin_p create( WgColor col );
	~WgColorSkin() {};

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgColorSkin_p	cast( const WgObject_p& pObject );

	void	render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const;
	bool	isOpaque() const;
	bool	isOpaque(WgState state) const;
	bool	isOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const;

	WgSize	minSize() const;
	WgSize	preferredSize() const;

	WgSize	contentPadding() const;
	WgSize	sizeForContent( const WgSize contentSize ) const;
	WgRect	contentRect( const WgRect& canvas, WgState state ) const;

	bool	markTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const;

	bool	isStateIdentical( WgState state, WgState comparedTo ) const;

private:
	WgColorSkin( WgColor col );

	WgColor		m_color;
	
};

#endif //WG_COLORSKIN_DOT_H
