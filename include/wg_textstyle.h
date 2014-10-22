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


class WgTextStyle;
typedef	WgStrongPtr<WgTextStyle,WgObjectPtr>		WgTextStylePtr;
typedef	WgWeakPtr<WgTextStyle,WgObjectWeakPtr>		WgTextStyleWeakPtr;

struct WgTextAttr
{
		WgFontPtr			pFont;
		int					size;
		WgFontStyle			fontStyle;
		WgColor				color;
		WgColor				bgColor;
		WgTextDecoration	decoration;
		WgTextLinkPtr		pLink;
};


class WgTextStyle : public WgObject
{
public:
	static WgTextStylePtr	Create() { return WgTextStylePtr(new WgTextStyle()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgTextStylePtr	Cast( const WgObjectPtr& pObject );

	bool			SetParent( const WgTextStylePtr& pParent );
	void			Cascade();

	void			SetFont( const WgFontPtr& pFont );
	void			SetLink( const WgTextLinkPtr& pLink );

	void			SetColor( WgColor color );
	void			SetBgColor( WgColor color );
	void			SetFontStyle( WgFontStyle style );
	void			SetSize( int size );
	void			SetDecoration( WgTextDecoration decoration );

	void			SetColor( WgColor color, WgState state );
	void			SetBgColor( WgColor color, WgState state );
	void			SetFontStyle( WgFontStyle style, WgState state );
	void			SetSize( int size, WgState state );
	void			SetDecoration( WgTextDecoration decoration, WgState state );

	void			ClearFont();
	void			ClearLink();

	void			ClearColor();
	void			ClearBgColor();
	void			ClearFontStyle();
	void			ClearSize();
	void			ClearDecoration();

	void			ClearColor( WgState state );
	void			ClearBgColor( WgState state );
	void			ClearFontStyle( WgState state );
	void			ClearSize( WgState state );
	void			ClearDecoration( WgState state );

	inline WgFontPtr		Font() const;
	inline WgTextLinkPtr	Link() const;
	inline WgColor			Color( WgState state ) const;
	inline 	WgColor			BgColor( WgState state ) const;
	inline WgFontStyle		FontStyle( WgState state ) const;
	inline int				Size( WgState state ) const;
	inline WgTextDecoration Decoration( WgState state ) const;

	inline WgFontPtr		CombFont() const;
	inline WgTextLinkPtr	CombLink() const;
	inline WgColor			CombColor( WgState state ) const;
	inline WgColor			CombBgColor( WgState state ) const;
	inline WgFontStyle		CombFontStyle( WgState state ) const;
	inline int				CombSize( WgState state ) const;
	inline WgTextDecoration CombDecoration( WgState state ) const;

	void			ExportAttr( WgState state, WgTextAttr * pDest );
	void			AddToAttr( WgState state, WgTextAttr * pDest );



protected:
	WgTextStyle();
	virtual ~WgTextStyle();

	struct AttrSet
	{
		WgFontPtr			pFont;
		int					size[WG_NB_STATES];
		WgFontStyle			fontStyle[WG_NB_STATES];
		WgColor				color[WG_NB_STATES];
		WgColor				bgColor[WG_NB_STATES];
		WgTextDecoration	decoration[WG_NB_STATES];
		WgTextLinkPtr		pLink;
	};

	bool		_compareSets( AttrSet * pSet1, AttrSet * pSet2 );
	bool		_refreshComb();
	void		_clearSet( AttrSet * pSet );

	WgTextStylePtr	m_pParent;
	WgTextStyle *	m_pFirstChild;
	WgTextStyle *	m_pNextSibling;
	WgTextStyle *	m_pPrevSibling;


	AttrSet		m_specAttr;
	AttrSet		m_combAttr;


};


//______________________________________________________________________________
inline WgFontPtr WgTextStyle::Font() const
{
	return m_specAttr.pFont;
}

//______________________________________________________________________________
inline WgTextLinkPtr WgTextStyle::Link() const
{
	return m_specAttr.pLink;
}

//______________________________________________________________________________
inline WgColor WgTextStyle::Color( WgState state ) const
{
	return m_specAttr.color[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline 	WgColor WgTextStyle::BgColor( WgState state ) const
{
	return m_specAttr.bgColor[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline WgFontStyle WgTextStyle::FontStyle( WgState state ) const
{
	return m_specAttr.fontStyle[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline int WgTextStyle::Size( WgState state ) const
{
	return m_specAttr.size[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline WgTextDecoration WgTextStyle::Decoration( WgState state ) const
{
	return m_specAttr.decoration[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________

inline WgFontPtr WgTextStyle::CombFont() const
{
	return m_combAttr.pFont;
}

//______________________________________________________________________________
inline WgTextLinkPtr WgTextStyle::CombLink() const
{
	return m_combAttr.pLink;
}

//______________________________________________________________________________
inline WgColor WgTextStyle::CombColor( WgState state ) const
{
	return m_combAttr.color[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline WgColor WgTextStyle::CombBgColor( WgState state ) const
{
	return m_combAttr.bgColor[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline WgFontStyle WgTextStyle::CombFontStyle( WgState state ) const
{
	return m_combAttr.fontStyle[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline int WgTextStyle::CombSize( WgState state ) const
{
	return m_combAttr.size[WgUtil::_stateToIndex(state)];
}

//______________________________________________________________________________
inline WgTextDecoration WgTextStyle::CombDecoration( WgState state ) const
{
	return m_combAttr.decoration[WgUtil::_stateToIndex(state)];
}




#endif //WG_TEXTSTYLE_DOT_H


