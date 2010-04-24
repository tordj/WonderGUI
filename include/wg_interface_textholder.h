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

#ifndef WG_INTERFACE_TEXTHOLDER_DOT_H
#define WG_INTERFACE_TEXTHOLDER_DOT_H

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_ORIGO_DOT_H
#	include <wg_origo.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

class WgFont;

//____ Wg_Interface_TextHolder ________________________________________________

class Wg_Interface_TextHolder : protected WgTextHolder
{

public:
	inline void			SetTextManager( WgTextManager * pManager ) { m_pText->setManager( pManager ); TextModified(); }
	inline WgTextManager * GetTextManager() const { return m_pText->getManager(); }

	inline bool			SetFonts( WgFont * pFont ) { SetFont(pFont); return true; }		// DEPRECATED!!!
	inline WgTextPropPtr GetTextDefaultProperties() { return m_pText->getDefaultProperties(); }


	inline WgColor		GetTextDefaultColor(WgMode mode) const { return m_pText->getDefaultColor(mode); }
	inline WgFontStyle	GetTextDefaultStyle(WgMode mode) const { return m_pText->getDefaultStyle(mode); }
	inline WgFont *	GetDefaultFont() const { return m_pText->getDefaultFont(); }


// -------------

	inline void			SetTextDefaultProperties( const WgTextPropPtr& pProp )		{ m_pText->setDefaultProperties(pProp); TextModified(); }

	inline void			SetTextDefaultColor( const WgColor color )					{ m_pText->setDefaultColor(color); TextModified(); }
	inline void			SetTextDefaultColor( const WgColor color, WgMode mode )		{ m_pText->setDefaultColor(color,mode); TextModified(); }

	inline void			SetTextDefaultStyle( WgFontStyle style )					{ m_pText->setDefaultStyle(style); TextModified(); }
	inline void			SetTextDefaultStyle( WgFontStyle style, WgMode mode )		{ m_pText->setDefaultStyle(style,mode); TextModified(); }

	inline void			SetDefaultFont( WgFont * pFont )							{ m_pText->setDefaultFont(pFont); TextModified(); }

// -------------

	inline void			ClearTextDefaultProperties()								{ m_pText->clearDefaultProperties(); TextModified(); }

	inline void			ClearTextDefaultColor()										{ m_pText->clearDefaultColor(); TextModified(); }
	inline void			ClearTextDefaultColor( WgMode mode )						{ m_pText->clearDefaultColor(mode); TextModified(); }

	inline void			ClearTextDefaultStyle()										{ m_pText->clearDefaultStyle(); TextModified(); }
	inline void			ClearTextDefaultStyle( WgMode mode )						{ m_pText->clearDefaultStyle(mode); TextModified(); }

	inline void			ClearDefaultFont()											{ m_pText->clearDefaultFont(); TextModified(); }

// -------------

	inline void			SetTextProperties( const WgTextPropPtr& pProp )				{ m_pText->setProperties(pProp); TextModified(); }
//	inline void			SetTextProperties( const WgTextPropPtr& pProp, const WgTextBlock& section )	{ m_pText->setProperties(pProp,section); TextModified(); }

	inline void			SetFont( WgFont * pFont )								{ m_pText->setFont(pFont); TextModified(); }
//	inline void			SetFont( WgFont * pFont, const WgTextBlock& section )	{ m_pText->setFont(pFont, section); TextModified(); }

	inline void			SetTextColor( const WgColor color )							{ m_pText->setColor(color); TextModified(); }
	inline void			setTextColor( const WgColor color, WgMode mode )			{ m_pText->setColor(color,mode); TextModified(); }
//	inline void			SetTextColor( const WgColor color, const WgTextBlock& section ) { m_pText->setColor(color,section); TextModified(); }
//	inline void			SetTextColor( const WgColor color, const WgTextBlock& section, WgMode mode )	{ m_pText->setColor(color,section,mode); TextModified(); }

	inline void			SetTextStyle( WgFontStyle style )							{ m_pText->setStyle(style); TextModified(); }
	inline void			SetTextStyle( WgFontStyle style, WgMode mode )				{ m_pText->setStyle(style,mode); TextModified(); }
//	inline void			SetTextStyle( WgFontStyle style, const WgTextBlock& section )	{ m_pText->setStyle(style,section); TextModified(); }
//	inline void			SetTextStyle( WgFontStyle style, const WgTextBlock& section, WgMode mode )	{ m_pText->setStyle(style,section,mode); TextModified(); }

	inline void			SetTextUnderlined()											{ m_pText->setUnderlined(); TextModified(); }
	inline void			SetTextUnderlined( WgMode mode )							{ m_pText->setUnderlined(mode); TextModified(); }
//	inline void			SetTextUnderlined( const WgTextBlock& section )			{ m_pText->setUnderlined(section); TextModified(); }
//	inline void			SetTextUnderlined( const WgTextBlock& section, WgMode mode ) { m_pText->setUnderlined(section,mode); TextModified(); }



// -------------

	void				ClearProperties()											{ m_pText->clearProperties(); TextModified(); }
//	void				ClearProperties( const WgTextBlock& section )				{ m_pText->clearProperties(section); TextModified(); }

	void				ClearFont()													{ m_pText->clearFont(); TextModified(); }
//	void				ClearFont( const WgTextBlock& section )					{ m_pText->clearFont(section); TextModified(); }

	void				ClearColor()												{ m_pText->clearColor(); TextModified(); }
	void				ClearColor( WgMode mode )									{ m_pText->clearColor(mode); TextModified(); }
//	void				ClearColor( const WgTextBlock& section )					{ m_pText->clearColor(section); TextModified(); }
//	void				ClearColor( const WgTextBlock& section, WgMode mode )		{ m_pText->clearColor(section,mode); TextModified(); }

	void				ClearStyle()												{ m_pText->clearStyle(); TextModified(); }
	void				ClearStyle( WgMode mode )									{ m_pText->clearStyle(mode); TextModified(); }
//	void				ClearStyle( const WgTextBlock& section )					{ m_pText->clearStyle(section); TextModified(); }
//	void				ClearStyle( const WgTextBlock& section, WgMode mode )		{ m_pText->clearStyle(section,mode); TextModified(); }

	void				ClearUnderlined()											{ m_pText->clearUnderlined(); TextModified(); }
//	void				ClearUnderlined( const WgTextBlock& section )				{ m_pText->clearUnderlined(section); TextModified(); }
	void				ClearUnderlined( WgMode mode )								{ m_pText->clearUnderlined(mode); TextModified(); }
//	void				ClearUnderlined( const WgTextBlock& section, WgMode mode ) { m_pText->clearUnderlined(section,mode); TextModified(); }

//	-----------

	void				SetTextMode( WgMode mode );
	void				SetTextAlignment( const WgOrigo& origo );
	void				SetTextTintMode( WgTintMode mode );

	void				ClearText();
	void				SetText( const char * _pStr );
	void				SetText( const Uint16 * _pStr );
	void				SetText( const WgChar * _pStr );
	void				SetText( const WgText * _pText );

	Uint32				SetFormattedText( const char * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );
	Uint32				SetFormattedText( const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );

	Uint32				AddFormattedText( const char * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );
	Uint32				InsertFormattedText( Uint32 line, Uint32 col, const char * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );

	void				SetLineSpaceAdjustment( Sint8 adjustment );
	Sint8				GetLineSpaceAdjustment() const;

	void				SetTextWrap(bool bWrap);
	bool				GetTextWrap() const { return m_pText->IsWrap(); }


	inline void			AddText( const char * pszText ) { AddFormattedText( pszText ); }
	void				AddText( const WgText * pText, Uint32 fromLine = 0, Uint32 nLines = 0xFFFFFFFF );
	void				AddText( const WgCharBuffer* charBuffer );
	void				RemoveTextLines( Uint32 start, Uint32 nLines = 0xFFFFFFFF );

	inline WgMode		TextMode() const { return m_pText->mode(); }
	inline WgTintMode	TextTintMode() const { return m_pText->tintMode(); }
	inline WgOrigo 		TextAlignment() const;
	inline WgText *		TextObj() const;
	inline void			SetTextObj(WgText* text) const;
	inline int			TextLines() const;

	inline Uint32		TextWidth() const { return m_pText->width(); }
	inline Uint32		TextHeight() const { return m_pText->height(); }

protected:
	Wg_Interface_TextHolder();

	inline void			TextModified( WgText * pText ) { TextModified(); }
	virtual void		TextModified() = 0;
	void				CloneInterface( Wg_Interface_TextHolder * _pClone );
	void				OnCloneContent( const Wg_Interface_TextHolder * pOrg );

	WgText *			m_pText;			// Must always be set by derived class!

};

//____ inline methods __________________________________________________________


WgOrigo Wg_Interface_TextHolder::TextAlignment() const
{
	return m_pText->alignment();
}

WgText* Wg_Interface_TextHolder::TextObj() const
{
	return m_pText;
}

int Wg_Interface_TextHolder::TextLines() const
{
	return m_pText->nbLines();
}



#endif // WG_INTERFACE_TEXTHOLDER_DOT_H
