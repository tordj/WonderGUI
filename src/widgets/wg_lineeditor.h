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

#ifndef	WG_LINEEDITOR_DOT_H
#define	WG_LINEEDITOR_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_edittext.h>

namespace wg 
{
	
	class LineEditor;
	typedef	StrongPtr<LineEditor,Widget_p>		LineEditor_p;
	typedef	WeakPtr<LineEditor,Widget_wp>	LineEditor_wp;
	
	class LineEditor:public Widget
	{
	public:
		//.____ Creation __________________________________________

		static LineEditor_p	create() { return LineEditor_p(new LineEditor()); }
	
		//.____ Components _______________________________________
	
		EditText		text;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LineEditor_p	cast( Object * pObject );
	
	
		//.____ Geometry ____________________________________________
	
		Size			preferredSize() const;
		
	protected:
		LineEditor();
		virtual ~LineEditor();
		virtual Widget* _newOfMyType() const { return new LineEditor(); };
	
		void			_cloneContent( const Widget * _pOrg );
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_setSize( const Size& size );
		void			_refresh();
		void			_receive( Msg * pMsg );
		void			_setState( State state );
		void			_setSkin( Skin * pSkin );

		Coord			_itemPos( const Item * pItem ) const;
		Size			_itemSize( const Item * pItem ) const;
		Rect			_itemGeo( const Item * pItem ) const;

		void			_itemRenderRequested(const Item * pItem);
		void			_itemRenderRequested( const Item * pItem, const Rect& rect );
		void			_itemResizeRequested( const Item * pItem );
		void			_itemVisibilityRequested(const Item * pItem, const Rect& preferred, const Rect& prio);

	private:
		
		EditTextItem	m_text;
		int				m_textScrollOfs;
	};
	
	
	
	
	

} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
