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

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif



class WgChar;
class WgGlyphset;
class WgTextprop;
class WgTextprop_p;
class WgTextlink;
class WgTextpropHolder;
class WgTextpropManager;

//____ WgTextprop_p __________________________________________________________

class WgTextprop_p
{
public:
	friend class WgChar;
	friend class WgLegacyTextField;
	friend class WgPen;

	WgTextprop_p( WgTextpropHolder * pProp = 0 );
	explicit WgTextprop_p( Uint16 hProp );
	WgTextprop_p(const WgTextprop_p& r);
	~WgTextprop_p();


	WgTextprop_p& operator=(const WgTextprop_p& ref);
	WgTextprop_p& operator=(int handle);

	inline bool operator==(const WgTextprop_p& other) const { return m_hProp == other.m_hProp; }
	inline bool operator!=(const WgTextprop_p& other) const { return m_hProp != other.m_hProp; }

	const WgTextprop & operator*() const;
	const WgTextprop * operator->() const;
	inline operator bool() const { return (m_hProp != 0); }

	inline	Uint16 getHandle() const { return m_hProp; }

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
	WgTextprop( const WgTextprop_p& pProp );
//	~WgTextprop();

	WgTextprop_p	reg() const;

	void			setColor( WgColor col );
	void			setBgColor( WgColor col );
	void			setStyle( WgFontAlt style );
	void			setSize( int size );
	void			setUnderlined();

	void			setColor( WgColor col, WgState state );
	void			setBgColor( WgColor col, WgState state );
	void			setStyle( WgFontAlt style, WgState state );
	void			setSize( int size, WgState state );
	void			setUnderlined( WgState state );

	inline void		setBreakLevel( int level ) { m_breakLevel = (char) level; }
	inline void		setLink( const WgTextLink_p& pLink ) { m_pLink = pLink; }
	inline void		setFont( const WgFont_p& pFont ) { m_pFont = pFont; }
	bool			setCharVisibility( Uint16 specialCharacter, bool bVisible );

	void			clearColor();
	void			clearBgColor();
	void			clearStyle();
	void			clearSize();
	void			clearUnderlined();

	void			clearColor( WgState state );
	void			clearBgColor( WgState state );
	void			clearStyle( WgState state );
	void			clearSize( WgState state );
	void			clearUnderlined( WgState state );

	inline void		clearBreakLevel() { m_breakLevel = -1; }
	inline void		clearLink() { m_pLink = 0; }
	inline void		clearFont() { m_pFont = 0; }

	void			clearAll();


	inline bool				isUnderlined( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bUnderlined; }
	inline bool				isColored( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bColored; }
	inline const WgColor&	color( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_color; }
	inline bool				isBgColored( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bBgColor; }
	inline const WgColor&	bgColor( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bgColor; }
	inline WgFontAlt		style( WgState state = WG_STATE_NORMAL ) const { return (WgFontAlt) m_stateProp[WgUtil::_stateToIndex(state)].m_style; }
	inline int				size( WgState state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_size; }
	bool					charVisibility( Uint16 specialCharacter ) const;
	inline int				charVisibilityFlags() const { return m_visibilityFlags; }

	inline WgTextLink_p	link() const { return m_pLink; }
	inline WgFont_p		font() const { return m_pFont; }
	inline int				breakLevel() const { return m_breakLevel; }

	void					assertIntegrity() const;

	bool					isEqual(WgState state0, WgState state1) const;
	bool					isColorStatic() const;
	bool					isBgColorStatic() const;
	bool					isStyleStatic() const;
	bool					isSizeStatic() const;
	bool					isUnderlineStatic() const;

	inline bool		compareTo( const WgTextprop_p& pProp ) const { return _compareTo( &(*pProp) ); }
	bool			compareColorTo( const WgTextprop_p& pProp ) const;
	bool			compareBgColorTo( const WgTextprop_p& pProp ) const;
	bool			compareStyleTo( const WgTextprop_p& pProp ) const;
	bool			compareSizeTo( const WgTextprop_p& pProp ) const;
	bool			compareUnderlineTo( const WgTextprop_p& pProp ) const;

private:
	bool			_compareTo( const WgTextprop * pProp ) const;

	Uint8			_calculateChecksum() const;


	// Struct holding WgState-specific parameters.

	struct WgStateProp
	{
		Uint16			m_style:5;		///< WgFontAlt for this state, WG_FONT_NORMAL is same as none and overriden if set higher in hierarchy.
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
	WgTextLink_p		m_pLink;			///< Hierarchally overrides.
	WgFont_p			m_pFont;			///< Hierarchally overrides.

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
	WgTextAttr() : pFont(0), size(0), style(WG_FONT_NORMAL), color(WgColor::white), bgColor(WgColor::transparent),
				   bUnderlined(false), breakLevel(3), visibilityFlags(0) {}

	inline void	clear() {	pFont = 0; size = 0; style = WG_FONT_NORMAL; color = WgColor::white;
							bgColor = WgColor::transparent; bUnderlined = false; breakLevel = 3;
							visibilityFlags = 0; pLink = 0; }

	WgFont_p		pFont;
	int				size;
	WgFontAlt		style;
	WgColor			color;
	WgColor			bgColor;
	bool			bUnderlined;
	int				breakLevel;
	int				visibilityFlags;
	WgTextLink_p	pLink;
};




#endif // WG_TEXTPROP_DOT_H


