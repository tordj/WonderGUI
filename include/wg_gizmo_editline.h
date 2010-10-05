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
#ifndef WG_GIZMO_EDITLINE_DOT_H
#define WG_GIZMO_EDITLINE_DOT_H


#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
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


//____ WgGizmoEditline ____________________________________________________________

class WgGizmoEditline : public WgGizmo, public WgInterfaceEditText
{
public:
	WgGizmoEditline();
	virtual ~WgGizmoEditline();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	inline void	SetPasswordMode( bool on_off ) { m_bPasswordMode = on_off; };
	inline bool PasswordMode() {return m_bPasswordMode;};
	inline Uint16 PasswordGlyph() const				 { return m_pwGlyph; };
	inline void SetMaxCharacters( Uint32 characters ) { m_maxCharacters = characters; }
	inline Uint32 MaxCharacters() { return m_maxCharacters; }
	void	SetPasswordGlyph( Uint16 glyph );

	void		goBOL();
	void		goEOL();
	void		goBOF() { goBOL(); }
	void		goEOF() { goEOL(); }

	Uint32	InsertTextAtCursor( const WgCharSeq& str );
	bool	InsertCharAtCursor( Uint16 c );

	virtual void		SetInputMode(InputMode mode);
	virtual InputMode	GetInputMode() const { return m_inputMode; }
	virtual bool		TempIsInputField() const	{ return IsEditable() && TextObj()->nbChars() > 0; }
	virtual Wg_Interface_TextHolder* TempGetText(){ return this; }

	bool	SetTextWrap(bool bWrap);						// Overloading SetTextWrap since we don't support wrapping text.

protected:

	bool	IsEditable() const { return m_inputMode == Editable; }
	bool	IsSelectable() const { return m_inputMode != Static; }

	void	OnUpdate( const WgUpdateInfo& _updateInfo );
	void	OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	OnGotInputFocus();
	void	OnLostInputFocus();
	void	OnCloneContent( const WgGizmo * _pOrg );
	void	OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	OnEnable();
	void	OnDisable();
	void	OnNewSize( const WgSize& size );

	void	TextModified();

private:

	void	AdjustViewOfs();

	WgText				m_text;

	bool				m_bPasswordMode;
	bool				m_bHasFocus;
	Uint32				m_maxCharacters;
	Uint16				m_pwGlyph;
	int					m_viewOfs;
	InputMode			m_inputMode;
};


#endif //WG_GIZMO_EDITLINE_DOT_H
