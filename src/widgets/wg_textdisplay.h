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


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

namespace wg 
{
	
	class TextDisplay;
	typedef	StrongPtr<TextDisplay,Widget_p>		TextDisplay_p;
	typedef	WeakPtr<TextDisplay,Widget_wp>	TextDisplay_wp;
	
	class TextDisplay:public Widget
	{
	public:

		//.____ Creation __________________________________________

		static TextDisplay_p	create() { return TextDisplay_p(new TextDisplay()); }

		//.____ Components ____________________________________

		ModText			text;
	
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static TextDisplay_p	cast( const Object_p& pObject );
	
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
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_setSize( const Size& size );
		void			_refresh();
		void			_receive( const Msg_p& pMsg );
		void			_setState( State state );
		void			_setSkin( const Skin_p& pSkin );
	
	private:
	
		TextItem		m_text;
	};
	
	
	

} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
