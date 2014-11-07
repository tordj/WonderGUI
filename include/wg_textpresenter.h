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

	int		CoordToCharPos( WgLegacyTextField * pField, WgState state, WgRect canvas, WgCoord pos ) = 0;
	int		CoordToCaretPos( WgLegacyTextField * pField, WgState state, WgRect canvas, WgCoord pos ) = 0;


	void 	RenderTextField( WgGfxDevice * pDevice, WgLegacyTextField * pText, WgRect canvas, WgRect clip ) = 0;
	void 	RenderEditTextField( WgGfxDevice * pDevice, WgLegacyTextField * pText, WgRect canvas, WgRect clip ) = 0;

	void	OnTextModified( WgLegacyTextField * pField, int ofs, int charsRemoved, int charsAdded, WgRect canvas, WgRect clip ) = 0;
	void	OnFieldResize( WgLegacyTextField * pField, WgSize newSize );
	void	OnStateChange( WgLegacyTextField * pField, WgState newState, WgState oldState );

	int		MoveCaret( int caretOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif ) = 0;

	WgSize	PreferredSize( WgLegacyTextField * pField );


protected:


};


class WgLegacyTextField2
{
	WgCharBuffer		m_text;
	WgTextStylePtr		m_pStyle;
	WgTextPresenterPtr	m_pPresenter;
	void *				m_pPresenterData;
	WgSize				m_size;
	WgState				m_state;
};


#endif //WG_TEXTPRESENTER_DOT_H
