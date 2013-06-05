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

#ifndef WG_TEXTPROP_DOT_H
#define WG_TEXTPROP_DOT_H


#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_TEXTLINK_DOT_H
#	include <wg_textlink.h>
#endif

#ifndef WG_UTIL_DOT_H
#	include	<wg_util.h>
#endif



class WgChar;
class WgFont;
class WgGlyphset;
class WgTextprop;
class WgTextpropPtr;
class WgTextlink;
class WgTextpropHolder;
class WgTextpropManager;

//____ WgTextpropPtr __________________________________________________________

class WgTextpropPtr
{
public:
	friend class WgChar;
	friend class WgText;
	friend class WgPen;

	WgTextpropPtr( WgTextpropHolder * pProp = 0 );
	explicit WgTextpropPtr( Uint16 hProp );
	WgTextpropPtr(const WgTextpropPtr& r);
	~WgTextpropPtr();


	WgTextpropPtr& operator=(const WgTextpropPtr& ref);
	WgTextpropPtr& operator=(int handle);

	inline bool operator==(const WgTextpropPtr& other) const { return m_hProp == other.m_hProp; }
	inline bool operator!=(const WgTextpropPtr& other) const { return m_hProp != other.m_hProp; }

	const WgTextprop & operator*() const;
	const WgTextprop * operator->() const;
	inline operator bool() const { return (m_hProp != 0); }

	inline	Uint16 GetHandle() const { return m_hProp; }

private:
	Uint16			m_hProp;
};


//____ WgTextprop _____________________________________________________________

class WgTextprop
{
	friend class WgChar;
	friend class WgTextTool;
	friend class WgGfxDevice;
	friend class WgTextpropManager;

public:

	WgTextprop();
	WgTextprop( const WgTextpropPtr& pProp );
//	~WgTextprop();

	WgTextpropPtr	Register() const;

	void			SetColor( WgColor col );
	void			SetBgColor( WgColor col );
	void			SetStyle( WgFontStyle style );
	void			SetSize( int size );
	void			SetUnderlined();

	void			SetColor( WgColor col, WgState state );
	void			SetBgColor( WgColor col, WgState state );
	void			SetStyle( WgFontStyle style, WgState state );
	void			SetSize( int size, WgState state );
	void			SetUnderlined( WgState state );

	inline void		SetBreakLevel( int level ) { m_breakLevel = (char) level; }
	inline void		SetLink( const WgTextLinkPtr& pLink ) { m_pLink = pLink; }
	inline void		SetFont( WgFont * pFont ) { m_pFont = pFont; }
	bool			SetCharVisibility( Uint16 specialCharacter, bool bVisible );

	void			ClearColor();
	void			ClearBgColor();
	void			ClearStyle();
	void			ClearSize();
	void			ClearUnderlined();

	void			ClearColor( WgState state );
	void			ClearBgColor( WgState state );
	void			ClearStyle( WgState state );
	void			ClearSize( WgState state );
	void			ClearUnderlined( WgState state );

	inline void		ClearBreakLevel() { m_breakLevel = -1; }
	inline void		ClearLink() { m_pLink = 0; }
	inline void		ClearFont() { m_pFont = 0; }

	void			ClearAll();


	inline bool				IsUnderlined( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bUnderlined; }
	inline bool				IsColored( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bColored; }
	inline const WgColor&	Color( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_color; }
	inline bool				IsBgColored( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bBgColor; }
	inline const WgColor&	BgColor( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bgColor; }
	inline WgFontStyle		Style( WgState state = WG_STATE_NORMAL ) const { return (WgFontStyle) m_stateProp[WgUtil::_stateToIndex(state)].m_style; }
	inline int				Size( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_size; }
	bool					CharVisibility( Uint16 specialCharacter ) const;
	inline int				CharVisibilityFlags() const { return m_visibilityFlags; }

	inline WgTextLinkPtr	Link() const { return m_pLink; }
	inline WgFont *			Font() const { return m_pFont; }
	inline int				BreakLevel() const { return m_breakLevel; }

	void					AssertIntegrity() const;

	bool					IsEqual(WgState state0, WgState state1) const;
	bool					IsColorStatic() const;
	bool					IsBgColorStatic() const;
	bool					IsStyleStatic() const;
	bool					IsSizeStatic() const;
	bool					IsUnderlineStatic() const;

	inline bool		CompareTo( const WgTextpropPtr& pProp ) const { return _compareTo( &(*pProp) ); }
	bool			CompareColorTo( const WgTextpropPtr& pProp ) const;
	bool			CompareBgColorTo( const WgTextpropPtr& pProp ) const;
	bool			CompareStyleTo( const WgTextpropPtr& pProp ) const;
	bool			CompareSizeTo( const WgTextpropPtr& pProp ) const;
	bool			CompareUnderlineTo( const WgTextpropPtr& pProp ) const;

private:
	bool			_compareTo( const WgTextprop * pProp ) const;

	Uint8			_calculateChecksum() const;


	// Struct holding WgState-specific parameters.

	struct WgStateProp
	{
		Uint16			m_style:5;		///< WgFontStyle for this state, WG_STYLE_NORMAL is same as none and overriden if set higher in hierarchy.
		Uint16			m_size:11;		///< Size in pixels of font. 0 = none specified, use sized specified higher up in hierarchy.
		bool			m_bUnderlined;	///< Hierarchally cumulative, anyone in hierarchy set gives underlined.
		bool			m_bColored;		///< Set if color of this struct should be used.
		WgColor			m_color;		///< Color for character if m_bColored is set. Search order is character, text.
		bool			m_bBgColor;		///< Set if we have a background color to use.
		WgColor			m_bgColor;		///< Color for background if m_bBgColor is set. Search order is character, text.
	};

	//____


	union
	{
		struct							///< Hierarchally cumulative, anyone in hierarchy set will set the flag.
		{
			Uint8 m_bShowLF						:1;
			Uint8 m_bShowTAB					:1;
			Uint8 m_bShowNoBreakSpace			:1;
			Uint8 m_bShowBreakPermitted			:1;
			Uint8 m_bShowHyphenBreakPermitted	:1;
			Uint8 m_bShowSpace					:1;
		};
		Uint8 m_visibilityFlags;
	};

	char				m_breakLevel;		///< How aggressively lines should be breaked. Higher value breaks on more characters. -1 = not set.
	WgTextLinkPtr		m_pLink;			///< Hierarchally overrides.
	WgFont *			m_pFont;			///< Hierarchally overrides.

	WgStateProp		m_stateProp[WG_NB_STATES];
};

// CR, NON-BREAK SPACE, BREAK PERMITTED, HYPHEN BREAK PERMITTED, TAB.

/*
	Text properties priority order:
	===============================

	1.	Characters own properties.
	2.	Link properties (if applicable).
	3.	Selection properties (if applicable).
	4.	Base properties.
	5.	WgBase::defaultProp.
	6.	Hardcoded defaults: TextColor=white, bgColor=transparent, style=normal, underline=false, size=0, breakLevel=3


*/


class WgTextAttr
{
public:
	WgTextAttr() : pFont(0), size(0), style(WG_STYLE_NORMAL), color(WgColor::white), bgColor(WgColor::transparent),
				   bUnderlined(false), breakLevel(3), visibilityFlags(0) {}

	inline void	Clear() {	pFont = 0; size = 0; style = WG_STYLE_NORMAL; color = WgColor::white;
							bgColor = WgColor::transparent; bUnderlined = false; breakLevel = 3;
							visibilityFlags = 0; pLink = 0; }

	WgFont *		pFont;
	int				size;
	WgFontStyle		style;
	WgColor			color;
	WgColor			bgColor;
	bool			bUnderlined;
	int				breakLevel;
	int				visibilityFlags;
	WgTextLinkPtr	pLink;
};




#endif // WG_TEXTPROP_DOT_H


