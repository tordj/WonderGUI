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

#ifndef	WG_TEXTPRESENTER_DOT_H
#define	WG_TEXTPRESENTER_DOT_H


class WgTextField;
class WgEditTextField;

class WgTextPresenter;
typedef	WgStrongPtr<WgTextPresenter,WgObjectPtr>	WgTextPresenterPtr;
typedef	WgWeakPtr<WgTextPresenter,WgObjectWeakPtr>	WgTextPresenterWeakPtr;

class WgTextPresenter : WgObject
{
public:
	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgTextPresenterPtr	Cast( const WgObjectPtr& pObject );

	int		CoordToChar( WgTextField * pField, WgCoord pos ) = 0;
	WgRect	CharToRect( WgTextField * pField, int charOfs ) = 0;

	int		CoordToCaretPos( WgTextField * pField, WgCoord pos ) = 0;



	void 	RenderTextField( WgTextField * pText, WgGfxDevice * pDevice, WgRect canvas, WgRect clip ) = 0;
	void 	RenderEditTextField( WgEditTextField * pText, WgGfxDevice * pDevice, WgRect canvas, WgRect clip ) = 0;

	void	OnTextModified( WgTextField * pField, int ofs, int charsRemoved, int charsAdded, WgRect canvas, WgRect clip ) = 0;
	void	OnFieldResize( WgTextField * pField, WgSize newSize );
	void	OnStateChange( WgTextField * pField, WgState newState, WgState oldState );

	int		MoveCaret( WgTextField * pField, int caretOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif ) = 0;

	WgSize	PreferredSize( WgTextField * pField );
	int		MatchingWidth( int height ) const;
	int		MatchingHeight( int width ) const;


protected:


};




#endif //WG_TEXTPRESENTER_DOT_H
