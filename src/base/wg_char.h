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


#ifndef	WG_CHAR_DOT_H
#define	WG_CHAR_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include "wg_types.h"
#endif

#ifndef WG_TEXTPROPMANAGER_DOT_H
#	include "wg_textpropmanager.h"
#endif

#include <wg_texttool.h>


//_____ WgChar ________________________________________________________________

/** The character class in WonderGUI is different from other toolkits in that
	it not only holds a 16-bit unicode character but also specifies what
	display properties the character has such as font, style, color etc by
	containing a handle to a properties specification.

	This makes the characters slightly heavier (32-bits instead of 16 or 8 bits)
	and adds some overhead to creation/deletion since WgCharProp objects needs to
	be referenced and dereferenced. On the flipside it gives all displayed text
	"rich text"-functionality in an easy and consistent way that hardly affects
	rendering performance.

	Because of the overhead involved in manipulation and storing WgChars, it is
	not recommended that WgChar or any of the WgChar containers (WgCharBuffer,
	WgString and WgText) is used for general purpose text not meant for display.

	Also, when manipulating chunks of text stored as WgChars it is highly
	recommended that you use the WgCharBuffer container class instead of
	manipulating arrays of WgChars. All the container classes
	have methods for manipulating properties on multiple characters which are
	both faster and more convenient than manipulating the WgChars one by one.

*/

class WgChar
{
friend class WgLegacyTextField;
friend class WgTextprop;
friend class WgTextTool;
friend class WgCharBuffer;

public:
	/// Initializes an empty character containing null (End of Text) and no properties.

	WgChar() { all = 0; };

	/// Initializes a character to contain the glyph and properties of the specified character.

	WgChar( const WgChar& r ) { all = r.all; if( properties) WgTextpropManager::incRef(properties,1); }

	/// Initializes a character to contain the specified glyph and no properties.

	WgChar( Uint16 _glyph ) { all = 0; glyph = _glyph; }

	/// Initializes a character to contain the glyph and properties as specified.

	WgChar( Uint16 _glyph, const WgTextprop_p& _pProperties ) { all = 0; glyph = _glyph; properties = _pProperties.m_hProp; WgTextpropManager::incRef(properties,1); }
	~WgChar() { if( properties ) WgTextpropManager::decRef(properties,1); };

	inline WgChar & operator=(const WgChar &ref)
	{
		if(properties == ref.properties)		// Don't dec/inc ref if properties are same! Could be a self-assignment...
		{										// also speeds things up...
			all = ref.all;
		}
		else
		{
			if(properties)
				WgTextpropManager::decRef(properties,1);
			all = ref.all;
			if(properties)
				WgTextpropManager::incRef(properties,1);
		}
		return *this;
	}

	inline bool				equals(const WgChar& ch) const { return all == ch.all; }

							/// Sets the glyph part of the character without affecting the display properties.

	inline void				setGlyph( Uint16 _glyph ) { glyph = _glyph; }

							/// Gets the glyph part of the character.
							///
							/// @return The glyph part of the character.

	inline Uint16			getGlyph() const { return glyph; }

							/// Sets the properties of the character.

	inline void				setProperties( const WgTextprop_p& pProperties ) { if(properties) WgTextpropManager::decRef(properties,1); properties = pProperties.m_hProp; WgTextpropManager::incRef(properties,1); }

							/// Gets the characters properties as a handle.
							///
							/// Getting the properties as a handle is faster than getting a pointer to the properties.
							/// Characters with identical properties always returns the same handle. A character with
							/// no properties set has a handle of 0.
							///
							/// @return The properties of the character as a handle.

	inline Uint16			propHandle() const { return (Uint16) properties; }

							/// Gets read-access to the properties of the character.
							///
							/// Characters with identical properties always returns identical pointers.
							///
							/// @return Read-only pointer to a WgTextprop specifying the properties of the character. A valid pointer
							/// is always returned, if the character has no properties set a pointer to an empty default WgTextprop is returned.

	inline WgTextprop_p	getProperties() const { return WgTextprop_p(properties); }

							/// Checks if the character is set to be underlined in the given state.

							/// @param state			The state of the Widget or Item containing the text.

							/// @return True if the character is set to underlined for the given state.

	inline bool				isUnderlined( WgState state ) const { return getPropRef().isUnderlined(state); }

							/// Checks if the character has a color specified for the given state.

							/// @param state			The state of the Widget or Item containing the text.

							/// @return True if the character has a color specified for the specified state.

	inline bool				isColored( WgState state ) const { return getPropRef().isColored(state); }

							/// Returns the characters specified color (if any) for the given state.

							/// @param state			The state of the Widget or Item containing the text.

							/// The method isColored() needs to be called to determine wether the
							/// character actually has a color for the given state or not.
							///
							/// @return If the character has a color specified for the given state that will be returned,
							///			otherwise an unspecified value will be returned.

	inline const WgColor	color( WgState state ) const { return getPropRef().color(state); }

							/// Checks if the character has a background color specified for the given state.

							/// @param state		The state of the Widget or Item containing the text.

							/// @return True if the character has a background color specified for the specified state.

	inline bool				isBgColored( WgState state ) const { return getPropRef().isBgColored(state); }

							/// Returns the characters specified background color (if any) for the given state.

							/// @param state		The state of the Widget or Item containing the text.

							/// The method isBgColored() needs to be called to determine wether the
							/// character actually has a background color for the given state or not.
							///
							/// @return If the character has a background color specified for the given state that will be returned,
							///			otherwise an unspecified value will be returned.

	inline const WgColor	bgColor( WgState state ) const { return getPropRef().bgColor(state); }

							/// Returns the text-link (weblink) this character links to, if any.
							///
							/// @return Pointer to the WgTextLink this character is part of or NULL if none.

	inline WgTextLink_p	link() const { return getPropRef().link(); }

							/// Returns the characters specified font (if any).
							///
							/// @return Pointer to the font specified for this character or NULL if none.

	inline WgFont_p		font() const { return getPropRef().font(); }


//	inline WgGlyphset *		glyphset( const WgTextprop_p& pDefProp, WgState state = WG_STATE_NORMAL ) const { return WgTextTool::GetCombGlyphset(pDefProp.getHandle(), properties, state); }

							/// Returns the characters font size for the given state.
							///
							/// @param state			The state of the Widget or Item containing the text.
							///						Specifying WG_MODE_ALL is not allowed and results in unspecified behavior.
							///
							///	Default value for all states is WG_STATE_NORMAL which is considered "no state specified"
							/// by most text managers who therefore will decide the characters style themselves.
							///
							/// @return Characters font size for the given state.

	inline int				size( WgState state = WG_STATE_NORMAL ) const { return getPropRef().size(state); }

							/// Returns the characters font style specification for the given state.
							///
							/// @param state			The state of the Widget or Item containing the text.
							///
							///	Default value for all states is WG_STATE_NORMAL which is considered "no state specified"
							/// by most text managers who therefore will decide the characters style themselves.
							///
							/// @return Font style specified for the given state.


	inline WgFontAlt		style( WgState state = WG_STATE_NORMAL ) const { return getPropRef().style(state); }

							/// Checks if the character terminates the current line.
							/// @return True if the glyph portion of the character contains End-Of-Line (\\n) or End-of-Text (null).

	inline bool 			isEndOfLine() const { if( glyph == '\n' || glyph == 0 ) return true; return false; }

							/// Checks if the character terminates the text.
							/// @return True if the glyph portion of the character is End-of-Text (null).

	inline bool				isEndOfText() const { if( glyph == 0 ) return true; return false; }

							/// Checks if the character is a whitespace.
							/// @return True if the glyph portion of the characer contains a space or WG_NO_BREAK_SPACE.

	inline bool 			isWhitespace() const { if( glyph == ' ' || glyph == WG_NO_BREAK_SPACE ) return true; return false; }

protected:

	inline const WgTextprop&	getPropRef() const { return WgTextpropManager::getProp(properties); }		// Use with caution! Not safe if adding/removing

private:

	union
	{
		struct
		{
			Uint16	glyph;
			Uint16	properties;				// 0 = Default properties of WgText.
		};
		Uint32	all;						// For quickly copying all...
	};
};



#endif //WG_CHAR_DOT_H

