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

#ifndef	WG_WRAPTEXT_DOT_H
#define WG_WRAPTEXT_DOT_H

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif




//____ WgWrapText _____________________________________________________________

class WgWrapText : public WgText
{
public:
	WgWrapText();
	WgWrapText( const char * pText );
	WgWrapText( const Uint16 * pText );
	~WgWrapText();

	const WgChar *		getText() const;

	Uint32				nbLines() const;
	Uint32				nbSoftLines() const;

	const WgTextLine *	getLines() const;
	const WgTextLine *	getSoftLines() const;

	WgTextLine *		getLine( Uint32 line ) const;
	WgTextLine *		getSoftLine( Uint32 line ) const;

	WgChar * 			getLineText( Uint32 line ) const;
	WgChar * 			getSoftLineText( Uint32 line ) const;

	void				posSoft2Hard( Uint32 &line, Uint32 &col ) const;
	void				posHard2Soft( Uint32 &line, Uint32 &col ) const;


	void			setLineWidth( Uint32 width );
	inline Uint32	getLineWidth() const { return m_lineWidth; }
	void			setFontSet( WgFont * pFonts ) { WgText::setFontSet(pFonts); regenSoftLines(); };

	void			SetWrap( bool bWrap );
	bool			IsWrap() const { return m_bWrap; }

protected:

	WgChar *		beginChange( Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar, 
								 Uint32 addLines, Uint32 delLines, WgChar ch );

	void			endChange( Uint32 startline, Uint32 nLines );


private:

	void			regenSoftLines();		// Set/remove softbreaks and regenerate the softlines-array (if necessary).

	WgTextLine*	m_pHardLines;
	WgTextLine* m_pSoftLines;

	Uint32		m_nSoftLines;
	Uint32		m_nHardLines;

	Uint32		m_lineWidth;

	bool		m_bWrap;
};


#endif // WG_WRAPTEXT_DOT_H
