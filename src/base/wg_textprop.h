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

namespace wg 
{
	
	
	
	class Char;
	class Glyphset;
	class Textprop;
	class Textprop_p;
	class Textlink;
	class TextpropHolder;
	class TextpropManager;
	
	//____ Textprop_p __________________________________________________________
	
	class Textprop_p
	{
	public:
		friend class Char;
		friend class LegacyTextField;
		friend class Pen;
	
		Textprop_p( TextpropHolder * pProp = 0 );
		explicit Textprop_p( uint16_t hProp );
		Textprop_p(const Textprop_p& r);
		~Textprop_p();
	
	
		Textprop_p& operator=(const Textprop_p& ref);
		Textprop_p& operator=(int handle);
	
		inline bool operator==(const Textprop_p& other) const { return m_hProp == other.m_hProp; }
		inline bool operator!=(const Textprop_p& other) const { return m_hProp != other.m_hProp; }
	
		const Textprop & operator*() const;
		const Textprop * operator->() const;
		inline operator bool() const { return (m_hProp != 0); }
	
		inline	uint16_t getHandle() const { return m_hProp; }
	
	private:
		uint16_t			m_hProp;
	};
	
	
	//____ Textprop _____________________________________________________________
	
	class Textprop
	{
		friend class Char;
		friend class TextTool;
		friend class GfxDevice;
		friend class TextpropManager;
	
	public:
	
		Textprop();
		Textprop( const Textprop_p& pProp );
	//	~Textprop();
	
		Textprop_p	reg() const;
	
		void			setColor( Color col );
		void			setBgColor( Color col );
		void			setStyle( FontAlt style );
		void			setSize( int size );
		void			setUnderlined();
	
		void			setColor( Color col, State state );
		void			setBgColor( Color col, State state );
		void			setStyle( FontAlt style, State state );
		void			setSize( int size, State state );
		void			setUnderlined( State state );
	
		inline void		setBreakLevel( int level ) { m_breakLevel = (char) level; }
		inline void		setLink( const TextLink_p& pLink ) { m_pLink = pLink; }
		inline void		setFont( const Font_p& pFont ) { m_pFont = pFont; }
		bool			setCharVisibility( uint16_t specialCharacter, bool bVisible );
	
		void			clearColor();
		void			clearBgColor();
		void			clearStyle();
		void			clearSize();
		void			clearUnderlined();
	
		void			clearColor( State state );
		void			clearBgColor( State state );
		void			clearStyle( State state );
		void			clearSize( State state );
		void			clearUnderlined( State state );
	
		inline void		clearBreakLevel() { m_breakLevel = -1; }
		inline void		clearLink() { m_pLink = 0; }
		inline void		clearFont() { m_pFont = 0; }
	
		void			clearAll();
	
	
		inline bool				isUnderlined( State state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bUnderlined; }
		inline bool				isColored( State state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bColored; }
		inline const Color&	color( State state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_color; }
		inline bool				isBgColored( State state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bBgColor; }
		inline const Color&	bgColor( State state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_bgColor; }
		inline FontAlt		style( State state = WG_STATE_NORMAL ) const { return (FontAlt) m_stateProp[WgUtil::_stateToIndex(state)].m_style; }
		inline int				size( State state = WG_STATE_NORMAL ) const { return m_stateProp[WgUtil::_stateToIndex(state)].m_size; }
		bool					charVisibility( uint16_t specialCharacter ) const;
		inline int				charVisibilityFlags() const { return m_visibilityFlags; }
	
		inline TextLink_p	link() const { return m_pLink; }
		inline Font_p		font() const { return m_pFont; }
		inline int				breakLevel() const { return m_breakLevel; }
	
		void					assertIntegrity() const;
	
		bool					isEqual(State state0, State state1) const;
		bool					isColorStatic() const;
		bool					isBgColorStatic() const;
		bool					isStyleStatic() const;
		bool					isSizeStatic() const;
		bool					isUnderlineStatic() const;
	
		inline bool		compareTo( const Textprop_p& pProp ) const { return _compareTo( &(*pProp) ); }
		bool			compareColorTo( const Textprop_p& pProp ) const;
		bool			compareBgColorTo( const Textprop_p& pProp ) const;
		bool			compareStyleTo( const Textprop_p& pProp ) const;
		bool			compareSizeTo( const Textprop_p& pProp ) const;
		bool			compareUnderlineTo( const Textprop_p& pProp ) const;
	
	private:
		bool			_compareTo( const Textprop * pProp ) const;
	
		uint8_t			_calculateChecksum() const;
	
	
		// Struct holding State-specific parameters.
	
		struct StateProp
		{
			uint16_t			m_style:5;		///< FontAlt for this state, WG_FONT_NORMAL is same as none and overriden if set higher in hierarchy.
			uint16_t			m_size:11;		///< Size in pixels of font. 0 = none specified, use sized specified higher up in hierarchy.
			bool			m_bUnderlined;	///< Hierarchally cumulative, anyone in hierarchy set gives underlined.
			bool			m_bColored;		///< Set if color of this struct should be used.
			Color			m_color;		///< Color for character if m_bColored is set. Search order is character, text.
			bool			m_bBgColor;		///< Set if we have a background color to use.
			Color			m_bgColor;		///< Color for background if m_bBgColor is set. Search order is character, text.
		};
	
		//____
	
	
		union
		{
			struct							///< Hierarchally cumulative, anyone in hierarchy set will set the flag.
			{
				uint8_t m_bShowLF						:1;
				uint8_t m_bShowTAB					:1;
				uint8_t m_bShowNoBreakSpace			:1;
				uint8_t m_bShowBreakPermitted			:1;
				uint8_t m_bShowHyphenBreakPermitted	:1;
				uint8_t m_bShowSpace					:1;
			};
			uint8_t m_visibilityFlags;
		};
	
		char				m_breakLevel;		///< How aggressively lines should be breaked. Higher value breaks on more characters. -1 = not set.
		TextLink_p		m_pLink;			///< Hierarchally overrides.
		Font_p			m_pFont;			///< Hierarchally overrides.
	
		StateProp		m_stateProp[WG_NB_STATES];
	};
	
	// CR, NON-BREAK SPACE, BREAK PERMITTED, HYPHEN BREAK PERMITTED, TAB.
	
	/*
		Text properties priority order:
		===============================
	
		1.	Characters own properties.
		2.	Link properties (if applicable).
		3.	Selection properties (if applicable).
		4.	Base properties.
		5.	Base::defaultProp.
		6.	Hardcoded defaults: TextColor=white, bgColor=transparent, style=normal, underline=false, size=0, breakLevel=3
	
	
	*/
	
	
	class TextAttr
	{
	public:
		TextAttr() : pFont(0), size(0), style(WG_FONT_NORMAL), color(Color::white), bgColor(Color::transparent),
					   bUnderlined(false), breakLevel(3), visibilityFlags(0) {}
	
		inline void	clear() {	pFont = 0; size = 0; style = WG_FONT_NORMAL; color = Color::white;
								bgColor = Color::transparent; bUnderlined = false; breakLevel = 3;
								visibilityFlags = 0; pLink = 0; }
	
		Font_p		pFont;
		int				size;
		FontAlt		style;
		Color			color;
		Color			bgColor;
		bool			bUnderlined;
		int				breakLevel;
		int				visibilityFlags;
		TextLink_p	pLink;
	};
	
	
	
	

} // namespace wg
#endif // WG_TEXTPROP_DOT_H


