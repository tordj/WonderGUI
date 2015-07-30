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

#ifndef WG_LEGACYMODTEXT_DOT_H
#define WG_LEGACYMODTEXT_DOT_H

#ifndef WG_LEGACYTEXT_DOT_H
#	include <wg_legacytext.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

#ifndef WG_TEXTLINK_DOT_H
#	include <wg_textlink.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

class WgColor;
class WgCharSeq;
class WgString;
class WgCharBuffer;

class WgLegacyModText;
typedef	WgIStrongPtr<WgLegacyModText,WgLegacyTextPtr>	WgLegacyModTextPtr;
typedef	WgIWeakPtr<WgLegacyModText,WgLegacyTextWeakPtr>	WgLegacyModTextWeakPtr;

/**
 * @brief Interface to a text field with text that is modifiable through the api
 * 
 * The text in a modifiable text field can be set through the API, but isn't
 * editable through the UI.
 * 
*/
 
class WgLegacyModText : public WgLegacyText
{
public:
	WgLegacyModText( WgLegacyTextField * pField ) : WgLegacyText(pField) {};

	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgLegacyModTextPtr		cast( const WgInterfacePtr& pInterface );
	inline WgLegacyModTextPtr		ptr() { return WgLegacyModTextPtr(_object(),this); }

	inline void				clear() { m_pField->clear(); }

	inline void				set( const WgCharSeq& seq ) { m_pField->set(seq); }
	inline void				set( const WgCharBuffer * pBuffer ) { m_pField->set(pBuffer); }
	inline void				set( const WgString& str ) { m_pField->set(str); }

	inline int				append( const WgCharSeq& seq ) { return m_pField->append(seq); }
	inline int				insert( int ofs, const WgCharSeq& seq ) { return m_pField->insert(ofs,seq); }
	inline int				replace( int ofs, int len, const WgCharSeq& seq ) { return m_pField->replace(ofs,len,seq); }
	inline int				remove( int ofs, int len ) { return m_pField->remove(ofs,len); }

};


#endif //WG_LEGACYMODTEXT_DOT_H
