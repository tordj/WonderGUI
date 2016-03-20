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
#ifndef WG_TEXTSTYLE_DOT_H
#define WG_TEXTSTYLE_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_UTIL_DOT_H
#	include <wg_util.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#ifndef WG_TEXTLINK_DOT_H
#	include <wg_textlink.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

namespace wg 
{
	
	class TextStyle;
	typedef	StrongPtr<TextStyle,Object_p>		TextStyle_p;
	typedef	WeakPtr<TextStyle,Object_wp>		TextStyle_wp;
	
	struct TextAttr
	{
			Font_p			pFont;
			int				size;
			Color			color;
			Color			bgColor;
			TextDecoration	decoration;
			TextLink_p		pLink;
			Caret_p			pCaret;

			
			// TODO: Add flags for showing linespace, tab, CR, NBSP and both BREAK PERMITTED CHARACTERS.
	};
	
	
	//TODO: Add support for subscript and superscript font-modifiers.
	//TODO: Add support for extra spacing between characters.
	
	class TextStyle : public Object
	{
		friend class Char;
		friend class CharBuffer;
		friend class TextTool;

		
	public:
		static TextStyle_p	create() { return TextStyle_p(new TextStyle()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TextStyle_p	cast( const Object_p& pObject );
	
		inline TextStyle_h	handle() { return m_handle; }
	
		bool			setParent( const TextStyle_p& pParent );
		void			cascade();
	
		void			setFont( const Font_p& pFont );
		void			setLink( const TextLink_p& pLink );
		void			setCaret( const Caret_p& pCaret );
	
		void			setColor( Color color );
		void			setBgColor( Color color );
		void			setSize( int size );
		void			setDecoration( TextDecoration decoration );
	
		void			setColor( Color color, State state );
		void			setBgColor( Color color, State state );
		void			setSize( int size, State state );
		void			setDecoration( TextDecoration decoration, State state );
	
		void			clearFont();
		void			clearLink();
		void			clearCaret();
	
		void			clearColor();
		void			clearBgColor();
		void			clearSize();
		void			clearDecoration();
	
		void			clearColor( State state );
		void			clearBgColor( State state );
		void			clearSize( State state );
		void			clearDecoration( State state );
	
		inline Font_p			font() const;
		inline TextLink_p		link() const;
		inline Caret_p			caret() const;
		inline Color			color( State state ) const;
		inline 	Color			bgColor( State state ) const;
		inline int				size( State state ) const;
		inline TextDecoration 	decoration( State state ) const;
	
		inline Font_p			combFont() const;
		inline TextLink_p		combLink() const;
		inline Caret_p			combCaret() const;
		inline Color			combColor( State state ) const;
		inline Color			combBgColor( State state ) const;
		inline int				combSize( State state ) const;
		inline TextDecoration 	combDecoration( State state ) const;
	
		void			exportAttr( State state, TextAttr * pDest ) const;
		void			addToAttr( State state, TextAttr * pDest ) const;
	
	
	
	protected:
		TextStyle();
		virtual ~TextStyle();
	
		struct AttrSet
		{
			Font_p			pFont;
			int				size[StateEnum_Nb];
			Color			color[StateEnum_Nb];
			Color			bgColor[StateEnum_Nb];
			TextDecoration	decoration[StateEnum_Nb];
			TextLink_p		pLink;
			Caret_p			pCaret;

		};
	
		bool		_compareSets( AttrSet * pSet1, AttrSet * pSet2 );
		bool		_refreshComb();
		void		_clearSet( AttrSet * pSet );
	
		TextStyle_p	m_pParent;
		TextStyle *	m_pFirstChild;
		TextStyle *	m_pNextSibling;
		TextStyle *	m_pPrevSibling;
	
	
		AttrSet		m_specAttr;
		AttrSet		m_combAttr;
	
		TextStyle_h	m_handle;
	};
	
	
	//______________________________________________________________________________
	inline Font_p TextStyle::font() const
	{
		return m_specAttr.pFont;
	}
	
	//______________________________________________________________________________
	inline TextLink_p TextStyle::link() const
	{
		return m_specAttr.pLink;
	}

	//______________________________________________________________________________
	inline Caret_p TextStyle::caret() const
	{
		return m_specAttr.pCaret;
	}

	
	//______________________________________________________________________________
	inline Color TextStyle::color( State state ) const
	{
		return m_specAttr.color[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline 	Color TextStyle::bgColor( State state ) const
	{
		return m_specAttr.bgColor[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline int TextStyle::size( State state ) const
	{
		return m_specAttr.size[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline TextDecoration TextStyle::decoration( State state ) const
	{
		return m_specAttr.decoration[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	
	inline Font_p TextStyle::combFont() const
	{
		return m_combAttr.pFont;
	}
	
	//______________________________________________________________________________
	inline TextLink_p TextStyle::combLink() const
	{
		return m_combAttr.pLink;
	}

	//______________________________________________________________________________
	inline Caret_p TextStyle::combCaret() const
	{
		return m_combAttr.pCaret;
	}
	
	//______________________________________________________________________________
	inline Color TextStyle::combColor( State state ) const
	{
		return m_combAttr.color[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline Color TextStyle::combBgColor( State state ) const
	{
		return m_combAttr.bgColor[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline int TextStyle::combSize( State state ) const
	{
		return m_combAttr.size[Util::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline TextDecoration TextStyle::combDecoration( State state ) const
	{
		return m_combAttr.decoration[Util::_stateToIndex(state)];
	}
	
	

} // namespace wg
#endif //WG_TEXTSTYLE_DOT_H
