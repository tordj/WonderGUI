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
	
	class WgBoxSkin;
	typedef	WgStrongPtr<WgBoxSkin,WgExtendedSkin_p>	WgBoxSkin_p;
	
	class WgBoxSkin : public WgExtendedSkin
	{
	public:
		static WgBoxSkin_p	create();
		static WgBoxSkin_p create( WgColor color, WgBorder frame, WgColor frameColor );
		~WgBoxSkin() {};
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgBoxSkin_p	cast( const WgObject_p& pObject );
	
		void	setColor( WgColor color );
		void	setFrameColor( WgColor color );
		void	setFrameThickness( WgBorder frame );
		void	setFrame( WgBorder frame, WgColor color );
	
		void	setStateColor( WgStateEnum state, WgColor color );
		void	setStateColor( WgStateEnum state, WgColor color, WgColor frameColor );
	
		void render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const;
		
		WgSize	minSize() const;
		WgSize	preferredSize() const;
	
		WgSize	sizeForContent( const WgSize contentSize ) const;
		bool	markTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const;
	
		bool	isOpaque() const;
		bool	isOpaque( WgState state ) const;
		bool	isOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const;
	
		bool	isStateIdentical( WgState state, WgState comparedTo ) const;
	
	private:
		WgBoxSkin();
		WgBoxSkin( WgColor color, WgBorder frame, WgColor frameColor );
		void	_updateOpaqueFlag();
	
		bool		m_bOpaque;
		WgColor		m_color[WG_NB_STATES];
		WgColor		m_frameColor[WG_NB_STATES];
	    WgBorder   m_frame;
	};
	

} // namespace wg
#endif //WG_BOXSKIN_DOT_H


