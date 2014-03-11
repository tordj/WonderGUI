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

#ifndef WG_ISTATICTEXT_DOT_H
#define WG_ISTATICTEXT_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
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

class WgIStaticText;
typedef	WgISmartPtr<WgIStaticText,WgInterfacePtr>		WgIStaticTextPtr;
typedef	WgWeakPtr<WgIStaticText,WgInterfacePtr>		WgIStaticTextWeakPtr;

class WgIStaticText : public WgInterface
{
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgIStaticTextPtr		Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.

	virtual void				SetManager( const WgTextManagerPtr& pManager ) = 0;
	virtual WgTextManagerPtr	Manager() const = 0;

	virtual void				SetProperties( const WgTextpropPtr& pProp ) = 0;
	virtual void				ClearProperties() = 0;
	virtual WgTextpropPtr		Properties() const = 0;

	virtual void				SetSelectionProperties( const WgTextpropPtr& pProp ) = 0;
	virtual void				ClearSelectionProperties() = 0;
	virtual WgTextpropPtr		SelectionProperties() const = 0;

	virtual void				SetLinkProperties( const WgTextpropPtr& pProp ) = 0;
	virtual void				ClearLinkProperties() = 0;
	virtual WgTextpropPtr		LinkProperties() const = 0;

	virtual void				SetFont( const WgFontPtr& pFont ) = 0;
	virtual void				ClearFont() = 0;
	virtual WgFontPtr			Font() const = 0;

	virtual void				SetColor( const WgColor color ) = 0;
	virtual void				SetColor( const WgColor color, WgState state ) = 0;
	virtual void				ClearColor() = 0;
	virtual void				ClearColor( WgState state ) = 0;
	virtual WgColor				Color(WgState state) const = 0;

	virtual void				SetStyle( WgFontStyle style ) = 0;
	virtual void				SetStyle( WgFontStyle style, WgState state ) = 0;
	virtual void				ClearStyle() = 0;
	virtual void				ClearStyle( WgState state ) = 0;
	virtual WgFontStyle			Style(WgState state) const = 0;

	virtual void				SetBreakLevel( int level ) = 0;
	virtual void				ClearBreakLevel() = 0;
	virtual int					BreakLevel() const = 0;

	virtual void				SetLink( const WgTextLinkPtr& pLink ) = 0;
	virtual void				ClearLink() = 0;
	virtual WgTextLinkPtr		Link() const = 0;

	virtual void				SetAlignment( WgOrigo alignment ) = 0;
	virtual WgOrigo				Alignment() const = 0;

	virtual void				SetTintMode( WgTintMode mode ) = 0;
	virtual WgTintMode			TintMode() const = 0;

	virtual void				SetLineSpacing( float adjustment ) = 0;
	virtual float				LineSpacing() const = 0;

	virtual void				SetWrap(bool bWrap) = 0;
	virtual bool				Wrap() const = 0;

	virtual void				SetAutoEllipsis(bool bAutoEllipsis) = 0;
	virtual bool				AutoEllipsis() const = 0;

	virtual WgState				State() const = 0;
	virtual int					Lines() const = 0;
	virtual int					Length() const = 0;
	virtual bool				IsEmpty() const = 0;
	virtual bool				IsEditable() const = 0;
	virtual bool				IsSelectable() const = 0;

	virtual int					Width() const = 0;
	virtual int					Height() const = 0;

	virtual void				Select( int ofs, int len ) = 0;
	virtual void				SelectAll() = 0;
	virtual int					SelectionStart() const = 0;
	virtual int					SelectionLength() const = 0;
	virtual void				ClearSelection() = 0;
};


#endif //WG_ISTATICTEXT_DOT_H