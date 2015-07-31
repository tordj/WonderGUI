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

#ifndef WG_CARET2_DOT_H
#	include <wg_caret2.h>
#endif

namespace wg 
{
	
	class WgTextStyle;
	typedef	WgStrongPtr<WgTextStyle,WgObject_p>		WgTextStyle_p;
	typedef	WgWeakPtr<WgTextStyle,WgObject_wp>		WgTextStyle_wp;
	
	struct WgTextAttr2
	{
			WgFont_p			pFont;
			int					size;
			WgColor				color;
			WgColor				bgColor;
			WgTextDecoration	decoration;
			WgTextLink_p		pLink;
			
			// TODO: Add flags for showing linespace, tab, CR, NBSP and both BREAK PERMITTED CHARACTERS.
	};
	
	
	//TODO: Add support for subscript and superscript font-modifiers.
	//TODO: Add support for extra spacing between characters.
	
	class WgTextStyle : public WgObject
	{
	public:
		static WgTextStyle_p	create() { return WgTextStyle_p(new WgTextStyle()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgTextStyle_p	cast( const WgObject_p& pObject );
	
		bool			setParent( const WgTextStyle_p& pParent );
		void			cascade();
	
		void			setFont( const WgFont_p& pFont );
		void			setLink( const WgTextLink_p& pLink );
	
		void			setColor( WgColor color );
		void			setBgColor( WgColor color );
		void			setSize( int size );
		void			setDecoration( WgTextDecoration decoration );
	
		void			setColor( WgColor color, WgState state );
		void			setBgColor( WgColor color, WgState state );
		void			setSize( int size, WgState state );
		void			setDecoration( WgTextDecoration decoration, WgState state );
	
		void			clearFont();
		void			clearLink();
	
		void			clearColor();
		void			clearBgColor();
		void			clearSize();
		void			clearDecoration();
	
		void			clearColor( WgState state );
		void			clearBgColor( WgState state );
		void			clearSize( WgState state );
		void			clearDecoration( WgState state );
	
		inline WgFont_p		font() const;
		inline WgTextLink_p	link() const;
		inline WgColor			color( WgState state ) const;
		inline 	WgColor			bgColor( WgState state ) const;
		inline int				size( WgState state ) const;
		inline WgTextDecoration decoration( WgState state ) const;
	
		inline WgFont_p		combFont() const;
		inline WgTextLink_p	combLink() const;
		inline WgColor			combColor( WgState state ) const;
		inline WgColor			combBgColor( WgState state ) const;
		inline int				combSize( WgState state ) const;
		inline WgTextDecoration combDecoration( WgState state ) const;
	
		void			exportAttr( WgState state, WgTextAttr2 * pDest ) const;
		void			addToAttr( WgState state, WgTextAttr2 * pDest ) const;
	
	
	
	protected:
		WgTextStyle();
		virtual ~WgTextStyle();
	
		struct AttrSet
		{
			WgFont_p			pFont;
			int					size[WG_NB_STATES];
			WgColor				color[WG_NB_STATES];
			WgColor				bgColor[WG_NB_STATES];
			WgTextDecoration	decoration[WG_NB_STATES];
			WgTextLink_p		pLink;
		};
	
		bool		_compareSets( AttrSet * pSet1, AttrSet * pSet2 );
		bool		_refreshComb();
		void		_clearSet( AttrSet * pSet );
	
		WgTextStyle_p	m_pParent;
		WgTextStyle *	m_pFirstChild;
		WgTextStyle *	m_pNextSibling;
		WgTextStyle *	m_pPrevSibling;
	
	
		AttrSet		m_specAttr;
		AttrSet		m_combAttr;
	
	
	};
	
	
	//______________________________________________________________________________
	inline WgFont_p WgTextStyle::font() const
	{
		return m_specAttr.pFont;
	}
	
	//______________________________________________________________________________
	inline WgTextLink_p WgTextStyle::link() const
	{
		return m_specAttr.pLink;
	}
	
	//______________________________________________________________________________
	inline WgColor WgTextStyle::color( WgState state ) const
	{
		return m_specAttr.color[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline 	WgColor WgTextStyle::bgColor( WgState state ) const
	{
		return m_specAttr.bgColor[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline int WgTextStyle::size( WgState state ) const
	{
		return m_specAttr.size[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline WgTextDecoration WgTextStyle::decoration( WgState state ) const
	{
		return m_specAttr.decoration[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	
	inline WgFont_p WgTextStyle::combFont() const
	{
		return m_combAttr.pFont;
	}
	
	//______________________________________________________________________________
	inline WgTextLink_p WgTextStyle::combLink() const
	{
		return m_combAttr.pLink;
	}
	
	//______________________________________________________________________________
	inline WgColor WgTextStyle::combColor( WgState state ) const
	{
		return m_combAttr.color[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline WgColor WgTextStyle::combBgColor( WgState state ) const
	{
		return m_combAttr.bgColor[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline int WgTextStyle::combSize( WgState state ) const
	{
		return m_combAttr.size[WgUtil::_stateToIndex(state)];
	}
	
	//______________________________________________________________________________
	inline WgTextDecoration WgTextStyle::combDecoration( WgState state ) const
	{
		return m_combAttr.decoration[WgUtil::_stateToIndex(state)];
	}
	
	

} // namespace wg
#endif //WG_TEXTSTYLE_DOT_H
