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
#ifndef WG_BOXSKIN_DOT_H
#define WG_BOXSKIN_DOT_H

#ifndef WG_EXTENDEDSKIN_DOT_H
#	include <wg_extendedskin.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

namespace wg 
{
	
	class BoxSkin;
	typedef	StrongPtr<BoxSkin,ExtendedSkin_p>	BoxSkin_p;
	
	class BoxSkin : public ExtendedSkin
	{
	public:
		static BoxSkin_p	create();
		static BoxSkin_p create( Color color, Border frame, Color frameColor );
		~BoxSkin() {};
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static BoxSkin_p	cast( const Object_p& pObject );
	
		void	setColor( Color color );
		void	setFrameColor( Color color );
		void	setFrameThickness( Border frame );
		void	setFrame( Border frame, Color color );
	
		void	setStateColor( StateEnum state, Color color );
		void	setStateColor( StateEnum state, Color color, Color frameColor );
	
		void render( GfxDevice * pDevice, const Rect& _canvas, State state, const Rect& _clip ) const;
		
		Size	minSize() const;
		Size	preferredSize() const;
	
		Size	sizeForContent( const Size contentSize ) const;
		bool	markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const;
	
		bool	isOpaque() const;
		bool	isOpaque( State state ) const;
		bool	isOpaque( const Rect& rect, const Size& canvasSize, State state ) const;
	
		bool	isStateIdentical( State state, State comparedTo ) const;
	
	private:
		BoxSkin();
		BoxSkin( Color color, Border frame, Color frameColor );
		void	_updateOpaqueFlag();
	
		bool		m_bOpaque;
		Color		m_color[StateEnum_Nb];
		Color		m_frameColor[StateEnum_Nb];
	    Border   m_frame;
	};
	

} // namespace wg
#endif //WG_BOXSKIN_DOT_H


