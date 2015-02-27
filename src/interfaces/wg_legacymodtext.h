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

	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgLegacyModTextPtr		Cast( const WgInterfacePtr& pInterface );
	inline WgLegacyModTextPtr		Ptr() { return WgLegacyModTextPtr(_object(),this); }

	inline void				Clear() { m_pField->Clear(); }

	inline void				Set( const WgCharSeq& seq ) { m_pField->Set(seq); }
	inline void				Set( const WgCharBuffer * pBuffer ) { m_pField->Set(pBuffer); }
	inline void				Set( const WgString& str ) { m_pField->Set(str); }

	inline int				Append( const WgCharSeq& seq ) { return m_pField->Append(seq); }
	inline int				Insert( int ofs, const WgCharSeq& seq ) { return m_pField->Insert(ofs,seq); }
	inline int				Replace( int ofs, int len, const WgCharSeq& seq ) { return m_pField->Replace(ofs,len,seq); }
	inline int				Delete( int ofs, int len ) { return m_pField->Delete(ofs,len); }

};


#endif //WG_LEGACYMODTEXT_DOT_H
