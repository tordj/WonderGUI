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

#ifndef	WG_SIMPLETEXT_DOT_H
#define WG_SIMPLETEXT_DOT_H

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

//____ WgSimpleText ____________________________________________________________

class WgSimpleText : public WgText
{
public:
	WgSimpleText();
	WgSimpleText( const char * pText );
	WgSimpleText( const Uint16 * pText );
	~WgSimpleText();

	Uint32					nbLines() const { return m_nLines; };
	const WgTextLine *		getLines() const;
	inline const WgChar *	getText() const { return (WgChar*) m_pLines[0].pText; }


	virtual WgTextLine *	getLine( Uint32 line ) const;
	WgChar * 				getLineText( Uint32 line ) const;

	const WgTextLine *		getSoftLines() const;
	WgTextLine *			getSoftLine( Uint32 line ) const;
	WgChar * 				getSoftLineText( Uint32 line ) const;
	Uint32					nbSoftLines() const;

	void				posSoft2Hard( Uint32 &line, Uint32 &col ) const {return;}
	void				posHard2Soft( Uint32 &line, Uint32 &col ) const {return;}


protected:
	WgChar * beginChange(   Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar, 
							Uint32 addLines, Uint32 delLines, WgChar ch );

	void	 endChange( Uint32 startline, Uint32 nLines );

private:
	WgTextLine *	m_pLines;
	Uint32			m_nLines;

};






#endif // WG_SIMPLETEXT_DOT_H
