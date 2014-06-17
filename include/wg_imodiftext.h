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

#ifndef WG_IMODIFTEXT_DOT_H
#define WG_IMODIFTEXT_DOT_H

#ifndef WG_ISTATICTEXT_DOT_H
#	include <wg_istatictext.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CURSOR_DOT_H
#	include <wg_cursor.h>
#endif

#ifndef WG_TEXTLINK_DOT_H
#	include <wg_textlink.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#ifndef WG_TEXTMANAGER_DOT_H
#	include <wg_textmanager.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

class WgColor;
class WgCharSeq;
class WgString;
class WgCharBuffer;

class WgIModifText;
typedef	WgIStrongPtr<WgIModifText,WgIStaticTextPtr>		WgIModifTextPtr;
typedef	WgWeakPtr<WgIModifText,WgIStaticTextPtr>		WgIModifTextWeakPtr;

/**
 * @brief Interface to a text field with text that is modifiable through the api
 * 
 * The text in a modifiable text field can be set through the API, but isn't
 * editable through the UI.
 * 
*/
 
class WgIModifText : public WgIStaticText
{
public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgIModifTextPtr		Cast( const WgInterfacePtr& pInterface );

	virtual void				Clear() = 0;

	virtual void				Set( const WgCharSeq& seq ) = 0;
	virtual void				Set( const WgCharBuffer * buffer ) = 0;
	virtual void				Set( const WgString& str ) = 0;

	virtual int					Append( const WgCharSeq& seq ) = 0;
	virtual int					Insert( int ofs, const WgCharSeq& seq ) = 0;
	virtual int					Replace( int ofs, int nDelete, const WgCharSeq& seq ) = 0;
	virtual int					Delete( int ofs, int len ) = 0;
};


#endif //WG_IMODIFTEXT_DOT_H
