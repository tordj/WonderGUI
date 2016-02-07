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

namespace wg 
{
	
	class ColorSkin;
	
	typedef	StrongPtr<ColorSkin,Skin_p>	ColorSkin_p;
	
	
	class ColorSkin : public Skin
	{
	public:
		static ColorSkin_p create( Color col );
		~ColorSkin() {};
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ColorSkin_p	cast( const Object_p& pObject );
	
		void	render( GfxDevice * pDevice, const Rect& _canvas, State state, const Rect& _clip ) const;
		bool	isOpaque() const;
		bool	isOpaque(State state) const;
		bool	isOpaque( const Rect& rect, const Size& canvasSize, State state ) const;
	
		Size	minSize() const;
		Size	preferredSize() const;
	
		Size	contentPadding() const;
		Size	sizeForContent( const Size contentSize ) const;
		Rect	contentRect( const Rect& canvas, State state ) const;
	
		bool	markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const;
	
		bool	isStateIdentical( State state, State comparedTo ) const;
	
	private:
		ColorSkin( Color col );
	
		Color		m_color;
		
	};
	

} // namespace wg
#endif //WG_COLORSKIN_DOT_H
