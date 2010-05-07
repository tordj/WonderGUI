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

#ifndef	WDG_EDITTEXTVIEW_DOT_H
#define	WDG_EDITTEXTVIEW_DOT_H


#ifndef WDG_BASECLASS_VIEW_DOT_H
#	include <wdg_baseclass_view.h>
#endif

#ifndef	WG_INTERFACE_EDITTEXT_DOT_H
#	include <wg_interface_edittext.h>
#endif

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_CURSORINSTANCE_DOT_H
#	include <wg_cursorinstance.h>
#endif


class Wdg_TextView:public Wdg_Baseclass_View, public WgInterfaceEditText
{
public:
	WIDGET_CONSTRUCTORS(Wdg_TextView, Wdg_Baseclass_View);
	virtual ~Wdg_TextView();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	inline void SetMaxCharacters( Uint32 characters ) { m_maxCharacters = characters; }
	inline Uint32 MaxCharacters() { return m_maxCharacters; }

	Uint32	InsertTextAtCursor( const WgCharSeq& str );
	bool	InsertCharAtCursor( Uint16 c );

	virtual void		SetInputMode(InputMode mode);
	virtual InputMode	GetInputMode() const { return m_inputMode; }

	void	SetNewlineCombo( WgKey	key, WgModifierKeys modifiers );

	void		goBOL();
	void		goEOL();
	void		goBOF();
	void		goEOF();

	virtual bool		IsInputField() const	{ return (m_text.nbChars() > 0); }

protected:
	WgWidget * NewOfMyType() const;
private:
	bool	IsEditable() const { return m_inputMode == Editable; }
	void	Init();
	
	void	DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
	void	DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 _layer );	
	void	DoMyOwnRefresh( void );
	void	DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
	bool 	DoMyOwnMarkTest( int _x, int _y );
	void	DoMyOwnInputFocusChange( bool _bFocus );
	void	DoMyOwnDisOrEnable();
	void	DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo );

	void	TextModified();

	void	AdjustViewOfs();

	WgText				m_text;

	Uint32				m_maxCharacters;

	WgKey				m_newlineKey;
	WgModifierKeys		m_newlineModif;

	InputMode			m_inputMode;
};





#endif // WDG_EDITTEXTVIEW_DOT_H
