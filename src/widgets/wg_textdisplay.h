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

#ifndef	WG_TEXTDISPLAY_DOT_H
#define	WG_TEXTDISPLAY_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_itextdisplay.h>

namespace wg 
{
	
	class TextDisplay;
	typedef	StrongPtr<TextDisplay>		TextDisplay_p;
	typedef	WeakPtr<TextDisplay>	TextDisplay_wp;
	
	class TextDisplay:public Widget
	{
	public:

		//.____ Creation __________________________________________

		static TextDisplay_p	create() { return TextDisplay_p(new TextDisplay()); }

		//.____ Interfaces ____________________________________

		ITextDisplay			text;
	
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static TextDisplay_p	cast( Object * pObject );
	
		//.____ Geometry ____________________________________________

		int				matchingWidth( int height ) const;
		int				matchingHeight( int width ) const;
		Size			preferredSize() const;

	
		//.____ Appearance _________________________________________________
	
		PointerStyle	pointerStyle() const;
		String			tooltipString() const;
	
	
	protected:
		TextDisplay();
		virtual ~TextDisplay();
		virtual Widget* _newOfMyType() const { return new TextDisplay(); };
	
		void			_cloneContent( const Widget * _pOrg );
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window );
		void			_setSize( const Size& size );
		void			_refresh();
		void			_receive( Msg * pMsg );
		void			_setState( State state );
		void			_setSkin( Skin * pSkin );
	
	private:
	
		CTextDisplay		m_text;
	};
	
	
	

} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
