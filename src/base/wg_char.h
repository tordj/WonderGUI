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

#ifndef WG_TEXTSTYLEMANAGER_DOT_H
#	include "wg_textstylemanager.h"
#endif

#include <wg_texttool.h>

namespace wg 
{
	
	
	//_____ Char ________________________________________________________________
	
	/** The character class in WonderGUI is different from other toolkits in that
		it not only holds a 16-bit unicode character but also specifies what
		display properties the character has such as font, style, color etc by
		containing a handle to a properties specification.
	
		This makes the characters slightly heavier (32-bits instead of 16 or 8 bits)
		and adds some overhead to creation/deletion since CharProp objects needs to
		be referenced and dereferenced. On the flipside it gives all displayed text
		"rich text"-functionality in an easy and consistent way that hardly affects
		rendering performance.
	
		Because of the overhead involved in manipulation and storing Chars, it is
		not recommended that Char or any of the Char containers (CharBuffer,
		String and Text) is used for general purpose text not meant for display.
	
		Also, when manipulating chunks of text stored as Chars it is highly
		recommended that you use the CharBuffer container class instead of
		manipulating arrays of Chars. All the container classes
		have methods for manipulating properties on multiple characters which are
		both faster and more convenient than manipulating the Chars one by one.
	
	*/
	
	class Char
	{
	friend class TextStyle;
	friend class TextTool;
	friend class CharBuffer;
	
	public:
		/// Initializes an empty character containing null (End of Text) and no properties.
	
		Char() : all(0) {};
	
		/// Initializes a character to contain the glyph and properties of the specified character.
	
		Char( const Char& r ) { all = r.all; if( style ) TextStyleManager::_getPointer(style)->_incRefCount(); }
	
		/// Initializes a character to contain the specified glyph and no properties.
	
		Char( uint16_t _glyph ) { style = 0; glyph = _glyph; }
	
		/// Initializes a character to contain the glyph and properties as specified.
	
		Char( uint16_t _glyph, const TextStyle_p& _pStyle ) { glyph = _glyph; style = _pStyle->handle(); _pStyle->_incRefCount(); }
		~Char() { if( style ) TextStyleManager::_getPointer(style)->_decRefCount(); }
	
		inline Char & operator=(const Char &ref)
		{
			if(style == ref.style)		// Don't dec/inc ref if properties are same! Could be a self-assignment...
			{							// also speeds things up...
				all = ref.all;
			}
			else
			{
				if(style)
					TextStyleManager::_getPointer(style)->_decRefCount();
				all = ref.all;
				if(style)
					TextStyleManager::_getPointer(style)->_incRefCount();
			}
			return *this;
		}
	
		inline bool				equals(const Char& ch) const { return all == ch.all; }
	
								/// Sets the glyph part of the character without affecting the display properties.
	
		inline void				setGlyph( uint16_t _glyph ) { glyph = _glyph; }
	
								/// Gets the glyph part of the character.
								///
								/// @return The glyph part of the character.
	
		inline uint16_t			getGlyph() const { return glyph; }
	
								/// Sets the properties of the character.
	
		inline void				setStyle( const TextStyle_p& pStyle ) { if(style) TextStyleManager::_getPointer(style)->_decRefCount(); style = pStyle->handle(); pStyle->_incRefCount(); }
	
								/// Gets the characters properties as a handle.
								///
								/// Getting the properties as a handle is faster than getting a pointer to the properties.
								/// Characters with identical properties always returns the same handle. A character with
								/// no properties set has a handle of 0.
								///
								/// @return The properties of the character as a handle.
	
		inline TextStyle_h		styleHandle() const { return style; }
	
								/// Gets read-access to the properties of the character.
								///
								/// Characters with identical properties always returns identical pointers.
								///
								/// @return Read-only pointer to a Textprop specifying the properties of the character. A valid pointer
								/// is always returned, if the character has no properties set a pointer to an empty default Textprop is returned.
	
		inline TextStyle_p		stylePtr() const { return TextStyleManager::_getPointer(style); }
	
								/// Checks if the character is set to be underlined in the given state.
	
								/// @param state			The state of the Widget or Item containing the text.
	
								/// @return True if the character is set to underlined for the given state.
	
	
		inline bool 			isEndOfLine() const { if( glyph == '\n' || glyph == 0 ) return true; return false; }
	
								/// Checks if the character terminates the text.
								/// @return True if the glyph portion of the character is End-of-Text (null).
	
		inline bool				isEndOfText() const { if( glyph == 0 ) return true; return false; }
	
								/// Checks if the character is a whitespace.
								/// @return True if the glyph portion of the characer contains a space or ExtChar::NoBreakSpace.
	
		inline bool 			isWhitespace() const { if( glyph == ' ' || glyph == (uint16_t)ExtChar::NoBreakSpace ) return true; return false; }
	
	protected:
	
	
	private:
	
		union
		{
			struct
			{
				uint16_t	glyph;
				TextStyle_h	style;				// 0 = Default properties of Text.
			};
			uint32_t	all;					// For quickly copying all...
		};
	};
	
	
	

} // namespace wg
#endif //WG_CHAR_DOT_H

