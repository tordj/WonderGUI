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
	enum InputMode
	{
		Static,
		Selectable,
		Editable,
	};
	virtual InputMode GetInputMode() const  { return Static; }

	virtual void			SetTextManager( WgTextManager * pManager ) { m_pText->setManager( pManager ); TextModified(); }
	inline WgTextManager * GetTextManager() const { return m_pText->getManager(); }

//	inline bool			SetFonts( WgFont * pFont ) { SetFont(pFont); return true; }		// DEPRECATED!!!
	inline WgTextPropPtr GetTextProperties() { return m_pText->getProperties(); }


	inline WgColor		GetTextColor() const { return m_pText->getColor(); }
	inline WgColor		GetTextColor(WgMode mode) const { return m_pText->getColor(mode); }
	inline WgFontStyle	GetTextStyle(WgMode mode) const { return m_pText->getStyle(mode); }
	inline int			GetBreakLevel() const { return m_pText->getBreakLevel(); }
	inline WgFont *		GetFont() const { return m_pText->getFont(); }


// -------------

	inline void			SetTextProperties( const WgTextPropPtr& pProp )		{ m_pText->setProperties(pProp); TextModified(); }

	inline void			SetTextColor( const WgColor color )					{ m_pText->setColor(color); TextModified(); }
	inline void			SetTextColor( const WgColor color, WgMode mode )		{ m_pText->setColor(color,mode); TextModified(); }

	inline void			SetTextStyle( WgFontStyle style )					{ m_pText->setStyle(style); TextModified(); }
	inline void			SetTextStyle( WgFontStyle style, WgMode mode )		{ m_pText->setStyle(style,mode); TextModified(); }

	inline void			SetBreakLevel( int level )							{ m_pText->setBreakLevel(level); TextModified(); }

	inline void			SetFont( WgFont * pFont )							{ m_pText->setFont(pFont); TextModified(); }

	inline void			SetSelectionColor( WgColor color )							{ m_pText->setSelectionColor(color); }
	inline WgColor		GetSelectionColor() const									{ return m_pText->getSelectionColor(); }

// -------------

	inline void			ClearTextProperties()								{ m_pText->clearProperties(); TextModified(); }

	inline void			ClearTextColor()										{ m_pText->clearColor(); TextModified(); }
	inline void			ClearTextColor( WgMode mode )						{ m_pText->clearColor(mode); TextModified(); }

	inline void			ClearTextStyle()										{ m_pText->clearStyle(); TextModified(); }
	inline void			ClearTextStyle( WgMode mode )						{ m_pText->clearStyle(mode); TextModified(); }

	inline void			ClearBreakLevel()									{ m_pText->clearBreakLevel(); TextModified(); }

	inline void			ClearFont()											{ m_pText->clearFont(); TextModified(); }

//	-----------

	void				SetTextMode( WgMode mode );
	void				SetTextAlignment( const WgOrigo& origo );
	void				SetTextTintMode( WgTintMode mode );

	void				ClearText();

	void		SetText( const WgCharSeq& seq );
	void		SetText( const WgCharBuffer * buffer );
	void		SetText( const WgString& str );
	void		SetText( const WgText * pText );

	int			AddText( const WgCharSeq& seq );
	int			InsertText( int ofs, const WgCharSeq& seq );
	int			ReplaceText( int ofs, int nDelete, const WgCharSeq& seq );
	int			DeleteText( int ofs, int len );
	void		DeleteSelectedText();

	int			AddChar( const WgChar& character );
	int			InsertChar( int ofs, const WgChar& character );
	int			ReplaceChar( int ofs, const WgChar& character );
	int			DeleteChar( int ofs );

	void				SetLineSpaceAdjustment( Sint8 adjustment );
	Sint8				GetLineSpaceAdjustment() const;

	virtual bool		SetTextWrap(bool bWrap);
	bool				GetTextWrap() const { return m_pText->IsWrap(); }

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
